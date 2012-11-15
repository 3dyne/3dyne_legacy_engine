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



// g_bmlink.c

#include "r_interfaces.h"
#include "g_blockmap.h"


/*
  ==============================
  G_BlockmapLinkObject

  ==============================
*/
void G_BlockmapLinkObject( blockmap_t *map, mapobject_t *obj )
{
	ivec3d_t		min, max;
	ivec3d_t		bpos;

	if ( obj->linknodes )
		__error( "G_BlockmapLinkObject: object already linked.\n" );

	G_IVec3dToBlockmapUnits( min, obj->min );
	G_IVec3dToBlockmapUnits( max, obj->max );

	for ( bpos[0] = min[0]; bpos[0] <= max[0]; bpos[0]++ )
	{
		for ( bpos[1] = min[1]; bpos[1] <= max[1]; bpos[1]++ )
		{
			for ( bpos[2] = min[2]; bpos[2] <= max[2]; bpos[2]++ )
			{
				mapblock_t	*block;
				mapln_t		*n;

				block = G_GiveMapblock( map, bpos );
				n = G_NewBlockmapLinknode();
				n->ref = obj;

				// the object knows its own linknodes
				n->next = obj->linknodes;
				obj->linknodes = n;
				
				// insert linknode into mapblock
				G_InsertLNIntoMapblock( block, n );
			}
		}
	}
}



/*
  ==============================
  G_BlockmapUnlinkObject

  ==============================
*/

void G_BlockmapUnlinkObject( blockmap_t *map, mapobject_t *obj )
{
	mapln_t		*n, *nnext;

	for ( n = obj->linknodes; n ; n=nnext )
	{
		nnext = n->next;

		// linknode removes itself from a mapblock
		G_RemoveLN( n );
		G_FreeBlockmapLinknode( n );
	}
}

