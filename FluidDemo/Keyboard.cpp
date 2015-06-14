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
	 * ���������豸
     */
	HR(Input()->GetDInput()->CreateDevice(GUID_SysKeyboard, &m_pDevice, NULL));
 
	/**
	 * ���ü������ݸ�ʽ
	 */
    HR(m_pDevice->SetDataFormat(&c_dfDIKeyboard)); 
 
    /**
	 * ���ú����ȼ� 
	 */
    HR(m_pDevice->SetCooperativeLevel(hWnd,DISCL_FOREGROUND | DISCL_NONEXCLUSIVE)); 

	memset( m_keyState, 0, 256*sizeof(bool) );	
}

Keyboard::~Keyboard()
{
	if( m_pDevice )
	{
		m_pDevice->Unacquire();//�ͷŶ��豸��ռ��
		SafeRelease( m_pDevice );
	}
}



void Keyboard::SetReceiver( iKeyboardReceiver* pTarget )
{
	//�����µ�Ŀ��
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
 
    hr = m_pDevice->Poll(); //ͨ����ѯ�ķ�ʽ�����豸����
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
#pragma  region //����״̬�仯�����
			
			if( m_keyState[i] != newState[i] )
			{
				// ĳ��������״̬�����˱仯
				if( !(newState[i] & 0x80) )
				{
					// ��������
					m_pTarget->KeyUp( i );
				}
				else
				{
					//����״̬����������˵���Ͳ�Ҫ�����£���������״̬������Ϊ�ᴦ������
				}
			}
			// �����ڵ�״̬�����������������Ѿ��Ǿɵ�״̬�ˣ�
			m_keyState[i] = newState[i];
#pragma  endregion

#pragma region//������ס�����״̬
			if( Poll( i ) )
			{
				m_pTarget->KeyDown( i );
			}
#pragma endregion
		}
	}
	else
	{
		//���µ�״̬����
		memcpy( m_keyState, newState, 256 );
	}

}


