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



// r_shapes.c

#include "interfaces.h"
#include "r_interfaces.h"
#include "r_interface.h"
#include "render.h"

#include "shape_instance.h"
#include "layer_instance.h"
#include "unitex.h"
#include "imgcache.h"
#include "layers.h"
#include "tf2d.h"

#include "db_lightmap.h"
#include "db_shape.h"

#include "u_pixel.h"

#include "r_shpctrljobs.h"

#define MAX_SFACE_SHAPES		( 10240 )
#define MAX_CFACE_SHAPES		( 1024 )
#define MAX_CSURF_SHAPES		( 1024 )
#define MAX_MESHTILE_SHAPES		( 1024 )

int		r_sfaceshapenum;
int		r_cfaceshapenum;
int		r_csurfshapenum;
int		r_meshtileshapenum;

shape_t		*r_sfaceshapes[MAX_SFACE_SHAPES];
shape_t		*r_cfaceshapes[MAX_CFACE_SHAPES];
shape_t		*r_csurfshapes[MAX_CSURF_SHAPES];
shape_t		*r_meshtileshapes[MAX_MESHTILE_SHAPES];


/*
  ==============================
  LinkLightmaps

  ==============================
*/

static lightmap_t	*link_lightmap_head = NULL;
static int		link_lightmap_type;
static int		link_lightmap_num;

static void LinkLightmapsSelect_func( lightmap_t *lmap )
{
	if ( lmap->type != link_lightmap_type )
	{
		return;
	}

	lmap->combine_link = link_lightmap_head;
	link_lightmap_head = lmap;

	link_lightmap_num++;
}

static lightmap_t * LinkLightmaps( lightmap_db_cpp *db, unique_t id_shape, int type )
{
	link_lightmap_head = NULL;
	link_lightmap_type = type;
	link_lightmap_num = 0;

	//LightmapDB_SelectLightmapsWhereShapeID( db, id_shape, LinkLightmapsSelect_func );
    db->SelectLightmapsWhereShapeID( id_shape, LinkLightmapsSelect_func );
    
    
	return link_lightmap_head;
}

/*
  ==============================
  R_SetupSFaceMultiLayerFromClass

  ==============================
*/

void R_RegisterShapeCtrlJob( shape_ctrl_job_t *job );

void R_SetupSFaceLayer( hobj_t *texdef, hobj_t *layer )
{
	// from texdef
	vec2d_t		shift;
	vec2d_t		scale;
	fp_t		rotate;
	int		projection;
	
	g_resource_t	*gltex_res;
	unitex_t	*uni;	

	tf2d_stack_t	*tf2d_stack;

	hpair_t		*pair;
	layer_gl_state_t	*glstate;

	layer_t		*l;

	hobj_search_iterator_t		iter;
	hobj_t				*job;

	// from layer
	vec2d_t		scale2;	// additional scale 
	color_rgba_t	color;

	// get texdef transform
	EasyFindVec2d( shift, texdef, "shift" );
	EasyFindVec2d( scale, texdef, "scale" );
	EasyFindFloat( &rotate, texdef, "rotate" );
	EasyFindInt( &projection, texdef, "flags" );

	// get layer transform
	pair = FindHPair( layer, "scale" );
	if ( pair )
	{
		HPairCastToVec2d( scale2, pair );
	}
	else 
	{
		scale2[0] = 1.0;
		scale2[1] = 1.0;
	}

	// get layer ambient color
	color[0] = 255;
	color[1] = 255;
	color[2] = 255;
	color[3] = 255;
	pair = FindHPair( layer, "color" );
	if ( pair )
	{
		int			i;
		vec3d_t		v;
		HPairCastToVec3d( v, pair );

		for ( i = 0; i < 3; i++ )
		{
			if ( v[i] < 0.0 )
				v[i] = 0.0;
			if ( v[i] > 1.0 )
				v[i] = 1.0;

			color[i] = (unsigned char) (v[i]*255.0);
		}		
	}
	
	pair = FindHPair( layer, "alpha" );
	if ( pair )
	{
		fp_t		a;
		HPairCastToFloat( &a, pair );

		if ( a < 0.0 )
			a = 0.0;
		if ( a > 1.0 )
			a = 1.0;

		color[3] = (unsigned char) (a*255.0);
	}

	pair = FindHPair( layer, "gltex_res" );
	if ( !pair )
		__error( "missing key 'gltex_res'\n" );

	gltex_res = G_ResourceSearch( g_rs, pair->value );
	
	if ( !gltex_res )
		__error( "can't find gltex resource '%s'\n", pair->value );

	uni = UniTex_NewFrom_gltexres( gltex_res );
	
	// layer gl state
	glstate = NEWTYPE( layer_gl_state_t );	


	//
	// blend sfactor
	//

	pair = FindHPair( layer, "sfactor" );
	if ( !pair )
		__error( "missing key 'sfactor'\n" );

	if ( !strcmp( pair->value, "GL_ONE" ) )
	{
		glstate->sfactor = GL_ONE;
	}
	else if ( !strcmp( pair->value, "GL_ZERO" ) )
	{
		glstate->sfactor = GL_ZERO;
	}
	else if ( !strcmp( pair->value, "GL_SRC_COLOR" ) )
	{
		glstate->sfactor = GL_SRC_COLOR;
	}
	else if ( !strcmp( pair->value, "GL_SRC_ALPHA" ) )
	{
		glstate->sfactor = GL_SRC_ALPHA;
	}
	else
	{
		__error( "unknown sfactor '%s'\n", pair->value );
	}

	//
	// blend dfactor
	//

	pair = FindHPair( layer, "dfactor" );
	if ( !pair )
		__error( "missing key 'dfactor'\n" );
	
	if ( !strcmp( pair->value, "GL_ONE" ) )
	{
		glstate->dfactor = GL_ONE;
	}
	else if ( !strcmp( pair->value, "GL_ZERO" ) )
	{
		glstate->dfactor = GL_ZERO;
	}
	else if ( !strcmp( pair->value, "GL_ONE_MINUS_SRC_ALPHA" ) )
	{
		glstate->dfactor = GL_ONE_MINUS_SRC_ALPHA;
	}
	else
	{
		__error( "unknown dfactor '%s'\n", pair->value );
	}

	
	Layer_BeginInstance( (layerMode)(LayerMode_projection | LayerMode_Texel0N) );
	Layer_SetGLstate( glstate );
	Layer_SetOrdinal( atoi( layer->name ) );
	Layer_SetUniTex( uni );

	Layer_SetAmbientColor( color );

	if ( (projection&3) == ProjectionType_X )
	{
		Layer_SetProjection( LayerProjection_x );
	}
	else if ( (projection&3) == ProjectionType_Y )
	{
		Layer_SetProjection( LayerProjection_y );
	}
	else if ( (projection&3) == ProjectionType_Z )
	{
		Layer_SetProjection( LayerProjection_z );
	}
	else
	{
		__error( "can't get projection plane\n" );
	}
	
	//
	// setup layer's transformation stack
	//

	tf2d_stack = NEWTYPE( tf2d_stack_t );
	TF2D_InitStack( tf2d_stack );

	Layer_SetTF2DStack( tf2d_stack );

	{
		tf2d_t		*tf2d;
		tf2d_matrix_t	*tf2d_matrix;
		tf2d = TF2D_CreateMatrix();
		tf2d_matrix = TF2D_IsMatrix( tf2d );
		TF2D_SetupMatrixByAngle( tf2d_matrix, -rotate );
		TF2D_StackPush( tf2d_stack, tf2d );
	}
	{
		tf2d_t		*tf2d;
		tf2d_scale_t	*tf2d_scale;
 
		scale[0] *= scale2[0];
		scale[1] *= scale2[1];
		
		scale[0] = 1.0/scale[0];
		scale[1] = 1.0/scale[1];		
		
		tf2d = TF2D_CreateScale();
		tf2d_scale = TF2D_IsScale( tf2d );
		Vec2dCopy( tf2d_scale->scale, scale );
		TF2D_StackPush( tf2d_stack, tf2d );
	}
	{
		tf2d_t		*tf2d;
		tf2d_shift_t	*tf2d_shift;
		
		shift[0] = -shift[0];		
		shift[1] = -shift[1];		
		tf2d = TF2D_CreateShift();
		tf2d_shift = TF2D_IsShift( tf2d );
		Vec2dCopy( tf2d_shift->shift, shift );
		TF2D_StackPush( tf2d_stack, tf2d );
	}


	l = Layer_EndInstance();
	
	Shape_AddLayer( l );

	//
	// Spawn ShapeCtrlJobs
	//

	InitClassSearchIterator( &iter, layer, "job" );
	for ( ; ( job = SearchGetNextClass( &iter ) ) ; )
	{
		if ( !strcmp( job->name, "ModulateAlpha" ) )
		{			
			R_RegisterShapeCtrlJob( ShapeCtrlJob_Create_ModulateAlpha( job, l ) );
		}
		else if ( !strcmp( job->name, "ModulateColor" ) )
		{			
			R_RegisterShapeCtrlJob( ShapeCtrlJob_Create_ModulateColor( job, l ) );
		}
		else if ( !strcmp( job->name, "ShiftTexCoord" ) )
		{			
			R_RegisterShapeCtrlJob( ShapeCtrlJob_Create_ShiftTexCoord( job, l ) );
		}
		else if ( !strcmp( job->name, "EnvMap" ) )
		{			
			R_RegisterShapeCtrlJob( ShapeCtrlJob_Create_EnvMap( job, l ) );
		}
		else if ( !strcmp( job->name, "DetailMap" ) )
		{			
			R_RegisterShapeCtrlJob( ShapeCtrlJob_Create_DetailMap( job, l ) );
		}
		else
		{
			__warning( "unknown ShapeCtrlJob '%s', ignore it\n", job->name );
		}
	}	
}

