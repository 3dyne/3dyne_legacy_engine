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



// u_packed.c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <string.h>

#include "sys_endian.h"
#include "shock.h"
#include "lib_pack.h"

static char	*cur_start;
static char	*cur_pos;
static int	cur_maxsize;
static int	cur_size;

static int	packedmode;
static char	u_token[255];


/*
  ====================
  U_BeginPack

  ====================
*/
void U_BeginPack( int mode, void *ptr, int maxsize )
{
	cur_start = ( char * ) ptr;
	cur_pos = cur_start;
	cur_maxsize = maxsize;
	cur_size = 0;
}

/*
  ====================
  U_EndPack

  ====================
*/
int U_EndPack( void )
{
	return cur_size;
}

/*
  ====================
  U_Packs8

  ====================
*/
void U_Packs8( char s8 )
{
	if ( cur_size + 1 >= cur_maxsize )
	{

	}
	*cur_pos = s8;
	cur_pos++;
	cur_size++;
}

/*
  ====================
  U_Packs16

  ====================
*/
void U_Packs16( short s16 )
{
	char	tmp[2];

	if ( cur_size + 2 >= cur_maxsize )
	{

	}

	s16 = SHORT( s16 );

	memcpy( tmp, &s16, 2 );

	*cur_pos = tmp[0];
	cur_pos++;
	*cur_pos = tmp[1];
	cur_pos++;
	cur_size += 2;
}

/*
  ====================
  U_Packs32

  ====================
*/
void U_Packs32( int s32 )
{
	char	tmp[4];

	if ( cur_size + 4 >= cur_maxsize )
	{

	}

	s32 = INT( s32 );

	memcpy( tmp, &s32, 4 );

	*cur_pos = tmp[0];
	cur_pos++;
	*cur_pos = tmp[1];
	cur_pos++;
	*cur_pos = tmp[2];
	cur_pos++;
	*cur_pos = tmp[3];
	cur_pos++;

	cur_size += 4;
}


/*
  ====================
  U_Packfp32

  ====================
*/
void U_Packfp32( float fp32 )
{
	char	tmp[4];

	if ( cur_size + 4 >= cur_maxsize )
	{

	}

	fp32 = FLOAT32( fp32 );

	memcpy( tmp, &fp32, 4 );

	*cur_pos = tmp[0];
	cur_pos++;
	*cur_pos = tmp[1];
	cur_pos++;
	*cur_pos = tmp[2];
	cur_pos++;
	*cur_pos = tmp[3];
	cur_pos++;

	cur_size += 4;	
}


/*
  ====================
  U_Packntstring

  ====================
*/
void U_PackntString( char *ntstring )
{
	for ( ;; )
	{
		if ( cur_size == cur_maxsize )
		{
			
		}
		*cur_pos = *ntstring;
		cur_pos++;
		ntstring++;
		cur_size++;

		if ( *(cur_pos-1) == 0 )
			break;
		
	}
}

/*
  ====================
  U_PackString

  ====================
*/
void U_PackString( char *string, int size )
{
	if( cur_size + size >=  cur_maxsize )
	{
	}

	memcpy( cur_pos, string, size );
}

/*
  ====================
  U_Pack

  ====================
*/

/*
  ====================
  U_BeginUnPack

  ====================
*/
void U_BeginUnpack( int mode, void *ptr, int size )
{
	if( mode & U_PACKED_ASC )
		packedmode = U_PACKED_ASC;

	if( mode & U_PACKED_BIN )
		packedmode = U_PACKED_BIN;
	
	cur_start = ( char * ) ptr;
	cur_pos = cur_start;

	cur_maxsize = size;
	cur_size = 0;

}


int U_EndUnpack()
{
	packedmode = 0;
	return cur_size;
}

void U_UnpackToken()
{
	int	c;
	int	quota = 0;
	int	pos = 0;

	//u_token = NULL;


	if( !cur_pos )
	{
		u_token[0] = 0;
		return;
	}

	for( ;; )
	{
		c = *(cur_pos);
		cur_pos++;
		cur_size++;
			

		if( c == '\"' )
		{
			quota = 1;
			c = *(cur_pos++);
			cur_size++;
			break;
		}
		
		if( !isspace( c ) )
			break;

		if( c == '#' )
		{
			u_token[0] = 0;
			return;
		}
	}
	
	for( ;; )
	{
		if( c == '\"' && quota )
			break;

		if( !c )
			break;

		if(  isspace( c )  && !quota )
			break;

		if( pos >= 255 )
			__error( "token too long\n" );
		
		if( c == '#' )
		{
			u_token[0] = 0;
			return;
		}
		
		u_token[pos] = ( char )c;
		
		pos++;
		c = *(cur_pos++);
		cur_size++;
	}
	if( pos > 255 )  // double save
		__error( "token too long\n" );	
	
	u_token[pos] = 0;
	return;
}



/*
  ====================
  U_UnPacks8

  ====================
*/

void U_Unpacks8( char *s8 )
{

	switch( packedmode )
	{
	case U_PACKED_BIN:
		*s8 = *cur_pos;
		cur_pos++;
		cur_size++;
		break;

	case U_PACKED_ASC:
		U_UnpackToken();
		*s8 = atoi( u_token );
		break;

	default:
		__error( "unknown packedmode\n" );
	}
}

void U_Unpacks16( short *s16 )
{

	switch( packedmode )
	{
	case U_PACKED_BIN:
		memcpy( s16, cur_pos, 2 );
		
		*s16 = SHORT( *s16 );
		cur_pos+=2;
		cur_size+=2;
		break;

	case U_PACKED_ASC:
		U_UnpackToken();
		*s16 = atoi( u_token );
		break;

	default:
		__error( "unknown packedmode\n" );
	}
}

void U_Unpacks32( int *s32 )
{

	switch( packedmode )
	{
	case U_PACKED_BIN:
		memcpy( s32, cur_pos, 4 );
		
		*s32 = INT( *s32 );
		cur_pos+=4;
		cur_size+=4;
		break;

	case U_PACKED_ASC:
		U_UnpackToken();
		*s32 = atoi( u_token );
		break;
		
	default:
		__error( "unknown packedmode\n" );
	}

}


void U_Unpackfp32( float *fp32 )
{

	switch( packedmode )
	{
	case U_PACKED_BIN:
		memcpy( fp32, cur_pos, 4 );

		*fp32 = FLOAT32( *fp32 );
		cur_pos+=4;
		cur_size+=4;
		break;
	
	case U_PACKED_ASC:
		U_UnpackToken();
		*fp32 = atof( u_token );
		break;

	default:
		__error( "unknown packedmode\n" );
	}
}


void U_UnpackString( char *dest, int size )
{
	memcpy( dest, cur_pos, size );
	
	cur_pos += size;
	cur_size += size;
}

void U_UnpackntString( char *dest, int maxsize )
{
	for ( ;; )
	{
		if ( cur_size == cur_maxsize )
		{
			
		}
		*dest = *cur_pos;
		cur_pos++;
		dest++;
		cur_size++;

		if ( *(cur_pos-1) == 0 )
			break;
		
	}
}

void U_UnpackSkip( int space )
{
	if( cur_size + space >= cur_maxsize )
	{
		__warning( "running out of input buffer\n" );
	}

	cur_pos += space;
	cur_size += space;
}
