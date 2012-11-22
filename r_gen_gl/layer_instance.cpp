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



// layer_instance.c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "lib_math.h"
#include "g_shared.h"
#include "ca_service.h"
#include "shock.h"

#include "layer_defs.h"
#include "layer_instance.h"

#include "unitex.h"
#include "imgcache.h"

#include "tf2d.h"

static layerMode	layer_mode;
static layerProjection	layer_projection;

static color_rgba_t	layer_color;

typedef struct 
{
	int	type;
	int	vnum;
	vec2d_t		v[4];
} layer_transformation_t;

// projection
static int			layer_transformation_num;
static layer_transformation_t	layer_transformations[MAX_LAYER_TRANSFORMATIONS];

static tf2d_stack_t		*layer_tf2d_stack;

// uv
static bool_t			layer_cps_valid;
static vec2d_t			layer_cps[4];

// texture source
static unitex_t			*layer_source;
static lightmap_t		*layer_lightmap_head;

// layer gl state
static layer_gl_state_t		*layer_glstate;

// layer ordinal
static int			layer_ordinal;

/*
  ==============================
  Layer_BeginInstance

  ==============================
*/
void Layer_BeginInstance( layerMode mode )
{
	layer_mode = mode;
	layer_projection = LayerProjection_none;
	
	layer_tf2d_stack = NULL;
	layer_transformation_num = 0;
	layer_source = NULL;
	layer_lightmap_head = NULL;
	layer_glstate = NULL;
	layer_ordinal = -1;
	layer_cps_valid = false;

	layer_color[0] = 255;
	layer_color[1] = 255;
	layer_color[2] = 255;
	layer_color[3] = 255;
}



/*
  ==============================
  Layer_SetAmbientColor

  ==============================
*/
void	Layer_SetAmbientColor( color_rgba_t color )

{	
	memcpy( layer_color, color, sizeof( color_rgba_t ) );
}


/*
  ==============================
  Layer_SetProjection

  ==============================
*/
void Layer_SetProjection( layerProjection proj )
{
	layer_projection = proj;
}


/*
  ==============================
  Layer_SetTF2DStack

  ==============================
*/
void Layer_SetTF2DStack( tf2d_stack_t *stack )
{
	layer_tf2d_stack = stack;	
}

/*
  ==============================
  Layer_AddRotate

  ==============================
*/
void	Layer_AddRotate( fp_t angle )
{
	vec2d_t		x, y;
	fp_t		alpha;
	fp_t		s, c;

	alpha = angle * D2R;

	s = sin( alpha );
	c = cos( alpha );
	
	x[0] = c;
	x[1] = -s;
	y[0] = s;
	y[1] = c;		

	if ( ! ( layer_mode & LayerMode_projection ) )
		__error( "request for projectiv transformation in none projectiv layer\n" );

	if ( layer_transformation_num >= MAX_LAYER_TRANSFORMATIONS )
		__error( "reached MAX_LAYER_TRANSFORMATIONS\n" );

	layer_transformations[layer_transformation_num].type = LAYER_FLAGS_TRANSFORMATION_AXIS;
	Vec2dCopy( layer_transformations[layer_transformation_num].v[0], x );
	Vec2dCopy( layer_transformations[layer_transformation_num].v[1], y );
	layer_transformations[layer_transformation_num].vnum = 2;
	
	layer_transformation_num++;
}


/*
  ==============================
  Layer_AddAxis

  ==============================
*/
void	Layer_AddAxis( vec2d_t x, vec2d_t y )
{
	if ( ! ( layer_mode & LayerMode_projection ) )
		__error( "request for projectiv transformation in none projectiv layer\n" );

	if ( layer_transformation_num >= MAX_LAYER_TRANSFORMATIONS )
		__error( "reached MAX_LAYER_TRANSFORMATIONS\n" );

	layer_transformations[layer_transformation_num].type = LAYER_FLAGS_TRANSFORMATION_AXIS;
	Vec2dCopy( layer_transformations[layer_transformation_num].v[0], x );
	Vec2dCopy( layer_transformations[layer_transformation_num].v[1], y );
	layer_transformations[layer_transformation_num].vnum = 2;
	
	layer_transformation_num++;
}