void R_SetupSFaceMultiLayerFromClass( hobj_t *texdef, hobj_t *multilayer )
{
	hobj_search_iterator_t		iter;
	hobj_t				*layer;

	InitClassSearchIterator( &iter, multilayer, "layer" );
	for ( ; ( layer = SearchGetNextClass( &iter ) ) ; )
	{
//		__named_message( "ordinal %s\n", layer->name );
		R_SetupSFaceLayer( texdef, layer );

	}
}

/*
  ==============================
  R_SetupSFaceShape

  ==============================
*/
shape_t * R_SetupSFaceShape( hobj_t *shape, g_map_t *map )
{
	hobj_t		*plane;	
	hobj_t		*polygon;
	vec3d_t		norm;
	fp_t		dist;
	int		pointnum;
	int		i;
	shape_t		*shp;

	int		multi_flags;

	multi_flags = LAYER_STATE_FLAGS_DEPTH_MASK;

	polygon = FindClassType( shape, "polygon" );
	if ( !polygon )
		__error( "missing class 'polygon' in shape '%s'\n", shape->name );

	// get control points
	EasyFindInt( &pointnum, polygon, "num" );

	if ( pointnum < 3 )
	{
		printf( " shape '%s' is degenerated\n", shape->name );
		return NULL;
	}

	Shape_BeginInstance();
	Shape_SetPrimaryKey( StringToUnique( shape->name ) );
	Shape_SetTesselation( ShapeTesselation_simple_face );

	// get plane
	plane = EasyLookupClsref( polygon, "plane", map->planehm );

	EasyFindVec3d( norm, plane, "norm" );
	EasyFindFloat( &dist, plane, "dist" );

	Shape_SetPlaneHint( norm, dist );

	Shape_SetCtrlPointNum( pointnum, 1 );
	for ( i = 0; i < pointnum; i++ )
	{
		vec3d_t		v;
		char		str[256];		

		sprintf( str, "%d", i );
		EasyFindVec3d( v, polygon, str );		
		Shape_AddCtrlPoint( Shape_NewCtrlPointUV3fv( i, 0, v ) );
	}

	//
	// texture layer
	//

	if ( 1 )
	{
		// texture proj_texdef0

		hobj_t		*texdef0;
		hobj_t		*texdef_real;
		hpair_t		*pair;

		vec2d_t		shift;
		vec2d_t		scale;
		fp_t		rotate;
//		vec2d_t		vecs[2];
		int		projection;
		
		hobj_t		*texture;
		g_resource_t	*gltex_res;
		char		str[256];

		layer_t		*layer;
		unitex_t	*uni;
		tf2d_stack_t	*tf2d_stack;

		texdef0 = FindClassType( shape, "proj_texdef0" );
		if ( !texdef0 )
			__error( "missing class 'proj_texdef0' in shape '%s'\n", shape->name );

		texdef_real = EasyLookupClsref( texdef0, "texdef", map->texdefhm );

		EasyFindVec2d( shift, texdef_real, "shift" );
		EasyFindVec2d( scale, texdef_real, "scale" );
		EasyFindFloat( &rotate, texdef_real, "rotate" );
//		EasyFindVec2d( vecs[0], texdef_real, "vec0" );
//		EasyFindVec2d( vecs[1], texdef_real, "vec1" );
		EasyFindInt( &projection, texdef_real, "flags" );

		texture = EasyLookupClsref( texdef_real, "texture", map->texturehm );
		pair = FindHPair( texture, "ident" );
		if ( !pair )
			__error( "missing key 'ident' in texture class '%s'\n", texture->name );

		//
		// check ident for multilayer definition
		//
		{
			hobj_t		*multilayer;

			multilayer = HManagerSearchClassName( map->multilayerhm, pair->value );
			if ( multilayer )
			{
//				__named_message( "MULTILAYER DEFINITON !\n" );

				R_SetupSFaceMultiLayerFromClass( texdef_real, multilayer );

				pair = FindHPair( multilayer, "depthmask" );
				if ( !pair )
				{
					// default is depthmask = true
					multi_flags |= LAYER_STATE_FLAGS_DEPTH_MASK;
				}
				else
				{
					if ( !strcmp( pair->value, "0" ) )
					{
						multi_flags &= ~LAYER_STATE_FLAGS_DEPTH_MASK;

						multi_flags |= LAYER_STATE_FLAGS_SHADE_P3;

					}
					else
					{
						multi_flags |= LAYER_STATE_FLAGS_SHADE_P3;

					}
				}
				

				pair = FindHPair( multilayer, "pass" );
				if ( pair )
				{
					multi_flags &= ~LAYER_STATE_FLAGS_SHADE_P1;
					multi_flags &= ~LAYER_STATE_FLAGS_SHADE_P2;
					multi_flags &= ~LAYER_STATE_FLAGS_SHADE_P3;

					if ( !strcmp( "1", pair->value ) )
					{
						multi_flags |= LAYER_STATE_FLAGS_SHADE_P1;
					}
					else if ( !strcmp( "2", pair->value ) )
					{
						multi_flags |= LAYER_STATE_FLAGS_SHADE_P2;
					}
					else if ( !strcmp( "3", pair->value ) )
					{
						multi_flags |= LAYER_STATE_FLAGS_SHADE_P3;
					}
					else
					{
						__error( "unknown pass\n" );
					}
				}



				goto texture_skip;
			}
		}
		
		// build gltex resource name from ident
		sprintf( str, "gltex.%s", pair->value );
		strcrpl( str, '/', '.' );
//		printf( "gltex: %s\n", str );
		gltex_res = G_ResourceSearch( g_rs, str );

		uni = UniTex_NewFrom_gltexres( gltex_res );
		
		Layer_BeginInstance( (layerMode)(LayerMode_projection | LayerMode_Texel0N) );
		
		{
			// hack: build layer_gl_state_t
			layer_gl_state_t	*glstate;

			glstate = NEWTYPE( layer_gl_state_t );
			glstate->sfactor = GL_ONE;
			glstate->dfactor = GL_ZERO;
			Layer_SetGLstate( glstate );
		}
		Layer_SetOrdinal( LAYER_TEXTURE_1 );

		Layer_SetUniTex( uni );
		
		if ( (projection&3) == ProjectionType_X )
		{
			Layer_SetProjection( LayerProjection_x );
		}
		else if ( (projection&3) == ProjectionType_Y )
		{
			Layer_SetProjection( LayerProjection_y );
		}
		else if ( (projection&3) == ProjectionType_Z )
		{
			Layer_SetProjection( LayerProjection_z );
		}
		else
		{
			__error( "can't get projection plane\n" );
		}
		
		//
		// setup layer's transformation stack
		//
		
		tf2d_stack = NEWTYPE( tf2d_stack_t );
		TF2D_InitStack( tf2d_stack );
		
		Layer_SetTF2DStack( tf2d_stack );
		
		
//		if ( 1 || projection&ProjectionType_Vecs )
		{
			tf2d_t		*tf2d;
			tf2d_matrix_t	*tf2d_matrix;
			tf2d = TF2D_CreateMatrix();
			tf2d_matrix = TF2D_IsMatrix( tf2d );
			TF2D_SetupMatrixByAngle( tf2d_matrix, -rotate );
			TF2D_StackPush( tf2d_stack, tf2d );
		}
		
		{
			tf2d_t		*tf2d;
			tf2d_scale_t	*tf2d_scale;
			
			scale[0] = 1.0/scale[0];
			scale[1] = 1.0/scale[1];

			tf2d = TF2D_CreateScale();
			tf2d_scale = TF2D_IsScale( tf2d );
			Vec2dCopy( tf2d_scale->scale, scale );
			TF2D_StackPush( tf2d_stack, tf2d );
		}


//		if ( 1 || projection&ProjectionType_Shift )
		{
			tf2d_t		*tf2d;
			tf2d_shift_t	*tf2d_shift;
			
			shift[0] = -shift[0];		
			shift[1] = -shift[1];		
			tf2d = TF2D_CreateShift();
			tf2d_shift = TF2D_IsShift( tf2d );
			Vec2dCopy( tf2d_shift->shift, shift );
			TF2D_StackPush( tf2d_stack, tf2d );
		}



		
		layer = Layer_EndInstance();

		Shape_AddLayer( layer );
	}

texture_skip:

	//
	// diffuse layer
	//


#if 1
	{
		hobj_t		*lightdef;
		vec2d_t		shift;
		int		width;
		int		height;
		int		projection;
		fp_t		patchsize;

		ic_subimage_t	*sub;
		unitex_t	*uni;
		
		lightmap_t	*lightmap_head;

		layer_t		*layer;

		lightdef = FindClassType( shape, "proj_lightdef" );
		if ( !lightdef )
		{
			goto no_lightdef;
		}

		EasyFindVec2d( shift, lightdef, "shift" );
		EasyFindInt( &width, lightdef, "width" );
		EasyFindInt( &height, lightdef, "height" );
		EasyFindFloat( &patchsize, lightdef, "patchsize" );
		EasyFindInt( &projection, lightdef, "projection" );

		lightmap_head = LinkLightmaps( a_map->lightmap_db, StringToUnique( shape->name ), 0 );
		if ( !lightmap_head && 0 )
			goto no_diffuse;
		
		{
			int		lightmap_bytes;
			void		*lightmap_pixels;
					
			lightmap_bytes = width*height*4;
			lightmap_pixels = NEWBYTES( lightmap_bytes );
			
			sub = IC_NewSubImage( width, height );
			IC_SubImageSetPixels( sub, lightmap_pixels );
		}
		
		uni = UniTex_NewFrom_subimage( sub );

		// create diffuse layer
		
		Layer_BeginInstance( (layerMode)(LayerMode_projection | LayerMode_Texel0N) );
		
		{
			// hack: build layer_gl_state_t
			layer_gl_state_t	*glstate;

			glstate = NEWTYPE( layer_gl_state_t );
			glstate->sfactor = GL_ZERO;
			glstate->dfactor = GL_SRC_COLOR;
			Layer_SetGLstate( glstate );
		}

		Layer_SetOrdinal( LAYER_DIFFUSE_1 );
		Layer_SetUniTex( uni );
		Layer_SetLightmapHead( lightmap_head );

		if ( (projection&3) == ProjectionType_X )
		{
			Layer_SetProjection( LayerProjection_x );
		}
		else if ( (projection&3) == ProjectionType_Y )
		{
			Layer_SetProjection( LayerProjection_y );
		}
		else if ( (projection&3) == ProjectionType_Z )
		{
			Layer_SetProjection( LayerProjection_z );
		}
		else
		{
			__error( "can't get projection plane\n" );
		}		

		{
			shift[0] = -shift[0];
			shift[1] = -shift[1];
			Layer_AddShift( shift );
		}
		
		{
			vec2d_t		scale;
			scale[0] = 1.0 / patchsize;
			scale[1] = 1.0 / patchsize;
			Layer_AddScale( scale );
		}

		layer = Layer_EndInstance();
		
		Shape_AddLayer( layer );

	no_diffuse:
		{}
	}

#endif

	//
	// specular layer
	//


#if 1
	{
		hobj_t		*lightdef;
		vec2d_t		shift;
		int		width;
		int		height;
		int		projection;
		fp_t		patchsize;

		ic_subimage_t	*sub;
		unitex_t	*uni;

		layer_t		*layer;
		lightmap_t	*lightmap_head;

		lightdef = FindClassType( shape, "proj_lightdef" );
		if ( !lightdef )
		{
//			__error( "missing class 'proj_lightdef' in shape '%s'\n", shape->name );
			goto no_lightdef;
		}

		EasyFindVec2d( shift, lightdef, "shift" );
		EasyFindInt( &width, lightdef, "width" );
		EasyFindInt( &height, lightdef, "height" );
		EasyFindFloat( &patchsize, lightdef, "patchsize" );
		EasyFindInt( &projection, lightdef, "projection" );


		lightmap_head = LinkLightmaps( a_map->lightmap_db, StringToUnique( shape->name ), 1 );
		if ( !lightmap_head )
			goto no_specular;
		// setup specular lightmap
		{
			int		lightmap_bytes;
			void		*lightmap_pixels;
			
			// check database for specular lightmaps

			if ( !lightmap_head )
				goto no_specular;

			lightmap_bytes = width*height*4;
			lightmap_pixels = NEWBYTES( lightmap_bytes );
			
			sub = IC_NewSubImage( width, height );
			IC_SubImageSetPixels( sub, lightmap_pixels );
		}
		
		uni = UniTex_NewFrom_subimage( sub );

		// create specular layer
		
		Layer_BeginInstance( (layerMode)(LayerMode_projection | LayerMode_Texel0N) );
		
		{
			// hack: build layer_gl_state_t
			layer_gl_state_t	*glstate;

			glstate = NEWTYPE( layer_gl_state_t );
			glstate->sfactor = GL_ONE;
			glstate->dfactor = GL_ONE;
			Layer_SetGLstate( glstate );
		}

		Layer_SetOrdinal( LAYER_SPECULAR_1 );
		Layer_SetUniTex( uni );
		Layer_SetLightmapHead( lightmap_head );

		if ( (projection&3) == ProjectionType_X )
		{
			Layer_SetProjection( LayerProjection_x );
		}
		else if ( (projection&3) == ProjectionType_Y )
		{
			Layer_SetProjection( LayerProjection_y );
		}
		else if ( (projection&3) == ProjectionType_Z )
		{
			Layer_SetProjection( LayerProjection_z );
		}
		else
		{
			__error( "can't get projection plane\n" );
		}		

		{
			shift[0] = -shift[0];
			shift[1] = -shift[1];
			Layer_AddShift( shift );
		}
		
		{
			vec2d_t		scale;
			scale[0] = 1.0 / patchsize;
			scale[1] = 1.0 / patchsize;
			Layer_AddScale( scale );
		}

		layer = Layer_EndInstance();
		
		Shape_AddLayer( layer );

	no_specular:
		{}
	}

#endif

no_lightdef:

	shp = Shape_EndInstance();


	//
	// gl state post-setup
	//

	//
	// depth mask
	//

	{	
		u_list_iter_t		iter;
		layer_t			*layer;
		
		U_ListIterInit( &iter, shp->layer_list );
		for ( ; ( layer = (layer_t *)U_ListIterNext( &iter ) ) ; )
		{
			layer->glstate->flags = multi_flags;
		}
	}

	return shp;
}

