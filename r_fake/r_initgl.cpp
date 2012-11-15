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



// r_initgl.c
#include "sh_parser.h"
#include "render.h"

gl_extensions_t		gl_ext;	// available gl extensions

static sh_var_t *size_x = NULL, *size_y = NULL;
 
void R_InitGL_Extensions( void )
{
	const char	*exten;

	memset( &gl_ext, 0, sizeof( gl_extensions_t ) );

	exten = ( const char * ) glGetString( GL_EXTENSIONS );

	printf( "R_InitGL_Extensions: %s\n", exten );
	
	if ( strstr( exten, "GL_ARB_multitexture" ) )
	{
		printf( " available: GL_ARB_multitexture\n" );
		gl_ext.have_arb_multitexture = true;
	}

	gl_ext.have_arb_multitexture = false;

	if ( strstr( exten, "GL_EXT_vertex_array" ) )
	{
		printf( " available: GL_EXT_vertex_array\n" );
		gl_ext.have_ext_va = true;
	}

	if ( strstr( exten, "GL_EXT_compiled_vertex_array" ) )
	{
		printf( " available: GL_EXT_compiled_vertex_array\n" );
		gl_ext.have_ext_cva = true;
	}

}

void R_InitGL_Version( void )
{
	printf( "R_InitGL_Version: %s\n", (char *) glGetString(GL_VERSION));	
}

void R_GetDepthBufferAccuracy( void )
{
	unsigned int	u1, u2;
	int		i;

	glReadBuffer( GL_BACK );
	glDrawBuffer( GL_BACK );
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);	
	glDepthFunc( GL_ALWAYS );
	glDepthRange( 0.0, 1.0 );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	u1 = 0x80000000;
	for( i = 0; i < 32; i++ )
	{
		glRasterPos2f( 61.0/320.0, 61.0/240.0 );
		
		glDrawPixels( 1, 1, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, &u1 );
		glReadPixels( 320+61, 240+61, 1, 1, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, &u2 );	

//		printf( "write %x, read %x\n", u1, u2 );

		if ( u1 != u2 )
		{
			break;
		}

		u1>>=1;
	}
	__named_message( "is %d bits\n", i );
	glDepthFunc( GL_GEQUAL );
}

void R_InitGL_rendering( void )
{
//	printf( "R_InitGL:\n" );

//	static int old_size_x = 0;
//	static int old_size_y = 0;

	if( !size_x )
	{
		size_x = SHP_GetVar( "r_devicewidth" );
		size_y = SHP_GetVar( "r_deviceheight" );

		glViewport( 0, 0, (GLint)size_x->ivalue, (GLint)size_y->ivalue );
	}

	glDisable(GL_CULL_FACE);
	glFrontFace( GL_CW );
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
#if 0	
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);	
	glDepthFunc( GL_GEQUAL );
	glDepthRange( 0.0, 1.0 );	

	glDisable( GL_TEXTURE_2D );
	glDisable( GL_BLEND );
#endif
	
//	glViewport( 0, 0, (GLint)size_x->ivalue, (GLint)size_y->ivalue );

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

	glMatrixMode( GL_MODELVIEW );
       	glLoadIdentity();	

	gl_ext.have_arb_multitexture = false;


}

void R_InitGL_drawing( void )
{
	if( !size_x )
	{
		size_x = SHP_GetVar( "r_devicewidth" );
		size_y = SHP_GetVar( "r_deviceheight" );
	}

	glViewport( 0, 0, (GLint)size_x->ivalue, (GLint)size_y->ivalue );

	glMatrixMode( GL_PROJECTION );                                               
	glLoadIdentity();                                                            
	glOrtho( 0.0, size_x->ivalue*1.0, 0.0, size_y->ivalue*1.0, -10.0, 10.0 );                     
	glMatrixMode( GL_MODELVIEW );                                                
	glLoadIdentity();

	glEnable( GL_TEXTURE_2D );
}
