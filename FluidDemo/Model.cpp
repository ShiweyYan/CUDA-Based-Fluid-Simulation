#include "Model.h"
#include "DXHelp.h"

Model::Model(ID3D11Device* device,ID3D11DeviceContext* context)
:d3dDevice(device), d3dContext(context)
{
}

Model::~Model()
{
	SafeRelease(VB);
	SafeRelease(IB);
	SafeRelease(InstanceBuffer);
	SafeRelease(InstanceBuffer);
}

void Model::CreateInstanceBuffer(void* instances, int instancesSize)
{
	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_DEFAULT;
	vbd.ByteWidth = instancesSize;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = instances;

	if (instancesSize > 0)
	{
		d3dDevice->CreateBuffer(&vbd, &vinitData, &InstanceBuffer);
	}
}

void Model::CreateVertexBuffer(void* vertices, int verticesSize)
{

	D3D11_BUFFER_DESC vbd;
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	vbd.ByteWidth = verticesSize;
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.CPUAccessFlags = 0;
	vbd.MiscFlags = 0;
	vbd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA vinitData;
	vinitData.pSysMem = vertices;

	if (verticesSize > 0)
	{
		d3dDevice->CreateBuffer(&vbd, &vinitData, &VB);
	}
}

void Model::CreateIndexBuffer(UINT* indices, int indexSize)
{
	indexCount = indexSize / sizeof(UINT);

	D3D11_BUFFER_DESC ibd;
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	ibd.ByteWidth = indexSize;
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.CPUAccessFlags = 0;
	ibd.MiscFlags = 0;
	ibd.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA iinitData;
	iinitData.pSysMem = indices;

	if (indexSize > 0)
	{
		d3dDevice->CreateBuffer(&ibd, &iinitData, &IB);
	}

}

void Model::Draw(ID3DX11EffectTechnique* tech, UINT passIndex, UINT indexCount, UINT startIndexLocation, int baseVertexLocation)
{
	if (indexCount==0)
	{
		indexCount = this->indexCount;
	}
	D3DX11_TECHNIQUE_DESC techDesc; 
	tech->GetDesc(&techDesc);
	tech->GetPassByIndex(passIndex)->Apply(0, d3dContext);
	//  绘制几何体
	d3dContext->DrawIndexed(indexCount, startIndexLocation, baseVertexLocation);
}

void Model::DrawInstance(ID3DX11EffectTechnique* tech, UINT passIndex, UINT indexCount, UINT instanceCount, UINT startIndexLocation, int baseVertexLocation,UINT startInstanceLocation)
{
	if (indexCount == 0)
	{
		indexCount = this->indexCount;
	}
	D3DX11_TECHNIQUE_DESC techDesc;
	tech->GetDesc(&techDesc);
	tech->GetPassByIndex(passIndex)->Apply(0, d3dContext);
	//  绘制几何体
	d3dContext->DrawIndexedInstanced(indexCount, instanceCount, startIndexLocation, baseVertexLocation, startInstanceLocation);
}

void Model::ReleaseInstanceBuffer()
{
	SafeRelease(InstanceBuffer);
}