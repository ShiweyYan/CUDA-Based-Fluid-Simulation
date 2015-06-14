#pragma once

#include <dinput.h>

/**
 * 任何执行这个借口的对象都可以接收鼠标输入
 */
struct iMouseReceiver
{
	virtual void MouseMoved( int dx, int dy ){};
	virtual void MouseButtonUp( int button ){};
	virtual void MouseButtonDown( int button ){};
};


class Mouse  
{
	LPDIRECTINPUTDEVICE8	m_pDevice; 

	DIMOUSESTATE			m_lastState;

	iMouseReceiver*			m_pTarget;

public:

	Mouse( HWND hWnd, bool bExclusive );
	~Mouse();

	/**
	 * 使用此建立MouseReceiver作为当前输入焦点
	 */
	void SetReceiver( iMouseReceiver* pTarget );

	void Update();

	void Acquire();
	void UnAcquire();

	DIMOUSESTATE GetLastState(){ return m_lastState; }
};