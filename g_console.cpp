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



// g_console.c

#include "interfaces.h"
#include "sys_console.h"
#include <stdarg.h>



void CON_GetInput( char* line )
{
	char	inputline[CON_MAXLINESIZE];
//	char	*ptr;

	inputline[0] = '\0';
	SHV_GetInputline( inputline );
//	SYS_GetInput( inputline );

	memcpy( line, inputline, CON_MAXLINESIZE );
}
	
	
void CON_Printf( const char* arg_text, ... )
{
	va_list		arg;
	char	buf[512];

	va_start( arg, arg_text );
	vsprintf( buf, arg_text, arg );
	va_end( arg );

	SHV_PutString( buf );

	SYS_PutString( buf );
}
