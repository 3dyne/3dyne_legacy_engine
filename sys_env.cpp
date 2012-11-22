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



// sys_env.c

#if defined( linux_i386 )  || defined( irix_mips )
	#include <unistd.h>
	#include <sys/types.h>
	#include <sys/stat.h>
#elif defined ( win32_x86 )
	#include <sys/stat.h> // cygnus
	#include <sys/types.h>
	#include <windows.h> // for WinMain
#include <direct.h>
HINSTANCE	g_wininstance; // extern defined in r_private.h
int		g_ncmdshow;
#endif

#include "interfaces.h"

static char 	text[128];


#if defined( linux_i386 ) || defined( irix_mips ) || defined( win32_x86 )
int main( int argc, char *argv[] )
{
	int	ret;
	_chdir("E:\\src\\dd-0.2-devel-exec");
	ret = g_main( argc, argv );
	return ret;
}


#elif defined( win32_x86 )

static const char	*faked_argv[] = {"DarkestDays"};
int APIENTRY WinMain( HINSTANCE hCurrentInst,
		      HINSTANCE hPreviousInst,
		      LPSTR lpszCmdLine,
		      int nCmdShow)

{
	g_wininstance = hCurrentInst;
	g_ncmdshow = nCmdShow;
	g_main( 1, const_cast<char **>(faked_argv) );
}
#endif

char *SYS_GetPADir()
{
	char	*ptr;
	
#if defined(linux_i386) || defined(irix_mips)
	ptr = getenv( "HOME" );
	__chkptr( ptr );
	
	memset( text, 0, 128 );
	strcat( text, ptr );
	strcat( text, "/.DarkestDays" );
	mkdir( text, 511 );

#elif defined (win32_x86)
	memset( text, 0, 128 );
	strcat( text, "./" ); // I hope win32 likes that
#endif
	return text;
	
}

char *SYS_GetPGDir( char *padir )
{
	
#if defined(linux_i386) || defined(irix_mips)
	memset( text, 0, 128 );
	strcat( text, padir );
	strcat( text, "/" );
	strcat( text, ( SHP_GetVar( "game" ))->string );
	mkdir( text, 511 );
#elif defined (win32_x86)
	memset( text, 0, 128 );
	strcat( text, padir );
	strcat( text, "/" );
	strcat( text, ( SHP_GetVar( "game" ))->string );
#endif

	return text;
}


#if defined( irix_mips )
// swap 16bit
unsigned short SYS_SwapShort( unsigned short x )
{
	return (x>>8) | (x<<8);
}

// swap 32bit
unsigned int SYS_SwapInt( unsigned int x )
{
	return
        	(x>>24)
        	| ((x>>8) & 0xff00)
        	| ((x<<8) & 0xff0000)
        	| (x<<24);
}
#endif
