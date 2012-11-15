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



// r_shpctrljobs.c

#include "sys/types.h"

#include "lib_math.h"
#include "lib_container.h"

#include "render.h"
#include "r_shpctrljobs.h"

#include "db_lightmap.h"
#include "imgcache.h"
#include "mesh.h"
#include "tess.h"
#include "shape_defs.h"
#include "layer.h"
#include "unitex.h"
#include "layers.h"
#include "shader.h"

#include "db_lightmap.h"

/*
  ==================================================
  reference impl job: ValidateShape

  ==================================================
*/

static bool_t			ftbl_validate_shape_valid = false;
static shape_ctrl_job_ftbl_t	ftbl_validate_shape;

// shape_ctrl_job ftbl functions


shapeCtrlJobRes ShapeCtrlJob_RunJob_ValidateShape( shape_ctrl_job_t *job )
{
	u_list_iter_t		iter;	
	shape_ctrl_job_validate_shape_t		*vs_job;
	shape_t			*shp;
	layer_t			*layer;

	vs_job = ADDR_OF_BASE( shape_ctrl_job_validate_shape_t, shape_ctrl_job_if, job );
	shp = vs_job->shape;
	__chkptr( shp );

	shp->flags &= ~SHAPE_FLAGS_IS_VALID;
	//
	// rebuild shape's mesh
	//
	if ( !(shp->flags & SHAPE_FLAGS_MESH_IS_VALID ) )
	{
		Tess_CreateMesh( shp );	       
		shp->flags |= SHAPE_FLAGS_MESH_IS_VALID;
	}

	//
	// rebuild layers
	//
	if ( !(shp->flags & SHAPE_FLAGS_LAYERS_ARE_VALID ) )
	{
		U_ListIterInit( &iter, shp->layer_list );
		for ( ; ( layer = (layer_t *)U_ListIterNext( &iter ) ); )
		{
			//
			// check the layers unitex
			//
			if ( !UniTex_IsValid( layer->real_source ) )
			{
				UniTex_TryToValidate( layer->real_source );
				if ( !UniTex_IsValid( layer->real_source ) )
				{
					// unitex validation failed, imgcache full ?
					
					// this job failed
					return ShapeCtrlJobRes_failed;
				}
				else
				{
					// if it is a subimage, we need an explicite upload
					ic_subimage_t	*sub;

					sub = UniTex_IsReal_subimage( layer->real_source );
					if ( sub )
					{
						if ( layer->lightmap_head )
						{
							char *pixels;
							pixels = LightmapDB_GetPixels( a_map->lightmap_db,layer->lightmap_head, sub->width*sub->height );
							memcpy( sub->pixels, pixels, sub->width*sub->height*4 );
//							FREE( pixels );
						}

						ICU_UploadFrameAddSubImage( &r_upload_frame, sub );
					}
				}
			}

			Layer_CreateTexCoords( layer, shp->mesh );			
		}
		shp->flags |= SHAPE_FLAGS_LAYERS_ARE_VALID;
	}		
	
	//
	// update shader_info
	//
	if ( ! (shp->flags & SHAPE_FLAGS_SHADER_INFO_IS_VALID ) )
	{
		layers_characteristic_t		*ca;
		shader_t			*shd;

		LayerList_BuildArrays( shp->layer_list, &ca, &shp->la );
		// try to find a shader for this layer
		shd = Shader_MgrSearchShader( a_map->shader_mgr, ca );
		if ( shd )
		{
				// ok, there is already a shader for this kind of layer characteristic
			FREE( ca );
		}
		else
		{
				// create a new shader for this layer characteristic
			shd = Shader_New( ca );		// shader is responsible for 'ca'
			Shader_MgrRegisterShader( a_map->shader_mgr, shd );
		}
		
		shp->shd = shd;
		shp->flags |= SHAPE_FLAGS_SHADER_INFO_IS_VALID;		
//		printf( "shader: %p\n", shp->shd );
	}
	
	shp->flags |= SHAPE_FLAGS_IS_VALID;

	return ShapeCtrlJobRes_success;
}

shapeCtrlJobRes ShapeCtrlJob_DestroyJob_ValidateShape( shape_ctrl_job_t *job )
{
	shape_ctrl_job_validate_shape_t		*vs_job;

	vs_job = ADDR_OF_BASE( shape_ctrl_job_validate_shape_t, shape_ctrl_job_if, job );

//	__named_message( "job: %p, vs_job: %p\n", job, vs_job );

	FREE( vs_job );
	
	return ShapeCtrlJobRes_success;
}

