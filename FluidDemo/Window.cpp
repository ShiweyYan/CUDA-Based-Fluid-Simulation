#include <assert.h>

#include "Window.h"
#include "MyApp.h"


Window* Window::pWindow = nullptr;

LRESULT CALLBACK GlobalWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	return MainWindow()->WndProc(hwnd,msg,wParam,lParam);
}

Window::Window(int w, int h,const TCHAR* title) :windowH(h), windowW(w), winHwnd(NULL)
{
	if (nullptr!=pWindow)
	{
		MessageBox(0, L"一个程序不能有两个窗口", 0, 0);
		assert(false);
	}
	pWindow = this;
	winName = std::wstring(title);
}

LRESULT Window::WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


void Window::RegisterClass(WNDCLASSEX* pWc)
{
	WNDCLASSEX wcex;
	if (pWc)
	{
		wcex = *pWc;
	}
	else
	{
		//填一些默认值
		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = GlobalWndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = AppInstance();
		wcex.hIcon = 0;
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = 0;
		wcex.lpszClassName = winName.c_str();
		wcex.hIconSm = 0;
	}

	if (0 == RegisterClassEx(&wcex))
	{
		MessageBox(0, L"窗口注册失败", 0, 0);
		assert(false);
	}
}

void Window::InitInstance()
{
	Window::RegisterClass(NULL);

	RECT rc;
	rc.left = 0;
	rc.top = 0;
	rc.right = windowW;
	rc.bottom = windowH;
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, FALSE);

	winHwnd = CreateWindow(
						(LPWSTR)winName.c_str(),
						(LPWSTR)winName.c_str(),
						WS_OVERLAPPEDWINDOW | WS_VISIBLE,
						0,
						0,
						rc.right - rc.left,
						rc.bottom - rc.top,
						NULL,
						NULL,
						AppInstance(),
						NULL);

	if (!winHwnd)
	{
		MessageBox(0, L"创建窗口失败", 0, 0);
		assert(false);
	}

	//绘制窗口
	ShowWindow(winHwnd, SW_SHOW);
	UpdateWindow(winHwnd);
}