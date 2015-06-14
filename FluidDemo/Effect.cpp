#include "Effect.h"
#include "DXHelp.h"
#include <D3DX11.h>
#include <DxErr.h>


Effect::Effect(ID3D11Device* device, std::wstring effectName) :d3dDevice(device)
{
	CompileEffect(effectName);
}

Effect::~Effect()
{
	SafeRelease(effect);

	for (auto& i : inputLayoutManager)
	{
		SafeRelease(i.second);
	}
}

void Effect::CompileEffect(std::wstring effectName)
{
	DWORD shaderFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
	ID3D10Blob * compiledShader = 0;
	ID3D10Blob * compilationMsgs = 0;
	HRESULT hr = D3DX11CompileFromFile(effectName.c_str(), 0,
		0, 0, "fx_5_0", shaderFlags,
		0, 0, &compiledShader, &compilationMsgs, 0);

	//  compilationMsgs包含错误或警告的信息
	if (compilationMsgs != 0)
	{
		MessageBoxA(0, (char*)compilationMsgs->GetBufferPointer(), 0, 0);
		SafeRelease(compilationMsgs);
	}

	// 就算没有compilationMsgs，也需要确保没有其他错误
	if (FAILED(hr))
	{
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX11Compile FromFile", true);
	}

	D3DX11CreateEffectFromMemory(
		compiledShader->GetBufferPointer(),
		compiledShader->GetBufferSize(),
		0, d3dDevice, &effect);

	// 编译完成释放资源
	SafeRelease(compiledShader);
}

void Effect::CreateInputLayout(D3D11_INPUT_ELEMENT_DESC vertexDesc[], int length, std::string techName)
{
	ID3DX11EffectTechnique* tech = effect->GetTechniqueByName(techName.c_str());
	D3DX11_PASS_DESC PassDesc;
	tech->GetPassByIndex(0)->GetDesc(&PassDesc);

	//SafeDelete(tech);

	ID3D11InputLayout* vertexLayout;
	d3dDevice->CreateInputLayout(vertexDesc, static_cast<UINT>(length),
		PassDesc.pIAInputSignature, PassDesc.IAInputSignatureSize,
		&vertexLayout);
	inputLayoutManager.insert({ techName, vertexLayout });

	//SafeRelease(vertexLayout);
}

ID3D11InputLayout* Effect::GetInputLayout(std::string techName)
{
	return inputLayoutManager[techName];
}