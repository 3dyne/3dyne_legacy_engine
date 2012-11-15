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



// lib_uoc.h

#ifndef __lib_uoc
#define __lib_uoc

#include "u_defs.h"
#include "lib_uocdefs.h"

void		UOC_InitNode( uoc_node_t *node, unique_t self_id, void *owner_obj, uoc_ftbl_t *ftbl, uocNodePriority level );
void *		UOC_NodeGetOwner( uoc_node_t *node );
uocNodePriority UOC_NodeGetPriority( uoc_node_t *node );
unique_t	UOC_NodeGetID( uoc_node_t *node );
uoc_ftbl_t *	UOC_NodeGetFTBL( uoc_node_t *node );

const void *		UOC_NodeGetPrimaryKey( const void *obj );
int		UOC_NodeComparePrimaryKeys( const void *key1, const void *key2 );

void		UOC_InitMessage( uoc_message_t *msg );
int		UOC_MessageGetMaxSize( uoc_message_t *msg );
const char *	UOC_MessageGetText( uoc_message_t *msg );
#endif

