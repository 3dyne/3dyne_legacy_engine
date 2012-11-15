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



// r_lightmap.c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "lib_math.h"
#include "lib_container.h"

#include "u_pixel.h"

#include "render.h"
#include "db_shape.h"
#include "db_lightmap.h"
#include "imgcache.h"
#include "unitex.h"

#include "r_shpctrljobs.h"

/*
  ==============================
  R_LightmapAccumulation

  ==============================
*/
static int	acc_pixelnum;
static void	*acc_pixels;

void R_BeginPixelsAccumulation8888( void *pixels, int pixelnum )
{
	acc_pixelnum = pixelnum;
	acc_pixels = pixels;

	memset( acc_pixels, 0, pixelnum*4 );
}

void R_LightmapAccumulate565_old( lightmap_t *lmap )
{
	int		i;

	unsigned char	*inout;
	unsigned short	*in;	

	inout = (unsigned char *)acc_pixels;
	in = (unsigned short*)lmap->pixel_ptr;

	for ( i = 0; i < acc_pixelnum; i++ )
	{
		unsigned char	p1[4];

		U_ConvertPixel_565to8888( p1, &in[i] );
		U_PixelAddWithSaturation_8888( &inout[i*4], &inout[i*4], p1 );
	}	
}

void R_LightmapAccumulateScaled565_old( lightmap_t *lmap, int scale )
{
	int		i, j;

	unsigned char	*inout;
	unsigned short	*in;	

	static bool_t		ramps_valid = false;
	static unsigned char	ramps[16][256];

	if ( ramps_valid == false )
	{ 
		ramps_valid = true;
		
		for ( i = 0; i < 16; i++ )
		{
			for ( j = 0; j < 256; j++ )
			{
				fp_t		b;
				int		c;
				
				b = ((fp_t) j * (i/16.0));
				c = (int) b;
				ramps[i][j] = (unsigned char) c;
				
			}
		}
	}

	inout = (unsigned char *)acc_pixels;
	in = (unsigned short *)lmap->pixel_ptr;

	if ( scale == 15 )
	{
		for ( i = 0; i < acc_pixelnum; i++ )
		{
			unsigned char	p1[4];
			
			U_ConvertPixel_565to8888( p1, &in[i] );
			U_PixelAddWithSaturation_8888( &inout[i*4], &inout[i*4], p1 );
		}
	}	
	else if ( scale > 0 )
	{
		for ( i = 0; i < acc_pixelnum; i++ )
		{
			unsigned char	p1[4];
			
			U_ConvertPixel_565to8888( p1, &in[i] );
			p1[0] = ramps[scale][p1[0]];
			p1[1] = ramps[scale][p1[1]];
			p1[2] = ramps[scale][p1[2]];

			U_PixelAddWithSaturation_8888( &inout[i*4], &inout[i*4], p1 );
		}		
	}
}

void R_LightmapAccumulate565( lightmap_t *lmap )
{
	int		i;

	unsigned char	*inout;
	unsigned short	*in;	

	inout = (unsigned char *)acc_pixels;
	in = (unsigned short *)lmap->pixel_ptr;

	for ( i = 0; i < acc_pixelnum; i++, in++ )
	{
		unsigned char	p1[4];		
		int		tmp;

		p1[0] = (((*in)>>11)&31)<<3;
		p1[1] = (((*in)>>5)&63)<<2;
		p1[2] = (((*in))&31)<<3;
		
		tmp = p1[0]+*inout;
		if ( tmp > 255 )
			tmp = 255;
		*inout = tmp;
		inout++;

		tmp = p1[1]+*inout;
		if ( tmp > 255 )
			tmp = 255;
		*inout = tmp;
		inout++;

		tmp = p1[2]+*inout;
		if ( tmp > 255 )
			tmp = 255;
		*inout = tmp;
		inout++;

		*inout = 255;
		inout++;
	}	
}


