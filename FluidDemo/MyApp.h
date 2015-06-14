#pragma once
#include "Window.h"
//#include "Kinect.h"
#include "Graphic.h"
#include "Timer.h"


class App
{
private:

	int appW;//应用程序宽度
	int appH;//应用程序高度
	std::wstring appName;//应用程序名称

	Graphic* pGraphic;//Graphic组件

	static App* pApp;

	App() :appW(800), appH(600), appName(L"D3D_DEMO"), pGraphic(nullptr){}
	~App();

	//初始化各组件
	void InitWindow();
	void InitGraphic();

	void CalculateFPS(Timer& timer);

public:
	
	
	static App* GetApp()
	{
		if (nullptr==pApp)
		{
			return pApp = new App();
		}
		return pApp;
	}

	void InitApp();

	void Run();//主循环
};

extern HINSTANCE AppInstance();