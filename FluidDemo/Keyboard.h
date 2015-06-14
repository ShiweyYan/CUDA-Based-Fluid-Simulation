#pragma once
#include <memory.h>
#include <dinput.h>

class InputLayer;

/*
 * 任何实现这个接口的对象都可以从键盘接收输入
 */
struct iKeyboardReceiver
{
	virtual void KeyUp( int key ){};
	virtual void KeyDown( int key ){};
};


class Keyboard  
{

	LPDIRECTINPUTDEVICE8	m_pDevice;	

	char					m_keyState[256];

	iKeyboardReceiver*		m_pTarget;

public:

	void ClearTable()
	{
		memset( m_keyState, 0, sizeof(char)*256 );
	}

	Keyboard( HWND hWnd );
	~Keyboard();

	// 看是否真的有一个键被按下
	bool Poll( int key );

	// 通过这个函数建立一个键盘接收者
	void SetReceiver( iKeyboardReceiver* pTarget );

	void Update();
};
