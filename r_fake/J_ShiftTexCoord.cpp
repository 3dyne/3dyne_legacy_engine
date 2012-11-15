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



// J_ShiftTexCoord.c

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

typedef struct shape_ctrl_job_ShiftTexCoord_s
{
	shape_ctrl_job_t	jif;

	// private data
	hobj_t			*job_obj;
	layer_t			*layer;

	tf2d_shift_t			*tf2d_shift;	// is popped on layer's tf2d stack

	// u
	int			u_style;	// 0: none, 1: sin, 2: cos, 3: snap4
	fp_t			u_timediv;	
	fp_t			u_scale;

	// v
	int			v_style;	// 0: none, 1: sin, 2: cos, 3: snap4
	fp_t			v_timediv;	
	fp_t			v_scale;

	int			run_count;
} shape_ctrl_job_ShiftTexCoord_t;

/*
  ==============================
  RunJob

  ==============================
*/
static shapeCtrlJobRes RunJob( shape_ctrl_job_t *jif )
{
	shape_ctrl_job_ShiftTexCoord_t		*job;
	
	msec_t		global_time;

	job = ADDR_OF_BASE( shape_ctrl_job_ShiftTexCoord_t, jif, jif );

	global_time = ms_rfbegin;

	if ( job->run_count == 0 )
	{
		tf2d_t		*tf2d;
		hpair_t		*pair;

		pair = FindHPair( job->job_obj, "u_style" );
		if ( !pair )
		{
			job->u_style = 0;
		}
		else if ( !strcmp( "none", pair->value ) )
		{
			job->u_style = 0;
		}
		else if ( !strcmp( "sin", pair->value ) )
		{
			job->u_style = 1;
		}
		else if ( !strcmp( "cos", pair->value ) )
		{
			job->u_style = 2;
		}
		else if ( !strcmp( "snap4", pair->value ) )
		{
			job->u_style = 3;
		}
			
		pair = FindHPair( job->job_obj, "v_style" );
		if ( !pair )
		{
			job->v_style = 0;
		}
		else if ( !strcmp( "none", pair->value ) )
		{
			job->v_style = 0;
		}
		else if ( !strcmp( "sin", pair->value ) )
		{
			job->v_style = 1;
		}
		else if ( !strcmp( "cos", pair->value ) )
		{
			job->v_style = 2;
		}		
		else if ( !strcmp( "snap4", pair->value ) )
		{
			job->v_style = 3;
		}


		pair = FindHPair( job->job_obj, "u_timediv" );
		if ( !pair )
		{
			job->u_timediv = 1.0;
		}
		else
		{
			job->u_timediv = atof( pair->value );
			if ( job->u_timediv == 0.0 )
			{
				J_ObjWarning( job->job_obj, "key 'u_timediv' got an invalid value" );
				goto destroy;				
			}
		}

		pair = FindHPair( job->job_obj, "v_timediv" );
		if ( !pair )
		{
			job->v_timediv = 1.0;
		}
		else
		{
			job->v_timediv = atof( pair->value );
			if ( job->v_timediv == 0.0 )
			{
				J_ObjWarning( job->job_obj, "key 'v_timediv' got an invalid value" );
				goto destroy;				
			}
		}

		pair = FindHPair( job->job_obj, "u_scale" );
		if ( !pair )
		{
			job->u_scale = 1.0;
		}
		else
		{
			job->u_scale = atof( pair->value );
		}

		pair = FindHPair( job->job_obj, "v_scale" );
		if ( !pair )
		{
			job->v_scale = 1.0;
		}
		else
		{
			job->v_scale = atof( pair->value );
		}		

		// init job
		tf2d = TF2D_CreateShift();
		__chkptr( job->layer->tf2d_stack );
		TF2D_StackPush( job->layer->tf2d_stack, tf2d );
		job->tf2d_shift = TF2D_IsShift( tf2d );			
	}

	if ( job->layer->shape->visible_rf != r_frame_count )
	{
		goto leave;
	}

	COUNT_INCR( count_rf_num_vis_job );


	if ( job->u_style == 0 )
	{
		job->tf2d_shift->shift[0] = ((global_time%((int)(job->u_timediv)))/job->u_timediv)*job->u_scale;
	}
	else if ( job->u_style == 1 )
	{
		job->tf2d_shift->shift[0] = sin(global_time/job->u_timediv)*job->u_scale;
	}
	else if ( job->u_style == 2 )
	{
		job->tf2d_shift->shift[0] = cos(global_time/job->u_timediv)*job->u_scale;
	}
	else if ( job->u_style == 3 )
	{
		fp_t	s;
		s = ((global_time%((int)(job->u_timediv)))/job->u_timediv)*job->u_scale;

		if ( s < job->u_scale*0.25 )
			s = 0.0;
		else if ( s < job->u_scale*0.5 )
			s = job->u_scale*0.25;
		else if ( s < job->u_scale*0.75 )
			s = job->u_scale*0.5;
		else
			s = job->u_scale*0.75;

		printf( " >>>>>>>>>>>>>>>>>>>>>>>>>>>> u = %f\n", s );
		job->tf2d_shift->shift[0] = s;
	}

	if ( job->v_style == 0 )
	{
		job->tf2d_shift->shift[1] = ((global_time%((int)(job->v_timediv)))/job->v_timediv)*job->v_scale;
	}
	else if ( job->v_style == 1 )
	{
		job->tf2d_shift->shift[1] = sin(global_time/job->v_timediv)*job->v_scale;
	}
	else if ( job->v_style == 2 )
	{
		job->tf2d_shift->shift[1] = cos(global_time/job->v_timediv)*job->v_scale;
	}
	else if ( job->v_style == 3 )
	{
		fp_t	s;
		s = ((global_time%((int)(job->v_timediv)))/job->v_timediv)*job->v_scale;
		if ( s < job->v_scale*0.25 )
			s = 0.0;
		else if ( s < job->v_scale*0.5 )
			s = job->v_scale*0.25;
		else if ( s < job->v_scale*0.75 )
			s = job->v_scale*0.5;
		else
			s = job->v_scale*0.75;
		printf( " >>>>>>>>>>>>>>>>>>>>>>>>>>>> v = %f\n", s );
		job->tf2d_shift->shift[1] = s;		
	}
		

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
	shape_ctrl_job_ShiftTexCoord_t		*job;

	job = ADDR_OF_BASE( shape_ctrl_job_ShiftTexCoord_t, jif, jif );
	FREE( job );
	
	return ShapeCtrlJobRes_success;	
}


/*
  ==================================================
  ShapeCtrlJob_Create_ShiftTexCoord

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

shape_ctrl_job_t * ShapeCtrlJob_Create_ShiftTexCoord( hobj_t *job_obj, layer_t *layer )
{
	shape_ctrl_job_ShiftTexCoord_t		*job;

	InitFTBL();

	job = NEWTYPE( shape_ctrl_job_ShiftTexCoord_t );
	job->jif.ftbl = &ftbl;
	job->jif.state = ShapeCtrlJobState_run;

	// init private data
	job->job_obj = job_obj;
	job->layer = layer;

	job->run_count = 0;

	return &job->jif;
}
