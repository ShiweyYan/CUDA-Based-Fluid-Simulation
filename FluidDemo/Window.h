#pragma once
#include <Windows.h>
#include <string>

class Window
{
private:

	int windowW;//窗口宽度
	int windowH;//窗口高度
	HWND winHwnd;//窗口句柄
	std::wstring winName;//窗口名称

	static Window* pWindow;//单例窗口

	void RegisterClass(WNDCLASSEX* pWc = NULL);//注册窗口：填写窗口描述信息

public:

	Window(int w, int h,const TCHAR* title);

	void InitInstance();//初始化窗口实例：注册并显示窗口

	static Window* getWindow(){ return pWindow; }//获取单例窗口

	HWND GetHwnd(){ return winHwnd; }//获取窗口句柄

	LRESULT WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);//windows消息处理
};

//获取主窗口
inline Window* MainWindow()
{
	return Window::getWindow();
}