#pragma once
#include "Window.h"
//#include "Kinect.h"
#include "Graphic.h"
#include "Timer.h"


class App
{
private:

	int appW;//Ӧ�ó�����
	int appH;//Ӧ�ó���߶�
	std::wstring appName;//Ӧ�ó�������

	Graphic* pGraphic;//Graphic���

	static App* pApp;

	App() :appW(800), appH(600), appName(L"D3D_DEMO"), pGraphic(nullptr){}
	~App();

	//��ʼ�������
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

	void Run();//��ѭ��
};

extern HINSTANCE AppInstance();