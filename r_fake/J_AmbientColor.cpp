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



// J_AmbientColor.c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "lib_math.h"
#include "lib_container.h"

#include "render.h"

//#include "r_shpctrljobs.h"
#include "shape_ctrl_job_defs.h"

typedef struct shape_ctrl_job_AmbientColor_s
{
	shape_ctrl_job_t	jif;		// inherited interface

	// private data:
	shape_t			*shape;		// target of the job
	wf_shape_info_t		*info;		// ref to the info of this job
	unique_t		order_count;	// if info.order_count differs from order_count, job dies

	msec_t			ms_start;	// job was started at this world time ( for local time styles )
} shape_ctrl_job_AmbientColor_t;

/*
  ==============================
  RunJob

  ==============================
*/
static shapeCtrlJobRes RunJob( shape_ctrl_job_t *jif )
{
	shape_ctrl_job_AmbientColor_t	*job;
	shape_t				*shp;
	layer_t				*layer;

	u_list_iter_t		iter;
	msec_t			global_time;

	job = ADDR_OF_BASE( shape_ctrl_job_AmbientColor_t, jif, jif );
	shp = job->shape;

	global_time = ms_rfbegin;

	if ( job->order_count != (unique_t)job->info->order_count )
	{
		jif->state = ShapeCtrlJobState_destroy; 
		return ShapeCtrlJobRes_success;	
	}
	
	//
	// hack: always set ambient_color of layer #1
	//
	U_ListIterInit( &iter, shp->layer_list );
	for ( ; ( layer = (layer_t *)U_ListIterNext( &iter ) ) ; )
	{
		if ( layer->ordinal == LAYER_TEXTURE_1 )
		{
			if ( job->info->style_string == NULL )
			{
				layer->ambient_color[C_R] = (unsigned char)(job->info->color[0] * 255.0);
				layer->ambient_color[C_G] = (unsigned char)(job->info->color[1] * 255.0);
				layer->ambient_color[C_B] = (unsigned char)(job->info->color[2] * 255.0);
				layer->ambient_color[C_A] = 255;				
			}
			else
			{
				int	style_len;
				fp_t	scale;

				style_len = strlen( job->info->style_string );
				
				scale = (job->info->style_string[(global_time/32)%style_len] - 'A')/15.0;
				layer->ambient_color[C_R] = (unsigned char)(job->info->color[0] * 255.0 * scale);
				layer->ambient_color[C_G] = (unsigned char)(job->info->color[1] * 255.0 * scale);
				layer->ambient_color[C_B] = (unsigned char)(job->info->color[2] * 255.0 * scale);
				layer->ambient_color[C_A] = 255;
			}
		}
	}

	if ( job->info->style_string == NULL )
	{
		// if there is no style to run, job can die
		jif->state = ShapeCtrlJobState_destroy; 
	}

	return ShapeCtrlJobRes_success;
}

/*
  ==============================
  DestroyJob

  ==============================
*/
static shapeCtrlJobRes DestroyJob( shape_ctrl_job_t *jif )
{
	shape_ctrl_job_AmbientColor_t	*job;

	job = ADDR_OF_BASE( shape_ctrl_job_AmbientColor_t, jif, jif );
	job->info->ref_count--;
	FREE( job );
	
	return ShapeCtrlJobRes_success;	
}

/*
  ==================================================
  ShapeCtrlJob_Create_AmbientColor

  ==================================================
*/
static bool_t			ftbl_valid = false;
static shape_ctrl_job_ftbl_t	ftbl;

static void InitFTBL( void )
{
	if ( ftbl_valid )
		return;

	ftbl_valid = true;

	INITTYPE( &ftbl, shape_ctrl_job_ftbl_t );
	
	ftbl.RunJob = RunJob;
	ftbl.DestroyJob = DestroyJob;
}

shape_ctrl_job_t * ShapeCtrlJob_Create_AmbientColor( shape_t *shp, wf_shape_info_t *info )
{
	shape_ctrl_job_AmbientColor_t		*job;

	__named_message( "\n" );

	InitFTBL();

	job = NEWTYPE( shape_ctrl_job_AmbientColor_t );
	job->jif.ftbl = &ftbl;
	job->jif.state = ShapeCtrlJobState_run;

	// init private data
	job->shape = shp;

	// info 
	info->ref_count++;
	job->info = info;
	job->order_count = info->order_count;

	return &job->jif;
}
