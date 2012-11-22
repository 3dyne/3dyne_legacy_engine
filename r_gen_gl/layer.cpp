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



// layer.c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "lib_math.h"
#include "lib_bezier.h"
#include "shock.h"

#include "mesh.h"
#include "layer.h"
#include "unitex.h"
#include "tf2d.h"

/*
  ==============================
  Layer_CreateTexCoord_projection

  ==============================
*/
void Layer_CreateTexCoord_projection( layer_t *layer, vec3d_t in, vec2d_t out )
{
	vec3d_t		v3;
	vec2d_t		v2;
	int		j;
	int		trans_num;
	
	Vec3dCopy( v3, in );

	if ( layer->flags & LAYER_FLAGS_PROJECTION_X )
	{
		v2[0] = v3[2];
		v2[1] = v3[1];
	}
	else if ( layer->flags & LAYER_FLAGS_PROJECTION_Y )
	{
		v2[0] = v3[0];
		v2[1] = v3[2];
	}
	else if ( layer->flags & LAYER_FLAGS_PROJECTION_Z )
	{
		v2[0] = v3[0];
		v2[1] = v3[1];
	}
	else
	{
		__error( "invalid projection plane\n" );
	}
	
	U_BeginMemUnpack( layer->packed_info );		// fixme: missing U_EndMemUnpack() ...
	U_MemUnpacki32( &trans_num );
	
	for ( j = 0; j < trans_num; j++ )
	{
		int		trans_type;
		
		U_MemUnpacki32( &trans_type );
		
		if ( trans_type & LAYER_FLAGS_TRANSFORMATION_AXIS )
		{
			vec2d_t		x, y;
			vec2d_t		tmp;
			
			U_MemUnpackf32v2( x );
			U_MemUnpackf32v2( y );
			
			Vec2dCopy( tmp, v2 );
			
			v2[0] = tmp[0]*x[0] + tmp[1]*x[1];
			v2[1] = tmp[0]*y[0] + tmp[1]*y[1];
		}
		else if ( trans_type & LAYER_FLAGS_TRANSFORMATION_SCALE )
		{
			vec2d_t		scale;
			
			U_MemUnpackf32v2( scale );
			
			v2[0] *= scale[0];
			v2[1] *= scale[1];
		}
		else if ( trans_type & LAYER_FLAGS_TRANSFORMATION_SHIFT )
		{
			vec2d_t		shift;

			U_MemUnpackf32v2( shift );

			v2[0] += shift[0];
			v2[1] += shift[1];
		}
		else 
		{
			__error( "invalid projectiv transformation\n" );
		}
	}
	
	out[0] = v2[0];
	out[1] = v2[1];	
}

