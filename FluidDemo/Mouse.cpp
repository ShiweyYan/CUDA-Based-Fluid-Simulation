#include "InputLayer.h"
#include "Mouse.h"
#include "DXHelp.h"

Mouse::Mouse(  HWND hWnd, bool bExclusive )
{
	m_pTarget = NULL;

	HRESULT hr;

	/**
	 * 创建设备
	 */
	HR(Input()->GetDInput()->CreateDevice( GUID_SysMouse, &m_pDevice, NULL ));

	/**
	 * 设置数据格式
	 */
	HR(m_pDevice->SetDataFormat(&c_dfDIMouse));

	/**
	 * 设置合作等级l
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
 
    hr = m_pDevice->Poll(); //通过轮询的方式更新设备数据
	hr = m_pDevice->GetDeviceState( sizeof(DIMOUSESTATE), (void*)&currState );

	//如果设备被抢占了
    if( FAILED(hr) )
    { 
		hr = m_pDevice->Acquire();//重新获取设备使用权
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
			//  button0处在被按下状态
			m_pTarget->MouseButtonDown( 0 );
		}
		if( currState.rgbButtons[1] & 0x80 )
		{
			// button1处在被按下状态
			m_pTarget->MouseButtonDown( 1 );
		}
		if( currState.rgbButtons[2] & 0x80 )
		{
			//  button2处在被按下状态
			m_pTarget->MouseButtonDown( 2 );
		}
		if( !(currState.rgbButtons[0] & 0x80) && (m_lastState.rgbButtons[0] & 0x80) )
		{
			// button0弹起
			m_pTarget->MouseButtonUp( 0 );
		}
		if( !(currState.rgbButtons[1] & 0x80) && (m_lastState.rgbButtons[1] & 0x80) )
		{
			// button1弹起
			m_pTarget->MouseButtonUp( 1 );
		}
		if( !(currState.rgbButtons[2] & 0x80) && (m_lastState.rgbButtons[2] & 0x80) )
		{
			// button2弹起
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
