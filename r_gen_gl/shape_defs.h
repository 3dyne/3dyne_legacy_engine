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



// shape_defs.h

#ifndef __shape_defs
#define __shape_defs


#include "lib_math.h"
#include "lib_hobjdefs.h"
#include "lib_containerdefs.h"
#include "lib_uocdefs.h"

#include "dlight_defs.h"

#include "u_defs.h"		// for unique_t
#include "layers_defs.h"
#include "mesh_defs.h"
#include "shader_defs.h"

//#include "shape_ctrl_job_defs.h"	// get shape_ctrl_job interface defs

#define SHAPE_BOUNDBOX_SIDE_SPACE	( 2.0 )

typedef enum
{
	ShapeTesselation_simple_face,
	ShapeTesselation_curved_face,
	ShapeTesselation_curved_surface,
	ShapeTesselation_meshtile,		// should be called triangles
	ShapeTesselation_glmesh,		// external tesselated mesh
	ShapeTesselation_num
} shapeTesselation;

typedef struct shape_ctrl_point_s
{
	int		u;
	int		v;
	vec3d_t		p;
}shape_ctrl_point_t;

// 
// shape flags
//

// internal flags
#define SHAPE_FLAGS_HAVE_TESSELATION	( 1 )
#define SHAPE_FLAGS_HAVE_PLANEHINT	( 2 )
#define SHAPE_FLAGS_HAVE_CTRLNUM	( 4 )
#define SHAPE_FLAGS_HAVE_CTRL		( 8 )
#define SHAPE_FLAGS_HAVE_LAYER		( 16 )
#define SHAPE_FLAGS_HAVE_PRIMARY_KEY	( 32 )
//#define SHAPE_FLAGS_HAVE_UOCID		( 64 )
#define SHAPE_FLAGS_HAVE_GLMESH		( 128 )

// public flags
#define SHAPE_FLAGS_IS_VALID		( 256 )	// set by render3.c if shape can be used by a shader

#define SHAPE_FLAGS_MESH_IS_VALID	( 512 )
#define SHAPE_FLAGS_LAYERS_ARE_VALID		( 1024 )
#define SHAPE_FLAGS_SHADER_INFO_IS_VALID	( 2048 )

#define SHAPE_FLAGS_LAYERS_NEED_CHECK		( 4096 ) // set by any layer of the shape, recalc all texcoords that are (null)

typedef struct shape_s
{
	// database primary key
	unique_t	self_id;

	// internal
	int		flags;

	// tesselation
	shapeTesselation	tess_name;

	// plane hint
	vec3d_t			norm;
	fp_t			dist;

	//
	// for internal tesselation
	//

	// lod hint
	int			u_lod;
	int			v_lod;

	// control points
	int			unum;
	int			vnum;
	u_list_t		*cp_list;

	//
	// for external tesselation
	//
	hobj_t			*glmesh_obj;	// mesh info
	unsigned char		*glmesh_base;		// mesh binary

	// layers
	u_list_t		*layer_list;

	// set by tesselation
	mesh_t			*mesh;
	vec3d_t			min;
	vec3d_t			max;

	// paint stuff
	int		vofs;		// offset in the gl backend va
	int		last_rf;	// last render frame number of pvs collect, to avoid multiple paint / shape is visible in this rf
	int		last_local;	// like last_rf but for different purpose

	int		visible_rf;	// last render frame the shape was visible

	// shader frame stuff
//	int			valid;
	int			has_light;
	struct shader_s		*shd;
	layers_array_t		*la;	// cached layer_ref_array from layer_list
	int			texobj;	// tmp sort key

	// dlight test stuff
	dlight_falloff_node_t	*falloff_nodes;	// list of tmp falloff nodes, valid for a render frame

//	char			pad[128-72];
} shape_t;

#endif
