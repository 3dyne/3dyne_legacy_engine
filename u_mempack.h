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



// u_mempack.h

#ifndef __u_mempack
#define __u_mempack

#include "lib_math.h"

typedef struct mempack_s
{
	int	size;
	char	data[4];	// variable
} mempack_t;

// pack

void		U_BeginMemPackInstance( void );
mempack_t *	U_EndMemPackInstance( void );
void		U_MemPacki32( int i32 );
void		U_MemPackf32( fp_t f32 );
void		U_MemPackf32v2( vec2d_t f32v2 );
void		U_MemPackf32v3( vec3d_t f32v3 );
void		U_MemPackPtr( void **ptr );

void		U_FreeMemPack( mempack_t *pack );

// unpack

void	U_BeginMemUnpack( mempack_t *pack );
int	U_EndMemUnpack( void );
void	U_MemUnpacki32( int *i32 );
void	U_MemUnpackf32( fp_t *f32 );
void	U_MemUnpackf32v2( vec2d_t f32v2 );
void	U_MemUnpackf32v3( vec3d_t f32v3 );
void	U_MemUnpackPtr( void **pptr );

#endif
