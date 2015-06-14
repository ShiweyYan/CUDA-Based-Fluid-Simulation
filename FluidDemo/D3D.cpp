#include "D3D.h"
#include "Window.h"

#include <DxErr.h>
#include <assert.h>

//bool cudaDevice(int* dev, IDXGIAdapter* adapter);
extern "C" void cudaSetD3DDevice();

D3D::D3D(int w, int h) :enable4xMsaa(false), d3DW(w), d3DH(h), d3dDevice(nullptr), 
	d3dContext(nullptr), swapChain(nullptr), renderTargetView(nullptr), 
	depthStencilBuffer(nullptr), depthStencilView(nullptr), rasterizeState(nullptr)
{
}

D3D::~D3D()
{
	//SafeRelease类内的各种COM接口
	SafeRelease(d3dDevice);
	SafeRelease(d3dContext);
	SafeRelease(swapChain);
	SafeRelease(renderTargetView);
	SafeRelease(depthStencilBuffer);
	SafeRelease(depthStencilView);
	SafeRelease(rasterizeState);
	//SafeDelete累内的各种非COM的堆内存

}

void D3D::InitD3D()
{
	CreateDeviceAndContext();
	CreateSwapChain();
	RenderToBackBuffer();
	SetRasterizerState();
	SetViewPort();
}

void D3D::CreateDeviceAndContext()
{

	UINT createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;

	HRESULT  hr = D3D11CreateDevice(
		0,                     //  0为默认显示适配器
		D3D_DRIVER_TYPE_HARDWARE ,
		0,                     //  不使用软件设备
		createDeviceFlags,
		0, 0,               //  默认的特征等级数组
		D3D11_SDK_VERSION,
		&d3dDevice,
		&featureLevel,
		&d3dContext);
	if (FAILED(hr))
	{
		MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
	}
	if (featureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, L"Direct3D FeatureLevel 11 unsupported.", 0, 0);
	}

	//给cuda分配设备
	cudaSetD3DDevice();
}

void D3D::CreateSwapChain()
{
	//检测多重采样质量
	d3dDevice->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R8G8B8A8_UNORM, 4, &m4xMsaaQuality);
	assert(m4xMsaaQuality > 0);

	//创建并填充交换链的描述
	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = d3DW;    // 使用窗口客户区宽度
	sd.BufferDesc.Height = d3DH;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	// 是否使用4X MSAA?
	if (enable4xMsaa)
	{
		sd.SampleDesc.Count = 4;
		// m4xMsaaQuality是通过CheckMultisampleQualityLevels()方法获得的
		sd.SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	// NoMSAA
	else
	{
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	}
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = MainWindow()->GetHwnd();
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	//创建交换链
	IDXGIDevice *  dxgiDevice = nullptr;
	d3dDevice->QueryInterface(__uuidof(IDXGIDevice),(void**)&dxgiDevice);
	IDXGIAdapter* dxgiAdapter = nullptr;
	dxgiDevice->GetParent(__uuidof(IDXGIAdapter),(void**)&dxgiAdapter);
	// 获得IDXGIFactory 接口
	IDXGIFactory*  dxgiFactory = nullptr;
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory),(void**)&dxgiFactory);
	// 现在，创建交换链
	dxgiFactory->CreateSwapChain(d3dDevice, &sd, &swapChain);

	// 释放COM接口
	SafeRelease(dxgiDevice);
	SafeRelease(dxgiAdapter);
	SafeRelease(dxgiFactory);
}

void D3D::SetRasterizerState()
{
	D3D11_RASTERIZER_DESC rsDesc;
	ZeroMemory(&rsDesc, sizeof(D3D11_RASTERIZER_DESC));
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_BACK;
	rsDesc.FrontCounterClockwise = false;
	rsDesc.DepthClipEnable = true;
	rsDesc.MultisampleEnable = true;

	HR(d3dDevice->CreateRasterizerState(&rsDesc, &rasterizeState));
	d3dContext->RSSetState(rasterizeState);
}


void D3D::SetViewPort()
{
	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = static_cast<float>(d3DW);
	vp.Height = static_cast<float>(d3DH);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

	d3dContext->RSSetViewports(1, &vp);
}

