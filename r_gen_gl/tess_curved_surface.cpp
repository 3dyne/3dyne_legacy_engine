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



// tess_curved_surface.c

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
#include "u_va.h"

#define	CURVED_SURFACE_MAX_U_ORDER	( 16 )
#define	CURVED_SURFACE_MAX_V_ORDER	( 16 )

#define CURVED_SURFACE_FLAGS_NONE	( 0 )
#define CURVED_SURFACE_FLAGS_USED	( 1 )

/*
  ==============================
  Tess_CurvedSurface_ClassifyLOD

  ==============================
*/
void Tess_CurvedSurface_ClassifyLOD( int *u_lod, int *v_lod, int u_order, int v_order, vec3d_t pts[CURVED_SURFACE_MAX_U_ORDER][CURVED_SURFACE_MAX_V_ORDER] )
{
	int		u, v;
	bool_t		u_remove;
	bool_t		v_remove;

	if ( u_order != 3 || v_order != 3 )
	{
		Error( "shame! only third order bezier surface by now ( u_order = %d, v_order = %d )\n", u_order, v_order );
	}

	u_remove = false;
	v_remove = false;

	// can drop u_order from 3 to 2 ? 
	for ( v = 0; v < 3; v++ )
	{
		if ( !Vec3dCheckColinear( pts[0][v], pts[2][v], pts[1][v] ) )
			break;
	}
	
	if ( v == 3 )
		v_remove = true;

	// can drop vorder from 3 to 2 ?
	for ( u = 0; u < 3; u++ )
	{
		if ( !Vec3dCheckColinear( pts[u][0], pts[u][2], pts[u][1] ) )
			break;
	}

	if ( u == 3 )
		u_remove = true;

	// set lods
	
	if ( u_remove )
	{
		*u_lod = 2;
	}
	else
	{
		*u_lod = 3;
	}

	if ( v_remove )
	{
		*v_lod = 2;
	}
	else
	{
		*v_lod = 3;
	}       
}


/*
  ==============================
  Tess_CurvedSurface_GenMesh

  ==============================
*/
mesh_t * Tess_CurvedSurface_GenMesh( int u_lod, int v_lod, int u_order, int v_order, vec3d_t pts[CURVED_SURFACE_MAX_U_ORDER][CURVED_SURFACE_MAX_V_ORDER] )
{
	int		u, v, i;

	surface_ctrl_t		*sc;
	surface_points_t	*sm;
	uv_points_t		*uv;

	mesh_t			*mesh;

	int		vertexnum;
	int		vrefnum;
	int		cmdnum;

	vec3d_t		*m_vertices;
	vec2d_t		*m_uvs;
	int		*m_vrefs;
	int		*m_cmds;

	int		tmp_int;

	sc = NewBezierSurface( u_order, v_order );

	for ( u = 0; u < u_order; u++ )
	{
		for ( v = 0; v < v_order; v++ )
		{
			SetSurfaceCtrlPoint( sc, u, v, pts[u][v] );
		}
	}

	sm = EvalSurfacePoints( sc, u_lod, v_lod );

	uv = EvalUVPoints( sc, u_lod, v_lod );

	vertexnum = u_lod * v_lod;
	if ( u_lod >= v_lod )
	{
		vrefnum = (v_lod-1)*u_lod*2;
		cmdnum = (v_lod-1)*2 + 1;

		Mesh_BeginInstance();
		Mesh_AddVec3dArray( vertexnum );
		Mesh_AddIntArray( vrefnum );
		Mesh_AddIntArray( cmdnum );
		Mesh_AddVec2dArray( vertexnum );
		mesh = Mesh_EndInstance();

		Mesh_GetVec3dBase( mesh, 0, &tmp_int, &m_vertices );
		Mesh_GetIntBase( mesh, 1, &tmp_int, &m_vrefs );
		Mesh_GetIntBase( mesh, 2, &tmp_int, &m_cmds );
		Mesh_GetVec2dBase( mesh, 3, &tmp_int, &m_uvs );

		// fill vertex buffer
		i = 0;
		for ( v = 0; v < v_lod; v++ )
		{
			for ( u = 0; u < u_lod; u++ )
			{
				vec3d_t		p;
				vec2d_t		q;

				GetSurfacePoint( sm, u, v, p );
				Vec3dCopy( m_vertices[i], p );
				GetUVPoint( uv, u, v, q );
				Vec2dCopy( m_uvs[i], q );
				i++;
			}
		}

		// fill vref buffer
		i = 0; 
		for ( v = 0; v < v_lod-1; v++ )
		{
			for ( u = 0; u < u_lod; u++ )
			{
				m_vrefs[i++] = (v+0)*u_lod + u;
				m_vrefs[i++] = (v+1)*u_lod + u;
			}
		}

		// fill cmd buffer
		i = 0;
		for ( v = 0; v < v_lod-1; v++ )
		{
			m_cmds[i++] = BE_CMD_TRIANGLE_STRIP;
			m_cmds[i++] = u_lod*2;
		}
		m_cmds[i++] = BE_CMD_END;		
	}
	else
	{
		vrefnum = (u_lod-1)*v_lod*2;
		cmdnum = (u_lod-1)*2 + 1;
		
		Mesh_BeginInstance();
		Mesh_AddVec3dArray( vertexnum );
		Mesh_AddIntArray( vrefnum );
		Mesh_AddIntArray( cmdnum );
		Mesh_AddVec2dArray( vertexnum );
		mesh = Mesh_EndInstance();

		Mesh_GetVec3dBase( mesh, 0, &tmp_int, &m_vertices );
		Mesh_GetIntBase( mesh, 1, &tmp_int, &m_vrefs );
		Mesh_GetIntBase( mesh, 2, &tmp_int, &m_cmds );	
		Mesh_GetVec2dBase( mesh, 3, &tmp_int, &m_uvs );

		// fill vertex buffer
		i = 0;
		for ( u = 0; u < u_lod; u++ )
		{
			for ( v = 0; v < v_lod; v++ )
			{
				vec3d_t		p;
				vec2d_t		q;

				GetSurfacePoint( sm, u, v, p );
				Vec3dCopy( m_vertices[i], p );
				GetUVPoint( uv, u, v, q );
				Vec2dCopy( m_uvs[i], q );
				i++;
			}
		}

		// fill vref buffer
		i = 0;
		for ( u = 0; u < u_lod-1; u++ )
		{
			for ( v = 0; v < v_lod; v++ )
			{
				m_vrefs[i++] = (u+1)*v_lod + v; 
				m_vrefs[i++] = (u+0)*v_lod + v;
			}
		}

		// fill cmd buffer
		i = 0;
		for ( u = 0; u < u_lod-1; u++ )
		{
			m_cmds[i++] = BE_CMD_TRIANGLE_STRIP;
			m_cmds[i++] = v_lod*2;
		}
		m_cmds[i++] = BE_CMD_END;		
	}

	FreeUVPoints( uv );
	FreeSurfacePoints( sm );
	FreeBezierSurface( sc );
	
	return mesh;

}

