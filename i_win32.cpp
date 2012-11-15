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



// i_win32.c
// input through normal windows
#include "interfaces.h"
#include "r_private.h"

extern char i_vktochar[];

void I_InitVKMap( void );

keyevent_t	keventlist[128];
unsigned int	keventlistptr;
int		md_x, md_y;

static int	type;
static int	isgrabed;
static char	shiftmap[256];

void I_Win32Update();

void I_StartUp()
{
	int	i;
	isgrabed = 0;

	
	for( i = 0; i < 256; i++ )
		shiftmap[i] = 0;

	shiftmap['1'] = '!';
	shiftmap['2'] = '@';
	shiftmap['3'] = '#';
	shiftmap['4'] = '$';
	shiftmap['5'] = '%';
	shiftmap['6'] = '^';
	shiftmap['7'] = '&';
	shiftmap['8'] = '*';
	shiftmap['9'] = '(';
	shiftmap['0'] = ')';
	shiftmap['-'] = '_';

	I_InitVKMap();
}

void I_ShutDown()
{
//	ReleaseCapture();
}

void I_GrabMouse( int flag )
{
	switch( flag )
	{
	case 1:
		if( isgrabed )
		{
			__warning( "mouse is already grabed\n" );
			break;
		}
		isgrabed = 1;
		SetCapture( g_hWnd );
		break;

	case 0:
		if( !isgrabed )
		{
			__warning( "mouse is not grabed\n" );
			break;
		}
		isgrabed = 0;
		ReleaseCapture();
		break;
	}
}

void I_Update()
{
	I_Win32Update();
}

int I_Win32TryVK( int vk, int *isshift )
{
	int	gsksym;
       
	switch( vk )
	{

	case VK_LBUTTON:
		gsksym = GSK_BUTTON1;
		break;


	case VK_MBUTTON:
		gsksym = GSK_BUTTON2;
		break;

	case VK_RBUTTON:
		gsksym = GSK_BUTTON3;
		break;
		
	case VK_UP:
		gsksym = GSK_CUP;
		break;

	case VK_DOWN:
		gsksym = GSK_CDOWN;
		break;

	case VK_LEFT:
		gsksym = GSK_CLEFT;
		break;

	case VK_RIGHT:
		gsksym = GSK_CRIGHT;
		break;
		
	case VK_SHIFT:
		gsksym = GSK_SHIFT;
		*isshift = 1;
		break;

	case VK_CONTROL:
		gsksym = GSK_CTRL;
		break;

	case VK_RETURN:
		gsksym = '\n';
		break;

	case VK_ESCAPE:
		gsksym = GSK_ESCAPE;
		break;

	case VK_TAB:
		gsksym = GSK_TAB;
		break;

	case VK_F1:
		gsksym = GSK_F1;
		break;

	case VK_F2:
		gsksym = GSK_F2;
		break;

	case VK_F3:
		gsksym = GSK_F3;
		break;

	case VK_F4:
		gsksym = GSK_F4;
		break;

	case VK_F5:
		gsksym = GSK_F5;
		break;

	case VK_F6:
		gsksym = GSK_F6;
		break;

	case VK_F7:
		gsksym = GSK_F7;
		break;

	case VK_F8:
		gsksym = GSK_F8;
		break;

	case VK_F9:
		gsksym = GSK_F9;
		break;

	case VK_F10:
		gsksym = GSK_F10;
		break;

	case VK_F11:
		gsksym = GSK_F11;
		break;

	case VK_F12:
		gsksym = GSK_F12;
		break;
	
	default:
		return 1;
	}

	keventlist[keventlistptr].sym = gsksym;
	keventlist[keventlistptr].type = type;
	keventlistptr++;
	return 0;
}

void I_Win32DoMouse()
{
	POINT	point;
	int	x, y;


	if( isgrabed )
	{
		GetCursorPos( &point );
		x = point.x - g_winrect.left;
		y = point.y - g_winrect.top;
		
		md_x = x - r_devicewidth->ivalue/2;
		md_y = y - r_deviceheight->ivalue/2;
		//	printf( "mdx: %d mdy: %d\n", md_x, md_y );

		SetCursorPos( g_winrect.left + r_devicewidth->ivalue/2, g_winrect.top + r_deviceheight->ivalue/2 );
	} else
	{
		x = y = 0;
	}
}

