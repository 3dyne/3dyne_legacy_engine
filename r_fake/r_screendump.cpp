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



// r_screendump.c

#include <math.h>
#include <vector>
//#include <alloca.h>

#include "r_interfaces.h"
#include "defs.h"
#include "shared.h"

#include "render.h"
#include "sh_parser.h"
//#include "tga.h"

/*
  ==============================
  R_ScreenDump

  ==============================
*/

void R_ScreenDump( const char *name )
{
#if 1
	unsigned char	*lfb;
	FILE		*h;
	int		width, height;
	short		tmp;
	char		header1[] = { 0,0,2,0,0,0,0,0,0,0,0,0 };
	int i;

	printf( "R_ScreenDump: write tga to file %s\n", name );

	width = ((sh_var_t*)SHP_GetVar( "r_devicewidth" ))->ivalue;
	height = ((sh_var_t*)SHP_GetVar( "r_deviceheight" ))->ivalue;
 
	SHV_Printf( "%d x %d", width, height );
	
	std::vector<unsigned char> lfb_data(width * height * 4);
	lfb = lfb_data.data();
	//lfb = (unsigned char *)alloca( width * height * 4 );

	glReadBuffer( GL_FRONT );
	glReadPixels( 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, lfb );


	h = fopen( name, "w" );
	if ( !h )
		__error( "can't open file.\n" );

	fwrite( header1, 12, 1, h );
	tmp = width;
	fwrite( &tmp, 2, 1, h );
	tmp = height;
	fwrite( &tmp, 2, 1, h );

	fputc( 0x18, h );
	fputc( 0x20, h );

	for ( i = 0; i < width*height; i++ )
	{
		fputc( lfb[i*3+2], h );
		fputc( lfb[i*3+1], h );
		fputc( lfb[i*3], h );
	}

	fclose( h );
#endif
}
