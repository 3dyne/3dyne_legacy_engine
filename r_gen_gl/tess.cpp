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



// tess.c

#include "shock.h"
#include "tess.h"

/*
  ==============================
  Tess_CreateMesh

  ==============================
*/

void Tess_CreateMesh( shape_t *shp )
{

	if ( shp->tess_name == ShapeTesselation_simple_face )
	{
		Tess_CreateMesh_simple_face( shp );
	}
	else if ( shp->tess_name == ShapeTesselation_curved_face )
	{
		Tess_CreateMesh_curved_face( shp );
	}
	else if ( shp->tess_name == ShapeTesselation_curved_surface )
	{
		Tess_CreateMesh_curved_surface( shp );
	}
	else if ( shp->tess_name == ShapeTesselation_meshtile )
	{
		Tess_CreateMesh_meshtile( shp );
	}
	else if ( shp->tess_name == ShapeTesselation_glmesh )
	{
		Tess_CreateMesh_glmesh( shp );
	}
	else
	{
		__error( "unknown tess_name\n" );
	}
}


/*
  ==============================
  Shape_BBDoIntersect

  ==============================
*/
bool_t Shape_BBDoIntersect( shape_t *shp, vec3d_t min, vec3d_t max )
{
	return Vec3dBBDoIntersect( min, max, shp->min, shp->max );
}