/*
  ==============================
  Layer_AddScale

  ==============================
*/
void Layer_AddScale( vec2d_t scale )
{
	if ( ! ( layer_mode & LayerMode_projection ) )
		__error( "request for projectiv transformation in none projectiv layer\n" );

	if ( layer_transformation_num >= MAX_LAYER_TRANSFORMATIONS )
		__error( "reached MAX_LAYER_TRANSFORMATIONS\n" );

	layer_transformations[layer_transformation_num].type = LAYER_FLAGS_TRANSFORMATION_SCALE;
	Vec2dCopy( layer_transformations[layer_transformation_num].v[0], scale );
	layer_transformations[layer_transformation_num].vnum = 1;

	layer_transformation_num++;
}

/*
  ==============================
  Layer_AddShift

  ==============================
*/
void Layer_AddShift( vec2d_t shift )
{
	if ( ! ( layer_mode & LayerMode_projection ) )
		__error( "request for projectiv transformation in none projectiv layer\n" );

	if ( layer_transformation_num >= MAX_LAYER_TRANSFORMATIONS )
		__error( "reached MAX_LAYER_TRANSFORMATIONS\n" );

	layer_transformations[layer_transformation_num].type = LAYER_FLAGS_TRANSFORMATION_SHIFT;
	Vec2dCopy( layer_transformations[layer_transformation_num].v[0], shift );
	layer_transformations[layer_transformation_num].vnum = 1;	

	layer_transformation_num++;	
}


/*
  ==============================
  Layer_SetUVCtrl

  ==============================
*/
void Layer_SetUVCtrl( vec2d_t cps[4] )
{
	if ( ! ( layer_mode & LayerMode_uv ) )
		__error( "try to set uv control points in none uv layer\n" );

	memcpy( layer_cps, cps, 4*sizeof(vec2d_t) );
	layer_cps_valid = true;
}


/*
  ==============================
  Layer_SetUniTex

  ==============================
*/
void Layer_SetUniTex( unitex_t *uni )
{
	layer_source = uni;
}

/*
  ==============================
  Layer_SetLightmapHead

  ==============================
*/
void Layer_SetLightmapHead( lightmap_t *lightmap_head )
{
	layer_lightmap_head = lightmap_head;
}

/*
  ==============================
  Layer_SetGLstate

  ==============================
*/
void Layer_SetGLstate( layer_gl_state_t *state )
{
	layer_glstate = state;
}

/*
  ==============================
  Layer_SetOrdinal

  ==============================
*/
void Layer_SetOrdinal( int ordinal )
{
	layer_ordinal = ordinal;
}

