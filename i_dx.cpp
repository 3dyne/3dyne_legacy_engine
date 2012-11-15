/* 
 * 3dyne Legacy Engine GPL Source Code
 * 
 * Copyright (C) 1996-2012 Matthias C. Berger & Simon Berger.
 * 
 * This file is part of the 3dyne Legacy Engine GPL Source Code ("3dyne Legacy
 * Engine Source Code").
 *   
 * 3dyne Legacy Engine Source Code is free software: you can redistribute it
 * and/or modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 3 of the License,
 * or (at your option) any later version.
 * 
 * 3dyne Legacy Engine Source Code is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along with
 * 3dyne Legacy Engine Source Code.  If not, see
 * <http://www.gnu.org/licenses/>.
 * 
 * In addition, the 3dyne Legacy Engine Source Code is also subject to certain
 * additional terms. You should have received a copy of these additional terms
 * immediately following the terms and conditions of the GNU General Public
 * License which accompanied the 3dyne Legacy Engine Source Code.
 * 
 * Contributors:
 *     Matthias C. Berger (mcb77@gmx.de) - initial API and implementation
 *     Simon Berger (simberger@gmail.com) - initial API and implementation
 */



// i_dx.c
#include "interfaces.h"
#include "r_private.h"


keyevent_t	keventlist[128];
unsigned int	keventlistptr;

static LPDIRECTINPUT  lpdi;
static LPDIRECTINPUTDEVICE   lpdiMouse;
static LPDIRECTINPUTDEVICE   lpdiKeyboard;


static DIMOUSESTATE	diMouseState;

static GUID	MouseGUID;
static GUID	KeyboardGUID;
static BOOL           MouseFound;
static BOOL           KeyboardFound;

static BYTE	diOldKeys[256];

int	md_x, md_y;

BOOL CALLBACK DIEnumDevicesProc( LPCDIDEVICEINSTANCE lpddi,LPVOID pvRef)
{
	
	if (GET_DIDEVICE_TYPE(lpddi->dwDevType) == DIDEVTYPE_MOUSE)
	{
		MouseGUID = lpddi->guidProduct;   
		MouseFound = TRUE;
	}
	if (GET_DIDEVICE_TYPE(lpddi->dwDevType) == DIDEVTYPE_KEYBOARD)
	{  
		KeyboardGUID = lpddi->guidProduct;   
		KeyboardFound = TRUE;
	}
	if (MouseFound && KeyboardFound)
		return DIENUM_STOP;
	else
		return DIENUM_CONTINUE;  
} // DIEnumDevicesProc


void I_Update()
{
	I_DXUpdate();
}

void I_DXUpdate()
{
	BYTE	diKeys[256];
	BYTE	diCurKeys[256];

	int	i;

	md_x = md_y = 0;
	keventlistptr = 1;
	keventlist[0].sym = keventlistptr;

// ==============================
// keyboard

	if( !lpdiKeyboard )
	{
		__error( "no mouse object\n" );
	}

	if( lpdiKeyboard->lpVtbl->GetDeviceState( lpdiKeyboard, 256, &diCurKeys ) != DI_OK )
	{
		__error( "GetDeviceState failed\n" );
	}
	for( i = 0; i < 256; i++ )
	{
		
		diKeys[i] = diCurKeys[i] ^ diOldKeys[i];

		if( diKeys[i] & 0x80 )
		{
			keventlist[keventlistptr].sym=i_dxscans[i];
			if( diCurKeys[i] )
				keventlist[keventlistptr].type = SYMTYPE_PRESS;
			else
				keventlist[keventlistptr].type = SYMTYPE_RELEASE;

			keventlistptr++;
		}
		
			
	//SHV_Printf( "key %d changed\n", i_dxscans[i] );
		
	}

	keventlist[0].sym = keventlistptr;

	memcpy( diOldKeys, diCurKeys, 256 );
#if 0
	if( diKeys[DIK_ESCAPE] & 0x80 )
		__error( "hard quit\n" );
#endif

// ==============================
// Mouse

	if( !lpdiMouse )
	{
		__error( "no keyboard object\n" );
	}
	
	if(lpdiMouse->lpVtbl->GetDeviceState( lpdiMouse, sizeof(diMouseState), &diMouseState)
	    != DI_OK )
	{
		__error( "Mouse: GetDeviceState failed\n" );
	}
	md_x = diMouseState.lX;
	md_y = diMouseState.lY;
}	