/*
  ==============================
  R_SetupCFaceShape

  ==============================
*/
shape_t * R_SetupCFaceShape( hobj_t *shape, g_map_t *map )
{
#if 1
	hobj_t		*plane;
	hobj_t		*cface;
	vec3d_t		norm;
	fp_t		dist;
	int		pointnum;
	int		i;
	shape_t		*shp;

	cface = FindClassType( shape, "cface" );
	if ( !cface )
		__error( "missing class 'cface' in shape '%s'\n", shape->name );

	// get control pointnum
	EasyFindInt( &pointnum, cface, "pointnum" );

	if ( pointnum < 6 )
	{
		printf( " shape '%s' is degenerated\n", shape->name );
		return NULL;
	}

	Shape_BeginInstance();
	Shape_SetPrimaryKey( StringToUnique( shape->name ) );
	Shape_SetTesselation( ShapeTesselation_curved_face );

	// get plane
	plane = EasyLookupClsref( cface, "plane", map->planehm );

	EasyFindVec3d( norm, plane, "norm" );
	EasyFindFloat( &dist, plane, "dist" );

	Shape_SetPlaneHint( norm, dist );

	// set control points
	Shape_SetCtrlPointNum( pointnum, 1 );
	for ( i = 0; i < pointnum; i++ )
	{
		vec3d_t		v;
		char		str[256];

		sprintf( str, "u%d_v0", i );
		EasyFindVec3d( v, cface, str );
		Shape_AddCtrlPoint( Shape_NewCtrlPointUV3fv( i, 0, v ) );
	}
	
	//
	// texture layer
	//

	if ( 1 )
	{
		// texture proj_texdef0

		hobj_t		*texdef0;
		hpair_t		*pair;

		fp_t		rotate;
		vec2d_t		shift;
		vec2d_t		scale;
		int		projection;
		
		g_resource_t	*gltex_res;
		char		str[256];

		layer_t		*layer;
		unitex_t	*uni;
		tf2d_stack_t	*tf2d_stack;

		texdef0 = FindClassType( shape, "proj_texdef0" );
		if ( !texdef0 )
			__error( "missing class 'proj_texdef0' in shape '%s'\n", shape->name );
		
		EasyFindVec2d( shift, texdef0, "shift" );
		EasyFindVec2d( scale, texdef0, "scale" );
		EasyFindFloat( &rotate, texdef0, "rotate" );

//		EasyFindInt( &projection, texdef_real, "flags" );
		projection = ProjectionTypeFromNorm( norm );

		pair = FindHPair( texdef0, "ident" );
		if ( !pair )
			__error( "missing key 'ident' in texdef class '%s'\n", texdef0->name );
		
		// build gltex resource name from ident
		sprintf( str, "gltex.%s", pair->value );
		strcrpl( str, '/', '.' );

		gltex_res = G_ResourceSearch( g_rs, str );

		uni = UniTex_NewFrom_gltexres( gltex_res );
		
		Layer_BeginInstance( (layerMode)(LayerMode_projection | LayerMode_Texel0N) );
		
		{
			// hack: build layer_gl_state_t
			layer_gl_state_t	*glstate;

			glstate = NEWTYPE( layer_gl_state_t );
			glstate->flags |= LAYER_STATE_FLAGS_DEPTH_MASK;
			glstate->sfactor = GL_ONE;
			glstate->dfactor = GL_ZERO;
			Layer_SetGLstate( glstate );
		}
		Layer_SetOrdinal( LAYER_TEXTURE_1 );

		Layer_SetUniTex( uni );
		
		if ( (projection&3) == ProjectionType_X )
		{
			Layer_SetProjection( LayerProjection_x );
		}
		else if ( (projection&3) == ProjectionType_Y )
		{
			Layer_SetProjection( LayerProjection_y );
		}
		else if ( (projection&3) == ProjectionType_Z )
		{
			Layer_SetProjection( LayerProjection_z );
		}
		else
		{
			__error( "can't get projection plane\n" );
		}
		
		//
		// setup layer's transformation stack
		//
		
		tf2d_stack = NEWTYPE( tf2d_stack_t );
		TF2D_InitStack( tf2d_stack );
		
		Layer_SetTF2DStack( tf2d_stack );
		
		{
			tf2d_t		*tf2d;
			tf2d_matrix_t	*tf2d_matrix;
			tf2d = TF2D_CreateMatrix();
			tf2d_matrix = TF2D_IsMatrix( tf2d );
			TF2D_SetupMatrixByAngle( tf2d_matrix, -rotate );
			TF2D_StackPush( tf2d_stack, tf2d );
		}
		{
			tf2d_t		*tf2d;
			tf2d_scale_t	*tf2d_scale;
			
			scale[0] = 1.0/scale[0];
			scale[1] = 1.0/scale[1];
			
			tf2d = TF2D_CreateScale();
			tf2d_scale = TF2D_IsScale( tf2d );
			Vec2dCopy( tf2d_scale->scale, scale );
			TF2D_StackPush( tf2d_stack, tf2d );
		}
		{
			tf2d_t		*tf2d;
			tf2d_shift_t	*tf2d_shift;
			
			shift[0] = -shift[0];		
			shift[1] = -shift[1];		
			tf2d = TF2D_CreateShift();
			tf2d_shift = TF2D_IsShift( tf2d );
			Vec2dCopy( tf2d_shift->shift, shift );
			TF2D_StackPush( tf2d_stack, tf2d );
		}
		
		
		layer = Layer_EndInstance();
		
		Shape_AddLayer( layer );
	}
	
	//
	// diffuse layer
	//


#if 1
	{
		hobj_t		*lightdef;
		vec2d_t		shift;
		int		width;
		int		height;
		int		projection;
		fp_t		patchsize;

		ic_subimage_t	*sub;
		unitex_t	*uni;
		layer_t		*layer;
		lightmap_t	*lightmap_head;

		lightdef = FindClassType( shape, "proj_lightdef" );
		if ( !lightdef )
		{
//			__error( "missing class 'proj_lightdef' in shape '%s'\n", shape->name );
			goto no_lightdef;
		}

		EasyFindVec2d( shift, lightdef, "shift" );
		EasyFindInt( &width, lightdef, "width" );
		EasyFindInt( &height, lightdef, "height" );
		EasyFindFloat( &patchsize, lightdef, "patchsize" );
		EasyFindInt( &projection, lightdef, "projection" );
		
		lightmap_head = LinkLightmaps( a_map->lightmap_db, StringToUnique( shape->name ), 0 );
		if ( !lightmap_head && 0 )
			goto no_diffuse;

		// setup diffuse lightmap
		{
			int		lightmap_bytes;
			void		*lightmap_pixels;
			
			lightmap_bytes = width*height*4;
			lightmap_pixels = NEWBYTES( lightmap_bytes );
			
			sub = IC_NewSubImage( width, height );
			IC_SubImageSetPixels( sub, lightmap_pixels );
		}

		uni = UniTex_NewFrom_subimage( sub );

		// create diffuse layer
		
		Layer_BeginInstance( (layerMode)(LayerMode_projection | LayerMode_Texel0N) );
		
		{
			// hack: build layer_gl_state_t
			layer_gl_state_t	*glstate;

			glstate = NEWTYPE( layer_gl_state_t );
			glstate->flags |= LAYER_STATE_FLAGS_DEPTH_MASK;
			glstate->sfactor = GL_ZERO;
			glstate->dfactor = GL_SRC_COLOR;
			Layer_SetGLstate( glstate );
		}
		Layer_SetOrdinal( LAYER_DIFFUSE_1 );
		Layer_SetUniTex( uni );
		Layer_SetLightmapHead( lightmap_head );

		if ( (projection&3) == ProjectionType_X )
		{
			Layer_SetProjection( LayerProjection_x );
		}
		else if ( (projection&3) == ProjectionType_Y )
		{
			Layer_SetProjection( LayerProjection_y );
		}
		else if ( (projection&3) == ProjectionType_Z )
		{
			Layer_SetProjection( LayerProjection_z );
		}
		else
		{
			__error( "can't get projection plane\n" );
		}		

		{
			shift[0] = -shift[0];
			shift[1] = -shift[1];
			Layer_AddShift( shift );
		}
		
		{
			vec2d_t		scale;
			scale[0] = 1.0 / patchsize;
			scale[1] = 1.0 / patchsize;
			Layer_AddScale( scale );
		}

		layer = Layer_EndInstance();
		
		Shape_AddLayer( layer );

	no_diffuse:
		{}
	}

#endif


	//
	// specular layer
	//


#if 0
	{
		hobj_t		*lightdef;
		hpair_t		*pair;
		vec2d_t		shift;
		int		width;
		int		height;
		int		projection;
		fp_t		patchsize;

		ic_subimage_t	*sub;
		unitex_t	*uni;

		layer_t		*layer;
		lightmap_t	*lightmap_head;

		lightdef = FindClassType( shape, "proj_lightdef" );
		if ( !lightdef )
		{
//			__error( "missing class 'proj_lightdef' in shape '%s'\n", shape->name );
			goto no_lightdef;
		}

		EasyFindVec2d( shift, lightdef, "shift" );
		EasyFindInt( &width, lightdef, "width" );
		EasyFindInt( &height, lightdef, "height" );
		EasyFindFloat( &patchsize, lightdef, "patchsize" );
		EasyFindInt( &projection, lightdef, "projection" );


		lightmap_head = LinkLightmaps( a_map->lightmap_db, StringToUnique( shape->name ), 1 );
		if ( !lightmap_head )
			goto no_specular;
		// setup specular lightmap
		{
			int		lightmap_bytes;
			void		*lightmap_pixels;
			
			lightmap_bytes = width*height*4;
			lightmap_pixels = NEWBYTES( lightmap_bytes );
			
			sub = IC_NewSubImage( width, height );
			IC_SubImageSetPixels( sub, lightmap_pixels );
		}
		
		uni = UniTex_NewFrom_subimage( sub );

		// create specular layer
		
		Layer_BeginInstance( LayerMode_projection | LayerMode_Texel0N );
		
		{
			// hack: build layer_gl_state_t
			layer_gl_state_t	*glstate;

			glstate = NEWTYPE( layer_gl_state_t );
			glstate->flags |= LAYER_STATE_FLAGS_DEPTH_MASK;
			glstate->sfactor = GL_ONE;
			glstate->dfactor = GL_ONE;
			Layer_SetGLstate( glstate );
		}

		Layer_SetOrdinal( LAYER_SPECULAR_1 );		
		Layer_SetUniTex( uni );
		Layer_SetLightmapHead( lightmap_head );

		if ( (projection&3) == ProjectionType_X )
		{
			Layer_SetProjection( LayerProjection_x );
		}
		else if ( (projection&3) == ProjectionType_Y )
		{
			Layer_SetProjection( LayerProjection_y );
		}
		else if ( (projection&3) == ProjectionType_Z )
		{
			Layer_SetProjection( LayerProjection_z );
		}
		else
		{
			__error( "can't get projection plane\n" );
		}		

		{
			shift[0] = -shift[0];
			shift[1] = -shift[1];
			Layer_AddShift( shift );
		}
		
		{
			vec2d_t		scale;
			scale[0] = 1.0 / patchsize;
			scale[1] = 1.0 / patchsize;
			Layer_AddScale( scale );
		}

		layer = Layer_EndInstance();
		
		Shape_AddLayer( layer );

	no_specular:
	}

#endif

no_lightdef:

	
	shp = Shape_EndInstance();
	
	return shp;

#else

	return NULL;

#endif

}


