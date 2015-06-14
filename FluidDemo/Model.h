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

	void CreateVertexBuffer(void* vertices, int verticesSize);//参数：1.顶点数据 2.顶点数组所占内存大小
	void CreateIndexBuffer(UINT* indices, int indexSize);//参数：1.索引数据 2.索引数组所占内存大小
	void CreateInstanceBuffer(void* instances, int instancesSize);//参数：1.实例数据 2.实例数组所占内存大小(实例buffer也是一种顶点缓存)
	void ReleaseInstanceBuffer();

	/*
	参数：1.绘制所使用的tech 2.使用哪个pass渲染，默认用0  3.绘制的索引数，不填默认就使用用索引缓存中的索引数量

	后边两个个参数自己看吧，MSDN上 ctrl+C/V来的，我都给了默认值，可以不填

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