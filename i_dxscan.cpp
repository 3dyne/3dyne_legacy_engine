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



// i_dxscan.c
#include "r_private.h"

unsigned char	i_dxscans[256];

#define tb i_dxscans
void I_DXInitScanTab( void )
{
	int	i;

	for ( i = 0; i < 256; i++ )
		tb[i] = 0;

	tb[DIK_ESCAPE] = GSK_ESCAPE;
	tb[DIK_1] = '1';
	tb[DIK_2] = '2';
	tb[DIK_3] = '3';
	tb[DIK_4] = '4';
	tb[DIK_5] = '5';
	tb[DIK_6] = '6';
	tb[DIK_7] = '7';
	tb[DIK_8] = '8';
	tb[DIK_9] = '9';
	tb[DIK_0] = '0';
	tb[DIK_MINUS] = '-';
	tb[DIK_EQUALS] = '=';
	tb[DIK_BACK] = 8;
	tb[DIK_TAB] = 9;
	tb[DIK_Q] = 'q';
	tb[DIK_W] = 'w';
	tb[DIK_E] = 'e';
	tb[DIK_R] = 'r';
	tb[DIK_T] = 't';
	tb[DIK_Y] = 'y';
	tb[DIK_U] = 'u';
	tb[DIK_I] = 'i';
	tb[DIK_O] = 'o';
	tb[DIK_P] = 'p';
	tb[DIK_LBRACKET] = '[';
	tb[DIK_RBRACKET] = ']';
	tb[DIK_RETURN] = GSK_ENTER;
	tb[DIK_LCONTROL] = GSK_CTRL;
	tb[DIK_A] = 'a';
	tb[DIK_S] = 's';
	tb[DIK_D] = 'd';
	tb[DIK_F] = 'f';
	tb[DIK_G] = 'g';
	tb[DIK_H] = 'h';
	tb[DIK_J] = 'j';
	tb[DIK_K] = 'k';
	tb[DIK_L] = 'l';
	tb[DIK_SEMICOLON] = 59;
	tb[DIK_APOSTROPHE] = 47;
	tb[DIK_GRAVE] = 96;
	tb[DIK_LSHIFT] = GSK_SHIFT;
	tb[DIK_BACKSLASH] = 92;
	tb[DIK_Z] = 'z';
	tb[DIK_X] = 'x';
	tb[DIK_C] = 'c';
	tb[DIK_V] = 'v';
	tb[DIK_B] = 'b';
	tb[DIK_N] = 'n';
	tb[DIK_M] = 'm';
	tb[DIK_COMMA] = 44;
	tb[DIK_PERIOD] = 46;
	tb[DIK_SLASH] = 47;
	tb[DIK_RSHIFT] = GSK_SHIFT;
	tb[DIK_MULTIPLY] = 42;
	tb[DIK_LMENU] = GSK_ALT;
	tb[DIK_SPACE] = 32;
	tb[DIK_CAPITAL] = '^';		// ???
	tb[DIK_F1] = GSK_F1;
	tb[DIK_F2] = GSK_F2;
	tb[DIK_F3] = GSK_F3;
	tb[DIK_F4] = GSK_F4;
	tb[DIK_F5] = GSK_F5;
	tb[DIK_F6] = GSK_F6;
	tb[DIK_F7] = GSK_F7;
	tb[DIK_F8] = GSK_F8;
	tb[DIK_F9] = GSK_F9;
	tb[DIK_F10] = GSK_F10;
	// skip 0x45 - 0x53
	tb[DIK_F11] = GSK_F11;
	tb[DIK_F12] = GSK_F12;
	tb[DIK_RMENU] = GSK_ALT;
	tb[DIK_HOME] = GSK_HOME;
	tb[DIK_UP] = GSK_CUP;
	tb[DIK_PRIOR] = GSK_PGUP;
	tb[DIK_LEFT] = GSK_CLEFT;
	tb[DIK_RIGHT] = GSK_CRIGHT;
	tb[DIK_END] = GSK_END;
	tb[DIK_DOWN] = GSK_CDOWN;
	tb[DIK_NEXT] = GSK_PGDN;
	tb[DIK_INSERT] = GSK_INSERT;
	tb[DIK_DELETE] = GSK_DEL;		
}
