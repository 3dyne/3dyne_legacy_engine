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



// tess_glmesh.c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "lib_math.h"
#include "lib_container.h"
#include "lib_hobj.h"
#include "shock.h"

#include "r_glbackenddefs.h"
#include "shape_defs.h"
#include "mesh.h"
#include "shape_util.h"
#include "u_va.h"

/*
  ==============================
  Tess_CreateMesh_glmesh

  ==============================
*/
void Tess_CreateMesh_glmesh( shape_t *shp )
{
	mesh_t		*mesh;

	int		ofs_vertex;
	int		num_vertex;
	int		ofs_vref;
	int		num_vref;
	int		ofs_cmd;
	int		num_cmd;

	vec3d_t		*m_vertices;
	int		tmp_int;
	int		i;

	if ( shp->tess_name != ShapeTesselation_glmesh )
		Error( "tess_name is not ShapeTesselation_meshtile\n" );
	
	if ( !(shp->flags & SHAPE_FLAGS_HAVE_GLMESH) )
		Error( "missing flag SHAPE_FLAGS_HAVE_GLMESH\n" );

	__chkptr( shp->glmesh_obj );
	__chkptr( shp->glmesh_base );

	EasyFindInt( &ofs_vertex, shp->glmesh_obj, "ofs_vertex" );
	EasyFindInt( &num_vertex, shp->glmesh_obj, "num_vertex" );
	EasyFindInt( &ofs_vref, shp->glmesh_obj, "ofs_vref" );
	EasyFindInt( &num_vref, shp->glmesh_obj, "num_vref" );
	EasyFindInt( &ofs_cmd, shp->glmesh_obj, "ofs_cmd" );
	EasyFindInt( &num_cmd, shp->glmesh_obj, "num_cmd" );

	Mesh_BeginInstance();
	Mesh_AddExternVec3dArray( num_vertex, &shp->glmesh_base[ofs_vertex] );
	Mesh_AddExternIntArray( num_vref, &shp->glmesh_base[ofs_vref] );
	Mesh_AddExternIntArray( num_cmd, &shp->glmesh_base[ofs_cmd] );
	mesh = Mesh_EndInstance();

	Mesh_GetVec3dBase( mesh, 0, &tmp_int, &m_vertices );
	U_VACalcBB( m_vertices, tmp_int, shp->min, shp->max );
	for ( i = 0; i < 3; i++ )
	{
		shp->min[i] -= SHAPE_BOUNDBOX_SIDE_SPACE;
		shp->max[i] += SHAPE_BOUNDBOX_SIDE_SPACE;
	}	

	if ( shp->mesh )
		Mesh_Free( shp->mesh );

	shp->mesh = mesh;	
}
