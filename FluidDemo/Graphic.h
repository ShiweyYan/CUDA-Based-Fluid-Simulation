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

//���嶥���ʽ
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

	//ָ��Effect��Model��Texture��Դ��ָ��
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


	ID3DX11EffectTechnique* baseTech;// baseEffect �е� Tech 
	ID3DX11EffectTechnique* depthMapTech;// depthMapEffect �е� Tech 

	//SphSystem* sphSystem;

	//��ʼ��coreEffect
	void InitDepthMapEffect();
	//��ʼ��baseEffect
	void InitBaseEffect();
	//���ڴ���Դ洫������
	void PassDataToShader();
	//�������
	void UpdateCamera();
public:
	Graphic(int w,int h);
	~Graphic();

	//��ʼ��ͼ�β�
	void InitGraphic();
	//ÿ֡��ִ��
	void Frame();
};