/*
  ==============================
  R_SetupCSurfMultiLayerFromClass

  ==============================
*/

void R_SetupCSurfLayer( hobj_t *texdef, hobj_t *layer )
{
	int		i;
	vec2d_t		vecs[4];
	vec2d_t		scale;
	
	g_resource_t	*gltex_res;
	
	layer_t		*l;
	layer_gl_state_t	*glstate;
	unitex_t	*uni;		
	hpair_t		*pair;

	color_rgba_t	color;

	EasyFindVec2d( vecs[0], texdef, "shift" );
	EasyFindVec2d( vecs[1], texdef, "vec2" );
	EasyFindVec2d( vecs[2], texdef, "vec0" );
	EasyFindVec2d( vecs[3], texdef, "vec1" );
	EasyFindVec2d( scale, texdef, "scale" );
	
	for ( i = 1; i < 4; i++ )
	{
		vecs[i][0] *= scale[0];
		vecs[i][1] *= scale[1];
	}
	
	Vec2dAdd( vecs[1], vecs[1], vecs[0] );
	Vec2dAdd( vecs[2], vecs[2], vecs[0] );
	Vec2dAdd( vecs[3], vecs[3], vecs[0] );

	// get layer ambient color
	color[0] = 255;
	color[1] = 255;
	color[2] = 255;
	color[3] = 255;
	pair = FindHPair( layer, "color" );
	if ( pair )
	{
		int			i;
		vec3d_t		v;
		HPairCastToVec3d( v, pair );

		for ( i = 0; i < 3; i++ )
		{
			if ( v[i] < 0.0 )
				v[i] = 0.0;
			if ( v[i] > 1.0 )
				v[i] = 1.0;

			color[i] = (unsigned char) (v[i]*255.0);
		}		
	}
	
	pair = FindHPair( layer, "alpha" );
	if ( pair )
	{
		fp_t		a;
		HPairCastToFloat( &a, pair );

		if ( a < 0.0 )
			a = 0.0;
		if ( a > 1.0 )
			a = 1.0;

		color[3] = (unsigned char) (a*255.0);
	}


	pair = FindHPair( layer, "gltex_res" );
	if ( !pair )
		__error( "missing key 'gltex_res'\n" );

	gltex_res = G_ResourceSearch( g_rs, pair->value );
	
	if ( !gltex_res )
		__error( "can't find gltex resource '%s'\n", pair->value );

	uni = UniTex_NewFrom_gltexres( gltex_res );

	// layer gl state
	glstate = NEWTYPE( layer_gl_state_t );	

	//
	// blend sfactor
	//

	pair = FindHPair( layer, "sfactor" );
	if ( !pair )
		__error( "missing key 'sfactor'\n" );

	if ( !strcmp( pair->value, "GL_ONE" ) )
	{
		glstate->sfactor = GL_ONE;
	}
	else if ( !strcmp( pair->value, "GL_ZERO" ) )
	{
		glstate->sfactor = GL_ZERO;
	}
	else if ( !strcmp( pair->value, "GL_SRC_COLOR" ) )
	{
		glstate->sfactor = GL_SRC_COLOR;
	}
	else if ( !strcmp( pair->value, "GL_SRC_ALPHA" ) )
	{
		glstate->sfactor = GL_SRC_ALPHA;
	}
	else
	{
		__error( "unknown sfactor '%s'\n", pair->value );
	}

	//
	// blend dfactor
	//

	pair = FindHPair( layer, "dfactor" );
	if ( !pair )
		__error( "missing key 'dfactor'\n" );
	
	if ( !strcmp( pair->value, "GL_ONE" ) )
	{
		glstate->dfactor = GL_ONE;
	}
	else if ( !strcmp( pair->value, "GL_ZERO" ) )
	{
		glstate->dfactor = GL_ZERO;
	}
	else if ( !strcmp( pair->value, "GL_ONE_MINUS_SRC_ALPHA" ) )
	{
		glstate->dfactor = GL_ONE_MINUS_SRC_ALPHA;
	}
	else
	{
		__error( "unknown dfactor '%s'\n", pair->value );
	}

	Layer_BeginInstance( (layerMode)(LayerMode_uv | LayerMode_Texel0N) );
	Layer_SetGLstate( glstate );
	Layer_SetOrdinal( atoi( layer->name ) );
	Layer_SetUniTex( uni );
	Layer_SetUVCtrl( vecs );

	Layer_SetAmbientColor( color );

	l = Layer_EndInstance();
	Shape_AddLayer( l );

	
}