void D3D::BindInputLayout(ID3D11InputLayout* inputLayout)
{
	d3dContext->IASetInputLayout(inputLayout);
}

void D3D::BindVertexBuffer(ID3D11Buffer* VB,UINT vertexSize)
{
	//UINT stride = sizeof(skeletonVertex);
	UINT offset = 0;
	d3dContext->IASetVertexBuffers(0, 1, &VB, &vertexSize, &offset);
	//SafeRelease(VB);
}

void D3D::BindInstanceBuffer(ID3D11Buffer* InstanceBuffer, UINT instanceSize)
{
	UINT offset = 0;
	d3dContext->IASetVertexBuffers(1, 1, &InstanceBuffer, &instanceSize, &offset);
}

void D3D::BindIndexBuffer(ID3D11Buffer* IB)
{
	d3dContext->IASetIndexBuffer(IB, DXGI_FORMAT_R32_UINT, 0);
	//SafeRelease(IB);
}

void D3D::SetTopology(D3D_PRIMITIVE_TOPOLOGY topology)
{
	d3dContext->IASetPrimitiveTopology(topology);
}

void D3D::BindRenderTarget(ID3D11Texture2D* renderTexture, ID3D11Texture2D* depthTexture, bool isBackBuffer)
{
	if (nullptr == renderTexture || nullptr == depthTexture)
	{
		MessageBox(0, L"渲染目标纹理 或者 深度纹理不能为空！", 0, 0);
		return;
	}

	if (nullptr!=renderTargetView)
	{
		SafeRelease(renderTargetView);
	}
	D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
	renderTargetViewDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	renderTargetViewDesc.Texture2D.MipSlice = 0;
	HRESULT hr;
	if (isBackBuffer)
	{
		hr = d3dDevice->CreateRenderTargetView(renderTexture, nullptr, &renderTargetView);
		if (S_OK != hr)
		{
			MessageBox(0, L"创建渲染目标视图失败！", 0, 0);
			assert(false);
		}
	}
	else
	{
		hr = d3dDevice->CreateRenderTargetView(renderTexture, &renderTargetViewDesc, &renderTargetView);
		if (S_OK != hr)
		{
			MessageBox(0, L"创建渲染目标视图失败！", 0, 0);
			assert(false);
		}
	}


	if (nullptr != depthStencilView)
	{
		SafeRelease(depthStencilView);
	}
	hr = d3dDevice->CreateDepthStencilView(depthTexture, 0, &depthStencilView);
	if (S_OK != hr)
	{
		MessageBox(0, L"创建深度模板视图失败！", 0, 0);
		assert(false);
	}

	d3dContext->OMSetRenderTargets(1, &renderTargetView, depthStencilView);//绑定
}

void D3D::RenderToBackBuffer()
{
	ID3D11Texture2D* backBuffer;
	HRESULT hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),reinterpret_cast<void**>(&backBuffer));
	if (S_OK != hr)
	{
		MessageBox(0, L"获取 backbuffer 失败!", 0, 0);
	}

	CreateDepthStencilBuffer();

	BindRenderTarget(backBuffer, depthStencilBuffer,true);

	SafeRelease(backBuffer);
}

void D3D::RenderToTexture(ID3D11Texture2D* renderTexture)
{
	CreateDepthStencilBuffer();

	BindRenderTarget(renderTexture, depthStencilBuffer,false);

	//SafeRelease(renderTexture);
}

void D3D::CreateDepthStencilBuffer()
{
	if (nullptr!=depthStencilBuffer)
	{
		SafeRelease(depthStencilBuffer);
	}

	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = d3DW;
	depthStencilDesc.Height = d3DH;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	// 使用4X MSAA?——必须与交换链的MSAA的值匹配
	if (enable4xMsaa)
	{
		depthStencilDesc.SampleDesc.Count = 4;
		depthStencilDesc.SampleDesc.Quality = m4xMsaaQuality - 1;
	}
	//  不使用MSAA
	else
	{
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	d3dDevice->CreateTexture2D(&depthStencilDesc, 0, &depthStencilBuffer);
}

void D3D::ClearBuffer(const FLOAT* color)
{
	d3dContext->ClearRenderTargetView(renderTargetView, color);
	d3dContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}