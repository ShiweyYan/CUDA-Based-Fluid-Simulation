#pragma once
#include <Windows.h>
#include <string>

class Window
{
private:

	int windowW;//���ڿ��
	int windowH;//���ڸ߶�
	HWND winHwnd;//���ھ��
	std::wstring winName;//��������

	static Window* pWindow;//��������

	void RegisterClass(WNDCLASSEX* pWc = NULL);//ע�ᴰ�ڣ���д����������Ϣ

public:

	Window(int w, int h,const TCHAR* title);

	void InitInstance();//��ʼ������ʵ����ע�Ტ��ʾ����

	static Window* getWindow(){ return pWindow; }//��ȡ��������

	HWND GetHwnd(){ return winHwnd; }//��ȡ���ھ��

	LRESULT WndProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam);//windows��Ϣ����
};

//��ȡ������
inline Window* MainWindow()
{
	return Window::getWindow();
}