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



// db_at.h

#ifndef __db_at
#define __db_at

#include "u_defs.h"
#include "db_atdefs.h"

void ATDB_Init( db_at_t *db );
void ATDB_CleanUp( db_at_t *db );

void ATDB_InsertArchetype( db_at_t *db, arche_t *obj );
void ATDB_RemoveArchetype( db_at_t *db, arche_t *obj );

int ATDB_SelectArchetypeWhere_selfid( db_at_t *db, unique_t self_id, void (*action_func())(arche_t *obj) );
int ATDB_SelectArchetypeWhere_type( db_at_t *db, char *type, void (*action_func())(arche_t *obj ) );

#endif
