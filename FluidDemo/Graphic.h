#pragma once

#include "D3D.h"
#include "Effect.h"
#include "Model.h"
#include "Texture.h"
#include "Camera.h"
#include "InputLayer.h"
#include "Controler.h"

#include "MyVector3.h"
//sph
//#include "SphSystem.h"

//方板顶点格式
struct BaseVertex
{
	XMFLOAT3 pos;
	XMFLOAT2 tex;
	XMFLOAT3 normal;
	XMFLOAT3 tangent;
};

struct SphVertex
{
	XMFLOAT3 pos;
	XMFLOAT4 color;
	XMFLOAT3 normal;
};

struct InstanceStruct
{
	MyVector3::vector3 pos;
};


class Graphic
{
private:
	D3D* d3d;
	ID3D11Device* d3dDevice;
	ID3D11DeviceContext* d3dContext;

	int graphicW;
	int graphicH;

	Camera* camera;
	KeyboardControler* keyboardControler;
	MouseControler* mouseControler;

	//指向Effect和Model和Texture资源的指针
	Effect* baseEffect;
	Effect* depthMapEffect;
	Model* ground;
	//Model* fluid;
	Texture* groundTex;

	Model* ballModel;
	SphVertex* ballVer;
	UINT* ballIndex;
	InstanceStruct* instanceStruct;

	int particleCount;


	ID3DX11EffectTechnique* baseTech;// baseEffect 中的 Tech 
	ID3DX11EffectTechnique* depthMapTech;// depthMapEffect 中的 Tech 

	//SphSystem* sphSystem;

	//初始化coreEffect
	void InitDepthMapEffect();
	//初始化baseEffect
	void InitBaseEffect();
	//在内存和显存传递数据
	void PassDataToShader();
	//相机更新
	void UpdateCamera();
public:
	Graphic(int w,int h);
	~Graphic();

	//初始化图形层
	void InitGraphic();
	//每帧都执行
	void Frame();
};