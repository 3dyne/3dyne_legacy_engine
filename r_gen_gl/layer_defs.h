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



// layer_defs.h

#ifndef layer_defs
#define layer_defs

#include "lib_math.h"
#include "u_mempack.h"

#include "unitex_defs.h"
#include "mesh_defs.h"
#include "tf2d_defs.h"
//#include "shape_defs.h"

#include "gl_defs.h"	// for gl
#include "u_defs.h"	// color_rgba_t

#include "db_lightmapdefs.h"

#define LAYER_STATE_FLAGS_DEPTH_MASK		( 1 )
#define LAYER_STATE_FLAGS_SHADE_LAST		( 2 )

#define LAYER_STATE_FLAGS_SHADE_P3              ( 2 ) 
#define LAYER_STATE_FLAGS_SHADE_P2              ( 4 )                           
#define LAYER_STATE_FLAGS_SHADE_P1              ( 8 ) 

typedef struct layer_gl_state_s
{
	int		flags;
	GLenum		sfactor;
	GLenum		dfactor;
} layer_gl_state_t;

// the enums are only public items
typedef enum
{
	LayerMode_none,
	
	LayerMode_projection = 1,
	LayerMode_uv = 2,
	LayerMode_Texel01 = 4,
	LayerMode_Texel0N = 8,

	LayerMode_last
} layerMode;

typedef enum
{
	LayerProjection_none,

	LayerProjection_x,
	LayerProjection_y,
	LayerProjection_z,

	LayerProjection_num
} layerProjection;

// internal flags

#define LAYER_FLAGS_TRANSFORMATION_AXIS		( 1L ) 
#define LAYER_FLAGS_TRANSFORMATION_SCALE	( 1L<<1 ) 
#define LAYER_FLAGS_TRANSFORMATION_SHIFT	( 1L<<2 ) 

#define LAYER_FLAGS_MODE_PROJECTION		( 1L<<3 )
#define LAYER_FLAGS_MODE_UV			( 1L<<4 )

#define LAYER_FLAGS_PROJECTION_X		( 1L<<5 )
#define LAYER_FLAGS_PROJECTION_Y		( 1L<<6 )
#define LAYER_FLAGS_PROJECTION_Z		( 1L<<7 )

#define LAYER_FLAGS_MODE_TEXEL01		( 1L<<8 )
#define LAYER_FLAGS_MODE_TEXEL0N		( 1L<<9 )

#define LAYER_FLAGS_HAVE_TF2D_STACK		( 1L<<10 )
#define LAYER_FLAGS_HAVE_LIGHTMAP_HEAD		( 1L<<11 )

// extern flags
#define LAYER_FLAGS_VALID_LIGHTMAP_COMBINE	( 1L<<16 )

#define MAX_LAYER_TRANSFORMATIONS	( 16 )

typedef struct layer_s
{
	struct shape_s		*shape;		// layer is part of this shape, set by Shape_AddLayer

	int		flags;

	int			ordinal;
	layer_gl_state_t	*glstate;

	// ambient/vertex color
	color_rgba_t		ambient_color;
	
	unitex_t	*real_source;
	lightmap_t	*lightmap_head;
	
	mempack_t	*packed_info;	// type coded into flags
	tf2d_stack_t	*tf2d_stack;
	
	// set by CreateTexCoords
	mesh_t		*tc;
} layer_t;

#endif