/*
  ==============================
  Layer_CreateTexCoords_projection

  ==============================
*/
void Layer_CreateTexCoords_projection( layer_t *layer, mesh_t *mesh )
{
	int		i, j;
	
	vec3d_t		*m_vertices;
	int		*m_vrefs;

	int		m_vertexnum;
	int		m_vrefnum;

	mesh_t		*tc_mesh;
	vec2d_t		*m_tc;
	int		tmp_int;

	int		trans_num;

	Mesh_GetVec3dBase( mesh, 0, &m_vertexnum, &m_vertices );	
	Mesh_GetIntBase( mesh, 1, &m_vrefnum, &m_vrefs );

	Mesh_BeginInstance();
	Mesh_AddVec2dArray( m_vrefnum );
	tc_mesh = Mesh_EndInstance();

	Mesh_GetVec2dBase( tc_mesh, 0, &tmp_int, &m_tc );

	// init tc_mesh with projected vertices
	for ( i = 0; i < m_vrefnum; i++ )
	{
		vec3d_t		v3;
		vec2d_t		v2;

		Vec3dCopy( v3, m_vertices[m_vrefs[i]] );

		if ( layer->flags & LAYER_FLAGS_PROJECTION_X )
		{
			v2[0] = v3[2];
			v2[1] = v3[1];
		}
		else if ( layer->flags & LAYER_FLAGS_PROJECTION_Y )
		{
			v2[0] = v3[0];
			v2[1] = v3[2];
		}
		else if ( layer->flags & LAYER_FLAGS_PROJECTION_Z )
		{
			v2[0] = v3[0];
			v2[1] = v3[1];
		}
		else 
		{
			__error( "invalid projection plane\n" );
		}

		Vec2dCopy( m_tc[i], v2 );
	}

	// unpack layer_info
	U_BeginMemUnpack( layer->packed_info );		// fixme: missing U_EndMemUnpack() ...
	U_MemUnpacki32( &trans_num );

	for ( j = 0; j < trans_num; j++ )
	{
		int		trans_type;

		U_MemUnpacki32( &trans_type );
		
		if ( trans_type & LAYER_FLAGS_TRANSFORMATION_AXIS )
		{
			vec2d_t		x, y;

			U_MemUnpackf32v2( x );
			U_MemUnpackf32v2( y );

			for ( i = 0; i < m_vrefnum; i++ )
			{
				vec2d_t		tmp;
				
				Vec2dCopy( tmp, m_tc[i] );

				m_tc[i][0] = tmp[0]*x[0] + tmp[1]*x[1];
				m_tc[i][1] = tmp[0]*y[0] + tmp[1]*y[1];
			}
		}
		else if ( trans_type & LAYER_FLAGS_TRANSFORMATION_SCALE )
		{
			vec2d_t		scale;

			U_MemUnpackf32v2( scale );

			for ( i = 0; i < m_vrefnum; i++ )
			{
				m_tc[i][0] *= scale[0];
				m_tc[i][1] *= scale[1];
			}
		}
		else if ( trans_type & LAYER_FLAGS_TRANSFORMATION_SHIFT )
		{
			vec2d_t		shift;

			U_MemUnpackf32v2( shift );

			for ( i = 0; i < m_vrefnum; i++ )
			{
				m_tc[i][0] += shift[0];
				m_tc[i][1] += shift[1];
			}
		}
		else 
		{
			__error( "invalid projectiv transformation\n" );
		}
	}

	for ( i = 0; i < m_vrefnum; i++ )
	{
		vec2d_t		tmp;

		Vec2dCopy( tmp, m_tc[i] );

		if ( layer->flags & LAYER_FLAGS_MODE_TEXEL0N )
			UniTex_RealTexCoord_0N( layer->real_source, m_tc[i], tmp );
		else if ( layer->flags & LAYER_FLAGS_MODE_TEXEL01 )
			UniTex_RealTexCoord_01( layer->real_source, m_tc[i], tmp );
		else
			__error( "unknown texcoord access type\n" );
	}

	if ( layer->tc )
		Mesh_Free( layer->tc );

	layer->tc = tc_mesh;
}

/*
  ==============================
  Layer_CreateTexCoords_projection_with_stack

  ==============================
*/
void Layer_CreateTexCoords_projection_with_stack( layer_t *layer, mesh_t *mesh )
{
	int		i;
	
	vec3d_t		*m_vertices;
	int		*m_vrefs;

	int		m_vertexnum;
	int		m_vrefnum;

	mesh_t		*tc_mesh;
	vec2d_t		*m_tc;
	int		tmp_int;

	Mesh_GetVec3dBase( mesh, 0, &m_vertexnum, &m_vertices );	
	Mesh_GetIntBase( mesh, 1, &m_vrefnum, &m_vrefs );

	Mesh_BeginInstance();
	Mesh_AddVec2dArray( m_vrefnum );
	tc_mesh = Mesh_EndInstance();

	Mesh_GetVec2dBase( tc_mesh, 0, &tmp_int, &m_tc );

	// init tc_mesh with projected vertices
	for ( i = 0; i < m_vrefnum; i++ )
	{
		vec3d_t		v3;
		vec2d_t		v2;

		Vec3dCopy( v3, m_vertices[m_vrefs[i]] );

		if ( layer->flags & LAYER_FLAGS_PROJECTION_X )
		{
			v2[0] = v3[2];
			v2[1] = v3[1];
		}
		else if ( layer->flags & LAYER_FLAGS_PROJECTION_Y )
		{
			v2[0] = v3[0];
			v2[1] = v3[2];
		}
		else if ( layer->flags & LAYER_FLAGS_PROJECTION_Z )
		{
			v2[0] = v3[0];
			v2[1] = v3[1];
		}
		else 
		{
			__error( "invalid projection plane\n" );
		}

		Vec2dCopy( m_tc[i], v2 );
	}

	for ( i = 0; i < m_vrefnum; i++ )
	{
		TF2D_StackCalc( layer->tf2d_stack, m_tc[i], m_tc[i] );
	}

	for ( i = 0; i < m_vrefnum; i++ )
	{
		vec2d_t		tmp;

		Vec2dCopy( tmp, m_tc[i] );

		if ( layer->flags & LAYER_FLAGS_MODE_TEXEL0N )
			UniTex_RealTexCoord_0N( layer->real_source, m_tc[i], tmp );
		else if ( layer->flags & LAYER_FLAGS_MODE_TEXEL01 )
			UniTex_RealTexCoord_01( layer->real_source, m_tc[i], tmp );
		else
			__error( "unknown texcoord access type\n" );
	}

	if ( layer->tc )
		Mesh_Free( layer->tc );

	layer->tc = tc_mesh;
}


