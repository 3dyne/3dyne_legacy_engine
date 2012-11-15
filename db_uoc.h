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



// db_uoc.h

#ifndef __db_uoc
#define __db_uoc

#include "db_uocdefs.h"
#include "lib_uoc.h"

void UOCDB_Init( db_uoc_t *db );
void UOCDB_CleanUp( db_uoc_t *db );

void UOCDB_InsertNode( db_uoc_t *db, uoc_node_t *node );
void UOCDB_RemoveNode( db_uoc_t *db, uoc_node_t *node );

uoc_node_t * UOCDB_SelectNodeWhere_nodeid( db_uoc_t *db, unique_t uoc_id );

bool_t UOCDBU_SendMessage( db_uoc_t *db, unique_t recv_id, uoc_message_t *send, uoc_message_t *recv );

#endif