void R_SetupCSurfMultiLayerFromClass( hobj_t *texdef, hobj_t *multilayer )
{
	hobj_search_iterator_t		iter;
	hobj_t				*layer;

	InitClassSearchIterator( &iter, multilayer, "layer" );

	for ( ; ( layer = SearchGetNextClass( &iter ) ) ; )
	{
		R_SetupCSurfLayer( texdef, layer );
	}
}

/*
  ==============================
  R_SetupCSurfShape

  ==============================
*/

shape_t * R_SetupCSurfShape( hobj_t *shape, g_map_t *map )
{
#if 1
	int		i, u, v;

	hobj_t		*multilayer;

	hobj_t		*csurf;
	hpair_t		*pair;
	shape_t		*shp;

	int		upointnum;
	int		vpointnum;

	multilayer = NULL;

	csurf = FindClassType( shape, "csurf" );
	if ( !csurf )
		__error( "missing class 'csurf' in shape '%s'\n", shape->name );

	// get control points
	EasyFindInt( &upointnum, csurf, "upointnum" );
	EasyFindInt( &vpointnum, csurf, "vpointnum" );

	Shape_BeginInstance();
	Shape_SetPrimaryKey( StringToUnique( shape->name ) );
	Shape_SetTesselation( ShapeTesselation_curved_surface );	

	Shape_SetCtrlPointNum( upointnum, vpointnum );
	for ( u = 0; u < upointnum; u++ )
	{
		for ( v = 0; v < vpointnum; v++ )
		{
			vec3d_t		p;
			char		str[256];

			sprintf( str, "u%d_v%d", u, v );
			EasyFindVec3d( p, csurf, str );
			Shape_AddCtrlPoint( Shape_NewCtrlPointUV3fv( u, v, p ) );
		}
	}

	//
	// texture layer
	//

	if ( 1 )
	{
		hobj_t		*texdef0;
		vec2d_t		vecs[4];
		vec2d_t		scale;

		g_resource_t	*gltex_res;
		char		str[256];

		layer_t		*layer;
		unitex_t	*uni;		

		texdef0 = FindClassType( shape, "uv_texdef0" );
		if ( !texdef0 )
			__error( "missing class 'uv_texdef0' in shape '%s'\n", shape->name );

		EasyFindVec2d( vecs[0], texdef0, "shift" );
		EasyFindVec2d( vecs[1], texdef0, "vec2" );
		EasyFindVec2d( vecs[2], texdef0, "vec0" );
		EasyFindVec2d( vecs[3], texdef0, "vec1" );
		EasyFindVec2d( scale, texdef0, "scale" );

		for ( i = 1; i < 4; i++ )
		{
			vecs[i][0] *= scale[0];
			vecs[i][1] *= scale[1];
		}

		Vec2dAdd( vecs[1], vecs[1], vecs[0] );
		Vec2dAdd( vecs[2], vecs[2], vecs[0] );
		Vec2dAdd( vecs[3], vecs[3], vecs[0] );


		pair = FindHPair( texdef0, "ident" );
		if ( !pair )
			__error( "missing key 'ident' in texdef class '%s'\n", texdef0->name );
		

		//
		// check ident for multilayer definition
		//
		{
			multilayer = HManagerSearchClassName( map->multilayerhm, pair->value );
			if ( multilayer )
			{
				__named_message( "MULTILAYER DEFINITON !\n" );

				R_SetupCSurfMultiLayerFromClass( texdef0, multilayer );
				goto skip_texture;
			}			
		}

		// build gltex resource name from ident
		sprintf( str, "gltex.%s", pair->value );
		strcrpl( str, '/', '.' );
		gltex_res = G_ResourceSearch( g_rs, str );

		uni = UniTex_NewFrom_gltexres( gltex_res );

		Layer_BeginInstance( (layerMode)(LayerMode_uv | LayerMode_Texel0N) );

		{
			// hack: build layer_gl_state_t
			layer_gl_state_t	*glstate;

			glstate = NEWTYPE( layer_gl_state_t );
			glstate->flags |= LAYER_STATE_FLAGS_DEPTH_MASK;
			glstate->sfactor = GL_ONE;
			glstate->dfactor = GL_ZERO;
			Layer_SetGLstate( glstate );
		}

		Layer_SetOrdinal( LAYER_TEXTURE_1 );	       
		Layer_SetUniTex( uni );		
		Layer_SetUVCtrl( vecs );
		layer = Layer_EndInstance();

		Shape_AddLayer( layer );
	}

skip_texture:

	//
	// diffuse layer
	//


       
#if 1
	{
		hobj_t		*lightdef;
		int		width;
		int		height;

		ic_subimage_t	*sub;
		unitex_t	*uni;

		lightmap_t	*lightmap_head;

		layer_t		*layer;

		lightdef = FindClassType( shape, "uv_lightdef" );
		if ( !lightdef )
		{
			goto no_lightdef;
		}

		EasyFindInt( &width, lightdef, "width" );
		EasyFindInt( &height, lightdef, "height" );
		
		lightmap_head = LinkLightmaps( a_map->lightmap_db, StringToUnique( shape->name ), 0 );
		
	       	if ( !lightmap_head && 0 )
			goto no_diffuse;
		
		// setup diffuse lightmap
		{
			int		lightmap_bytes;
			void		*lightmap_pixels;

			lightmap_bytes = width*height*4;
			lightmap_pixels = NEWBYTES( lightmap_bytes );
			
			sub = IC_NewSubImage( width, height );
			IC_SubImageSetPixels( sub, lightmap_pixels );
		}

		uni = UniTex_NewFrom_subimage( sub );		

		// create diffuse layer
		
		Layer_BeginInstance( (layerMode)(LayerMode_uv | LayerMode_Texel01) );
		
		{
			// hack: build layer_gl_state_t
			layer_gl_state_t	*glstate;

			glstate = NEWTYPE( layer_gl_state_t );
			glstate->flags |= LAYER_STATE_FLAGS_DEPTH_MASK;
			glstate->sfactor = GL_ZERO;
			glstate->dfactor = GL_SRC_COLOR;
			Layer_SetGLstate( glstate );
		}
		Layer_SetOrdinal( LAYER_DIFFUSE_1 );
		Layer_SetUniTex( uni );
		Layer_SetLightmapHead( lightmap_head );
		{
			int		i;
			vec2d_t		vecs[4] = { {0,0},{1,0},{0,1},{1,1} };

			// shrink and shift for lightmap boarders

			for ( i = 0; i < 4; i++ )
			{
				vecs[i][0] *= (width*1.0)/(width+1.0);
				vecs[i][1] *= (height*1.0)/(height+1.0);

				vecs[i][0] += 0.5/(width*1.0);
				vecs[i][1] += 0.5/(height*1.0);
			}

			Layer_SetUVCtrl( vecs );
		}

		layer = Layer_EndInstance();
		
		Shape_AddLayer( layer );		

	no_diffuse:
		{}		
	}

#endif
	

	//
	// specular layer
	//


#if 1

	{
		hobj_t		*lightdef;
		int		width;
		int		height;

		ic_subimage_t	*sub;
		unitex_t	*uni;

		layer_t		*layer;
		lightmap_t	*lightmap_head;

		lightdef = FindClassType( shape, "uv_lightdef" );
		if ( !lightdef )
		{
			goto no_lightdef;
		}

		EasyFindInt( &width, lightdef, "width" );
		EasyFindInt( &height, lightdef, "height" );

		lightmap_head = LinkLightmaps( a_map->lightmap_db, StringToUnique( shape->name ), 1 );
		if ( !lightmap_head )
			goto no_specular;
		// setup specular lightmap
		{
			int		lightmap_bytes;
			void		*lightmap_pixels;
			
			lightmap_bytes = width*height*4;
			lightmap_pixels = NEWBYTES( lightmap_bytes );
			
			sub = IC_NewSubImage( width, height );
			IC_SubImageSetPixels( sub, lightmap_pixels );
		}
		
		uni = UniTex_NewFrom_subimage( sub );		

		// create specular layer
		
		Layer_BeginInstance( (layerMode)(LayerMode_uv | LayerMode_Texel01) );
		
		{
			// hack: build layer_gl_state_t
			layer_gl_state_t	*glstate;

			glstate = NEWTYPE( layer_gl_state_t );
			glstate->flags |= LAYER_STATE_FLAGS_DEPTH_MASK;
			glstate->sfactor = GL_ONE;
			glstate->dfactor = GL_ONE;
			Layer_SetGLstate( glstate );
		}
		Layer_SetOrdinal( LAYER_SPECULAR_1 );
		Layer_SetUniTex( uni );
		Layer_SetLightmapHead( lightmap_head );
		{
			int		i;
			vec2d_t		vecs[4] = { {0,0},{1,0},{0,1},{1,1} };

			// shrink and shift for lightmap boarders

			for ( i = 0; i < 4; i++ )
			{
				vecs[i][0] *= (width*1.0)/(width+1.0);
				vecs[i][1] *= (height*1.0)/(height+1.0);

				vecs[i][0] += 0.5/(width*1.0);
				vecs[i][1] += 0.5/(height*1.0);
			}

			Layer_SetUVCtrl( vecs );
		}

		layer = Layer_EndInstance();
		
		Shape_AddLayer( layer );		

	no_specular:
		{}		
	}

#endif
	

no_lightdef:

	shp = Shape_EndInstance();

	if ( multilayer )
	{
		hobj_t		*mesh_obj;
		hobj_search_iterator_t		iter;
		hobj_t				*job;

		int		multi_flags;

		mesh_obj = FindClassType( multilayer, "mesh" );
		
		if ( mesh_obj )
		{
			InitClassSearchIterator( &iter, mesh_obj, "job" );
			for ( ; ( job = SearchGetNextClass( &iter ) ) ; )
			{
				if ( !strcmp( job->name, "SurfaceDeform" ) )
				{
					R_RegisterShapeCtrlJob( ShapeCtrlJob_Create_SurfaceDeform( job, shp ) );
				}
				else
				{
					__warning( "unknown ShapeCtrlJob '%s', ignore it\n", job->name );
				}
			}	
		}

		multi_flags = LAYER_STATE_FLAGS_DEPTH_MASK;
		pair = FindHPair( multilayer, "depthmask" );
		if ( !pair )
		{
			// default is depthmask = true
			multi_flags |= LAYER_STATE_FLAGS_DEPTH_MASK;
		}
		else
		{
			if ( !strcmp( pair->value, "0" ) )
			{
				multi_flags &= ~LAYER_STATE_FLAGS_DEPTH_MASK;
				multi_flags |= LAYER_STATE_FLAGS_SHADE_P3;

			}
			else
			{
				multi_flags |= LAYER_STATE_FLAGS_DEPTH_MASK;
			}
		}
		{	
			u_list_iter_t		iter;
			layer_t			*layer;
			
			U_ListIterInit( &iter, shp->layer_list );
			for ( ; ( layer = (layer_t *)U_ListIterNext( &iter ) ) ; )
			{
				layer->glstate->flags = multi_flags;
			}
		}

	}
	
	return shp;

#else

	return NULL;

#endif
}

