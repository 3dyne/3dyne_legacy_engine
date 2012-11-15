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



// trify_generic_polygon.c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "lib_math.h"
#include "shock.h"

#include "r_glbackenddefs.h"

#include "mesh.h"
#include "trify.h"


#define TRIFY_GENERIC_POLY_MAX_POINTS	( 128 )
#define TRIFY_GENERIC_POLY_MAX_TRIANGLS ( 1024 )

//#define __error		Error

/*
  =============================================================================
  generic polygon triangulation
  alpha version
  
  =============================================================================
*/
static vec3d_t	t_norm;
static fp_t	t_dist;
static int	t_vertexnum;
static vec3d_t	t_pts[TRIFY_GENERIC_POLY_MAX_POINTS];
static int	t_pnum;
static int	t_remains[TRIFY_GENERIC_POLY_MAX_POINTS];

static int	t_trinum;
static int	t_tris[TRIFY_GENERIC_POLY_MAX_TRIANGLS][3];

/*
  ==============================
  Trify_RemovePoint

  ==============================
*/
void Trify_RemovePoint( int point )
{
	int		i;
	
	if ( t_pnum == 0 )
		__error( "no more points\n" );

//	printf( "remove: %d\n", t_remains[point] );

	t_pnum--;	
	for ( i = point; i < t_pnum; i++ )
	{
		t_remains[i] = t_remains[i+1];
	}
}

/*
  ==============================
  Trify_GenTri

  ==============================
*/
void Trify_GenTri( int p1, int p2, int p3 )
{
	if ( t_trinum == TRIFY_GENERIC_POLY_MAX_TRIANGLS )
		__error( "reached TRIFY_GENERIC_POLY_MAX_TRIANGLS\n" );

	t_tris[t_trinum][0] = p1;
	t_tris[t_trinum][1] = p2;
	t_tris[t_trinum][2] = p3;

//	printf( "Trify_GenTri: %d-%d-%d\n", p1, p2, p3 );

#if 0
	glBegin( GL_TRIANGLES );
	glColor3f( 1,0,0 );
	glVertex3fv( t_pts[p1] );
	glVertex3fv( t_pts[p2] );
	glVertex3fv( t_pts[p3] );	
	glEnd();
	glFlush();
#endif

	t_trinum++;
}

/*
  ==============================
  Trify_SetupPlane

  ==============================
*/
void Trify_SetupPlane( vec3d_t norm, fp_t *dist, vec3d_t p1, vec3d_t p2 )
{
	vec3d_t		v1;

	Vec3dSub( v1, p2, p1 );
	Vec3dUnify( v1 );
	Vec3dCrossProduct( norm, v1, t_norm );
	*dist = Vec3dDotProduct( norm, p1 );
//	Vec3dPrint( norm );
}

bool_t Trify_TestTri( vec3d_t p1, vec3d_t p2, vec3d_t p3, vec3d_t t )
{
	vec3d_t		sum;
	vec3d_t		v1, v2, v3, norm;

	Vec3dSub( v1, p1, t );
	Vec3dSub( v2, p2, t );
	Vec3dSub( v3, p3, t );
	Vec3dCrossProduct( sum, v1, v2 );
	Vec3dUnify( sum );
	Vec3dCrossProduct( norm, v2, v3 );
	Vec3dUnify( norm );
	Vec3dAdd( sum, sum, norm );
	Vec3dCrossProduct( norm, v3, v1 );
	Vec3dUnify( norm );
	Vec3dAdd( sum, sum, norm );

//	printf( "len: %f\n", Vec3dLen( sum ) );
	
	if ( Vec3dLen( sum ) < 2.9 )
		return false;
	return true;
}

