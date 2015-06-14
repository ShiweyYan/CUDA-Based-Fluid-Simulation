#include "InputLayer.h"
#include "Mouse.h"
#include "DXHelp.h"

Mouse::Mouse(  HWND hWnd, bool bExclusive )
{
	m_pTarget = NULL;

	HRESULT hr;

	/**
	 * �����豸
	 */
	HR(Input()->GetDInput()->CreateDevice( GUID_SysMouse, &m_pDevice, NULL ));

	/**
	 * �������ݸ�ʽ
	 */
	HR(m_pDevice->SetDataFormat(&c_dfDIMouse));

	/**
	 * ���ú����ȼ�l
	 */
	if( bExclusive )
	{
		HR( m_pDevice->SetCooperativeLevel( hWnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND ));
	}
	else
	{
		HR(m_pDevice->SetCooperativeLevel( hWnd, DISCL_NONEXCLUSIVE | DISCL_FOREGROUND));
	}

	m_lastState.lX = 0;
	m_lastState.lY = 0;
	m_lastState.lZ = 0;
	m_lastState.rgbButtons[0] = 0;
	m_lastState.rgbButtons[1] = 0;
	m_lastState.rgbButtons[2] = 0;
	m_lastState.rgbButtons[3] = 0;
}

Mouse::~Mouse()
{
	if( m_pDevice )
	{
		m_pDevice->Unacquire();
		SafeRelease( m_pDevice );
	}
}


void Mouse::SetReceiver( iMouseReceiver* pTarget )
{
	m_pTarget = pTarget;
}


void Mouse::Update()
{
	DIMOUSESTATE currState;
    HRESULT  hr; 
 
    hr = m_pDevice->Poll(); //ͨ����ѯ�ķ�ʽ�����豸����
	hr = m_pDevice->GetDeviceState( sizeof(DIMOUSESTATE), (void*)&currState );

	//����豸����ռ��
    if( FAILED(hr) )
    { 
		hr = m_pDevice->Acquire();//���»�ȡ�豸ʹ��Ȩ
		if (S_OK!=hr)
		{
			return;
		}
	    hr = m_pDevice->Poll(); 
	    hr= m_pDevice->GetDeviceState( sizeof(DIMOUSESTATE),(void*)&currState ); 
		if (S_OK != hr)
		{
			return;
		}
    } 
	if( m_pTarget )
	{
		int dx = currState.lX;
		int dy = currState.lY;
		if( dx || dy )
		{
			m_pTarget->MouseMoved( dx, dy );
		}
		if( currState.rgbButtons[0] & 0x80 )
		{
			//  button0���ڱ�����״̬
			m_pTarget->MouseButtonDown( 0 );
		}
		if( currState.rgbButtons[1] & 0x80 )
		{
			// button1���ڱ�����״̬
			m_pTarget->MouseButtonDown( 1 );
		}
		if( currState.rgbButtons[2] & 0x80 )
		{
			//  button2���ڱ�����״̬
			m_pTarget->MouseButtonDown( 2 );
		}
		if( !(currState.rgbButtons[0] & 0x80) && (m_lastState.rgbButtons[0] & 0x80) )
		{
			// button0����
			m_pTarget->MouseButtonUp( 0 );
		}
		if( !(currState.rgbButtons[1] & 0x80) && (m_lastState.rgbButtons[1] & 0x80) )
		{
			// button1����
			m_pTarget->MouseButtonUp( 1 );
		}
		if( !(currState.rgbButtons[2] & 0x80) && (m_lastState.rgbButtons[2] & 0x80) )
		{
			// button2����
			m_pTarget->MouseButtonUp( 2 );
		}
	}
	m_lastState = currState;
}


void Mouse::Acquire()
{
	HR(m_pDevice->Acquire());
}


void Mouse::UnAcquire()
{
	HR(m_pDevice->Unacquire());
}
