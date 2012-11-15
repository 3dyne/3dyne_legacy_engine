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



// g_bmdefs.h

/*
  =============================================================================
  blockmap defs

  =============================================================================
*/

#ifndef __g_bmdefs
#define __g_bmdefs

#include "lib_math.h"

#define MAPBLOCK_SIZE			( 128 )
#define BLOCKMAP_HASHSIZE		( 256 )

typedef struct blockmap_s
{
	int			mapblocknum;
	struct mapblock_s	*hash[BLOCKMAP_HASHSIZE];

	// hack: list of all blockmap objects
	u_list_t		obj_list;
} blockmap_t;



typedef struct mapln_s
{
	struct mapln_s		*prevb;
	struct mapln_s		*nextb;

	struct mapln_s		*next;

	struct mapobject_s	*ref;
} mapln_t;

typedef struct mapblock_s
{
	ivec3d_t		pos;

	// double link list head and tail
	struct mapln_s			head;		// first real linknode is head.next
	struct mapln_s			tail;

        struct mapblock_s	*next;		// next in hashline
} mapblock_t;

#define BM_SURF_MAP_WALL		( 1 )
#define BM_SURF_MAP_SKY			( 2 )

#define BM_SURF_MAP_ALL	( BM_SURF_MAP_WALL | BM_SURF_MAP_SKY )

typedef struct
{
	vec3d_t		norm;
	fp_t		dist;
	flag_t		surf_flags;
} bmplane_t;

typedef struct mapobject_s
{
	mapln_t		*linknodes;
	ivec3d_t		min;
	ivec3d_t		max;	
	int		lasttrace;		// test only once per trace
	int			planenum;	// can be zero for bound box only test
	bmplane_t		planes[6];	// variable sized
} mapobject_t;

#endif
