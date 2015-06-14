#include "MyApp.h"
#include "InputLayer.h"
#include <sstream>
//#include <conio.h>//����̨���

App* App::pApp = nullptr;

HINSTANCE appInstance;

HINSTANCE AppInstance()
{
	return appInstance;
}

//�������
int APIENTRY WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR     lpCmdLine,
	int       nCmdShow)
{
	appInstance = hInstance;

	App::GetApp();//����Ӧ�ó���

	App::GetApp()->InitApp();

	//AllocConsole();//�򿪸����ƴ���������������Ϣ
	//freopen("CONOUT$","w", stdout);

	App::GetApp()->Run();

	return 0;
}

App::~App()
{
	//FreeConsole();//�رտ���̨
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
	//����Ϣѭ����Ҳ����Ϸ���е���ѭ��
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

		}
		else
		{
			//�����豸����
			Input()->UpdateDevices();
			//����������Ⱦ���߼�������
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