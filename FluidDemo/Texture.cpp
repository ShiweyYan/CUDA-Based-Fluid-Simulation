#include "Texture.h"
#include "DXHelp.h"
#include <assert.h>
#include <D3DX11.h>

Texture::Texture() :texture(nullptr), SRV(nullptr)
{
}

Texture::~Texture()
{
	//SafeRelease(d3dDevice);
	SafeRelease(texture);
	SafeRelease(SRV);
}

void Texture::Create(ID3D11Device* d3dDevice, int textureW, int textureH, DXGI_FORMAT format, bool asRenderTarget, bool asShaderResource)
{
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = textureW;
	texDesc.Height = textureH;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = format;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	if (asRenderTarget)
	{
		if (asShaderResource)
		{
			texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		}
		else
			texDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
	}
	else
	{
		if (asShaderResource)
		{
			texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		}
		else
		{
			MessageBox(0, L"[Texture::Create] Both para is false!", 0, 0);
			assert(false);
		}
	}

	d3dDevice->CreateTexture2D(&texDesc, nullptr, &texture);

	if (asShaderResource)
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		shaderResourceViewDesc.Format = texDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;
		d3dDevice->CreateShaderResourceView(texture, &shaderResourceViewDesc, &SRV);
	}
	
}

void Texture::CreateFromFile(ID3D11Device* d3dDevice, std::wstring texName)
{
	if (nullptr!=SRV)
	{
		MessageBox(0, L"one Texture just can create one time", 0, 0);
		return;
	}
	else
	{
		D3DX11CreateShaderResourceViewFromFile(d3dDevice,
			texName.c_str(), 0, 0, &SRV, 0);
	}
}

void Texture::CreateTexture2DArray(
	ID3D11Device* d3dDevice,
	ID3D11DeviceContext* d3dDeviceContext,
	std::vector<std::wstring>& filenames,
	DXGI_FORMAT format,
	UINT filter,
	UINT mipFilter)
{
	if (nullptr != SRV)
	{
		MessageBox(0, L"[Texture::CreateTexture2DArraySRV] This View have been created!", 0, 0);
		assert(false);
	}
	//
	// 从文件加载单独的纹理元素。这些纹理不能被GPU使用(0 bind flags)，
	// 只是用来从文件中加载图像数据。我们使用STAGING让CPU可以访问资源。
	//

	UINT size = filenames.size();

	std::vector<ID3D11Texture2D*> srcTex(size);
	for (UINT i = 0; i < size; ++i)
	{
		D3DX11_IMAGE_LOAD_INFO loadInfo;

		loadInfo.Width = D3DX11_FROM_FILE;
		loadInfo.Height = D3DX11_FROM_FILE;
		loadInfo.Depth = D3DX11_FROM_FILE;
		loadInfo.FirstMipLevel = 0;
		loadInfo.MipLevels = D3DX11_FROM_FILE;
		loadInfo.Usage = D3D11_USAGE_STAGING;
		loadInfo.BindFlags = 0;
		loadInfo.CpuAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
		loadInfo.MiscFlags = 0;
		loadInfo.Format = format;
		loadInfo.Filter = filter;
		loadInfo.MipFilter = mipFilter;
		loadInfo.pSrcInfo = 0;

		HR(D3DX11CreateTextureFromFile(d3dDevice, filenames[i].c_str(),
			&loadInfo, 0, (ID3D11Resource**)&srcTex[i], 0));
	}

	//
	// 创建纹理数组。每个纹理元素都具有相同的格式/大小。
	//

	D3D11_TEXTURE2D_DESC texElementDesc;
	srcTex[0]->GetDesc(&texElementDesc);

	D3D11_TEXTURE2D_DESC texArrayDesc;
	texArrayDesc.Width = texElementDesc.Width;
	texArrayDesc.Height = texElementDesc.Height;
	texArrayDesc.MipLevels = texElementDesc.MipLevels;
	texArrayDesc.ArraySize = size;
	texArrayDesc.Format = texElementDesc.Format;
	texArrayDesc.SampleDesc.Count = 1;
	texArrayDesc.SampleDesc.Quality = 0;
	texArrayDesc.Usage = D3D11_USAGE_DEFAULT;
	texArrayDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texArrayDesc.CPUAccessFlags = 0;
	texArrayDesc.MiscFlags = 0;

	ID3D11Texture2D* texArray = 0;
	HR(d3dDevice->CreateTexture2D(&texArrayDesc, 0, &texArray));

	//
	// 将单独的纹理元素复制到纹理数组中。
	//

	// 处理每个纹理元素...
	for (UINT texElement = 0; texElement < size; ++texElement)
	{
		// 处理每个渐进纹理层...
		for (UINT mipLevel = 0; mipLevel < texElementDesc.MipLevels; ++mipLevel)
		{
			D3D11_MAPPED_SUBRESOURCE mappedTex2D;
			HR(d3dDeviceContext->Map(srcTex[texElement], mipLevel, D3D11_MAP_READ, 0, &mappedTex2D));

			d3dDeviceContext->UpdateSubresource(texArray,
				D3D11CalcSubresource(mipLevel, texElement, texElementDesc.MipLevels),
				0, mappedTex2D.pData, mappedTex2D.RowPitch, mappedTex2D.DepthPitch);

			d3dDeviceContext->Unmap(srcTex[texElement], mipLevel);
		}
	}

	//
	// 创建纹理数组的资源视图
	//

	D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;
	viewDesc.Format = texArrayDesc.Format;
	viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	viewDesc.Texture2DArray.MostDetailedMip = 0;
	viewDesc.Texture2DArray.MipLevels = texArrayDesc.MipLevels;
	viewDesc.Texture2DArray.FirstArraySlice = 0;
	viewDesc.Texture2DArray.ArraySize = size;

	HR(d3dDevice->CreateShaderResourceView(texArray, &viewDesc, &SRV));

	//
	// 清除--我们要的只是资源视图
	//

	SafeRelease(texArray);

	for (UINT i = 0; i < size; ++i)
		SafeRelease(srcTex[i]);
}

void Texture::CreateRandomVectorTexture(ID3D11Device* d3dDevice, int textureW, int textureH)
{
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = textureW;
	texDesc.Height = textureH;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_IMMUTABLE;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA initData = { 0 };
	initData.SysMemPitch = textureW * sizeof(XMCOLOR);

	XMCOLOR* color = new XMCOLOR[textureW * textureH];
	for (int i = 0; i < textureH; ++i)
	{
		for (int j = 0; j < textureW; ++j)
		{
			XMFLOAT3 v(RandFloat(), RandFloat(), RandFloat());

			color[i * textureW + j] = XMCOLOR(v.x, v.y, v.z, 0.0f);
		}
	}

	initData.pSysMem = color;

	ID3D11Texture2D* tex = 0;
	HR(d3dDevice->CreateTexture2D(&texDesc, &initData, &tex));

	HR(d3dDevice->CreateShaderResourceView(tex, 0, &SRV));

	delete [] color;
	color = nullptr;

	// view saves a reference.
	SafeRelease(tex);
}