/*
  ==============================
  Trify_ClipEar

  ==============================
*/
bool_t Trify_ClipEar( void )
{
	int		p, n, i, j;
	vec3d_t		norm;
	fp_t		dist;
	fp_t		d;
	bool_t		ok;

	for ( i = 0; i < t_pnum; i++ )
	{
		p = (i==0)?(t_pnum-1):(i-1);
		n = (i==(t_pnum-1))?0:(i+1);

//		printf( "p - i - n: %d %d %d\n", t_remains[p], t_remains[i], t_remains[n] );

		Trify_SetupPlane( norm, &dist, t_pts[t_remains[p]], t_pts[t_remains[n]] );

		ok = false;
		for ( j = 0; j < t_pnum; j++ )
		{
			if ( j == p || j == n )
				continue;

//			printf( "test: %d\n", t_remains[j] );

			d = Vec3dDotProduct( norm, t_pts[t_remains[j]] ) - dist;

			if ( d > 0 )
			{
				if ( j != i )
				{
					// failed
					if ( Trify_TestTri( t_pts[t_remains[p]], t_pts[t_remains[i]], t_pts[t_remains[n]], t_pts[t_remains[j]] ) )
					{
						break;
					}
				}
				else
				{
//					printf( "ok\n" );
					ok = true;
				}
			}
			else
			{
				
			}
//			printf( " %d d: %f\n", j, d );
//			if ( d < 0 )
		}
		
		if ( j != t_pnum )
		{
//			printf( "ear test failed\n" );
		}
		else
		{		    
			if ( ok )
			{
//				printf( "ear test ok\n" );
				
			// take the first ear that's ok
				Trify_GenTri( t_remains[p], t_remains[i], t_remains[n] );
				Trify_RemovePoint( i );

//				getchar();
				
				return true;
			}
		}

	}
	return false;
}


/*
  ==============================
  Trify_GenMesh

  ==============================
*/
mesh_t * Trify_GenMesh( void )
{
	int			i;
	mesh_t	*mesh;

	vec3d_t		*m_vertices;
	int		*m_vrefs;
	int		*m_cmds;

	int		tmp_int;

//	mesh = Shape_NewMesh( t_vertexnum, t_trinum*3, 3 /* commands */ );
//	Shape_MeshGetBuffers( mesh, &m_vertices, &m_vrefs, &m_cmds );

	Mesh_BeginInstance();
	Mesh_AddVec3dArray( t_vertexnum );
	Mesh_AddIntArray( t_trinum*3 );
	Mesh_AddIntArray( 3 );
	mesh = Mesh_EndInstance();

	Mesh_GetVec3dBase( mesh, 0, &tmp_int, &m_vertices );
	Mesh_GetIntBase( mesh, 1, &tmp_int, &m_vrefs );
	Mesh_GetIntBase( mesh, 2, &tmp_int, &m_cmds );

	//
	// fill vertex buffer
	//
	
	for ( i = 0; i < t_vertexnum; i++ )
	{
		Vec3dCopy( m_vertices[i], t_pts[i] );
	}

	//
	// fill vref buffer
	//

	for ( i = 0; i < t_trinum; i++ )
	{
		m_vrefs[i*3] = t_tris[i][0];
		m_vrefs[i*3+1] = t_tris[i][1];
		m_vrefs[i*3+2] = t_tris[i][2];
	}

	//
	// fill cmd buffer
	//
	m_cmds[0] = BE_CMD_TRIANGLES;
	m_cmds[1] = t_trinum*3;
	m_cmds[2] = BE_CMD_END;

	return mesh;
}

/*
  ==================================================
  public interface

  ==================================================
*/

/*
  ==============================
  Trify_BeginGenericPolygon

  ==============================
*/
void Trify_BeginGenericPolygon( void )
{
	// input
	t_pnum = 0;

	// output
	t_trinum = 0;
}

/*
  ==============================
  Trify_SetPlane

  ==============================
*/
void Trify_SetPlane( vec3d_t norm, fp_t dist )
{
	Vec3dCopy( t_norm, norm );
	t_dist = dist;
}

/*
  ==============================
  Trify_AddPoint

  ==============================
*/
void Trify_AddPoint( vec3d_t p )
{
	if ( t_pnum == TRIFY_GENERIC_POLY_MAX_POINTS )
		__error( "reached TRIFY_GENERIC_POLY_MAX_POINTS\n" );

	Vec3dCopy( t_pts[t_pnum], p );
	t_pnum++;
}

/*
  ==============================
  Trify_End

  ==============================
*/
mesh_t * Trify_End( void )
{
	int		i;

	for ( i = 0; i < t_pnum; i++ )
	{
		t_remains[i] = i;
	}

	t_vertexnum = t_pnum;

	for ( ; t_pnum > 3 ; )
	{
		if ( !Trify_ClipEar() )
		{
			__named_message( "Triangulation failed\n" );
			break;
		}
	}
	if ( t_pnum == 3 )
		Trify_GenTri( t_remains[0], t_remains[1], t_remains[2] );

	// generate mesh
	
	return Trify_GenMesh();
}