static i_scantosyms[128] = {
	0, GSK_ESCAPE, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
	'-', '=', 8, 9, 
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']',
	GSK_ENTER, GSK_CTRL, 
	'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', 59, 47, 96,
	GSK_SHIFT, 92,
	'z', 'x', 'c', 'v', 'b', 'n', 'm', 44, 46, 47,
	GSK_SHIFT, 42, GSK_ALT, 32, 0, 
	GSK_F1, GSK_F2, GSK_F3, GSK_F4, GSK_F5, GSK_F6, GSK_F7, GSK_F8, GSK_F9, GSK_F10,
	0, 0, GSK_HOME, GSK_CUP, GSK_PGUP, '-', GSK_CLEFT, '5', GSK_CRIGHT, '+',
	GSK_END, GSK_CDOWN, GSK_PGDN, GSK_INSERT, GSK_DEL, 0, 0, 0,
	GSK_F11, GSK_F12, 0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0	
};


static int ScanCodeToSym( int scan )
{
	scan = ( scan >> 16 ) & 0xff;
	if ( scan < 128 )
	{
		return i_scantosyms[scan];
	}

	return 0;
}

void KeyEvent( int sym, int type )
{
	keventlist[keventlistptr].sym = sym;
	keventlist[keventlistptr].type = type;
	keventlistptr++;	
}

		
void I_Win32Update()
{
	MSG	msg;
	int	isshift = 0;
	int	upfake;

	
	keventlistptr = 1;
	for( ;; )
	{
	       
		if( !PeekMessage( &msg, g_hWnd, 0, 0, PM_REMOVE ) )
		{
		  //			printf( "no more events\n" );
			break;
		}
		switch( msg.message )
		{
		case WM_SYSCHAR:
			break;
			
		case WM_KEYDOWN:
		case WM_SYSKEYDOWN:

			type = SYMTYPE_PRESS;
			upfake = 0;

#if 0
//			SHV_Printf( "vk: %c\n", i_vktochar[msg.wParam] );
			if( I_Win32TryVK( ( int )msg.wParam, &isshift ))
			{
				if( ( int ) msg.wParam > 255 )
					break;

				keventlist[keventlistptr].sym = i_vktochar[( int ) msg.wParam];
				if( isshift && shiftmap[i_vktochar[(int)msg.wParam]] )
					keventlist[keventlistptr].sym = ( int )shiftmap[i_vktochar[(int)msg.wParam]];
				
				keventlist[keventlistptr].type = SYMTYPE_PRESS;
				keventlistptr++;

			}
#else
			{
				int sym;
				
				sym = ScanCodeToSym( msg.lParam );
				KeyEvent( sym, SYMTYPE_PRESS );
			}
#endif
			break;
			
		case WM_KEYUP:
		case WM_SYSKEYUP:
#if 0
			type = SYMTYPE_RELEASE;
			if( I_Win32TryVK( ( int )msg.wParam, &isshift ) )
			{
				if( ( int ) msg.wParam > 255 )
					break;

				keventlist[keventlistptr].sym = i_vktochar[( int ) msg.wParam];
				if( isshift && shiftmap[i_vktochar[(int)msg.wParam]] )
				keventlist[keventlistptr].sym = ( int )shiftmap[i_vktochar[(int)msg.wParam]];
				
				keventlist[keventlistptr].type = SYMTYPE_RELEASE;
				keventlistptr++;


			}
#else
			{
				int sym;
				
				sym = ScanCodeToSym( msg.lParam );
				KeyEvent( sym, SYMTYPE_RELEASE );			
			}
#endif
			break;
			


		case WM_LBUTTONDOWN:
			keventlist[keventlistptr].sym = GSK_BUTTON1;
			keventlist[keventlistptr].type = SYMTYPE_PRESS;
			keventlistptr++;
			break;
			
		case WM_LBUTTONUP:
			keventlist[keventlistptr].sym = GSK_BUTTON1;
			keventlist[keventlistptr].type = SYMTYPE_RELEASE;
			keventlistptr++;
			break;

		case WM_RBUTTONDOWN:
			keventlist[keventlistptr].sym = GSK_BUTTON3;
			keventlist[keventlistptr].type = SYMTYPE_PRESS;
			keventlistptr++;
			break;
			
		case WM_RBUTTONUP:
			keventlist[keventlistptr].sym = GSK_BUTTON3;
			keventlist[keventlistptr].type = SYMTYPE_RELEASE;
			keventlistptr++;
			break;

		case WM_MBUTTONDOWN:
			keventlist[keventlistptr].sym = GSK_BUTTON2;
			keventlist[keventlistptr].type = SYMTYPE_PRESS;
			keventlistptr++;
			break;
			
		case WM_MBUTTONUP:
			keventlist[keventlistptr].sym = GSK_BUTTON2;
			keventlist[keventlistptr].type = SYMTYPE_RELEASE;
			keventlistptr++;
			break;


	
		default:
		  //			printf( "unhandeled message\n" );
			DispatchMessage( &msg );
			break;
		}
	}
	I_Win32DoMouse();
	keventlist[0].sym = keventlistptr;
}
