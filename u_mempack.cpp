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



// u_mempack.c

#include "u_mempack.h"
#include "g_shared.h"
#include "ca_service.h"
#include "shock.h"

#define	MEMPACK_TMP_BUFFER	( 1024 )

static bool_t	mempack_in_pack = false;

// pack/unpack
static int	mempack_pos;
static char	mempack_tmp[MEMPACK_TMP_BUFFER];

// unpack
static char	*mempack_src;
static int	mempack_size;

/*
  ==============================
  U_BeginMemPackInstance

  ==============================
*/
void U_BeginMemPackInstance( void )
{
	if ( mempack_in_pack != false )
		__error( "U_BeginMemPackInstance is already in use\n" );

	mempack_in_pack = true;
	mempack_pos = 0;
}

/*
  ==============================
  U_EndMemPackInstance

  ==============================
*/
mempack_t * U_EndMemPackInstance( void )
{
	mempack_t	*pack;
	size_t		size;

	size = (size_t)&(((mempack_t *)0)->data[mempack_pos]);

	pack = (mempack_t *)NEWBYTES( size );
	memcpy( pack->data, mempack_tmp, mempack_pos );

	mempack_in_pack = false;

	return pack;
}


/*
  ==============================
  U_FreeMemPack

  ==============================
*/
void U_FreeMemPack( mempack_t *pack )
{
	FREE( pack );
}


/*
  ==============================
  U_PackI32

  ==============================
*/
void U_MemPacki32( int i32 )
{
	if ( mempack_pos+4 >= MEMPACK_TMP_BUFFER )
		__error( "reached MEMPACK_TMP_BUFFER size\n" );

	memcpy( &mempack_tmp[mempack_pos], &i32, 4 );
	mempack_pos+=4;
}

/*
  ==============================
  U_PackF32

  ==============================
*/
void U_MemPackf32( fp_t f32 )
{
	if ( mempack_pos+4 >= MEMPACK_TMP_BUFFER )
		__error( "reached MEMPACK_TMP_BUFFER size\n" );

	memcpy( &mempack_tmp[mempack_pos], &f32, 4 );
	mempack_pos+=4;	
}

/*
  ==============================
  U_PackF32V2

  ==============================
*/
void U_MemPackf32v2( vec2d_t f32v2 )
{
	if ( mempack_pos+8 >= MEMPACK_TMP_BUFFER )
		__error( "reached MEMPACK_TMP_BUFFER size\n" );

	memcpy( &mempack_tmp[mempack_pos], f32v2, 8 );
	mempack_pos+=8;		
}

/*
  ==============================
  U_Unpackf32v3

  ==============================
*/
void U_MemPackf32v3( vec3d_t f32v3 )
{
	if ( mempack_pos+12 >= MEMPACK_TMP_BUFFER )
		__error( "reached MEMPACK_TMP_BUFFER size\n" );

	memcpy( &mempack_tmp[mempack_pos], f32v3, 12 );
	mempack_pos+=12;		
}

/*
  ==============================
  U_PackPtr

  ==============================
*/
void U_MemPackPtr( void **ptr )
{
	if ( mempack_pos+4 >= MEMPACK_TMP_BUFFER )
		__error( "reached MEMPACK_TMP_BUFFER size\n" );

	memcpy( &mempack_tmp[mempack_pos], ptr, 4 );
	mempack_pos+=4;		
}

/*
  ==============================
  U_BeginMemUnpack

  ==============================
*/
void U_BeginMemUnpack( mempack_t *pack )
{
	mempack_pos = 0;
	mempack_src = pack->data;
	mempack_size = pack->size;
}

/*
  ==============================
  U_EndMemUnpack

  ==============================
*/
int U_EndMemUnpack( void )
{
	return mempack_pos;
}

/*
  ==============================
  U_Unpacki32
 
  ==============================
*/
void	U_MemUnpacki32( int *i32 )
{
	if ( mempack_pos+4 <= mempack_size )
		__error( "reached mempack_size\n" );

	memcpy( i32, &mempack_src[mempack_pos], 4 );
	mempack_pos+=4;
}

/*
  ==============================
  U_Unpackf32

  ==============================
*/
void	U_MemUnpackf32( fp_t *f32 )
{
	if ( mempack_pos+4 <= mempack_size )
		__error( "reached mempack_size\n" );

	memcpy( f32, &mempack_src[mempack_pos], 4 );
	mempack_pos+=4;	
}

/*
  ==============================
  U_Unpackf32v2

  ==============================
*/
void	U_MemUnpackf32v2( vec2d_t f32v2 )
{
	if ( mempack_pos+8 <= mempack_size )
		__error( "reached mempack_size\n" );

	memcpy( f32v2, &mempack_src[mempack_pos], 8 );
	mempack_pos+=8;		
}

/*
  ==============================
  U_Unpackf32v3

  ==============================
*/
void	U_MemUnpackf32v3( vec3d_t f32v3 )
{
	if ( mempack_pos+12 <= mempack_size )
		__error( "reached mempack_size\n" );

	memcpy( f32v3, &mempack_src[mempack_pos], 12 );
	mempack_pos+=12;		
}

/*
  ==============================
  U_UnpackPtr

  ==============================
*/
void	U_MemUnpackPtr( void **pptr )
{
	if ( mempack_pos+4 <= mempack_size )
		__error( "reached mempack_size\n" );

	memcpy( pptr, &mempack_src[mempack_pos], 4 );
	mempack_pos+=4;	
}
