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



// gl_service.c
// #define GLAPI

#include "gl_service.h"
#include "interfaces.h"

int	gl_h;

static void GL_GetSpecialInfo( gl_info_t *info ) // maintain insane drivers ( 3dfx mesa ... )
{
	char	*ver;

	ver = ( char * )glGetString( GL_RENDERER );


// probe for insane 3dfx mesa
	if( strstr( ver, "Glide" ))
	{
		__named_message( "\tMesa Glide driver flush workaround activated\n" );
	
		info->flushinterval = 0;
	}

	// all the others should work with default values
}

void GL_GetInfo( gl_info_t *info ) // fill malloced struct
{
	info->extstring = (char*) glGetString( GL_EXTENSIONS );
	printf( "%s\n", info->extstring );

	if( strstr( info->extstring, "GL_ARB_multitexture" ))
	{
//		printf( "arb mulititexture\n" );
		__warning( " ***** ignore GL_MAX_TEXTURE_UNITS_ARB *****\n" );
		info->texenv_units = 1;
//		glGetIntegerv( GL_MAX_TEXTURE_UNITS_ARB, &info->texenv_units );
	} else
	{
		info->texenv_units = 1;
	}
	if( strstr( info->extstring, "GL_EXT_texture_env_add" ))
	{
		info->texenv_have_add = 1;
	} else
	{
		info->texenv_have_add = 0;
	}		
	

	printf( "texunits %d\n", info->texenv_units );
	printf( "have add %d\n", info->texenv_have_add );

	info->flushinterval = 16;
	GL_GetSpecialInfo( info );
	
}
#ifndef GL_LINKED
extern "C" {
void oldc_ghetto_load_gl( int );
}
#endif

void GL_LoadGLApi( char *name )
{

#ifdef GL_LINKED
    __named_message( "gl lib is linked. noing nothing\n" );
    return;
#else 

	gl_h = SYS_DllBegin( name );

    oldc_ghetto_load_gl( gl_h );


	__named_message( "\tGL symbols are from %s\n", name );

#endif
}


void GL_RejectGLApi()
{
#ifdef GL_LINKED
    __named_message( "leaving\n" );
    return;
#else

	SYS_DllEnd( gl_h );
	__named_message( "\tdone\n" );
#endif
}
