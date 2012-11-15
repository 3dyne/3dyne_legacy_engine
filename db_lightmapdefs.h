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



// db_lightmapdefs.h

#ifndef __db_lightmapdefs
#define __db_lightmapdefs

#include "u_defs.h"
#include "g_shareddefs.h"
#include "lib_containerdefs.h"

#include "lib_math.h"

typedef struct lightmap_s
{
	// init by lightmap_class
	unique_t	id_shape;	
	unique_t	id_source;

	// linked to lightsource_class, for intnes to rgb mapping and modulation fx
	struct lightsource_s	*source_ref;
	struct layer_s		*layer_ref;

	int		type;		// 0 = diffuse, 1 = specular
	unsigned char		*pixel_ptr;	// lightmap binary property
	
	// linked by a pixel combiner
	struct lightmap_s	*combine_link;	// list of all lightmaps of different sources combined to one texture layer
	struct lightmap_s	*source_link;	// list of all lightmaps with the same source
	// lightmaps are hashed by id_shape
	struct lightmap_s	*hash_link;
	
} lightmap_t;

// lightsource_t is in g_shareddefs.h


#define X_CACHE_NUM_ENTRY	( 1024*64 )

//
// lightmap combine cache
//
typedef struct x_cache_entry_s
{
	unsigned int	real_addr[4];
	
	// data != NULL => entry in use
	void		*data[4];
	int		size[4];

	unsigned char		lru[4];	// 0 = often used, 255 = seldom used
} x_cache_entry_t;

typedef struct x_cache_s
{
	int	data_mem;
	int	max_mem;
	int	highwater_mem;
	int	lowwater_mem;

	int	rollover;	// if we have to free data continue with this entry

	x_cache_entry_t		entries[X_CACHE_NUM_ENTRY];
} x_cache_t;

#define LIGHTMAP_SHAPE_ID_HASH_SIZE	( 4096 )

typedef struct db_lightmap_s
{
	// no special sort order
	u_list_t	lightsource_list;	
//	u_list_t	lightmap_list;	

	unsigned char	*lightmap_bin;		// lightmap pixel binary

	x_cache_t	cache;			// lightmap combine cache

	lightmap_t     *lightmap_hash[LIGHTMAP_SHAPE_ID_HASH_SIZE];
} db_lightmap_t;

#endif
