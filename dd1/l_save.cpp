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



// l_save.c

#include "l_locals.h"

void SAVE_ArcheTS( unsigned char *dir )
{
	atm_header_t	*header;
	arche_t		*at;
	FILE	*h;
	int	i;

	h = fopen( "test.save", "wb" );

	SAVE_Begin( h );

	SAVE_BeginSection( "prep" );

	SAVE_PackInt( "aggregate table checksum", AG_Checksum() );

	SAVE_EndSection( "end" );


	for( header = at_head; header; header = header->next )
	{
		__named_message( "%p %p\n", header, header->next );
		at = header + 1;
		SAVE_BeginSection( "{" );
	
		for( i = 0; i < AG_MAXNUM; i++ )
		{
			if( ! at->ags[i] )
				continue;
			SAVE_BeginSection( ag_funcs[i].name );
			ag_funcs[i].savefunc( AG( at, i ));
			SAVE_EndSection( "ag_end" );
		}
		SAVE_EndSection( "}" );
	}
	fclose( h );
}

void SAVE_Life( unsigned char* save_buf, unsigned int save_size )
{
}	


// ========================================
// SAVE_ and LOAD_ : abstraction for file to
// state machine.

static unsigned char	save_indent[SAVE_MAXINDENT];
static FILE		*save_h;

static int		save_indentdepth;

static char		save_buf[256];

void SAVE_Begin( FILE *arg_h )
{
	save_h = arg_h;
	memset( save_indent, 0, SAVE_MAXINDENT );
	save_indentdepth = 0;
}

void SAVE_End()
{
	save_h = NULL;
}

void SAVE_BeginSection( char *name )
{
	fprintf( save_h, "%s%s\n", save_indent, name );
	if( save_indentdepth >= SAVE_MAXINDENT - 1 )
		__error( "SAVE_MAXINDENT is too small: %d\n", SAVE_MAXINDENT );


	save_indent[save_indentdepth++] = '\t';
	
}

void SAVE_EndSection( char *name )
{
	if( !save_indentdepth )
		__error( "end without begin\n" );

	save_indent[--save_indentdepth] = '\0';

	fprintf( save_h, "%s%s\n", save_indent, name );
}

void SAVE_PackInt( char *key, int value )
{
#ifdef SAVE_DEBUGMODE
	fprintf( save_h, "\n%s# %s\n", save_indent, key );
#endif

	__named_message( "%d\n", value );
	fprintf( save_h, "%s\"%d\"\n", save_indent, value );
}

void SAVE_PackFloat( char *key, float value )
{
#ifdef SAVE_DEBUGMODE
	fprintf( save_h, "\n%s# %s\n", save_indent, key );
#endif	
	fprintf( save_h, "%s\"%f\"\n", save_indent, value );
}

void SAVE_PackVec3d( char *key, vec3d_t value )
{
#ifdef SAVE_DEBUGMODE
	fprintf( save_h, "\n%s# %s\n", save_indent, key );
#endif

	fprintf( save_h, "%s\"%f %f %f\"\n", save_indent, value[0]
		 , value[1], value[2] );
}


void LOAD_Begin( FILE *h )
{

}


unsigned char *LOAD_Section()
{
}	

unsigned int LOAD_UnpackInt()
{
}

unsigned int LOAD_UnpackFloat()
{
}


unsigned int LOAD_UnpackVec3d()
{
}
