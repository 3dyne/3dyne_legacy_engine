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



// db_uoc.c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "lib_math.h"
#include "lib_hobj.h"
#include "lib_container.h"

#include "db_uoc.h"

#include "shock.h"
/*
  ==============================
  UOCDB_Init

  ==============================
*/
void UOCDB_Init( db_uoc_t *db )
{
	db->by_nodeid_num = 0;
	U_InitMap( db->by_nodeid_map = NEWTYPE( u_map_t ), map_default, UOC_NodeComparePrimaryKeys, UOC_NodeGetPrimaryKey );
}

/*
  ==============================
  UOCDB_CleanUp

  ==============================
*/
void UOCDB_CleanUp( db_uoc_t *db )
{
	__warning( "not impl\n" );
}

/*
  ==============================
  UOCDB_InsertNode

  ==============================
*/
void UOCDB_InsertNode( db_uoc_t *db, uoc_node_t *node )
{
	if ( !U_MapInsert( db->by_nodeid_map, node ) )
	{
		__error( "there is already an uoc_node with id '#%u' in the database\n", node->self_id );
	}
	else
	{
		db->by_nodeid_num++;
	}
}

/*
  ==============================
  UOCDB_RemoveNode

  ==============================
*/
void UOCDB_RemoveNode( db_uoc_t *db, uoc_node_t *node )
{
	if ( !U_MapRemove( db->by_nodeid_map, UOC_NodeGetPrimaryKey( node ) ) )
	{
		__error( "there is no uoc_node with id '#%u' in the database \n", node->self_id );
	}
	else
	{
		db->by_nodeid_num--;
	}
}

/*
  ==============================
  UOCDB_SelectNodeWhere_nodeid

  ==============================
*/
uoc_node_t * UOCDB_SelectNodeWhere_nodeid( db_uoc_t *db, unique_t uoc_id )
{
	uoc_node_t	tmp;
	uoc_node_t	*node;

	tmp.self_id = uoc_id;

	node = (uoc_node_t *)U_MapSearch( db->by_nodeid_map, UOC_NodeGetPrimaryKey( &tmp ) );

	return node;
}

/*
  ==============================
  UOCDBU_SendMessage

  ==============================
*/
bool_t UOCDBU_SendMessage( db_uoc_t *db, unique_t recv_id, uoc_message_t *send, uoc_message_t *recv )
{
	uoc_node_t	*reciver;

	reciver = UOCDB_SelectNodeWhere_nodeid( db, recv_id );
	
	if ( !reciver )
		return false;

	reciver->ftbl->Comm( reciver, send, recv );
	return true;
}
