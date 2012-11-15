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



// g_blockmap.h

#ifndef __g_blockmap
#define __g_blockmap

#include "g_bmdefs.h"

//
// g_blockmap.c
//
void G_InitBlockmap( g_map_t *map );
void G_CleanUpBlockmap( blockmap_t *map );

blockmap_t * G_NewBlockmap( void );
void G_FreeBlockmap( blockmap_t *map );
mapblock_t * G_FindMapblock( blockmap_t *map, ivec3d_t pos );	// pos in blockmap units
void G_InsertMapblock( blockmap_t *map, mapblock_t *block );
void G_RemoveMapblock( blockmap_t *map, mapblock_t *block );
int G_CalcBlockmapHashkey( ivec3d_t pos );
mapln_t * G_NewBlockmapLinknode( void );
void G_FreeBlockmapLinknode( mapln_t *n );
void G_InsertLN( mapln_t *prev, mapln_t *next, mapln_t *insert );
void G_RemoveLN( mapln_t *n );
mapblock_t * G_NewMapblock( ivec3d_t pos );
void G_FreeMapblock( mapblock_t *block );
mapblock_t * G_GiveMapblock( blockmap_t *map, ivec3d_t pos );	// pos in blockmap units
void G_IVec3dToBlockmapUnits( ivec3d_t out, ivec3d_t in );
void G_InsertLNIntoMapblock( mapblock_t *block, mapln_t *n );

//
// g_bmobjects.c
//

mapobject_t * G_NewBlockmapObject( int planenum );
void G_FreeBlockmapObject( mapobject_t *obj );

//
// g_bmlink.c
//

void G_BlockmapLinkObject( blockmap_t *map, mapobject_t *obj );
void G_BlockmapUnlinkObject( blockmap_t *map, mapobject_t *obj );

#endif
