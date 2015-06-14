#pragma once

#include <dinput.h>

/**
 * �κ�ִ�������ڵĶ��󶼿��Խ����������
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
	 * ʹ�ô˽���MouseReceiver��Ϊ��ǰ���뽹��
	 */
	void SetReceiver( iMouseReceiver* pTarget );

	void Update();

	void Acquire();
	void UnAcquire();

	DIMOUSESTATE GetLastState(){ return m_lastState; }
};