void R_LightmapAccumulateScaled565( lightmap_t *lmap, int scale )
{
	int		i, j;

	unsigned char	*inout;
	unsigned short	*in;	

	static bool_t		ramps_valid = false;
	static unsigned char	ramps[16][256];

//	__named_message( "scale %d\n", scale );

	if ( ramps_valid == false )
	{ 
		ramps_valid = true;
		
		for ( i = 0; i < 16; i++ )
		{
			for ( j = 0; j < 256; j++ )
			{
				fp_t		b;
				int		c;
				
				b = ((fp_t) j * (i/15.0));
				c = (int) b;
				ramps[i][j] = (unsigned char) c;
				
			}
		}
	}

	inout = (unsigned char *)acc_pixels;
	in = (unsigned short *)lmap->pixel_ptr;

	for ( i = 0; i < acc_pixelnum; i++, in++ )
	{
		unsigned char	p1[4];		
		int		tmp;

		p1[0] = (((*in)>>11)&31)<<3;
		p1[1] = (((*in)>>5)&63)<<2;
		p1[2] = (((*in))&31)<<3;

		p1[0] = ramps[scale][p1[0]];
		p1[1] = ramps[scale][p1[1]];
		p1[2] = ramps[scale][p1[2]];
		
		tmp = p1[0]+inout[i*4];
		if ( tmp > 255 )
			tmp = 255;
		inout[i*4] = tmp;

		tmp = p1[1]+inout[i*4+1];
		if ( tmp > 255 )
			tmp = 255;
		inout[i*4+1] = tmp;

		tmp = p1[2]+inout[i*4+2];
		if ( tmp > 255 )
			tmp = 255;
		inout[i*4+2] = tmp;

		inout[i*4+3] = 255;
	}
}


/*
  ==============================
  R_ShapeLoadLightmaps

  ==============================
*/
void R_ShapeLoadLightmaps( shape_t *shp )
{
#if 0
	u_list_iter_t		iter;
	lightmap_t		*head;
	layer_t			*layer;
	int		type;
	ic_subimage_t	*sub;

	U_ListIterInit( &iter, shp->layer_list );
	for ( ; ( layer = U_ListIterNext( &iter ) ) ; )
	{
		if ( layer->ordinal == LAYER_DIFFUSE_1 )
		{
			// diffuse lightmap layer
			type = 0;
		}
		else if ( layer->ordinal == LAYER_SPECULAR_1 )
		{
			// specular lightmap layer
			type = 1;
		}
		else
		{
			continue;
		}

		sub = UniTex_IsReal_subimage( layer->real_source );
		if ( !sub )
		{
			// no subimage, don't know what to do
			continue;
		}
		
		R_BeginPixelsAccumulation8888( sub->pixels, sub->width*sub->height );
		head = LightmapDB_SelectLightmapsWhere_shapeid( a_map->lightmap_db, shp->self_id );
		for ( ; head ; head=head->shape_link )
		{
			if ( head->type == type && (head->flags & LIGHTMAP_STATE_FLAGS_ACTIVE ) )
			{
				R_LightmapAccumulate565( head );	
			}
		}

		{
			shape_ctrl_job_t	*job;

			job = ShapeCtrlJob_Create_UpdateLayerSubimage( shp, layer->ordinal, sub->pixels );
			job->ftbl->RunJob( job );
			job->ftbl->DestroyJob( job );
		}

//		if ( IC_SubImageIsCached( sub ) )
//			IC_SubImageInsertIntoImage( sub, sub->pixels );
	}

#endif
}

/*
  ==============================
  R_AllShapesLoadLightmaps

  ==============================
*/
void R_AllShapesLoadLightmaps( void )
{
	printf( " load all lightmaps ...\n" );
	ShapeDB_SelectShapeWhere_selfid_CB( a_map->shape_db, UNIQUE_ALL, R_ShapeLoadLightmaps );	
}


/*
  ==================================================
  ShapeControlJob: LightmapStateWatch

  ==================================================
*/

static bool_t			ftbl_LightmapStateWatch_valid = false;
static shape_ctrl_job_ftbl_t	ftbl_LightmapStateWatch;


typedef struct lightmap_finger_print_s
{
	lightmap_t	*lmap;
	int		scale;
} lightmap_finger_print_t;

#define MAX_LIGHTMAPS_PER_FINGER_PRINT		( 256 )

typedef struct pixel_cache_entry_s
{
	int				fpnum;
	lightmap_finger_print_t		fps[MAX_LIGHTMAPS_PER_FINGER_PRINT];
	void				*pixel_ptr;
} pixel_cache_entry_t;

void * FingerPrintCombinePixels( lightmap_finger_print_t array[], int len, int pixel_num )
{
	int		i;
	void	*pixels;

	pixels = NEWBYTES( pixel_num*4 );
	
	R_BeginPixelsAccumulation8888( pixels, pixel_num );	
	for ( i = 0; i < len; i++ )
	{
		R_LightmapAccumulateScaled565( array[i].lmap, array[i].scale );	
	}

	return pixels;
}

