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



// lib_container.h

#ifndef __lib_container
#define __lib_container

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "s_config.h"
#include "s_mem.h"

#include "lib_containerdefs.h"

/* double linked node */

u_dln_t *	U_NewDLN( void );
void		U_FreeDLN( u_dln_t *node );
u_dln_t *	U_DLNInsert( u_dln_t *prev, u_dln_t *next, u_dln_t *insert );	// return insert
u_dln_t *	U_DLNRemove( u_dln_t *remove );					// return remove

void *		U_DLNGetObjPtr( u_dln_t *node );
void *		U_DLNSetObjPtr( u_dln_t *node, void *objptr );			// return node



/* double linked list */

u_list_t *	U_NewList( void );		// dynamic
void   		U_InitList( u_list_t *list );	// static 
void		U_CleanUpList( u_list_t *list, void (*cleanup_func)(void *obj) );
void		U_FreeList( u_list_t *list );
bool_t		U_ListIsNotEmpty( u_list_t *list );
void		U_ListInsertAtHead( u_list_t *list, void *obj );
void		U_ListInsertAtTail( u_list_t *list, void *obj );
void *		U_ListRemoveAtHead( u_list_t *list );
void *		U_ListRemoveAtTail( u_list_t *list );
void		U_ListForEach( u_list_t *list, void (*action_func)(void *obj) );
int		U_ListLength( u_list_t *list );

void *		U_ListBuildObjPtrArray( u_list_t *list, int *out_len );
void *		U_ListBuildFlatCopyArray( u_list_t *list, int *out_len, size_t objsize );

/* double linked list iterator */

void		U_ListIterInit( u_list_iter_t *iter, u_list_t *list );	// static
void		U_ListIterToHead( u_list_iter_t *iter );
void		U_ListIterToTail( u_list_iter_t *iter );
listIterState	U_ListIterGetState( u_list_iter_t *iter );
void *		U_ListIterGetCurrent( u_list_iter_t *iter );
void *		U_ListIterNext( u_list_iter_t *iter );
void *		U_ListIterPrev( u_list_iter_t *iter );
void 		U_ListIterInsertAsNext( u_list_iter_t *iter, void *objptr );
void		U_ListIterInsertAsPrev( u_list_iter_t *iter, void *objptr );
void		U_ListIterRemoveGoNext( u_list_iter_t *iter );
void		U_ListIterRemoveGoPrev( u_list_iter_t *iter );


/* priority queue container */
#if 0
u_pqueue_t *	U_NewPQueue( int (*key_compare_func)(const void *k1, const void *k2), void * (*get_key_func)(const void *obj) );
void		U_FreePQueue( u_pqueue_t *pq );
void		U_PQueueInsert( u_pqueue_t *pq, void *obj );
//void *		U_PQueueRemove( u_pqueue_t *pq,
#endif


/* ring container */

u_ring_t *	U_NewRing( void );			// dynamic
void		U_InitRing( u_ring_t *ring );		// static
void		U_FreeRing( u_ring_t *ring );
void *		U_RingGetCurrent( u_ring_t *ring );
void		U_RingWalk( u_ring_t *ring, int dir );
void		U_RingInsertAsNext( u_ring_t *ring, void *objptr );
void		U_RingInsertAsPrev( u_ring_t *ring, void *objptr );
void		U_RingRemoveGoNext( u_ring_t *ring );
void		U_RingRemoveGoPrev( u_ring_t *ring );


/* stack container - not impl */

u_stack_t *	U_NewStack( int fixsize );
void		U_FreeStack( u_stack_t *stack );
void		U_StackPush( u_stack_t *stack, void *objptr );
void *		U_StackPop( u_stack_t *stack );
 


/* avl-tree container */

u_avln_t *	U_NewAVLN( void );
void		U_FreeAVLN( u_avln_t *node );

//u_avl_tree_t *	U_NewAVLTree( int (*key_compare_func)(const void *k1, const void *k2), void * (*get_key_func)(const void *obj) );
//void		U_FreeAVLTree( u_avl_tree_t *tree );

void		U_InitAVLTree( u_avl_tree_t *tree, int (*key_compare_func)(const void *k1, const void *k2), const void * (*get_key_func)(const void *obj) );
void		U_CleanUpAVLTree( u_avl_tree_t *tree, void (*cleanup_func)(void *obj) );

int		U_AVLTreeInsert( u_avl_tree_t *tree, void *obj );	// return 0 if there is already a object with the same key
void *		U_AVLTreeRemove( u_avl_tree_t *tree, void *key );	// return NULL if no object with the key could be removed
void *		U_AVLTreeSearch( u_avl_tree_t *tree, void *key );	// return NULL if no object with the key was found
void		U_AVLTreeForEach( u_avl_tree_t *tree, void (*action_func)(void *obj) );		// in ascend order
const void *		U_AVLTreeGetTopKey( u_avl_tree_t *tree );

/* map container */

//u_map_t *	U_NewMap( mapAlgorithm type, int (*key_compare_func)(const void *k1, const void *k2), void * (*get_key_func)(const void *obj) );
void		U_InitMap( u_map_t *map, mapAlgorithm type, int (*key_compare_func)(const void *k1, const void *k2), const void * (*get_key_func)(const void *obj) );
void		U_CleanUpMap( u_map_t *map, void (*claenup_func)(void *obj ) );
int		U_MapInsert( u_map_t *map, void *obj );		// return 0 if there is already a object with the key
void *		U_MapRemove( u_map_t *map, const void *key );		// return NULL if no object with the key could be removed
void *		U_MapSearch( u_map_t *map, const void *key );		// return NULL if no object with the key was found
void		U_MapForEach( u_map_t *map, void (*action_func)(void *obj) );

//void		U_MapDestroy( u_map_t *map, void (*object_destroy_func)(void *obj) );
u_list_t *	U_MapGetKeyList( u_map_t *map, void * (*key_clone_func)(void *key) );
int		U_MapSize( u_map_t *map );

#endif
