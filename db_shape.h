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



// db_shape.h

#ifndef __db_shape
#define __db_shape

#include "u_defs.h"
#include "db_shapedefs.h"
#include "shape_defs.h"
//#include "shape_db.h"

void ShapeDB_Init( db_shape_t *db );
void ShapeDB_CleanUp( db_shape_t *db );

void ShapeDB_InsertShape( db_shape_t *db, shape_t *obj );
void ShapeDB_SelectShapeWhere_selfid_CB( db_shape_t *db, unique_t self_id, void (*action_func)(shape_t *obj) );
shape_t * ShapeDB_SelectShapeWhere_selfid( db_shape_t *db, unique_t self_id );

#endif
