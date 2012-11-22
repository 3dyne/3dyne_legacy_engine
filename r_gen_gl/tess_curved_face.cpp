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



// tess_curved_face.c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "lib_math.h"
#include "lib_bezier.h"
#include "lib_container.h"
#include "shock.h"

#include "r_glbackenddefs.h"
#include "shape_defs.h"
#include "mesh.h"
#include "shape_util.h"

#include "trify.h"
#include "u_va.h"

#define CURVED_FACE_MAX_POINTS	( 128*3 )

#define CURVED_FACE_FLAGS_NONE	( 0 )
#define CURVED_FACE_FLAGS_USED	( 1 )

/*
  ==============================
  Tess_CurvedFace_GenCurvedEdge

  ==============================
*/
void Tess_CurvedFace_GenCurvedEdge( int lod, int pnum, vec3d_t pts[] )
{
	int		p1, p2, p3;
	int		i, j;

	for ( i = 0; i < pnum; i+=2 )
	{
		p1 = i;
		p2 = i+1;
		p3 = ((i+2)==pnum)?0:(i+2);
		
		if ( ShapeU_AreCollinearPoints( pts[p1], pts[p3], pts[p2] ) )
		{
			// no curve, use only p1
			Trify_AddPoint( pts[p1] );
		}
		else
		{
			// curve, setup bezier curve through p1-p2-p3
			curve_ctrl_t	*curve;
			curve_points_t	*cpts;

			curve = NewBezierCurve( 3 );
			SetCurveCtrlPoint( curve, 0, pts[p1] );
			SetCurveCtrlPoint( curve, 1, pts[p2] );
			SetCurveCtrlPoint( curve, 2, pts[p3] );

			cpts = EvalCurvePoints( curve, lod );

			// the last point is NOT used
			for ( j = 0; j < lod-1; j++ )
			{
				vec3d_t		pos;
				
				GetCurvePoint( cpts, j, pos );
				Trify_AddPoint( pos );
			}

			FreeCurvePoints( cpts );
			FreeBezierCurve( curve );

		}				
	}
}

/*
  ==============================
  Tess_CurvedFace_CalcBoundBox

  ==============================
*/
void Tess_CurvedFace_CalcBoundBox( shape_t *shp )
{
	vec3d_t		*verts;
	int		vnum;
	int		i;

	if ( shp->tess_name != ShapeTesselation_curved_face )
		__error( "tess_name %d is invalid\n", shp->tess_name );

	if ( !shp->mesh )
		__error( "(null) mesh\n" );
	
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
  Tess_CreateMesh_curved_face

  ==============================
*/
void Tess_CreateMesh_curved_face( shape_t *shp )
{
	int		i;
	mesh_t		*mesh;
	u_list_iter_t	iter;
	shape_ctrl_point_t	*cp;

	int		pnum;
	vec3d_t		pts[CURVED_FACE_MAX_POINTS];
	int		fs[CURVED_FACE_MAX_POINTS];

	if ( shp->tess_name != ShapeTesselation_curved_face )
		__error( "tess_name invalid\n" );

	if ( shp->norm[0] == 0 &&
	     shp->norm[1] == 0 &&
	     shp->norm[2] == 0 )
		__error( "missing plane hint\n" );
	
	if ( shp->vnum != 1 )
		__error( "invalid v order\n" );

	if ( shp->unum <= 0 )
		__error( "invalid u order\n" );

	if ( shp->unum != U_ListLength( shp->cp_list ) )
		__error( "control point inconsitancy\n" );

	// fixme: lod init
	if ( shp->u_lod == 0 )
	{
		shp->u_lod = 3;
	}

	pnum = shp->unum;

	for ( i = 0; i < pnum; i++ )
		fs[i] = CURVED_FACE_FLAGS_NONE;

	// setup and check control points
	U_ListIterInit( &iter, shp->cp_list );
	for ( ; ( cp = (shape_ctrl_point_t *)U_ListIterNext( &iter ) ) ; )
	{
		if ( cp->u < 0 || cp->u > pnum-1 || cp->v != 0 )
			__error( "invalid control point\n" );

		if ( fs[cp->u] != CURVED_FACE_FLAGS_NONE )
			__error( "dublicated control points\n" );

		fs[cp->u] |= CURVED_FACE_FLAGS_USED;
		Vec3dCopy( pts[cp->u], cp->p );
	}		

	Trify_BeginGenericPolygon();
	Trify_SetPlane( shp->norm, shp->dist );
	
	Tess_CurvedFace_GenCurvedEdge( shp->u_lod, pnum, pts );
	
	mesh = Trify_End();

	if ( shp->mesh )
		Mesh_Free( shp->mesh );

	shp->mesh = mesh;

	Tess_CurvedFace_CalcBoundBox( shp );	
}
