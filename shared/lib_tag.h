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



// lib_tag.h

#ifndef __lib_tag
#define __lib_tag

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "s_config.h"
#include "s_mem.h"

typedef struct u_tag_s
{
	int	data_size;	// aka name_ofs
	char	text[4];	// variable sized
} u_tag_t;

#define TGO( tag ) ( (void *)(&tag->text[0]) )

u_tag_t * U_NewTag( char *tag_name, int objsize );
void * U_TagGetObjPtr( u_tag_t *tag );
int U_TagGetObjSize( u_tag_t *tag );

// for map
void * U_TagGetKey( const void *tag );
int U_TagKeyCompare( const void *k1, const void *k2 ); 

#endif
