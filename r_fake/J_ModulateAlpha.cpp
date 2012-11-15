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



// J_ModulateAlpha.c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "lib_math.h"
#include "lib_container.h"

#include "r_count.h"
#include "render.h"

//#include "r_shpctrljobs.h"
#include "shape_ctrl_job_defs.h"

typedef struct shape_ctrl_job_ModulateAlpha_s
{
	shape_ctrl_job_t	jif;

	// private data:
	hobj_t		*job_obj;	// job was created from this job_obj
	layer_t		*layer;	
	char		*style_string;

	int		run_count;
} shape_ctrl_job_ModulateAlpha_t;

/*
  ==============================
  RunJob

  ==============================
*/
static shapeCtrlJobRes RunJob( shape_ctrl_job_t *jif )
{
	shape_ctrl_job_ModulateAlpha_t		*job;
	hpair_t			*pair;
	
	msec_t				global_time;

	job = ADDR_OF_BASE( shape_ctrl_job_ModulateAlpha_t, jif, jif );

	global_time = ms_rfbegin;

	if ( job->run_count == 0 )
	{
		// get 'style' from job_obj
		pair = FindHPair( job->job_obj, "style" );
		if ( !pair )
		{
			J_ObjWarning( job->job_obj, "missing key 'style'" );
			goto destroy;
		}
		job->style_string = pair->value;
	}

	if ( job->layer->shape->visible_rf != r_frame_count )
	{
		goto leave;
	}

	COUNT_INCR( count_rf_num_vis_job );
	

	{
		int	style_len;
		fp_t	scale;
		
		style_len = strlen( job->style_string );
		
		scale = (job->style_string[(global_time/32)%style_len] - 'A')/15.0;
		job->layer->ambient_color[C_A] = (unsigned char)(255.0 * scale);
	}	
	
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
	shape_ctrl_job_ModulateAlpha_t		*job;

	job = ADDR_OF_BASE( shape_ctrl_job_ModulateAlpha_t, jif, jif );
	FREE( job );

	return ShapeCtrlJobRes_success;	
}

/*
  ==================================================
  ShapeCtrlJob_Create_ModulateAlpha

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

shape_ctrl_job_t * ShapeCtrlJob_Create_ModulateAlpha( hobj_t *job_obj, layer_t *layer )
{
	shape_ctrl_job_ModulateAlpha_t		*job;

	__named_message( "\n" );

	InitFTBL();
		
	job = NEWTYPE( shape_ctrl_job_ModulateAlpha_t );
	job->jif.ftbl = &ftbl;
	job->jif.state = ShapeCtrlJobState_run;

	// init private data
	job->job_obj = job_obj;
	job->layer = layer;

	
	job->run_count = 0;
	
	return &job->jif;
}
