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



// g_defs.h

// type declaration of g_ modules

#ifndef g_defs_h
#define g_defs_h

#include "u_defs.h"
#include "u_expand.h"
#include "lib_modeldefs.h"
#include "imgcache_defs.h"
#include "shader_defs.h"
#include "lib_hmanagerdefs.h"
#include "g_bmdefs.h"
#include "db_lightmapdefs.h"
#include "db_shapedefs.h"
#include "db_uocdefs.h"

#define MAP_CLASS_NAME_PLANES		( "_plane.hobj" )
#define MAP_CLASS_NAME_MAPNODES		( "_mapnode.hobj" )
#define MAP_CLASS_NAME_TEXDEFS		( "_texdef.hobj" )
#define MAP_CLASS_NAME_TEXTURES		( "_texture.hobj" )
#define MAP_CLASS_NAME_VISLEAFS		( "_pvsout_visleaf.hobj" )
#define MAP_CLASS_NAME_BLOCKMAP		( "_blockmap_bspbrush.hobj" )

#define MAP_CLASS_NAME_SHAPES		( "_tjfix_shape.hobj" )
#define MAP_BIN_NAME_GLMESH		( "_glmesh_meshtile.bin" )

#define MAP_CCMAP3_NAME_VOLUME		( "_clustermap_volume.bin" )
#define MAP_CCMAP3_NAME_FIELD		( "_clustermap_field.bin" )

#define MAP_BIN_NAME_LIGHTMAP		( "_lightmap.bin" )
#define MAP_CLASS_NAME_LIGHTMAP		( "_lightmap.hobj" )

typedef struct ccluster_s
{
	ivec3d_t		pos;
	struct ccluster_s *next;	// next in hashline

	cinfo_t		cinfo;		// compression info
	unsigned char	cdata[16];	// compressed data, variable sized
	
} ccluster_t;

typedef struct ccmap3_s
{
	int		hashsize;
	int		clusternum;
	int		clustersize;
	int		cellbytes;

	int		clustercellnum;

	int		hashshift;
	int		keyshift;
	int		keymask;

	int		clusterunits; 
	int		cellunits;

	ccluster_t	*hash[256];	// variable sized
} ccmap3_t;

typedef enum
{
	gMapState_none,

	gMapState_try_init,
	gMapState_is_init,

	gMapState_num
} gMapState;

typedef struct g_map_s 
{
	gMapState		state;

	hmanager_t		*planehm;
	hmanager_t		*mapnodehm;
	hmanager_t		*lightdefhm;
	hmanager_t		*texdefhm;
	hmanager_t		*texturehm;
	hmanager_t		*visleafhm;
	hmanager_t		*blockmaphm;
	hmanager_t		*shapehm;

	blockmap_t		*blockmap;

	ccmap3_t		*volume_ccmap3;
	ccmap3_t		*field_ccmap3;
	
	ic_mgr_t		*imgcache;
	shader_mgr_t		*shader_mgr;       

	db_lightmap_t		*lightmap_db;
	db_shape_t		*shape_db;
	
	db_uoc_t		*uoc_db;

	hmanager_t		*multilayerhm;

	unsigned char		*glmesh;
} g_map_t;

typedef struct g_life_s
{
	
} g_life_t;

typedef struct g_vec_ipo_s
{
	unsigned int		msec1;
	unsigned int		msec2;
	vec3d_t		pos;
	vec3d_t		delta;
} g_vec_ipo_t;

typedef struct g_angle_ipo_s
{
	unsigned int		msec1;
	unsigned int		msec2;
	fp_t		lat, lat_delta;
	fp_t		lon, lon_delta;
} g_angle_ipo_t;

#define WF_MAX_MODELS	( 1024 )
#define WF_MAX_DLIGHTS	( 256 )
#define WF_MAX_SPRITES	( 256 )
#define WF_MAX_FPSYSS	( 256 )
#define WF_MAX_LIGHTS	( 256 )
#define WF_MAX_SHAPES	( 256 )

typedef struct g_state_s
{
//	g_map_t		*map;
	g_life_t	*life;


	// world frame view angle accumulation
	fp_t		view_lat;
	fp_t		view_lon;

	// world frame render setup, set by G_RenderHints
	g_vec_ipo_t	view_origin_ipo;
	g_angle_ipo_t	view_angle_ipo;

	// render frame setup
	vec3d_t		render_origin;
	fp_t		render_lat;
	fp_t		render_lon;
	fp_t		render_yaw;

	matrix3_t	render_matrix;

} g_state_t;



#endif
