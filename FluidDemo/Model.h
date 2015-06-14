#pragma once
#include <D3D11.h>
#include <d3dx11effect.h>
#include "GeometryGens.h"

class Model
{
private:
	ID3D11Device* d3dDevice;
	ID3D11DeviceContext* d3dContext;
	ID3D11Buffer* VB;
	ID3D11Buffer* IB;
	ID3D11Buffer* InstanceBuffer;

	UINT indexCount;

	Model(Model& model) = delete;
public:
	Model(ID3D11Device* device,ID3D11DeviceContext* context);
	~Model();

	void CreateVertexBuffer(void* vertices, int verticesSize);//������1.�������� 2.����������ռ�ڴ��С
	void CreateIndexBuffer(UINT* indices, int indexSize);//������1.�������� 2.����������ռ�ڴ��С
	void CreateInstanceBuffer(void* instances, int instancesSize);//������1.ʵ������ 2.ʵ��������ռ�ڴ��С(ʵ��bufferҲ��һ�ֶ��㻺��)
	void ReleaseInstanceBuffer();

	/*
	������1.������ʹ�õ�tech 2.ʹ���ĸ�pass��Ⱦ��Ĭ����0  3.���Ƶ�������������Ĭ�Ͼ�ʹ�������������е���������

	��������������Լ����ɣ�MSDN�� ctrl+C/V���ģ��Ҷ�����Ĭ��ֵ�����Բ���

	StartIndexLocation [in]
	Type: UINT
	The location of the first index read by the GPU from the index buffer.

	BaseVertexLocation [in]
	Type: INT
	A value added to each index before reading a vertex from the vertex buffer.
	*/
	void Draw(ID3DX11EffectTechnique* tech, UINT passIndex = 0, UINT indexCount = 0, UINT startIndexLocation = 0, int baseVertexLocation = 0);
	void DrawInstance(ID3DX11EffectTechnique* tech, UINT passIndex = 0, UINT indexCount = 0, UINT instanceCount = 0,UINT startIndexLocation = 0, int baseVertexLocation = 0,UINT startInstanceLocation=0);


	ID3D11Buffer* GetVertexBuffer() const{ return VB; }
	ID3D11Buffer* GetIndexBuffer() const{ return IB; }
	ID3D11Buffer* GetInstanceBuffer() const{ return InstanceBuffer; }
};