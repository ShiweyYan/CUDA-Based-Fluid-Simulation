#pragma once
#include <D3D11.h>
#include <string>
#include <vector>

class Texture
{
private:
	ID3D11Texture2D* texture;
	ID3D11ShaderResourceView* SRV;

public:
	Texture();
	~Texture();
	Texture(Texture& tex) = delete;

	//直接在内存中创建一张纹理（可以用来作为渲染目标）
	void Create(ID3D11Device* d3dDevice, int textureW, int textureH, DXGI_FORMAT format = DXGI_FORMAT_R32G32B32A32_FLOAT, bool AsRenderTarget = true, bool AsShaderResource = true);

	//从文件中载入一张资源纹理
	void CreateFromFile(ID3D11Device* d3dDevice, std::wstring fileName);

	//从文件数组中载入一个资源纹理数组
	void CreateTexture2DArray(
		ID3D11Device* d3dDevice,
		ID3D11DeviceContext* d3dDeviceContext,
		std::vector<std::wstring>& filenames,
		DXGI_FORMAT format,
		UINT filter,
		UINT mipFilter);

	void CreateRandomVectorTexture(ID3D11Device* d3dDevice, int textureW, int textureH);
	
	ID3D11ShaderResourceView* GetShaderResourceView() const { return SRV; }

	ID3D11Texture2D* GetTexture() const { return texture; }

	//operator ID3D11Texture2D*() const{ return texture; }
};