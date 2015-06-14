#include "Graphic.h"
#include <string>
#include "GeometryGens.h"
#include "Light.h"


extern "C" {
	void initData();
    void shutDown();
	void runOnCuda();
	MyVector3::vector3 getParticlePos(int i);
	int getParticleCount();
	void registerResource(ID3D11Buffer* positionsVB);
	void releaseCudaVB();
	void writeData();
}

Graphic::Graphic(int w, int h) :graphicW(w), graphicH(h), d3dDevice(nullptr), d3dContext(nullptr), d3d(nullptr), baseEffect(nullptr)//, sphSystem(nullptr)
{

}

Graphic::~Graphic()
{
	shutDown();//释放GPU资源

	SafeDelete(baseEffect);
	SafeDelete(depthMapEffect);
	SafeDelete(ground);
	SafeDelete(groundTex);
	SafeDelete(baseTech);
	SafeDelete(camera);
	SafeDelete(keyboardControler);
	SafeDelete(mouseControler);
	SafeDelete(ballModel);
	SafeDelete(ballVer);
	SafeDelete(ballIndex);
	SafeDelete(instanceStruct);
	releaseCudaVB();
}

void Graphic::InitGraphic()
{
	//初始化D3D
	d3d = new D3D(graphicW, graphicH);
	d3d->InitD3D();

	d3dDevice = d3d->GetD3DDevice();
	d3dContext = d3d->GetDeviceContext();

	unsigned short maxCount = 4096;
	//float3 wallBoxMin(-8.f, -1.f, -5.f);
	//float3 wallBoxMax(8.f, 20.f, 5.f);
	//float3 fluidBoxMin(-4.f, 10.f, -4.f);
	//float3 fluidBoxMax(4.f, 16.f, 4.f);
	//float3 gravity(0.f, -9.8f, 0.f);
	//sphSystem = new SphSystem;
	//sphSystem->init(maxCount, wallBoxMin, wallBoxMax, fluidBoxMin, fluidBoxMax, gravity);
	initData();

	//设定顶点拓扑结构
	d3d->SetTopology();

	//创建地面模型
	//GeoGen::MeshData plane;
	//GeoGen::CreateGrid(40.f, 40.f, 1, 1, plane);

	//BaseVertex groundVers[4];
	//for (int i = 0; i < 4; ++i)
	//{
	//	groundVers[i].pos = plane.vertices[i].pos;
	//	groundVers[i].tex = plane.vertices[i].tex;
	//	groundVers[i].normal = plane.vertices[i].normal;
	//	groundVers[i].tangent = plane.vertices[i].tangent;
	//}
	//UINT groundIndices[6];
	//for (int i = 0; i < 6; ++i)
	//{
	//	groundIndices[i] = plane.indices[i];
	//}

	//ground = new Model(d3dDevice, d3dContext);
	//ground->CreateVertexBuffer(groundVers, sizeof(groundVers));
	//ground->CreateIndexBuffer(groundIndices, sizeof(groundIndices));

	//球
	GeoGen::MeshData ball;
	GeoGen::CreateSphere(.2f, 8, 8, ball);
	int countV = ball.vertices.size();
	ballVer = new SphVertex[countV];
	for (int i = 0; i < countV;++i)
	{
		ballVer[i].pos = ball.vertices[i].pos;
		ballVer[i].color = { 0.4f, 0.7f, 1.f, 1.f };
		ballVer[i].normal = ball.vertices[i].normal;
	}
	int countI = ball.indices.size();
	ballIndex = new UINT[countI];
	for (int i = 0; i < countI; ++i)
	{
		ballIndex[i] = ball.indices[i];
	}
	ballModel = new Model(d3dDevice, d3dContext);
	ballModel->CreateVertexBuffer(ballVer, sizeof(SphVertex)*countV);
	ballModel->CreateIndexBuffer(ballIndex, sizeof(UINT)*countI);

	//纹理
	groundTex = new Texture();
	groundTex->CreateFromFile(d3dDevice, L"textures/brick.dds");

	//初始化Effect
	InitBaseEffect();
	InitDepthMapEffect();

	//新建一个相机
	camera = new Camera();
	//新建一个键盘控制器
	keyboardControler = new KeyboardControler(camera);
	//设置键盘控制器
	Input()->GetKeyboard()->SetReceiver(keyboardControler);
	//新建一个鼠标控制器
	mouseControler = new MouseControler(camera);
	//设置鼠标控制器
	Input()->GetMouse()->SetReceiver(mouseControler);

	PassDataToShader();

	d3d->BindInputLayout(baseEffect->GetInputLayout("Tech"));

	particleCount = getParticleCount();

	instanceStruct = new InstanceStruct[particleCount];

	ballModel->CreateInstanceBuffer(instanceStruct, sizeof(InstanceStruct)*particleCount);
	d3d->BindInstanceBuffer(ballModel->GetInstanceBuffer(), sizeof(InstanceStruct));
	d3d->BindVertexBuffer(ballModel->GetVertexBuffer(), sizeof(SphVertex));
	d3d->BindIndexBuffer(ballModel->GetIndexBuffer());

	registerResource(ballModel->GetInstanceBuffer());
}

