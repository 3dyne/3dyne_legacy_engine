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



// wgl_service.c
#define GLAPI
#include "wgl_service.h"
#include "interfaces.h"

void (*DllInitialize)(void);


static int	wgl_h;

extern int gl_h;

extern "C" {
void oldc_ghetto_load_wgl( int gl_h );
}
void GL_LoadWGLApi( char *name )
{
//	wgl_h = SYS_DllBegin( name );
	wgl_h = gl_h;

#ifdef GL_LINKED
    __named_message( "gl lib is linked. doing nothing\n" );
    return;
#else

	oldc_ghetto_load_wgl( wgl_h );
	__named_message( "\nwgl symbols are from %s\n", name );
#endif
}	

void GL_RejectWGLApi()
{
#ifdef GL_LINKED
    __named_message( "leaving\n" );
    return;
#else

	SYS_DllEnd( wgl_h );
	__named_message( "\tdone\n" );
#endif
}