int SignatureForBuffer( void *buf, int size )
{
	int		i;
	int	val;
	char	*c;

	c = (char *)buf;
	val = 0;

	for ( i = 0; i < size; i++ )
	{
		val = (val<<3) ^ ((val&((1<<31)+(1<<30)+(1<<29)))>>29);
//		val <<= 3;
		val ^= *c;
		c++;
	}

	return val;
}

static shapeCtrlJobRes ShapeCtrlJob_RunJob_LightmapStateWatch( shape_ctrl_job_t *job_if )
{
	shape_ctrl_job_LightmapStateWatch_t		*job;
// 	shape_t						*shp;

	job = ADDR_OF_BASE( shape_ctrl_job_LightmapStateWatch_t, shape_ctrl_job_if, job_if );
// 	shp = job->shape;

#if 0
	if ( job->run_count == 0 )
	{
		//
		// first run, init the job
		//

		job->ms_start = ms_rfbegin;

		// get lightmap list from db
		job->lightmap_head = LightmapDB_SelectLightmapsWhere_shapeid( a_map->lightmap_db, shp->self_id );

		// reset state_changed_flag int all lightmaps
		job->instable_states = false;
		for ( l = job->lightmap_head; l; l=l->shape_link )
		{
			l->flags &= ~LIGHTMAP_STATE_FLAGS_STATE_CHANGED;

			if ( ( l->flags & LIGHTMAP_STATE_FLAGS_HAVE_STYLE ) )
			{
				job->instable_states = true;
			}
		}
	}

	if ( job->run_count != 0 )
	{
		//
		// not first run, check if job should dy
		//
		
		// if there is just one lightmap with state_changed_flag set, kill job
		for ( l = job->lightmap_head; l ; l=l->shape_link )
		{
			if ( l->flags & LIGHTMAP_STATE_FLAGS_STATE_CHANGED )
			{
				job_if->state = ShapeCtrlJobState_destroy; 
				return ShapeCtrlJobRes_success;
			}
		}
	}

	//
	// do lightmap update
	//

	local_time = ms_rfbegin - job->ms_start;
	global_time = ms_rfbegin;

	if ( job->instable_states == true && shp->last_rf != r_frame_count )
	{
		//
		// instable states are not updated if the shape is not visible
		//
//		printf( "instable_states job on invisilbe shape\n" );
	}
	else
	{
		u_list_iter_t		iter;
		layer_t			*layer;
		ic_subimage_t		*sub;
		int			lmap_type;

		U_ListIterInit( &iter, shp->layer_list );
		for ( ; ( layer = U_ListIterNext( &iter ) ) ; )
		{
			if ( layer->ordinal == LAYER_DIFFUSE_1 )
			{
				// diffuse lightmap ( type 0 )
				lmap_type = 0;
			}
			else if ( layer->ordinal == LAYER_SPECULAR_1 )
			{
				// specular lightmap ( type 1 )
				lmap_type = 1;
			}
			else
			{
				continue;
			}			
			
			sub = UniTex_IsReal_subimage( layer->real_source );
			if ( !sub )
			{
				// no subimage, don't know what to do
				continue;
			}

//			R_BeginPixelsAccumulation8888( sub->pixels, sub->width*sub->height );

			//
			// find all lightmaps of this layer and set the scale
			//

			r_lmapfpnum = 0;
			for ( l = job->lightmap_head; l ; l=l->shape_link )
			{
				int		scale;

				if ( l->type != lmap_type )
				{
					continue;
				}
				
				if ( l->flags & LIGHTMAP_STATE_FLAGS_ACTIVE )
				{
					if ( l->flags & LIGHTMAP_STATE_FLAGS_HAVE_STYLE )
					{
						int		style_len;
						__chkptr( l->style_string );
						style_len = strlen( l->style_string );
						
						if ( l->flags & LIGHTMAP_STATE_FLAGS_LOCAL_TIME )
							// local time base
							scale = (int)(l->style_string[(local_time/32)%style_len] - 'A');
						else
							// global time base
							scale = (int)(l->style_string[(global_time/32)%style_len] - 'A');

						if ( scale < 0 )
							scale = 0;
						if ( scale > 15 )
							scale = 15;
						
//						R_LightmapAccumulateScaled565( l, scale );
					}
					else
					{
						scale = 15;
//						R_LightmapAccumulate565( l );	
					}
				}
				else
				{
					scale = 0;
				}
				
				if ( r_lmapfpnum >= MAX_LIGHTMAPS_PER_FINGER_PRINT )
					__error( "reached MAX_LIGHTMAPS_PER_FINGER_PRINT\n" );
				
				r_lmapfps[r_lmapfpnum].scale = scale;
				r_lmapfps[r_lmapfpnum].lmap = l;
				r_lmapfpnum++;
			}
			
			if ( r_lmapfpnum > 0 )
			{
				void		*pixels;
				int		i;
				int		key;

//				__named_message( " 

				key = SignatureForBuffer( r_lmapfps, sizeof( lightmap_finger_print_t ) * r_lmapfpnum );
//				printf( "%x\n", key );

				// search cache
				pixels = NULL;
				for ( i = 0; i < 32; i++ )
				{
					if ( job->cache[i].key == key )
					{
						pixels = job->cache[i].pixel;
//						__named_message( "cache hit\n" );
						break;
					}				       
				}
			
				if ( !pixels )
				{
//					__named_message( "cache miss\n" );
					pixels = FingerPrintCombinePixels( r_lmapfps, r_lmapfpnum, sub->width*sub->height );
					__chkptr( pixels );
				
					// insert in cache
					
					if ( job->cache[job->cache_pos].pixel )
					{
						// free old
//						__named_message( "cache flush\n" );
						FREE( job->cache[job->cache_pos].pixel );
					}
					job->cache[job->cache_pos].key = key;
					job->cache[job->cache_pos].pixel = pixels;
					job->cache_pos = (job->cache_pos+1) & 31;
//					InsertPixelCache( &job->pixel_cache, r_lmapfps, r_lmapfpnum, pixels );
				}
				
				memcpy( sub->pixels, pixels, sub->width*sub->height*4 );
//				FREE( pixels );

//				R_BeginPixelsAccumulation8888( sub->pixels, sub->width*sub->height );			
//				for ( i = 0; i < r_lmapfpnum; i++ )
//				{			
//					R_LightmapAccumulateScaled565( r_lmapfps[i].lmap, r_lmapfps[i].scale );	
//				}				
				ICU_UploadFrameAddSubImage( &r_upload_frame, sub );			       
			}
		}
	}


	//
	// if there are no instable states, the job dies
	//
	if ( job->instable_states == false )
	{
		job_if->state = ShapeCtrlJobState_destroy; 
		return ShapeCtrlJobRes_success;		
	}

#else
	job_if->state = ShapeCtrlJobState_destroy; 
#endif

	job->run_count++;
	return ShapeCtrlJobRes_success;
}

