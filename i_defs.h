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



// i_defs.h

#ifndef i_defs_h
#define i_defs_h

#define IMODE_NORMAL	( 0 )  // behave normal ( callback everytime )
#define IMODE_GETCHAR	( 1 )  // behave like getchar ( no callback on most asciis + some exceptions )

#define SYMTYPE_PRESS	( 0 )
#define SYMTYPE_RELEASE	( 1 )

typedef enum {
	GSK_ENTER = 10,
	GSK_DEL = 127,
	GSK_CUP,
	GSK_CDOWN,
	GSK_CLEFT,
	GSK_CRIGHT,
	
	GSK_ALT,
	GSK_CTRL,
	GSK_SHIFT,


	GSK_FUCK0,
	GSK_FUCK1,

	GSK_TAB,
	GSK_INSERT,
	GSK_HOME,
	GSK_PGUP,
	GSK_END,
	GSK_PGDN,

	GSK_ESCAPE,

	GSK_F1,
	GSK_F2,
	GSK_F3,
	GSK_F4,
	GSK_F5,
	GSK_F6,
	GSK_F7,
	GSK_F8,
	GSK_F9,
	GSK_F10,
	GSK_F11,
	GSK_F12,

	

	GSK_BUTTON1,
	GSK_BUTTON2,
	GSK_BUTTON3,
	GSK_BUTTON4,
	GSK_BUTTON5,
	
} gs_ksym;


typedef struct keyevent_s {
	gs_ksym		sym;	// ksym of the event
	unsigned int	type;   // event type: SYMTYPE_PRESS / SYMTYPE_RELEASE
} keyevent_t; 	


#endif
