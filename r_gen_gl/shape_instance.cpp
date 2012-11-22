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



// shape_instance.c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "lib_math.h"
#include "lib_container.h"
#include "shock.h"

#include "shape_defs.h"
#include "shape_instance.h"

#include "lib_uoc.h"
#include "db_uoc.h"

#include "layer_instance.h"		// for Layer_CleanUp() ...

static bool_t		shape_in_instance = false;
static shape_t		*shape_tmp = NULL;


//extern g_map_t		*a_map;



/*
  ==============================
  Shape_BeginInstance

  ==============================
*/
void Shape_BeginInstance( void )
{
	if ( shape_in_instance == true )
		__error( "already in instance\n" );

	shape_in_instance = true;
	shape_tmp = NEWTYPE( shape_t );
	shape_tmp->self_id = UNIQUE_INVALIDE;
	shape_tmp->flags = 0;

	shape_tmp->cp_list = U_NewList();
	shape_tmp->layer_list = U_NewList();
}

/*
  ==============================
  Shape_SetPrimaryKey

  ==============================
*/
void Shape_SetPrimaryKey( unique_t self_id )
{
	shape_tmp->flags |= SHAPE_FLAGS_HAVE_PRIMARY_KEY;
	shape_tmp->self_id = self_id;
}


/*
  ==============================
  Shape_SetTesselation

  ==============================
*/
void Shape_SetTesselation( shapeTesselation tess_name )
{
	shape_tmp->flags |= SHAPE_FLAGS_HAVE_TESSELATION;
	shape_tmp->tess_name = tess_name;
}

/*
  ==============================
  Shape_SetPlaneHint

  ==============================
*/
void Shape_SetPlaneHint( vec3d_t norm, fp_t dist )
{
	shape_tmp->flags |= SHAPE_FLAGS_HAVE_PLANEHINT;
	Vec3dCopy( shape_tmp->norm, norm );
	shape_tmp->dist = dist;
}

/*
  ==============================
  Shape_SetGLMesh

  ==============================
*/
void Shape_SetGLMesh( hobj_t *glmesh, unsigned char *base )
{
	shape_tmp->flags |= SHAPE_FLAGS_HAVE_GLMESH;

	shape_tmp->glmesh_obj = glmesh;
	shape_tmp->glmesh_base = base;
}

/*
  ==============================
  Shape_SetCtrlPointNum

  ==============================
*/
void Shape_SetCtrlPointNum( int unum, int vnum )
{
	shape_tmp->flags |= SHAPE_FLAGS_HAVE_CTRLNUM;
	shape_tmp->unum = unum;
	shape_tmp->vnum = vnum;
}

/*
  ==============================
  Shape_NewCtrlPointUV3f

  ==============================
*/
shape_ctrl_point_t * Shape_NewCtrlPointUV3fv( int u, int v, vec3d_t p )
{
	shape_ctrl_point_t	*cp;

	cp = NEWTYPE( shape_ctrl_point_t );
	cp->u = u;
	cp->v = v;
	Vec3dCopy( cp->p, p );

	return cp;
}

/*
  ==============================
  Shape_AddCtrlPoint

  ==============================
*/
void Shape_AddCtrlPoint( shape_ctrl_point_t *cp )
{
	shape_tmp->flags |= SHAPE_FLAGS_HAVE_CTRL;
	U_ListInsertAtTail( shape_tmp->cp_list, cp );
}

/*
  ==============================
  Shape_AddLayer

  ==============================
*/
void Shape_AddLayer( layer_t *layer )
{
	shape_tmp->flags |= SHAPE_FLAGS_HAVE_LAYER;
	layer->shape = shape_tmp;
	
	U_ListInsertAtHead( shape_tmp->layer_list, layer );
}

/*
  ==============================
  Shape_EndInstance

  ==============================
*/
shape_t * Shape_EndInstance( void )
{
	shape_t		*ret;

	if ( !( shape_tmp->flags & SHAPE_FLAGS_HAVE_TESSELATION ) )
		__error( "missing SHAPE_FLAGS_HAVE_TESSELATION\n" );
	if ( !( shape_tmp->flags & SHAPE_FLAGS_HAVE_CTRLNUM ) && !(shape_tmp->flags & SHAPE_FLAGS_HAVE_GLMESH) )
		__error( "missing SHAPE_FLAGS_HAVE_CTRLNUM\n" );
	if ( !( shape_tmp->flags & SHAPE_FLAGS_HAVE_PRIMARY_KEY ) )
		__warning( "shape ain't got a valid primary key\n" );


	if ( shape_in_instance == false )
		__error( "no new instance\n" );
	shape_in_instance = false;

	ret = shape_tmp;
	shape_tmp = NULL;
       
	return ret;
}


/*
  ==============================
  Shape_CleanUp

  ==============================
*/
void Shape_CleanUp( shape_t *shp )
{
	layer_t		*l;
	shape_ctrl_point_t *cp;

	// cleanup+free ControlPoints in list
	for ( ; ( cp = (shape_ctrl_point_t *)U_ListRemoveAtHead( shp->cp_list ) ) ; )
	{
		FREE( cp );
	}
	// cleanup+free ControlPoint list
	U_CleanUpList( shp->cp_list, NULL );
	FREE( shp->cp_list );

	// cleanup+free Layers in list
	for ( ; ( l = (layer_t *)U_ListRemoveAtHead( shp->layer_list ) ) ; )
	{
		Layer_CleanUp( l );
	}
	// cleanup+free Layer list
	U_CleanUpList( shp->layer_list, NULL );
	FREE( shp->layer_list );

	// free mesh
	FREE( shp->mesh );
	
	// free layers_array_t ?
}
