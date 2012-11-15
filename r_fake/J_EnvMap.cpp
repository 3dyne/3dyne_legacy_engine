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



// J_EnvMap.c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "lib_math.h"
#include "lib_container.h"

#include "r_count.h"
#include "render.h"
#include "tf2d.h"
#include "mesh.h"
#include "shape_ctrl_job_defs.h"

typedef struct shape_ctrl_job_EnvMap_s
{
	shape_ctrl_job_t	jif;

	// private data
	hobj_t			*job_obj;
	layer_t			*layer;
	tf2d_shift_t		*tf2d_shift;
	tf2d_scale_t		*tf2d_scale;

	fp_t			scale;
	fp_t			offset;

	int			run_count;	
} shape_ctrl_job_EnvMap_t;


/*
  ==============================
  RunJob

  ==============================
*/
static shapeCtrlJobRes RunJob( shape_ctrl_job_t *jif )
{
	shape_ctrl_job_EnvMap_t		*job;
	fp_t				d;
	hpair_t				*pair;
	
	job = ADDR_OF_BASE( shape_ctrl_job_EnvMap_t, jif, jif );

	if ( job->run_count == 0 )
	{
		tf2d_t		*tf2d;

		pair = FindHPair( job->job_obj, "scale" );
		if ( !pair )
		{
			J_ObjWarning( job->job_obj, "missing key 'scale'" );
			goto destroy;				
		}
		else
		{
			job->scale = atof( pair->value );
			
			if ( job->scale == 0.0 )
			{
				J_ObjWarning( job->job_obj, "key 'scale' got an invalid value" );
				goto destroy;	
			}
		}
		
		pair = FindHPair(job->job_obj, "offset" );
		if ( !pair )
		{
			J_ObjWarning( job->job_obj, "missing key 'offset'" );
			goto destroy;				
		}
		else
		{
			job->offset = atof( pair->value );
		} 

		// init job
		tf2d = TF2D_CreateShift();
		__chkptr( job->layer->tf2d_stack );
		TF2D_StackPush( job->layer->tf2d_stack, tf2d );
		job->tf2d_shift = TF2D_IsShift( tf2d );			

		tf2d = TF2D_CreateScale();
		__chkptr( job->layer->tf2d_stack );
		TF2D_StackPush( job->layer->tf2d_stack, tf2d );
		job->tf2d_scale = TF2D_IsScale( tf2d );			
	}

	//
	// run job on a visible shape ?
	//

	if ( job->layer->shape->visible_rf != r_frame_count )
	{
		goto leave;
	}

	COUNT_INCR( count_rf_num_vis_job );


	if ( job->layer->flags & LAYER_FLAGS_PROJECTION_X )
	{
		job->tf2d_shift->shift[0] = -r_origin[2];
		job->tf2d_shift->shift[1] = -r_origin[1];
	}
	else if ( job->layer->flags & LAYER_FLAGS_PROJECTION_Y )
	{
		job->tf2d_shift->shift[0] = -r_origin[0];
		job->tf2d_shift->shift[1] = -r_origin[2];
	}
	else if ( job->layer->flags & LAYER_FLAGS_PROJECTION_Z )
	{
		job->tf2d_shift->shift[0] = -r_origin[0];
		job->tf2d_shift->shift[1] = -r_origin[1];
	}	

	d = Vec3dDotProduct( job->layer->shape->norm, r_origin ) - job->layer->shape->dist;

	if ( d < 0.99 )
		goto leave;

	d += job->offset;

#if 1
	{
		fp_t		xs, ys;

		xs = ((int)(fabs(r_origin[0]+r_origin[1]+r_origin[2]))%256);
		ys = ((int)(fabs(r_origin[0]+r_origin[1]+r_origin[2]+128))%256);
//		printf( "#%u: %f %f\n", job->layer->shape->self_id, xs, ys );

		xs = 0.9+(fabs(xs-128)/128.0)*0.2;
		ys = 0.9+(fabs(ys-128)/128.0)*0.2;

//		xs = ys = 1.0;

		job->tf2d_scale->scale[0] = job->scale / (d*ys);
		job->tf2d_scale->scale[1] = job->scale / (d*xs);
	}
#else
	job->tf2d_scale->scale[0] = job->scale / d;
	job->tf2d_scale->scale[1] = job->scale / d;
#endif

	if ( job->layer->tc )
	{
		Mesh_Free( job->layer->tc );
		job->layer->tc = NULL;
			
	}
	job->layer->shape->flags |= SHAPE_FLAGS_LAYERS_NEED_CHECK;
		

leave:
	job->run_count++;
	return ShapeCtrlJobRes_success;	

destroy:
	job->jif.state = ShapeCtrlJobState_destroy;
	return ShapeCtrlJobRes_success;
}

/*
  ==============================
  DestroyJob

  ==============================
*/
static shapeCtrlJobRes DestroyJob( shape_ctrl_job_t *jif )
{
	shape_ctrl_job_EnvMap_t		*job;

	job = ADDR_OF_BASE( shape_ctrl_job_EnvMap_t, jif, jif );
	FREE( job );
	
	return ShapeCtrlJobRes_success;	
}



/*
  ==================================================
  ShapeCtrlJob_Create_EnvMap

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

shape_ctrl_job_t * ShapeCtrlJob_Create_EnvMap( hobj_t *job_obj, layer_t *layer )
{
	shape_ctrl_job_EnvMap_t		*job;

	InitFTBL();

	job = NEWTYPE( shape_ctrl_job_EnvMap_t );
	job->jif.ftbl = &ftbl;
	job->jif.state = ShapeCtrlJobState_run;

	// init private data
	job->job_obj = job_obj;
	job->layer = layer;

	job->run_count = 0;

	return &job->jif;
}
