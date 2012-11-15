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



// J_SurfaceDeform.c

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

typedef enum
{
	sdStyle_none,
	sdStyle_fake_x,
	sdStyle_fake_y,
	sdStyle_fake_z,

	sdStyle_num
} sdStyle;

typedef struct shape_ctrl_job_SurfaceDeform_s
{
	shape_ctrl_job_t	jif;

	// private data
	hobj_t			*job_obj;
	shape_t			*shape;

	sdStyle			style;

	unique_t		mesh_name;	// name of the mesh, the vertex copy is from

	int		        num_vertex;	// vertex copy
	vec3d_t			*vertices;
	
	int			run_count;
} shape_ctrl_job_SurfaceDeform_t;

/*
  ==============================
  RunJob

  ==============================
*/
static shapeCtrlJobRes RunJob( shape_ctrl_job_t *jif )
{
	shape_ctrl_job_SurfaceDeform_t	*job;       
	vec3d_t		*m_verts;
	vec2d_t		*m_uvs;
	int		tmp_int;
	int		i;
	msec_t		global_time;
	hpair_t		*pair;

	static int	sin_valid = false;
	static fp_t		sin_tbl[256];

	if ( sin_valid == false )
	{
		int		i;
		sin_valid = true;

		for ( i = 0; i < 256; i++ )
		{
			fp_t	scl;

			scl = i/255.0;
			sin_tbl[i] = sin(scl*M_PI*2);
		}
	}

	job = ADDR_OF_BASE( shape_ctrl_job_SurfaceDeform_t, jif, jif );

	global_time = ms_rfbegin;

	if ( job->run_count == 0 )
	{
		if ( job->shape->tess_name != ShapeTesselation_curved_surface )
		{
			J_ObjWarning( job->job_obj, "shape is not a curved surface" );
			goto destroy;		
		}

		pair = FindHPair( job->job_obj, "style" );
		if ( !pair )
		{
			J_ObjWarning( job->job_obj, "no style key" );
			goto destroy;
		}
		if ( !strcmp( pair->value, "fake_z" ) )
		{
			job->style = sdStyle_fake_z;
		}
		else if ( !strcmp( pair->value, "fake_y" ) )
		{
			job->style = sdStyle_fake_y;
		}
		else if ( !strcmp( pair->value, "fake_x" ) )
		{
			job->style = sdStyle_fake_x;
		}
		else
		{
			J_ObjWarning( job->job_obj, "invalide style value" );
			goto destroy;
		}
	}
	
	if ( job->shape->visible_rf != r_frame_count )
	{
		goto leave;
	}

	COUNT_INCR( count_rf_num_vis_job );


	if ( !job->shape->mesh )
	{
		// no mesh that can be deformed
		goto leave;
	}

	if ( !Mesh_CheckName( job->shape->mesh, job->mesh_name ) )
	{
		// the mesh changed
		
		if ( job->vertices )
		{
			FREE( job->vertices );
		}
		
		// create a copy
		{
			Mesh_GetVec3dBase( job->shape->mesh, 0, &job->num_vertex, &m_verts );
			job->vertices = (vec3d_t *)NEWBYTES( job->num_vertex*sizeof(vec3d_t) );
			memcpy( job->vertices, m_verts, job->num_vertex*sizeof(vec3d_t) );
		}
		job->mesh_name = Mesh_GetName( job->shape->mesh );
	}

	Mesh_GetVec3dBase( job->shape->mesh, 0, &tmp_int, &m_verts );
	Mesh_GetVec2dBase( job->shape->mesh, 3, &tmp_int, &m_uvs );
	for ( i = 0; i < job->num_vertex; i++ )
	{		
		Vec3dCopy( m_verts[i], job->vertices[i] );

		if ( job->style == sdStyle_fake_y )
		{
#if 1
			m_verts[i][1] += sin_tbl[ (int)((m_uvs[i][0]*10.0+m_uvs[i][1]*10.0+(global_time/500.0))*(128.0/M_PI))%256 ]*2.0;
			m_verts[i][0] += sin_tbl[ (int)((global_time/2000.0)*(128.0/M_PI))%256 ] *16.0*(1.0-fabs(m_uvs[i][0]-0.5)*2.0)*(1.0-fabs(m_uvs[i][1]-0.5)*2.0);
			m_verts[i][2] += sin_tbl[ ((int)((global_time/3000.0)*(128.0/M_PI))+64)%256 ] *16.0*(1.0-fabs(m_uvs[i][0]-0.5)*2.0)*(1.0-fabs(m_uvs[i][1]-0.5)*2.0);

#else
			m_verts[i][0] += sin( (global_time/2000.0) )*16.0*(1.0-fabs(m_uvs[i][0]-0.5)*2.0)*(1.0-fabs(m_uvs[i][1]-0.5)*2.0);
			m_verts[i][2] += cos( (global_time/3000.0) )*16.0*(1.0-fabs(m_uvs[i][0]-0.5)*2.0)*(1.0-fabs(m_uvs[i][1]-0.5)*2.0);
//		m_verts[i][1] += sin( m_uvs[i][0]+m_uvs[i][1]+(global_time/100.0) )*16.0;
			m_verts[i][1] += sin( m_uvs[i][0]*10.0+m_uvs[i][1]*10.0+(global_time/500.0) )*2.0;		
#endif
		}
		else if ( job->style == sdStyle_fake_z )
		{

			m_verts[i][2] += sin_tbl[ (int)((m_uvs[i][0]*10.0+m_uvs[i][1]*10.0+(global_time/500.0))*(128.0/M_PI))%256 ]*2.0;				
		}
		else if ( job->style == sdStyle_fake_x )
		{
			m_verts[i][0] += sin_tbl[ (int)((m_uvs[i][0]*10.0+m_uvs[i][1]*10.0+(global_time/500.0))*(128.0/M_PI))%256 ]*2.0;	
		}
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
	shape_ctrl_job_SurfaceDeform_t		*job;

	job = ADDR_OF_BASE( shape_ctrl_job_SurfaceDeform_t, jif, jif );
	FREE( job );
	
	return ShapeCtrlJobRes_success;	
}

/*
  ==================================================
  ShapeCtrlJob_Create_SurfaceDeform

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

shape_ctrl_job_t * ShapeCtrlJob_Create_SurfaceDeform( hobj_t *job_obj, shape_t *shape )
{
	shape_ctrl_job_SurfaceDeform_t		*job;

	__named_message( "\n" );

	InitFTBL();

	job = NEWTYPE( shape_ctrl_job_SurfaceDeform_t );
	job->jif.ftbl = &ftbl;
	job->job_obj = job_obj;
	job->shape = shape;

	job->run_count = 0;

	return &job->jif;
}
