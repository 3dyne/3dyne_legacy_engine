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



// i_vkmap.c

#include <windows.h>
#include "r_private.h"

unsigned char	i_vktochar[256];
#define tb i_vktochar
void I_InitVKMap( void )
{
	int	i;

	for ( i = 0; i < 256; i++ )
		tb[i] = 0;

	tb[VK_ESCAPE] = GSK_ESCAPE;
	tb['1'] = '1';
	tb['2'] = '2';
	tb['3'] = '3';
	tb['4'] = '4';
	tb['5'] = '5';
	tb['6'] = '6';
	tb['7'] = '7';
	tb['8'] = '8';
	tb['9'] = '9';
	tb['0'] = '0';
	tb[189] = '-';
	tb[187] = '=';
	tb[VK_BACK] = 8;
	tb[VK_TAB] = 9;
	tb['Q'] = 'q';
	tb['W'] = 'w';
	tb['E'] = 'e';
	tb['R'] = 'r';
	tb['T'] = 't';
	tb['Y'] = 'y';
	tb['U'] = 'u';
	tb['I'] = 'i';
	tb['O'] = 'o';
	tb['P'] = 'p';
	tb[219] = '[';
	tb[221] = ']';
	tb[VK_RETURN] = GSK_ENTER;
	tb[VK_LCONTROL] = GSK_CTRL;
	tb['A'] = 'a';
	tb['S'] = 's';
	tb['D'] = 'd';
	tb['F'] = 'f';
	tb['G'] = 'g';
	tb['H'] = 'h';
	tb['J'] = 'j';
	tb['K'] = 'k';
	tb['L'] = 'l';
	tb[186] = 59;
	tb[222] = 47;
	tb[192] = 96;
	tb[VK_LSHIFT] = GSK_SHIFT;
	tb[220] = 92;
	tb['Z'] = 'z';
	tb['X'] = 'x';
	tb['C'] = 'c';
	tb['V'] = 'v';
	tb['B'] = 'b';
	tb['N'] = 'n';
	tb['M'] = 'm';
	tb[188] = 44;
//	tb['V'] = 46;
	tb[191] = 47;
	tb[VK_RSHIFT] = GSK_SHIFT;
	tb[VK_MULTIPLY] = 42;
	tb[VK_LMENU] = GSK_ALT;
	tb[VK_SPACE] = 32;
	tb[VK_CAPITAL] = '^';		// ???
	tb[VK_F1] = GSK_F1;
	tb[VK_F2] = GSK_F2;
	tb[VK_F3] = GSK_F3;
	tb[VK_F4] = GSK_F4;
	tb[VK_F5] = GSK_F5;
	tb[VK_F6] = GSK_F6;
	tb[VK_F7] = GSK_F7;
	tb[VK_F8] = GSK_F8;
	tb[VK_F9] = GSK_F9;
	tb[VK_F10] = GSK_F10;
	// skip 0x45 - 0x53
	tb[VK_F11] = GSK_F11;
	tb[VK_F12] = GSK_F12;
	tb[VK_RMENU] = GSK_ALT;
	tb[VK_HOME] = GSK_HOME;
	tb[VK_UP] = GSK_CUP;
	tb[VK_PRIOR] = GSK_PGUP;
	tb[VK_LEFT] = GSK_CLEFT;
	tb[VK_RIGHT] = GSK_CRIGHT;
	tb[VK_END] = GSK_END;
	tb[VK_DOWN] = GSK_CDOWN;
	tb[VK_NEXT] = GSK_PGDN;
	tb[VK_INSERT] = GSK_INSERT;
	tb[VK_DELETE] = GSK_DEL;		
}

