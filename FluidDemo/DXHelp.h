#pragma once
#include <xnamath.h>
#include <DxErr.h>

namespace Colors
{
	const FLOAT White[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
	const FLOAT Black[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	const FLOAT Red[4] = { 1.0f, 0.0f, 0.0f, 1.0f };
	const FLOAT Green[4] = { 0.0f, 1.0f, 0.0f, 1.0f };
	const FLOAT Blue[4] = { 0.0f, 0.0f, 1.0f, 1.0f };
	const FLOAT Yellow[4] = { 1.0f, 1.0f, 0.0f, 1.0f };
	const FLOAT Cyan[4] = { 0.0f, 1.0f, 1.0f, 1.0f };
	const FLOAT Magenta[4] = { 1.0f, 0.0f, 1.0f, 1.0f };

	const FLOAT Silver[4] = { 0.75f, 0.75f, 0.75f, 1.0f };
	const FLOAT LightSteelBlue[4] = { 0.69f, 0.77f, 0.87f, 1.0f };
}

template<typename T>
void SafeRelease(T& COM)
{
	if (nullptr != COM)
	{
		COM->Release();
		COM = nullptr;
	}
}

template<typename T>
void SafeDelete(T& p)
{
	if (nullptr != p)
	{
		delete p;
		p = nullptr;
	}
}

inline void HR(HRESULT hResult)
{
	if (S_OK != hResult)
	{
		DXTrace(__FILE__, (DWORD)__LINE__, hResult, nullptr, true);
	}
}

//----------------------------------------------------------------------
//用来获取一个给定范围内的随机数，默认范围是0到1
//----------------------------------------------------------------------
inline float RandFloat(float min = 0.f, float max = 1.f)
{
	return min + (max - min)*((float)rand() / RAND_MAX);
}