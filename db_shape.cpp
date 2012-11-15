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



// db_shape.c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "lib_math.h"
#include "lib_hobj.h"
#include "lib_container.h"

#include "db_shape.h"
#include "shape_db.h"

#include "shock.h"
/*
  ==============================
  ShapeDB_Init

  ==============================
*/
void ShapeDB_Init( db_shape_t *db )
{
	U_InitMap( db->by_selfid_map = NEWTYPE( u_map_t ), map_default, Shape_ComparePrimaryKey, Shape_GetPrimaryKey );
	db->by_selfid_num = 0;
}

/*
  ==============================
  ShapeDB_CleanUp

  ==============================
*/
void ShapeDB_CleanUp( db_shape_t *db )
{
//	__warning( "not impl\n" );	
	U_CleanUpMap( db->by_selfid_map, NULL );
}

/*
  ==============================
  ShapeDB_InsertShape

  ==============================
*/
void ShapeDB_InsertShape( db_shape_t *db, shape_t *obj )
{
	shape_t		tmp;
	shape_t		*search;

	tmp.self_id = obj->self_id;

	search = (shape_t *)U_MapSearch( db->by_selfid_map, &tmp );

	if ( search )
	{
		__error( "there is already a shape with primary key '#%u'\n in the database", obj->self_id );
	}
	else
	{
		U_MapInsert( db->by_selfid_map, obj );
		db->by_selfid_num++;
	}	
}

/*
  ==============================
  ShapeDB_SelectShapeWhere_selfid

  ==============================
*/
void ShapeDB_SelectShapeWhere_selfid_CB( db_shape_t *db, unique_t self_id, void (*action_func)(shape_t *obj) )
{
	shape_t		tmp;
	shape_t		*shp;

	if ( self_id == UNIQUE_ALL )
	{
		U_MapForEach( db->by_selfid_map, (void(*)(void*)) action_func );
	}
	else
	{
		tmp.self_id = self_id;
		
		shp = (shape_t *)U_MapSearch( db->by_selfid_map, &tmp );
		
		if ( !shp )
			return;
		
		action_func( shp );
	}
}


/*
  ==============================
  ShapeDB_SelectShapeWhere_selfid

  ==============================
*/
shape_t * ShapeDB_SelectShapeWhere_selfid( db_shape_t *db, unique_t self_id )
{
	shape_t		tmp;
	shape_t		*shp;
	
	tmp.self_id = self_id;
	
	shp = (shape_t *)U_MapSearch( db->by_selfid_map, &tmp );
	return shp;
}
