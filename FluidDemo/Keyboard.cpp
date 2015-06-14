#include "InputLayer.h"
#include "Keyboard.h"
#include "DXHelp.h"
#include <assert.h>
#include <stack>
//using namespace std;




Keyboard::Keyboard( HWND hWnd )
{
	m_pTarget = NULL;
	
	HRESULT hr;

	/**
	 * 创建键盘设备
     */
	HR(Input()->GetDInput()->CreateDevice(GUID_SysKeyboard, &m_pDevice, NULL));
 
	/**
	 * 设置键盘数据格式
	 */
    HR(m_pDevice->SetDataFormat(&c_dfDIKeyboard)); 
 
    /**
	 * 设置合作等级 
	 */
    HR(m_pDevice->SetCooperativeLevel(hWnd,DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)); 

	memset( m_keyState, 0, 256*sizeof(bool) );	
}

Keyboard::~Keyboard()
{
	if( m_pDevice )
	{
		m_pDevice->Unacquire();//释放对设备的占有
		SafeRelease( m_pDevice );
	}
}



void Keyboard::SetReceiver( iKeyboardReceiver* pTarget )
{
	//设置新的目标
	m_pTarget = pTarget;
}


bool Keyboard::Poll( int key )
{
	if( m_keyState[key] & 0x80 )
		return true;
	return false;
}


void Keyboard::Update()
{
	BYTE     newState[256]; 
    HRESULT  hr; 
 
    hr = m_pDevice->Poll(); //通过轮询的方式更新设备数据
    hr = m_pDevice->GetDeviceState(sizeof(newState),(LPVOID)&newState); 

    if( FAILED(hr) )
    { 
		hr = m_pDevice->Acquire();
		if (FAILED(hr))
		{
			return;
		}

	    hr = m_pDevice->Poll(); 
	    hr = m_pDevice->GetDeviceState(sizeof(newState),(LPVOID)&newState); 
		if( FAILED( hr ) )
		{
			return;
		}
    } 


	if( m_pTarget )
	{
		int i;
		for( i=0; i< 256; i++ )
		{
#pragma  region //处理状态变化的情况
			
			if( m_keyState[i] != newState[i] )
			{
				// 某个按键的状态发生了变化
				if( !(newState[i] & 0x80) )
				{
					// 按键弹起
					m_pTarget->KeyUp( i );
				}
				else
				{
					//按下状态，如果处理了弹起就不要处理按下（除非特殊状态），因为会处理两次
				}
			}
			// 把现在的状态保存起来（现在它已经是旧的状态了）
			m_keyState[i] = newState[i];
#pragma  endregion

#pragma region//处理“按住”这个状态
			if( Poll( i ) )
			{
				m_pTarget->KeyDown( i );
			}
#pragma endregion
		}
	}
	else
	{
		//用新的状态覆盖
		memcpy( m_keyState, newState, 256 );
	}

}