void I_StartUp()
{
	HRESULT	hr;

// create direct input

	lpdiKeyboard = NULL;
	lpdiMouse = NULL;

	hr = DirectInputCreate( g_wininstance, DIRECTINPUT_VERSION, &lpdi, NULL );

	if( FAILED( hr ))
	{
		__error( "DirectInputCreate failed\n" );
	}

	 hr = lpdi->lpVtbl->EnumDevices( lpdi, 0, 
					 DIEnumDevicesProc,
					 NULL, 
					 DIEDFL_ATTACHEDONLY);
	 
	 if( !KeyboardFound )
	 {
		 __error( "keyboard not found\n" );
	 }

	 if( !MouseFound )
	 {
		 __error( "Mouse not found\n" );
	 }

	 if( FAILED( hr ))
	 {
		 __error( "EnumDevices failed\n" );
	 }
		 


// ==============================
// Keyboard

// get keyboard device
	hr = lpdi->lpVtbl->CreateDevice( lpdi, &KeyboardGUID, &lpdiKeyboard, NULL );
	
	if( FAILED( hr ))
	{
		__error( "cannot create Keyboard device\n" );
	}

	// <microsoft>

	// Set the data format using the predefined keyboard data 
// format provided by the DirectInput object for keyboards. 
	hr = lpdiKeyboard->lpVtbl->SetDataFormat( lpdiKeyboard, &c_dfDIKeyboard); 
	if( FAILED(hr)) 
	{ 
		__error( "SetDataFormat failed\n" );
	} 
	
// Set the cooperative level 
	hr = lpdiKeyboard->lpVtbl->SetCooperativeLevel( lpdiKeyboard, g_hWnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE );
	if(FAILED(hr)) 
	{ 
		__error( "SetCooperativeLevel failed\n" );
	} 
	

// Get access to the input device. 
	hr = lpdiKeyboard->lpVtbl->Acquire( lpdiKeyboard ); 
	if( FAILED(hr)) 
	{ 
		__error( "cannot aquire keyboard\n" );
	} 
//	__named_message( "finished. object: %d\n", lpdiKeyboard );


	
// =========================
// Mouse

// get keyboard device
	hr = lpdi->lpVtbl->CreateDevice( lpdi, &MouseGUID, &lpdiMouse, NULL );
	
	if( FAILED( hr ))
	{
		__error( "cannot create Mouse device\n" );
	}

	// <microsoft>

	// Set the data format using the predefined keyboard data 
// format provided by the DirectInput object for keyboards. 
	hr = lpdiMouse->lpVtbl->SetDataFormat( lpdiMouse, &c_dfDIMouse); 
	if( FAILED(hr)) 
	{ 
		__error( "Mouse: SetDataFormat failed\n" );
	} 
	
// Set the cooperative level 
	hr = lpdiMouse->lpVtbl->SetCooperativeLevel( lpdiMouse, g_hWnd, DISCL_FOREGROUND | DISCL_EXCLUSIVE );
	if(FAILED(hr)) 
	{ 
		__error( "Mouse: SetCooperativeLevel failed\n" );
	} 
	

// Get access to the input device. 
	hr = lpdiMouse->lpVtbl->Acquire( lpdiMouse ); 
	if( FAILED(hr)) 
	{
		__error( "cannot aquire mouse\n" );
	} 
	
	memset( diOldKeys, 0, 256 );
	I_DXInitScanTab();
}


void I_ShutDown()
{

	if( lpdiKeyboard )
	{
		lpdiKeyboard->lpVtbl->Unacquire(lpdiKeyboard);
		lpdiKeyboard->lpVtbl->Release(lpdiKeyboard);
	}

	if( lpdiMouse )
	{
		lpdiMouse->lpVtbl->Unacquire(lpdiMouse);
		lpdiMouse->lpVtbl->Release(lpdiMouse);
	}
	
	if( lpdi )
	{
		lpdi->lpVtbl->Release(lpdi);
	}
		
}

unsigned char I_GetChar()
{
}

void I_SetInputMode( int mode )
{
}
