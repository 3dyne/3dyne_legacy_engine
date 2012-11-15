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



// sys_console


#include <interfaces.h>

#include <sys_console.h>

// ======================================================================
// unix shell:
#if defined ( linux_i386 )
// is this right?
#define STDIN_FD        0
static int      stdin_isup = 0;
static fd_set	fs;

static void SetupStdin()
{
//	cprintf( "setting up stdin ...\n" );
	FD_ZERO( &fs );
	FD_SET( STDIN_FD, &fs );
	stdin_isup = 0;
}


void SYS_PutString( char *string )
{
	printf( "%s", string );
}

void SYS_GetInput( char *line )
{
	// todo implement code from sscanf.c testfile.

	struct timeval	tv;
	int	ret;
       

	if( !stdin_isup )
	{
		SetupStdin();
	}
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	ret = select( FD_SETSIZE, &fs, NULL, NULL, &tv );
	
	if( ret <= 0 )
		return;

	fgets( line, CON_MAXLINESIZE, stdin );

	__named_message( "line: %s\n", line );
}

void SYS_CloseConsole()
{
}

#elif defined( win32_x86 )

static FILE	*outfile = NULL;
static int	badoutfile = 0;

static void OpenOutfile()
{
	if( badoutfile )
	{
		return;
	}

	outfile = fopen( "console.log", "wb" );
	if( !outfile )
	{
		badoutfile = 1;
		return;
	}

}

void SYS_PutString( char *string )
{
	if( !outfile )
	{
		OpenOutfile();
	}

	if( badoutfile )
	{
		return;
	}

	fprintf( outfile, string );
}

void SYS_GetInput( char *line )
{
	//todo: what todo?
}

void SYS_CloseConsole()
{
	if( outfile )
		fclose( outfile );
}

#endif
