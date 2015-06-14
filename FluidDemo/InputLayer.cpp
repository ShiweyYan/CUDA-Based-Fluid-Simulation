#include "InputLayer.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "DXHelp.h"
#include <assert.h>

InputLayer* InputLayer::m_pGlobalILayer = NULL;

InputLayer::InputLayer( 
	HINSTANCE hInst, 
	HWND hWnd, 
	bool bExclusive, 
	bool bUseKeyboard, 
	bool bUseMouse )
{


	m_pKeyboard = NULL;
	m_pMouse = NULL;

	if( m_pGlobalILayer )
	{
		assert(false);
	}
	m_pGlobalILayer = this;

	HRESULT hr;

	/**
	 * 创建DI8 对象
	 */
	HR(DirectInput8Create( hInst, DIRECTINPUT_VERSION, 
						IID_IDirectInput8, (void**)&m_pDI, NULL ));

	if( bUseKeyboard )
	{
		m_pKeyboard = new Keyboard( hWnd );
	}
	if( bUseMouse )
	{
		m_pMouse = new Mouse( hWnd, bExclusive );
	}

}


InputLayer::~InputLayer()
{
	if( m_pDI )
	{
		if( m_pKeyboard )
		{
			SafeDelete(m_pKeyboard); 
		}

		if( m_pMouse )
		{
			SafeDelete(m_pMouse);
		}
		SafeRelease( m_pDI );
	}
	m_pGlobalILayer = NULL;
}

void InputLayer::UpdateDevices()
{
	if( m_pKeyboard )
	{
		m_pKeyboard->Update();
	}
	if( m_pMouse )
	{
		m_pMouse->Update();
	}
}


void InputLayer::SetFocus()
{
	if( m_pKeyboard )
	{
		m_pKeyboard->ClearTable();
	}
	if( m_pMouse )
	{
		m_pMouse->Acquire();
	}
}


void InputLayer::KillFocus()
{
	if( m_pKeyboard )
	{
		m_pKeyboard->ClearTable();
	}
	if( m_pMouse )
	{
		m_pMouse->UnAcquire();
	}
}