static shapeCtrlJobRes ShapeCtrlJob_DestroyJob_LightmapStateWatch( shape_ctrl_job_t *job_if )
{
	shape_ctrl_job_LightmapStateWatch_t		*job;
	int		i;

	job = ADDR_OF_BASE( shape_ctrl_job_LightmapStateWatch_t, shape_ctrl_job_if, job_if );

	// empty pixel cache
	for ( i = 0; i < 32; i++ )
	{
		if ( job->cache[i].pixel )
			FREE( job->cache[i].pixel );
	}

	FREE( job );
		
	return ShapeCtrlJobRes_success;	
}

static void ShapeCtrlJob_InitFTBL_LightmapStateWatch( void )
{
	if ( ftbl_LightmapStateWatch_valid == true )
		return;

	ftbl_LightmapStateWatch_valid = true;

	INITTYPE( &ftbl_LightmapStateWatch, shape_ctrl_job_ftbl_t );

	ftbl_LightmapStateWatch.RunJob = /*(shapeCtrlJobRes (*)( shape_ctrl_job_t * )) */ShapeCtrlJob_RunJob_LightmapStateWatch;
	ftbl_LightmapStateWatch.DestroyJob = /*(shapeCtrlJobRes (*)( shape_ctrl_job_t * ))*/ ShapeCtrlJob_DestroyJob_LightmapStateWatch;	
}

shape_ctrl_job_t * ShapeCtrlJob_Create_LightmapStateWatch( shape_t *shp )
{
	shape_ctrl_job_LightmapStateWatch_t	*job;

	ShapeCtrlJob_InitFTBL_LightmapStateWatch();
	
	job = NEWTYPE( shape_ctrl_job_LightmapStateWatch_t );
	job->shape_ctrl_job_if.ftbl = &ftbl_LightmapStateWatch;
	job->shape_ctrl_job_if.state = ShapeCtrlJobState_run;

	// init private data in derived class
	job->shape = shp;
	job->run_count = 0;

	return &job->shape_ctrl_job_if;
}