void ShapeCtrlJob_InitFTBL_ValidateShape( void )
{
	if ( ftbl_validate_shape_valid == true )
		return;

	ftbl_validate_shape_valid = true;

	memset( &ftbl_validate_shape, 0, sizeof( shape_ctrl_job_ftbl_t ) );

	ftbl_validate_shape.RunJob = ShapeCtrlJob_RunJob_ValidateShape;
	ftbl_validate_shape.DestroyJob = ShapeCtrlJob_DestroyJob_ValidateShape;
}

shape_ctrl_job_t * ShapeCtrlJob_Create_ValidateShape( shape_t *shp )
{
	shape_ctrl_job_validate_shape_t		*job;

	__chkptr( shp );

	ShapeCtrlJob_InitFTBL_ValidateShape();

	job = NEWTYPE( shape_ctrl_job_validate_shape_t );
	job->shape_ctrl_job_if.ftbl = &ftbl_validate_shape;
	job->shape_ctrl_job_if.state = ShapeCtrlJobState_run;

	// init private data in derived class
	job->shape = shp;

	return &job->shape_ctrl_job_if;
}


/*
  ==================================================
  ShapeControlJob_UpdateLayerSubimage

  ==================================================
*/

static bool_t			ftbl_UpdateLayerSubimage_valid = false;
static shape_ctrl_job_ftbl_t	ftbl_UpdateLayerSubimage;


shapeCtrlJobRes ShapeCtrlJob_RunJob_UpdateLayerSubimage( shape_ctrl_job_t *job )
{
	shape_ctrl_job_UpdateLayerSubimage_t		*uls_job;
	ic_subimage_t				*sub;

	u_list_iter_t			iter;
	layer_t				*layer;

	uls_job = ADDR_OF_BASE( shape_ctrl_job_UpdateLayerSubimage_t, shape_ctrl_job_if, job );

	//
	// search layer with ordinal
	//
	U_ListIterInit( &iter, uls_job->shape->layer_list );
	for ( ; ( layer = (layer_t *)U_ListIterNext( &iter ) ) ; )
	{
		if ( layer->ordinal == uls_job->layer_ordinal )
		{			
			break;
		}
	}

	if ( !layer )
	{
		__warning( "no layer with ordinal '%d' found in shape, ignored\n", uls_job->layer_ordinal );
		return ShapeCtrlJobRes_failed;
	}

	//
	// has layer a subimage
	//
	sub = UniTex_IsReal_subimage( layer->real_source );

	if ( !sub )
	{
		__warning( "the requested layer ain't got a ic_subimage type unitex, ignored\n" );
		return ShapeCtrlJobRes_failed;
	}
	
	IC_SubImageSetPixels( sub, uls_job->update_pixels );
	
	//
	// if the subimage is cached, force upload
	//

#if 1
	ICU_UploadFrameAddSubImage( &r_upload_frame, sub );
#else	
	if ( IC_SubImageIsCached( sub ) )
	{
		IC_SubImageInsertIntoImage( sub, sub->pixels );
	}
#endif

	return ShapeCtrlJobRes_success;	
}

shapeCtrlJobRes ShapeCtrlJob_DestroyJob_UpdateLayerSubimage( shape_ctrl_job_t *job )
{
	shape_ctrl_job_UpdateLayerSubimage_t		*vs_job;

	vs_job = ADDR_OF_BASE( shape_ctrl_job_UpdateLayerSubimage_t, shape_ctrl_job_if, job );
	FREE( vs_job );
	
	return ShapeCtrlJobRes_success;	
}

void ShapeCtrlJob_InitFTBL_UpdateLayerSubimage( void )
{
	if ( ftbl_UpdateLayerSubimage_valid == true )
		return;
	
	ftbl_UpdateLayerSubimage_valid = true;

	memset( &ftbl_UpdateLayerSubimage, 0, sizeof( shape_ctrl_job_ftbl_t ) );

	ftbl_UpdateLayerSubimage.RunJob = ShapeCtrlJob_RunJob_UpdateLayerSubimage;
	ftbl_UpdateLayerSubimage.DestroyJob = ShapeCtrlJob_DestroyJob_UpdateLayerSubimage;
}

shape_ctrl_job_t * ShapeCtrlJob_Create_UpdateLayerSubimage( shape_t *shp, int layer_ordinal, void *pixels )
{
	shape_ctrl_job_UpdateLayerSubimage_t	*job;

	__chkptr( shp );

	ShapeCtrlJob_InitFTBL_UpdateLayerSubimage();

	job = NEWTYPE( shape_ctrl_job_UpdateLayerSubimage_t );
	job->shape_ctrl_job_if.ftbl = &ftbl_UpdateLayerSubimage;
	job->shape_ctrl_job_if.state = ShapeCtrlJobState_run;

	// init private data in derived class
	job->shape = shp;
	job->layer_ordinal = layer_ordinal;
	job->update_pixels = pixels;

	return &job->shape_ctrl_job_if;
}


