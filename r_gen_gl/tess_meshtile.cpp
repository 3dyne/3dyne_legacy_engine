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



// tess_meshtile.c

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


/*
  ==============================
  Tess_CreateMesh_meshtile

  ==============================
*/
void Tess_CreateMesh_meshtile( shape_t *shp )
{
	mesh_t		*mesh;

	if ( shp->tess_name != ShapeTesselation_meshtile )
		Error( "tess_name is not ShapeTesselation_meshtile\n" );

	if ( shp->vnum != 1 )
		Error( "invalid v order\n" );

	if ( shp->unum < 3 )
		Error( "invalid u order\n" );

	if ( shp->unum != U_ListLength( shp->cp_list ) )
		Error( "control point inconsitancy\n" );

	{
		int		i;

		vec3d_t		*m_vertices;
		int		*m_vrefs;
		int		*m_cmds;
		
		int		tmp_int;

		u_list_iter_t	iter;
		shape_ctrl_point_t	*cp;

		Mesh_BeginInstance();
		Mesh_AddVec3dArray( shp->unum );
		Mesh_AddIntArray( shp->unum );
		Mesh_AddIntArray( 3 );
		mesh = Mesh_EndInstance();

		Mesh_GetVec3dBase( mesh, 0, &tmp_int, &m_vertices );
		Mesh_GetIntBase( mesh, 1, &tmp_int, &m_vrefs );
		Mesh_GetIntBase( mesh, 2, &tmp_int, &m_cmds );

		U_ListIterInit( &iter, shp->cp_list );
		for ( i = 0; ( cp = (shape_ctrl_point_t *)U_ListIterNext( &iter ) ) ; i++ )
		{
			Vec3dCopy( m_vertices[i], cp->p );
			m_vrefs[i] = i;
		}

		m_cmds[0] = BE_CMD_TRIANGLES;
		m_cmds[1] = shp->unum;
		m_cmds[2] = BE_CMD_END;


		U_VACalcBB( m_vertices, shp->unum, shp->min, shp->max );
		
		for ( i = 0; i < 3; i++ )
		{
			shp->min[i] -= SHAPE_BOUNDBOX_SIDE_SPACE;
			shp->max[i] += SHAPE_BOUNDBOX_SIDE_SPACE;
		}		
	}
	
	if ( shp->mesh )
		Mesh_Free( shp->mesh );

	shp->mesh = mesh;

	
}