/*
  ==============================
  R_SetupMeshTileShape

  ==============================
*/

shape_t * R_SetupMeshTileShape( hobj_t *shape, g_map_t *map )
{
	hobj_t		*polygon;
	hobj_t		*plane;
	hobj_t		*glmesh_obj;
	vec3d_t		norm;
	fp_t		dist;
	hobj_t		*texdef0;
	hobj_t		*texdef_real;
	vec2d_t		shift;
	vec2d_t		scale;
	fp_t		rotate;
	int		projection;
	hobj_t		*texture;
	hpair_t		*pair;
	char		str[256];
	g_resource_t	*gltex_res;
	unitex_t	*uni;
	tf2d_stack_t	*tf2d_stack;
	layer_t		*layer;
	shape_t		*shp;

	polygon = FindClassType( shape, "polygon" );
	if ( !polygon )
		__error( "missing class 'polygon' in shape '%s'\n", shape->name );
	

	Shape_BeginInstance();
	Shape_SetPrimaryKey( StringToUnique( shape->name ) );
	Shape_SetTesselation( ShapeTesselation_glmesh );

	// get plane
	plane = EasyLookupClsref( polygon, "plane", map->planehm );

	EasyFindVec3d( norm, plane, "norm" );
	EasyFindFloat( &dist, plane, "dist" );

	Shape_SetPlaneHint( norm, dist );

	glmesh_obj = FindClassType( shape, "glmesh" );
	if ( !glmesh_obj )
		__error( "missing class 'glmesh' in shape '%s'\n", shape->name );
	
	Shape_SetGLMesh( glmesh_obj, a_map->glmesh );

#if 0
	meshtile = FindClassType( shape, "meshtile" );
	if ( !meshtile )
		__error( "missing class 'meshtile' in shape '%s'\n", shape->name );

	EasyFindInt( &trinum, meshtile, "polynum" );


	Shape_SetCtrlPointNum( trinum*3, 1 );
	InitClassSearchIterator( &iter, meshtile, "*" );
	for ( i = 0; ( tri = SearchGetNextClass( &iter ) ) ; )
	{
		EasyFindVec3d( v, tri, "0" );
		Shape_AddCtrlPoint( Shape_NewCtrlPointUV3fv( i, 0, v ) );
		i++;
		EasyFindVec3d( v, tri, "1" );
		Shape_AddCtrlPoint( Shape_NewCtrlPointUV3fv( i, 0, v ) );
		i++;
		EasyFindVec3d( v, tri, "2" );
		Shape_AddCtrlPoint( Shape_NewCtrlPointUV3fv( i, 0, v ) );
		i++;
	}
#endif


	//
	// texture layer
	//

	texdef0 = FindClassType( shape, "proj_texdef0" );
	if ( !texdef0 )
		__error( "missing class 'proj_texdef0' in shape '%s'\n", shape->name );
	
	texdef_real = EasyLookupClsref( texdef0, "texdef", map->texdefhm );
	
	EasyFindVec2d( shift, texdef_real, "shift" );
	EasyFindVec2d( scale, texdef_real, "scale" );
	EasyFindFloat( &rotate, texdef_real, "rotate" );
	EasyFindInt( &projection, texdef_real, "flags" );
	

	texture = EasyLookupClsref( texdef_real, "texture", map->texturehm );
	pair = FindHPair( texture, "ident" );
	if ( !pair )
		__error( "missing key 'ident' in texture class '%s'\n", texture->name );
	
	// build gltex resource name from ident
	sprintf( str, "gltex.%s", pair->value );
	strcrpl( str, '/', '.' );

	gltex_res = G_ResourceSearch( g_rs, str );
	
	uni = UniTex_NewFrom_gltexres( gltex_res );
	
	Layer_BeginInstance( (layerMode)(LayerMode_projection | LayerMode_Texel0N) );
	
	{
		// hack: build layer_gl_state_t
		layer_gl_state_t	*glstate;
		
		glstate = NEWTYPE( layer_gl_state_t );
		glstate->sfactor = GL_ONE;
		glstate->dfactor = GL_ZERO;
		glstate->flags = LAYER_STATE_FLAGS_DEPTH_MASK;
		Layer_SetGLstate( glstate );
	}
	Layer_SetOrdinal( LAYER_TEXTURE_1 );
	
	Layer_SetUniTex( uni );
	
	if ( (projection&3) == ProjectionType_X )
	{
		Layer_SetProjection( LayerProjection_x );
	}
	else if ( (projection&3) == ProjectionType_Y )
	{
		Layer_SetProjection( LayerProjection_y );
	}
	else if ( (projection&3) == ProjectionType_Z )
	{
		Layer_SetProjection( LayerProjection_z );
	}
	else
	{
		__error( "can't get projection plane\n" );
	}
	
	//
	// setup layer's transformation stack
	//
	
	tf2d_stack = NEWTYPE( tf2d_stack_t );
	TF2D_InitStack( tf2d_stack );
	
	Layer_SetTF2DStack( tf2d_stack );
	
	
	{
		tf2d_t		*tf2d;
		tf2d_matrix_t	*tf2d_matrix;
		tf2d = TF2D_CreateMatrix();
		tf2d_matrix = TF2D_IsMatrix( tf2d );
		TF2D_SetupMatrixByAngle( tf2d_matrix, -rotate );
		TF2D_StackPush( tf2d_stack, tf2d );
	}       
	{
		tf2d_t		*tf2d;
		tf2d_scale_t	*tf2d_scale;
		
		scale[0] = 1.0/scale[0];
		scale[1] = 1.0/scale[1];
		
		tf2d = TF2D_CreateScale();
		tf2d_scale = TF2D_IsScale( tf2d );
		Vec2dCopy( tf2d_scale->scale, scale );
		TF2D_StackPush( tf2d_stack, tf2d );
	}
	{
		tf2d_t		*tf2d;
		tf2d_shift_t	*tf2d_shift;
		
		shift[0] = -shift[0];		
		shift[1] = -shift[1];		
		tf2d = TF2D_CreateShift();
		tf2d_shift = TF2D_IsShift( tf2d );
		Vec2dCopy( tf2d_shift->shift, shift );
		TF2D_StackPush( tf2d_stack, tf2d );
	}
	
	layer = Layer_EndInstance();

	Shape_AddLayer( layer );	



	//
	// diffuse layer
	//


#if 1
	{
		hobj_t		*lightdef;
		vec2d_t		shift;
		int		width;
		int		height;
		int		projection;
		fp_t		patchsize;

		ic_subimage_t	*sub;
		unitex_t	*uni;

		layer_t		*layer;
		lightmap_t	*lightmap_head;

		lightdef = FindClassType( shape, "proj_lightdef" );
		if ( !lightdef )
		{
			goto no_lightdef;
		}

		EasyFindVec2d( shift, lightdef, "shift" );
		EasyFindInt( &width, lightdef, "width" );
		EasyFindInt( &height, lightdef, "height" );
		EasyFindFloat( &patchsize, lightdef, "patchsize" );
		EasyFindInt( &projection, lightdef, "projection" );
		
		lightmap_head = LinkLightmaps( a_map->lightmap_db, StringToUnique( shape->name ), 0 );
		if ( !lightmap_head && 0 )
			goto no_diffuse;
		{
			int		lightmap_bytes;
			void		*lightmap_pixels;
			
			lightmap_bytes = width*height*4;
			lightmap_pixels = NEWBYTES( lightmap_bytes );
			
			sub = IC_NewSubImage( width, height );
			IC_SubImageSetPixels( sub, lightmap_pixels );
		}
		
		uni = UniTex_NewFrom_subimage( sub );

		// create diffuse layer
		
		Layer_BeginInstance( (layerMode)(LayerMode_projection | LayerMode_Texel0N) );
		
		{
			// hack: build layer_gl_state_t
			layer_gl_state_t	*glstate;

			glstate = NEWTYPE( layer_gl_state_t );
			glstate->flags = LAYER_STATE_FLAGS_DEPTH_MASK;
			glstate->sfactor = GL_ZERO;
			glstate->dfactor = GL_SRC_COLOR;
			Layer_SetGLstate( glstate );
		}

		Layer_SetOrdinal( LAYER_DIFFUSE_1 );		
		Layer_SetUniTex( uni );
		Layer_SetLightmapHead( lightmap_head );

		if ( (projection&3) == ProjectionType_X )
		{
			Layer_SetProjection( LayerProjection_x );
		}
		else if ( (projection&3) == ProjectionType_Y )
		{
			Layer_SetProjection( LayerProjection_y );
		}
		else if ( (projection&3) == ProjectionType_Z )
		{
			Layer_SetProjection( LayerProjection_z );
		}
		else
		{
			__error( "can't get projection plane\n" );
		}		

		{
			shift[0] = -shift[0];
			shift[1] = -shift[1];
			Layer_AddShift( shift );
		}
		
		{
			vec2d_t		scale;
			scale[0] = 1.0 / patchsize;
			scale[1] = 1.0 / patchsize;
			Layer_AddScale( scale );
		}

		layer = Layer_EndInstance();
		
		Shape_AddLayer( layer );

	no_diffuse:
		{}
	}

#endif	

	//
	// specular layer
	//


#if 1
	{
		hobj_t		*lightdef;
		vec2d_t		shift;
		int		width;
		int		height;
		int		projection;
		fp_t		patchsize;

		ic_subimage_t	*sub;
		unitex_t	*uni;

		layer_t		*layer;
		lightmap_t	*lightmap_head;

		lightdef = FindClassType( shape, "proj_lightdef" );
		if ( !lightdef )
		{
//			__error( "missing class 'proj_lightdef' in shape '%s'\n", shape->name );
			goto no_lightdef;
		}

		EasyFindVec2d( shift, lightdef, "shift" );
		EasyFindInt( &width, lightdef, "width" );
		EasyFindInt( &height, lightdef, "height" );
		EasyFindFloat( &patchsize, lightdef, "patchsize" );
		EasyFindInt( &projection, lightdef, "projection" );

		lightmap_head = LinkLightmaps( a_map->lightmap_db, StringToUnique( shape->name ), 1 );
		if ( !lightmap_head )
			goto no_specular;
		// setup specular lightmap
		{
			int		lightmap_bytes;
			void		*lightmap_pixels;
			
			lightmap_bytes = width*height*4;
			lightmap_pixels = NEWBYTES( lightmap_bytes );
			
			sub = IC_NewSubImage( width, height );
			IC_SubImageSetPixels( sub, lightmap_pixels );
		}
		
		uni = UniTex_NewFrom_subimage( sub );

		// create specular layer
		
		Layer_BeginInstance( (layerMode)(LayerMode_projection | LayerMode_Texel0N) );
		
		{
			// hack: build layer_gl_state_t
			layer_gl_state_t	*glstate;

			glstate = NEWTYPE( layer_gl_state_t );
			glstate->flags = LAYER_STATE_FLAGS_DEPTH_MASK;
			glstate->sfactor = GL_ONE;
			glstate->dfactor = GL_ONE;
			Layer_SetGLstate( glstate );
		}

		Layer_SetOrdinal( LAYER_SPECULAR_1 );
		Layer_SetLightmapHead( lightmap_head );
		Layer_SetUniTex( uni );

		if ( (projection&3) == ProjectionType_X )
		{
			Layer_SetProjection( LayerProjection_x );
		}
		else if ( (projection&3) == ProjectionType_Y )
		{
			Layer_SetProjection( LayerProjection_y );
		}
		else if ( (projection&3) == ProjectionType_Z )
		{
			Layer_SetProjection( LayerProjection_z );
		}
		else
		{
			__error( "can't get projection plane\n" );
		}		

		{
			shift[0] = -shift[0];
			shift[1] = -shift[1];
			Layer_AddShift( shift );
		}
		
		{
			vec2d_t		scale;
			scale[0] = 1.0 / patchsize;
			scale[1] = 1.0 / patchsize;
			Layer_AddScale( scale );
		}

		layer = Layer_EndInstance();
		
		Shape_AddLayer( layer );

	no_specular:
		{}
	}

#endif


no_lightdef:
	
	shp = Shape_EndInstance();

	return shp;
}