/*
  ==============================
  Layer_CreateTexCoords_uv

  ==============================
*/
void Layer_CreateTexCoords_uv( layer_t *layer, mesh_t *mesh )
{
	int			i;
	
	surface_ctrl_t		*sc;
	vec2d_t			v;

	vec2d_t		*m_uvs;       
	int		*m_vrefs;

	int		m_vertexnum;
	int		m_vrefnum;

	mesh_t		*tc_mesh;
	vec2d_t		*m_tc;
	int		tmp_int;
	

	sc = NewBezierSurface( 2, 2 );

	U_BeginMemUnpack( layer->packed_info );
	
	// fixme: move insane cotrol point order to the caller
	U_MemUnpackf32v2( v );
	SetSurfaceCtrlPoint3f( sc, 0, 0, v[0], v[1], 0 );
	U_MemUnpackf32v2( v );
	SetSurfaceCtrlPoint3f( sc, 0, 1, v[0], v[1], 0 );
	U_MemUnpackf32v2( v );
	SetSurfaceCtrlPoint3f( sc, 1, 0, v[0], v[1], 0 );
	U_MemUnpackf32v2( v );
	SetSurfaceCtrlPoint3f( sc, 1, 1, v[0], v[1], 0 );

	U_EndMemUnpack();
	
	Mesh_GetIntBase( mesh, 1, &m_vrefnum, &m_vrefs );
	Mesh_GetVec2dBase( mesh, 3, &m_vertexnum, &m_uvs );

	Mesh_BeginInstance();
	Mesh_AddVec2dArray( m_vrefnum );
	tc_mesh = Mesh_EndInstance();
	Mesh_GetVec2dBase( tc_mesh, 0, &tmp_int, &m_tc );
	
	for ( i = 0; i < m_vrefnum; i++ )
	{
		vec3d_t		v3;

		EvalSurfacePoint( sc, m_uvs[m_vrefs[i]][0], m_uvs[m_vrefs[i]][1], v3 );
		v[0] = v3[0];
		v[1] = v3[1];

		if ( layer->flags & LAYER_FLAGS_MODE_TEXEL0N )
			UniTex_RealTexCoord_0N( layer->real_source, m_tc[i], v );
		else if ( layer->flags & LAYER_FLAGS_MODE_TEXEL01 )
			UniTex_RealTexCoord_01( layer->real_source, m_tc[i], v );
		else
			__error( "unknown texcoord access type\n" );		
	}

	if ( layer->tc )
		Mesh_Free( layer->tc );

	layer->tc = tc_mesh;
}

/*
  ==============================
  Layer_CreateTexCoords

  ==============================
*/
void Layer_CreateTexCoords( layer_t *layer, mesh_t *mesh )
{
	if ( !mesh )
	{
		__error( "(null) mesh\n" );
	}

	// fixme: is it really a job that should be done here
	if ( !UniTex_IsValid( layer->real_source ) )
	{
		__error( "layer unitex is not vaild\n" );
	}

	if ( layer->flags & LAYER_FLAGS_MODE_PROJECTION )
	{
		if ( layer->flags & LAYER_FLAGS_HAVE_TF2D_STACK )
		{
			Layer_CreateTexCoords_projection_with_stack( layer, mesh );
		}
		else
		{
			Layer_CreateTexCoords_projection( layer, mesh );		       
		}
	}
	else if ( layer->flags & LAYER_FLAGS_MODE_UV )
	{
		Layer_CreateTexCoords_uv( layer, mesh );
	}
	else
	{
		__error( "Layer_CreateTexCoords: no valid layer mode\n" );
	}
}


/*
  ==============================
  Layer_FreeTexCoords

  ==============================
*/
void Layer_FreeTexCoords( layer_t *layer )
{
	if ( layer->tc )
	{
		Mesh_Free( layer->tc );
		layer->tc = NULL;
	}
}
