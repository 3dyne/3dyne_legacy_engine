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



// sys_dll.c

#define	SYS_DLL_MAXOPEN	( 8 )

// test


#if defined( linux_i386 ) || defined( irix_mips )
	#include <dlfcn.h>

	#define SYS_DLLSUFFIX	".so"

	static void* h[SYS_DLL_MAXOPEN];

#elif defined( win32_x86 )
	#include <windows.h>
	#define SYS_DLLSUFFIX	".dll"
	static HMODULE	h[SYS_DLL_MAXOPEN];

#else
	#error Cool system, guy. But how do we use dlls on it
#endif

#include "interfaces.h"
#include "version.h"

static int	firstcall = 0;

static int	inuse[SYS_DLL_MAXOPEN];


void SYS_DllAddSuffix( char *string, int mode )
{
	__chkptr( string );
	
	// we hope that there's enough space in string.
	
	strcat( string, SYS_DLLSUFFIX );
	
	if( mode == SYS_DLLSSYSTEM )
	{
		strcat( string, "." );
		strcat( string, BHOSTSYSTEM );
	}

}

int SYS_DllBegin( const char* dll_name )
{
	int	i;

	char	sys_dll[256];
#if defined(win32_x86)
	char	errbuf[512];
#endif 
		
	if( !firstcall )
	{
		for( i = 0; i < SYS_DLL_MAXOPEN; i++ )
			inuse[i] = 0;
		firstcall = 1;
	}
	
	for( i = 0; i < SYS_DLL_MAXOPEN; i++ )
		if( !inuse[i] )
			break;

	if( i == SYS_DLL_MAXOPEN )
		__error( "out of handles\n" );
		
	
		
#if defined(linux_i386) || defined(irix_mips)
// =======================================
	sprintf( sys_dll, "%s", dll_name );

	h[i] = dlopen( sys_dll, RTLD_LAZY );
	if( !h[i] )
		__error( "dlopen failed to open '%s' with the following message:\n\"%s\"\n", sys_dll, dlerror() );
	
	
#elif defined(win32_x86)
// =======================================
	// todo: test me!
	sprintf( sys_dll, "%s", dll_name );
	__named_message( "sys_dll: %s\n", sys_dll );
	h[i] = ( HMODULE )LoadLibrary( sys_dll );
	if( !h[i] )
	{
		FormatMessage((FORMAT_MESSAGE_FROM_SYSTEM), NULL, GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		errbuf, 512, NULL);
		__error( "LoadLibrary failed to open '%s' with message:\n%s\n", sys_dll, errbuf );
	}

#endif
	inuse[i] = 1;

	return i;
}

void * SYS_DllSym( int hi, const char* arg_name )
{
	void *ptr;

	if( !h[hi] )
		__error( "no open dll\n" );
	
	
	if( !inuse[hi] )
	{
		__error( "dll handle not valid\n" );
	}

#if defined(linux_i386) || defined(irix_mips)
// =======================================
	ptr = dlsym( h[hi], arg_name );
//	__chkptr( ptr );
	
	
#elif defined( win32_x86 )
// =======================================
	ptr = ( void * )GetProcAddress((HMODULE)h[hi], arg_name );
//	__chkptr( ptr );
#endif
	return ptr;
}


void SYS_DllEnd( int hi )
{

	if( !h[hi] )
	{
		__warning( "no open dll. doing noting.\n" );
		return;
	}

#if defined( linux_i386 ) || defined( irix_mips )
// =======================================
	__chkptr( h[hi] );
	dlclose( h[hi] );
	
#elif defined(win32_x86)
// =======================================
	// todo: impement me!
	FreeLibrary( (HMODULE)h[hi] );
#endif
	inuse[hi] = 0;

}
