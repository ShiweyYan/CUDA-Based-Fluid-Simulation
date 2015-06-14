#pragma once

#include <D3D11.h>
#include <D3DX11.h>
#include <DXGI.h>

#include <xnamath.h>
#include <vector>

#include "DXHelp.h"


class D3D
{
private:
	ID3D11Device* d3dDevice;
	ID3D11DeviceContext* d3dContext;
	IDXGISwapChain* swapChain;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11RasterizerState* rasterizeState;

	ID3D11RenderTargetView* renderTargetView;
	ID3D11Texture2D* depthStencilBuffer;
	ID3D11DepthStencilView* depthStencilView;

	bool enable4xMsaa;//是否使用4x的MSAA
	UINT  m4xMsaaQuality;//多重采样的质量

	int d3DW;
	int d3DH;

	void CreateDeviceAndContext();
	void CreateSwapChain();
	void CreateDepthStencilBuffer();
	void SetViewPort();
	void SetRasterizerState();

	void BindRenderTarget(ID3D11Texture2D* renderTexture, ID3D11Texture2D* depthTexture,bool isBackBuffer);

public:
	D3D(int w,int h);
	~D3D();
	void InitD3D();
	
	void BindInputLayout(ID3D11InputLayout* inputLayout);
	void BindVertexBuffer(ID3D11Buffer* VB, UINT vertexSize);
	void BindInstanceBuffer(ID3D11Buffer* InstanceBuffer, UINT instanceSize);
	void BindIndexBuffer(ID3D11Buffer* IB);
	void SetTopology(D3D_PRIMITIVE_TOPOLOGY topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	void RenderToBackBuffer();
	void RenderToTexture(ID3D11Texture2D* renderTexture);//这个项目应该只会更改渲染目标视图，所以就只给一个参数
	void ClearBuffer(const FLOAT* color = Colors::Black);//清除帧缓存和深度模板缓存

	ID3D11Device* GetD3DDevice(){ return d3dDevice; }
	ID3D11DeviceContext* GetDeviceContext(){ return d3dContext; }
	IDXGISwapChain* GetSwapChain(){ return swapChain; }
	ID3D11RenderTargetView* GetRenderTargetView(){ return renderTargetView; }
	ID3D11DepthStencilView* GetDepthStencilView(){ return depthStencilView; }
};