/*
  ==============================
  Tess_CurvedSurface_CalcBoundBox

  ==============================
*/
void Tess_CurvedSurface_CalcBoundBox( shape_t *shp )
{
	vec3d_t		*verts;
	int		vnum;
	int		i;

	if ( shp->tess_name != ShapeTesselation_curved_surface )
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
  Tess_CreateMesh_curved_surface

  ==============================
*/
void Tess_CreateMesh_curved_surface( shape_t *shp )
{
	int	u, v;
	int	u_order, v_order;
	u_list_iter_t	iter;
	shape_ctrl_point_t	*cp;
	int		cpnum;
	mesh_t		*mesh;
	vec3d_t	pts[CURVED_SURFACE_MAX_U_ORDER][CURVED_SURFACE_MAX_V_ORDER];
	int	fs[CURVED_SURFACE_MAX_U_ORDER][CURVED_SURFACE_MAX_V_ORDER];

		
	if ( shp->tess_name != ShapeTesselation_curved_surface )
		Error( "tess_name invalid\n" );

	u_order = shp->unum;
	v_order = shp->vnum;
	
	if ( u_order <= 1 || u_order >= CURVED_SURFACE_MAX_U_ORDER )
		Error( "invalid u order\n" );
	if ( v_order <= 1 || v_order >= CURVED_SURFACE_MAX_V_ORDER )
		Error( "invalid v order\n" );

	for ( u = 0; u < u_order; u++ )
	{
		for ( v = 0; v < v_order; v++ )
		{
			fs[u][v] = CURVED_SURFACE_FLAGS_NONE;
		}
	}

	// setup and check control points
	U_ListIterInit( &iter, shp->cp_list );
	for ( cpnum = 0; ( cp = (shape_ctrl_point_t *)U_ListIterNext( &iter ) ); cpnum++ )
	{
		u = cp->u;
		v = cp->v;

		if ( u < 0 || u >= u_order )
			Error( "invalid control point\n" );
		if ( v < 0 || v >= v_order )
			Error( "invalid control point\n" );	    

		if ( fs[u][v] != CURVED_SURFACE_FLAGS_NONE )
			Error( "dublicated control points\n" );

		fs[u][v] |= CURVED_SURFACE_FLAGS_USED;	
		Vec3dCopy( pts[u][v], cp->p );
	}

	if ( cpnum != u_order*v_order )
		Error( "not enough control points\n" );

	// fixme: lod init
	if ( shp->u_lod == 0 && shp->v_lod == 0 )
	{
		Tess_CurvedSurface_ClassifyLOD( &shp->u_lod, &shp->v_lod, u_order, v_order, pts );
	}

	mesh = Tess_CurvedSurface_GenMesh( shp->u_lod, shp->v_lod, u_order, v_order, pts );
	
	if ( shp->mesh )
		Mesh_Free( shp->mesh );

	shp->mesh = mesh;

	Tess_CurvedSurface_CalcBoundBox( shp );
}