/*
  ==============================
  R_SetupShapes

  ==============================
*/
void R_SetupShapes( g_map_t *map )
{
	hobj_search_iterator_t		iter;
	hobj_t		*shape_root;
	hobj_t		*shape;
	hpair_t		*pair;

	shape_t		*shp;

	r_cfaceshapenum = 0;
	r_csurfshapenum = 0;
	r_sfaceshapenum = 0;
	r_meshtileshapenum = 0;

	shp = NULL;
	shape_root = HManagerGetRootClass( map->shapehm );

	InitClassSearchIterator( &iter, shape_root, "shape" );

	for ( ; ( shape = SearchGetNextClass( &iter ) ) ; )
	{
		pair = FindHPair( shape, "tess_name" );
		if ( !pair )
			__error( "missing key 'tess_name' in shape '%s'\n", shape->name );

		if ( !strcmp( pair->value, "sface" ) )
		{
			// simple face shape
			
			if ( r_sfaceshapenum >= MAX_SFACE_SHAPES )
				__error( "reached MAX_SFACE_SHAPES\n" );

			shp = R_SetupSFaceShape( shape, map );

			if ( !shp )
				continue;

			ShapeDB_InsertShape( a_map->shape_db, shp );
			r_sfaceshapes[r_sfaceshapenum] = shp;
			r_sfaceshapenum++;
		}
		else if ( !strcmp( pair->value, "cface" ) )
		{
			// curved face shape

			if ( r_cfaceshapenum >= MAX_CFACE_SHAPES )
				__error( "reached MAX_CFACE_SHAPES\n" );

			shp = R_SetupCFaceShape( shape, map );

			if ( !shp )
				continue;

			ShapeDB_InsertShape( a_map->shape_db, shp );
			r_cfaceshapes[r_cfaceshapenum] = shp;
			r_cfaceshapenum++;
		}
		else if ( !strcmp( pair->value, "csurf" ) )
		{
			// curved surface

			if ( r_csurfshapenum >= MAX_CSURF_SHAPES )
				__error( "reached MAX_CSURF_SHAPES\n" );

			shp = R_SetupCSurfShape( shape, map );

			if ( !shp )
				continue;

			ShapeDB_InsertShape( a_map->shape_db, shp );
			r_csurfshapes[r_csurfshapenum] = shp;
			r_csurfshapenum++;	
		}
		else if ( !strcmp( pair->value, "meshtile" ) )
		{
			// meshtile shape
			
			if ( r_meshtileshapenum >= MAX_MESHTILE_SHAPES )
				__error( "reached MAX_MESHTILE_SHAPES\n" );

			shp = R_SetupMeshTileShape( shape, map );

			if ( !shp )
				continue;

			ShapeDB_InsertShape( a_map->shape_db, shp );
			r_meshtileshapes[r_meshtileshapenum] = shp;
			r_meshtileshapenum++;
		}
		else
		{
			__error( "unkown tesselation methode in shape '%s'\n", shape->name );
		}
	}

	printf( " %d simple face shapes\n", r_sfaceshapenum );
	printf( " %d curved face shapes\n", r_cfaceshapenum );
	printf( " %d curved surface shapes\n", r_csurfshapenum );
	printf( " %d meshtile shapes\n", r_meshtileshapenum );
}



/*
  ==============================
  R_CleanUpAllShapes

  ==============================
*/
void R_CleanUpAllShapes( void )
{
	int		i;

	__named_message( "\n" );

	for ( i = 0; i < r_sfaceshapenum; i++ )
	{
		Shape_CleanUp( r_sfaceshapes[i] );
		FREE( r_sfaceshapes[i] );
	}

	for ( i = 0; i < r_cfaceshapenum; i++ )
	{
		Shape_CleanUp( r_cfaceshapes[i] );
		FREE( r_cfaceshapes[i] );
	}

	for ( i = 0; i < r_csurfshapenum; i++ )
	{
		Shape_CleanUp( r_csurfshapes[i] );
		FREE( r_csurfshapes[i] );
	}

	for ( i = 0; i < r_meshtileshapenum; i++ )
	{
		Shape_CleanUp( r_meshtileshapes[i] );
		FREE( r_meshtileshapes[i] );
	}
}