void Graphic::InitBaseEffect()
{
	baseEffect = new Effect(d3dDevice, std::wstring(L"baseEffect.fx"));

	baseTech = baseEffect->GetEffect()->GetTechniqueByName("Tech");//获取指向Tech的指针

	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
		D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{"NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,28,D3D11_INPUT_PER_VERTEX_DATA},

		//Instance
		{ "INSTANCEPOS", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0,
		D3D11_INPUT_PER_INSTANCE_DATA, 1 },
	};

	baseEffect->CreateInputLayout(vertexDesc, 4, std::string("Tech"));
}

void Graphic::InitDepthMapEffect()
{
	depthMapEffect = new Effect(d3dDevice, std::wstring(L"DepthMapEffect.fx"));

	depthMapTech = depthMapEffect->GetEffect()->GetTechniqueByName("Tech");//获取指向Tech的指针

	D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12,
		D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 20,
		D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 32,
		D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	depthMapEffect->CreateInputLayout(vertexDesc, 4, std::string("Tech"));
}

void Graphic::PassDataToShader()
{
	// 创建视矩阵(主相机）
	XMVECTOR pos = XMVectorSet(25.0f, 25.0f, -15.0f, 1.0f);
	XMVECTOR target = XMVectorSet(15.0f, 0.0f, 5.0f, 1.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	camera->LookAt(pos, target, up);
	camera->UpdateViewMatrix();

	//透视投影矩阵
	float fovy = XM_PIDIV4;
	float farZ = 1000.0f;
	camera->SetLens(fovy, static_cast<float>(graphicW / graphicH), 1.0f, farZ);
	
	//baseEffect传值
	ID3DX11EffectMatrixVariable* Proj = baseEffect->GetEffect()->GetVariableByName("Proj")->AsMatrix();
	Proj->SetMatrix(reinterpret_cast<float*>(&camera->Proj()));

	ID3DX11EffectMatrixVariable* World = baseEffect->GetEffect()->GetVariableByName("World")->AsMatrix();
	XMMATRIX W = XMMatrixIdentity();
	World->SetMatrix(reinterpret_cast<float*>(&W));

	//传递材质属性
	Material material;
	material.ambient = { 0.5f, 0.5f, 0.5f, 1.0f };
	material.diffuse = { 0.4f, 0.7f, 1.f, 1.0f };
	material.specular = { 0.3f, 0.3f, 0.3f, 1.0f };
	ID3DX11EffectVariable* pMaterial = baseEffect->GetEffect()->GetVariableByName("Mat");
	pMaterial->SetRawValue(&material, 0, sizeof(Material));

	//传递方向光
	DirectLight light;
	light.ambient = { 0.4f, 0.7f, 1.0f, 1.0f };
	light.diffuse = { 0.4f, 0.7f, 1.0f, 1.0f };
	light.specular = { 0.5f, 0.5f, 0.5f, 1.0f };
	light.direction = { 1.0f, -1.0f, 1.0f };
	ID3DX11EffectVariable* lightsArray = baseEffect->GetEffect()->GetVariableByName("DLight");
	lightsArray->SetRawValue(&light, 0, sizeof(DirectLight));

	//depthMapEffect传值
	//Proj = depthMapEffect->GetEffect()->GetVariableByName("Proj")->AsMatrix();
	//Proj->SetMatrix(reinterpret_cast<float*>(&camera->Proj()));
}

void Graphic::UpdateCamera()
{
	//更新视矩阵
	camera->UpdateViewMatrix();

	//DepthMapEffect
	ID3DX11EffectMatrixVariable* View = depthMapEffect->GetEffect()->GetVariableByName("View")->AsMatrix();
	View->SetMatrix(reinterpret_cast<float*>(&camera->View()));
	//baseEffect
	View = baseEffect->GetEffect()->GetVariableByName("View")->AsMatrix();
	View->SetMatrix(reinterpret_cast<float*>(&camera->View()));
}

//每帧都要执行
void Graphic::Frame()
{
	UpdateCamera();

	d3d->ClearBuffer();

	runOnCuda();

	//----------------------------------------------------新添加的代码----------------------------
	
	writeData();

	ballModel->DrawInstance(baseTech, 0,0,particleCount);
	//----------------------------------------------------------------------------------------------------------

	d3d->GetSwapChain()->Present(0, 0);

}