/*
  ==============================
  Layer_EndInstance

  ==============================
*/
layer_t * Layer_EndInstance( void )
{
	int		i, j;
	int		flags;
	layer_t		*layer;

	// layer mempack layout:
	// [	int layerFlags
	//	int blendMode
	//	void *	{ gltex | subimage }
	//	projective transnum | uv ctrl
	//	projective trans ...


	layer = NEWTYPE( layer_t );

	// layer flags

	flags = 0;

	if ( layer_mode & LayerMode_projection )
		flags |= LAYER_FLAGS_MODE_PROJECTION;
	else if ( layer_mode & LayerMode_uv )
		flags |= LAYER_FLAGS_MODE_UV;
	else
		__error( "no texcoord generation type\n" );
	
	if ( layer_mode & LayerMode_Texel01 )
		flags |= LAYER_FLAGS_MODE_TEXEL01;
	else if ( layer_mode & LayerMode_Texel0N )
		flags |= LAYER_FLAGS_MODE_TEXEL0N;
	else
		__error( "no texcoord access type\n" );
	
	if ( layer_mode & LayerMode_projection )
	{
		if ( layer_projection == LayerProjection_x )
			flags |= LAYER_FLAGS_PROJECTION_X;
		else if ( layer_projection == LayerProjection_y )
			flags |= LAYER_FLAGS_PROJECTION_Y;
		else if ( layer_projection == LayerProjection_z )
			flags |= LAYER_FLAGS_PROJECTION_Z;
		else
			__error( "projectiv layer without valid projection plane\n" );
	}

	layer->flags = flags;
	
	// layer source
	if ( !layer_source )
		__error( "(null) layer texture source\n" );
	
	layer->real_source = layer_source;

	// layer lightmap_head
	if ( layer_lightmap_head )
	{
		lightmap_t	*lmap;

		layer->lightmap_head = layer_lightmap_head;
		layer->flags |= LAYER_FLAGS_HAVE_LIGHTMAP_HEAD;

		for ( lmap = layer_lightmap_head ; lmap ; lmap=lmap->combine_link )
		{
			lmap->layer_ref = layer;
		}
	}

	// layer glstate
	if ( !layer_glstate )
		__error( "(null) layer glstate\n" );
	
	layer->glstate = layer_glstate;

	// layer ordinal
	if ( layer_ordinal == -1 )
		__error( "(-1) layer ordinal\n" );
	
	layer->ordinal = layer_ordinal;

	// layer ambient color
	memcpy( layer->ambient_color, layer_color, sizeof( color_rgba_t ) );

	// stack
	if ( layer_tf2d_stack )
	{
		layer->tf2d_stack = layer_tf2d_stack;
		layer->flags |= LAYER_FLAGS_HAVE_TF2D_STACK;
	}
	
	// put the rest into a mempack

	U_BeginMemPackInstance();

	if ( layer_mode & LayerMode_projection )
	{
		// projectiv layer

		// transformation num
		U_MemPacki32( layer_transformation_num );

		for ( i = 0; i < layer_transformation_num; i++ )
		{
			U_MemPacki32( layer_transformations[i].type );
			for ( j = 0; j < layer_transformations[i].vnum; j++ )
			{
				U_MemPackf32v2( layer_transformations[i].v[j] );
			}
		}
	}       
	else if ( layer_mode & LayerMode_uv )
	{
		// uv layer
		
		if ( layer_cps_valid == false )
			__error( "(null) uv control points\n" );

		for ( i = 0; i < 4; i++ )
		{
			U_MemPackf32v2( layer_cps[i] );
		}
	}

	layer->packed_info = U_EndMemPackInstance();

	return layer;

}


/*
  ==============================
  Layer_Free

  ==============================
*/
void Layer_Free( layer_t *layer )
{
	if ( layer->packed_info )
		U_FreeMemPack( layer->packed_info );
	
	FREE( layer );
}


/*
  ==============================
  Layer_CleanUp

  ==============================
*/
void Layer_CleanUp( layer_t *layer )
{
	// free glstate
	if ( layer->glstate )
	{
		FREE( layer->glstate );
	}

	// free unitex
	if ( layer->real_source )
	{
		ic_subimage_t	*sub;

		sub = UniTex_IsReal_subimage( layer->real_source );
		if ( sub )
		{
			if ( sub->pixels )
				FREE( sub->pixels );

			IC_FreeSubImage( sub );
		}

		FREE( layer->real_source );
	}

	// free packed_info
	if ( layer->packed_info )
	{
		U_FreeMemPack( layer->packed_info );
	}

	// free tf2d_stack
	if ( layer->tf2d_stack )
	{
		TF2D_CleanUpStack( layer->tf2d_stack );
		FREE( layer->tf2d_stack );
	}

	// free mesh
	if ( layer->tc )
	{
		FREE( layer->tc );
	}
}
