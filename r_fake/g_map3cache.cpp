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



// g_map3cache.c

#include "interfaces.h"
#include "g_map3cache.h"
#include "g_shared.h"

/*
  ==============================
  G_NewMap3Cache

  ==============================
*/
map3_cache_t * G_NewMap3Cache( ivec3d_t min, ivec3d_t max, int cellsize )
{
	int		pointnum;
	size_t		size;
	int		xs, ys, zs;
	map3_cache_t	*cache;

	xs = 1+max[0]-min[0];
	ys = 1+max[1]-min[1];
	zs = 1+max[2]-min[2];

	pointnum = xs*ys*zs;

	size = (size_t)&(((map3_cache_t *)0)->data[pointnum*cellsize]);
	cache = (map3_cache_t *) NEWBYTES( size );

	cache->xs = xs;
	cache->ys = ys;
	cache->zs = zs;

	cache->cellbytes = cellsize;

	IVec3dCopy( cache->min, min );
	IVec3dCopy( cache->max, max );

	return cache;
}



/*
  ==============================
  G_NewMap3Cache

  ==============================
*/
void G_FreeMap3Cache( map3_cache_t *cache )
{
	FREE( cache );
}



/*
  ==============================
  G_AccessMap3Cache

  ==============================
*/
void * G_AccessMap3Cache( map3_cache_t *cache, ivec3d_t pos )
{
	int		i;
	ivec3d_t	cpos;
	void		*addr;

	for ( i = 0; i < 3; i++ )
	{
		if ( pos[i] < cache->min[i] || pos[i] > cache->max[i] )
			return NULL;
	}
	
	cpos[0] = pos[0]-cache->min[0];
	cpos[1] = pos[1]-cache->min[1];
	cpos[2] = pos[2]-cache->min[2];
	
	addr = (void *) &cache->data[(cpos[0]+cpos[1]*cache->xs+cpos[2]*cache->xs*cache->ys)*cache->cellbytes];
	return addr;	
}
