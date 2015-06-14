#pragma once

#include <dinput.h>
#include "Keyboard.h"
#include "Mouse.h"

class InputLayer
{
	Keyboard*		m_pKeyboard;
	Mouse*			m_pMouse;

	// DI8对象
	LPDIRECTINPUT8	m_pDI;
	
	static InputLayer* m_pGlobalILayer;

	InputLayer( 
		HINSTANCE hInst, 
		HWND hWnd, 
		bool bExclusive, 
		bool bUseKeyboard = true, 
		bool bUseMouse = true );

public:

	virtual ~InputLayer();

	Keyboard* GetKeyboard()
	{
		return m_pKeyboard;
	}

	Mouse* GetMouse()
	{
		return m_pMouse;
	}

	void UpdateDevices();

	static InputLayer* GetInput()
	{
		return m_pGlobalILayer;
	}

	LPDIRECTINPUT8 GetDInput()
	{
		return m_pDI; 
	}

	void SetFocus(); // 当应用获取焦点时调用
	void KillFocus(); // 当应用释放焦点时调用

	static void Create( 
		HINSTANCE hInst, 
		HWND hWnd, 
		bool bExclusive, 
		bool bUseKeyboard = true, 
		bool bUseMouse = true )
	{
		new InputLayer( 
			hInst, 
			hWnd, 
			bExclusive, 
			bUseKeyboard, 
			bUseMouse );
	}
};

inline InputLayer* Input()
{
	return InputLayer::GetInput();
}
