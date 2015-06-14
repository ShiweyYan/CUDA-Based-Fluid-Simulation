#include "MyApp.h"
#include "InputLayer.h"
#include <sstream>
//#include <conio.h>//控制台相关

App* App::pApp = nullptr;

HINSTANCE appInstance;

HINSTANCE AppInstance()
{
	return appInstance;
}

//程序入口
int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	appInstance = hInstance;

	App::GetApp();//构造应用程序

	App::GetApp()->InitApp();

	//AllocConsole();//打开个控制带，用来看调试信息
	//freopen("CONOUT$","w", stdout);

	App::GetApp()->Run();

	return 0;
}

App::~App()
{
	//FreeConsole();//关闭控制台
	SafeDelete(pGraphic);
	if (nullptr!=MainWindow())
	{
		delete MainWindow();
	}
}

void App::InitApp()
{
	InitWindow();
	InputLayer::Create(AppInstance(), MainWindow()->GetHwnd(), true);
	InitGraphic();
}



void App::InitWindow()
{
	new Window(appW, appH,appName.c_str());
	MainWindow()->InitInstance();

}

void App::InitGraphic()
{
	pGraphic = new Graphic(appW, appH);
	pGraphic->InitGraphic();
}

void App::Run()
{
	Timer timer;
	timer.Reset();

	MSG msg = { 0 };
	//主消息循环，也是游戏当中的主循环
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}
		else
		{
			//输入设备更新
			Input()->UpdateDevices();
			//在这里做渲染，逻辑等任务
			pGraphic->Frame();
		}
		CalculateFPS(timer);
	}
}

void App::CalculateFPS(Timer& timer)
{
	static int frameCount = 0;
	static double time = 0.0;
	timer.Tick();
	++frameCount;
	time += timer.DeltaTime();
	if (time>1.0)
	{
		std::wostringstream titleString;
		titleString << L" FPS : " << frameCount;
		SetWindowText(MainWindow()->GetHwnd(),(appName+titleString.str()).c_str() );
		time = 0.0;
		frameCount = 0;
	}
}