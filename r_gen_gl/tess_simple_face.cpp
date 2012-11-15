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



// tess_simple_face.c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "lib_math.h"
#include "lib_container.h"
#include "shock.h"

#include "r_glbackenddefs.h"
#include "shape_defs.h"
#include "mesh.h"
#include "shape_util.h"
#include "u_va.h"

#define	SIMPLE_FACE_FLAGS_NONE		( 0 )
#define SIMPLE_FACE_FLAGS_USED		( 1 )
#define SIMPLE_FACE_FLAGS_COLLINEAR	( 2 )

#define SIMPLE_FACE_MAX_POINTS		( 1024 )

/*
  ==============================
  Tess_SimpleFace_TriFanWithCenter

  ==============================
*/
mesh_t * Tess_SimpleFace_TriFanWithCenter( vec3d_t center, int pnum, vec3d_t pts[] )
{
	int		i;
	int		vertexnum;
	int		vrefnum;
	mesh_t	*mesh;

	vec3d_t		*m_vertices;
	int		*m_vrefs;
	int		*m_cmds;

	int		tmp_int;

	vertexnum = pnum + 1;
	vrefnum = pnum + 2;

	Mesh_BeginInstance();
	Mesh_AddVec3dArray( vertexnum );
	Mesh_AddIntArray( vrefnum );
	Mesh_AddIntArray( 3 );
	mesh = Mesh_EndInstance();

	Mesh_GetVec3dBase( mesh, 0, &tmp_int, &m_vertices );
	Mesh_GetIntBase( mesh, 1, &tmp_int, &m_vrefs );
	Mesh_GetIntBase( mesh, 2, &tmp_int, &m_cmds );

	//
	// fill vertex buffer
	//

	// first vertex of tri_fan is the cente
	Vec3dCopy( m_vertices[0], center );

	for ( i = 0; i < pnum; i++ )
	{
		Vec3dCopy( m_vertices[i+1], pts[i] );
	}

	//
	// fill vref buffer
	//
	m_vrefs[0] = 0;
	for ( i = 0; i < pnum; i++ )
	{
		m_vrefs[i+1] = i+1;
	}
	m_vrefs[i+1] = 1;

	//
	// fill cmd buffer
	//
	m_cmds[0] = BE_CMD_TRIANGLE_FAN;
	m_cmds[1] = vrefnum;
	m_cmds[2] = BE_CMD_END;

	return mesh;
}


/*
  ==============================
  Tess_SimpleFace_TriFanWithStart

  ==============================
*/
mesh_t * Tess_SimpleFace_TriFanWithStart( int start, int pnum, vec3d_t pts[] )
{
	int		i, j;
	mesh_t	*mesh;

	vec3d_t		*m_vertices;
	int		*m_vrefs;
	int		*m_cmds;

	int		tmp_int;

	Mesh_BeginInstance();
	Mesh_AddVec3dArray( pnum );
	Mesh_AddIntArray( pnum );
	Mesh_AddIntArray( 3 );
	mesh = Mesh_EndInstance();

	Mesh_GetVec3dBase( mesh, 0, &tmp_int, &m_vertices );
	Mesh_GetIntBase( mesh, 1, &tmp_int, &m_vrefs );
	Mesh_GetIntBase( mesh, 2, &tmp_int, &m_cmds );

	for ( i = 0; i < pnum; i++ )
	{
		Vec3dCopy( m_vertices[i], pts[i] );
	}

	j = start;
	for ( i = 0; i < pnum; i++, j++ )
	{
		j = (j==pnum)?0:j;
		m_vrefs[i] = j;
	}

	m_cmds[0] =  BE_CMD_TRIANGLE_FAN;
	m_cmds[1] = pnum;
	m_cmds[2] = BE_CMD_END;

	return mesh;
}


/*
  ==============================
  Tess_SimpleFace_CalcBoundBox

  ==============================
*/
void Tess_SimpleFace_CalcBoundBox( shape_t *shp )
{
	vec3d_t		*verts;
	int		vnum;
	int		i;

	if ( shp->tess_name != ShapeTesselation_simple_face )
		Error( "tess_name %d is invalid\n", shp->tess_name );

	if ( !shp->mesh )
		Error( "(null) mesh\n" );

	Mesh_GetVec3dBase( shp->mesh, 0, &vnum, &verts );
	U_VACalcBB( verts, vnum, shp->min, shp->max );

	for ( i = 0; i < 3; i++ )
	{
		shp->min[i] -= SHAPE_BOUNDBOX_SIDE_SPACE;
		shp->max[i] += SHAPE_BOUNDBOX_SIDE_SPACE;
	}	
}
 

/*
  ==============================
  Tess_CreateMesh_simple_face

  ==============================
*/
void Tess_CreateMesh_simple_face( shape_t *shp )
{
	int			i, n, p;
	mesh_t		*mesh;
	u_list_iter_t	iter;
	shape_ctrl_point_t	*cp;

	int			pnum;
	vec3d_t			pts[SIMPLE_FACE_MAX_POINTS];
	int			fs[SIMPLE_FACE_MAX_POINTS];

	if ( shp->tess_name != ShapeTesselation_simple_face )
		Error( "tess_name %d is invalid\n", shp->tess_name );

	if ( shp->vnum != 1 )
		Error( "invalid v order\n" );

	if ( shp->unum <= 0 )
		Error( "invalid u order\n" );

	if ( shp->unum != U_ListLength( shp->cp_list ) )
		Error( "control point inconsitancy\n" );

	// unum seems to be valid
	pnum = shp->unum;

	for ( i = 0; i < pnum; i++ )
		fs[i] = SIMPLE_FACE_FLAGS_NONE;

	// setup and check control points
	U_ListIterInit( &iter, shp->cp_list );
	for ( ; ( cp = (shape_ctrl_point_t *)U_ListIterNext( &iter ) ) ; )
	{
		if ( cp->u < 0 || cp->u > pnum-1 || cp->v != 0 )
			Error( "invalid control point\n" );

		if ( fs[cp->u] != SIMPLE_FACE_FLAGS_NONE )
			Error( "dublicated control points\n" );

		fs[cp->u] |= SIMPLE_FACE_FLAGS_USED;
		Vec3dCopy( pts[cp->u], cp->p );
	}

	// classify control points
	for ( i = 0; i < pnum; i++ )
	{
		p = (i==0)?(pnum-1):(i-1);
		n = (i==(pnum-1))?0:(i+1);

		if ( ShapeU_AreCollinearPoints( pts[p], pts[n], pts[i] ) )
			fs[i] |= SIMPLE_FACE_FLAGS_COLLINEAR;
	}
	
	// search a point, that neighbors aren't part of a collinear edge
	for ( i = 0; i < pnum; i++ )
	{
		p = (i==0)?(pnum-1):(i-1);
		n = (i==(pnum-1))?0:(i+1);

		if ( ( fs[p]|fs[n]|fs[i] ) & SIMPLE_FACE_FLAGS_COLLINEAR )
			continue;

		break;
	}
	if ( i == pnum )
	{
		vec3d_t		center;

		// no point can be used as start point
		// mesh will be a tri_fan around the face center

		ShapeU_VACalcCenter( center, pnum, pts );
		mesh = Tess_SimpleFace_TriFanWithCenter( center, pnum, pts );
	}
	else
	{
		// mesh will be a tri_fan around the start point 'i'
		mesh = Tess_SimpleFace_TriFanWithStart( i, pnum, pts );
	}

	if ( shp->mesh )
		Mesh_Free( shp->mesh );

	shp->mesh = mesh;

	Tess_SimpleFace_CalcBoundBox( shp );
}
