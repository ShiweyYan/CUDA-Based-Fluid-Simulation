#pragma once
#include <memory.h>
#include <dinput.h>

class InputLayer;

/*
 * �κ�ʵ������ӿڵĶ��󶼿��ԴӼ��̽�������
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

	// ���Ƿ������һ����������
	bool Poll( int key );

	// ͨ�������������һ�����̽�����
	void SetReceiver( iKeyboardReceiver* pTarget );

	void Update();
};
