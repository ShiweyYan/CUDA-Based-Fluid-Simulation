#pragma once

#include <d3dx11effect.h>
//#include <vector>
#include <map>
#include <string>

class Effect
{
private:
	ID3D11Device* d3dDevice;

	ID3DX11Effect* effect;

	//std::vector<ID3D11InputLayout*> vertexLayoutVector;
	std::map<std::string, ID3D11InputLayout*> inputLayoutManager;

	void CompileEffect(std::wstring effectName);

public:
	Effect(ID3D11Device* d3dDevice, std::wstring effectName);
	~Effect();
	Effect(Effect& effect) = delete;

	void CreateInputLayout(D3D11_INPUT_ELEMENT_DESC vertexDesc[],int length,std::string techName);

	ID3D11InputLayout* GetInputLayout(std::string techName);

	ID3DX11Effect* GetEffect() const{ return effect; }
};