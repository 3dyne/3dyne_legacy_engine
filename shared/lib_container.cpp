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



// lib_container.c

#include "lib_container.h"

#include "shock.h"

#define printf  S_Printf                                                        
#define Error   __error

#define __inline__ inline

/*
  ==================================================
  double linked node

  ==================================================
*/

/*
  ==============================
  U_NewDLN

  ==============================
*/
u_dln_t * U_NewDLN( void )
{
	u_dln_t		*node;

//	node = (u_dln_t *) malloc( sizeof( u_dln_t * ) );
//	memset( node, 0, sizeof( u_dln_t * ) );
	node = NEWTYPE( u_dln_t );

	return node;
}



/*
  ==============================
  U_FreeDLN

  ==============================
*/
void U_FreeDLN( u_dln_t *node )
{
	FREE( node );
}



/*
  ==============================
  U_DLNInsert

  ==============================
*/
u_dln_t * U_DLNInsert( u_dln_t *prev, u_dln_t *next, u_dln_t *insert )
{
	prev->next = insert;
	insert->prev = prev;
	next->prev = insert;
	insert->next = next;

	return insert;
}



/*
  ==============================
  U_DLNRemove

  ==============================
*/

u_dln_t * U_DLNRemove( u_dln_t *remove )
{
	remove->prev->next = remove->next;
	remove->next->prev = remove->prev;
	
	return remove;
}



/*
  ==============================
  U_DLNGetObjPtr

  ==============================
*/

void * U_DLNGetObjPtr( u_dln_t *node )
{
	return node->objptr;
}



/*
  ==============================
  U_DLNSetObjPtr

  ==============================
*/

void * U_DLNSetObjPtr( u_dln_t *node, void *objptr )
{
	node->objptr = objptr;

	return node;
}



/*
  =============================================================================
  double linked list

  =============================================================================
*/

/*
  ==============================
  U_NewList

  ==============================
*/

u_list_t * U_NewList( void )
{
	u_list_t	*list;

//	list = (u_list_t *) malloc( sizeof ( u_list_t ) );
	list = NEWTYPE( u_list_t );
	U_InitList( list );

	list->free_self = 1;	// dynamic

	return list;
}

/*
  ==============================
  U_NewList

  ==============================
*/

void U_InitList( u_list_t *list )
{
	memset( list, 0, sizeof ( u_list_t ) );

	list->head = U_NewDLN();
	list->tail = U_NewDLN();
	
	list->head->prev = list->head;
	list->tail->next = list->tail;

	list->head->next = list->tail;
	list->tail->prev = list->head;
}

/*
  ==============================
  U_CleanUpList

  ==============================
*/
void U_CleanUpList( u_list_t *list, void (*cleanup_func)(void *obj) )
{
	u_dln_t		*n, *next;

	for ( n = list->head->next; n != list->tail; n=next )
	{
		next = n->next;

		if ( cleanup_func )
		{
			if ( n->objptr )
			{
				cleanup_func( n->objptr );
			}
		}
		U_FreeDLN( n );
	}

	U_FreeDLN( list->head );
	U_FreeDLN( list->tail );

	list->head = NULL;
	list->tail = NULL;
}

/*
  ==============================
  U_FreeList

  ==============================
*/

void U_FreeList( u_list_t *list )
{
	U_FreeDLN( list->head );
	U_FreeDLN( list->tail );

	if ( list->free_self )
		FREE( list );
}

/*
  ==============================
  U_ListIsNotEmpty

  ==============================
*/
bool_t U_ListIsNotEmpty( u_list_t *list )
{
	if ( list->head->next == list->tail )
		return false;
	return true;
}

/*
  ==============================
  U_ListInsertAtHead

  ==============================
*/

void U_ListInsertAtHead( u_list_t *list, void *obj )
{
	u_dln_t		*node;

	node = U_NewDLN();
	U_DLNSetObjPtr( node, obj );

	U_DLNInsert( list->head, list->head->next, node );
}



/*
  ==============================
  U_ListInsertAtTail

  ==============================
*/

void U_ListInsertAtTail( u_list_t *list, void *obj )
{
	u_dln_t		*node;
	
	node = U_NewDLN();
	U_DLNSetObjPtr( node, obj );

	U_DLNInsert( list->tail->prev, list->tail, node );
}

/*
  ==============================
  U_ListRemoveAtHead

  ==============================
*/
void * U_ListRemoveAtHead( u_list_t *list )
{
	u_dln_t *remove;
	void	*obj;

	if ( list->head->next == list->tail )
		return NULL;

	remove = U_DLNRemove( list->head->next );
	obj = U_DLNGetObjPtr( remove );
	U_FreeDLN( remove );

	return obj;
}

/*
  =============================
  U_ListRemoveAtTail

  =============================
*/	
void * U_ListRemoveAtTail( u_list_t *list )
{
	u_dln_t *remove;
	void	*obj;

	if ( list->head->next == list->tail )
		return NULL;

	remove = U_DLNRemove( list->tail->prev );
	obj = U_DLNGetObjPtr( remove );
	U_FreeDLN( remove );

	return obj;
}

/*
  ==============================
  U_ListForEach

  ==============================
*/

void U_ListForEach( u_list_t *list, void (*action_func)(void *obj) )
{
	u_dln_t		*n;

	for ( n = list->head->next; n != list->tail; n=n->next )
	{
		action_func( n->objptr );
	}
}


/*
  ==============================
  U_ListLength

  ==============================
*/
int U_ListLength( u_list_t *list )
{
	int		i;
	u_dln_t		*n;

	i = 0;
	for ( n = list->head->next; n != list->tail; n=n->next )
	{
		i++;
	}	

	return i;
}

/*
  ==============================
  U_ListBuildPtrArray

  ==============================
*/
void * U_ListBuildObjPtrArray( u_list_t *list, int *out_len )
{
	int		len;
	maddr_t		*base, *ptr;
	u_dln_t		*n;

	len = U_ListLength( list );
	*out_len = len;
	if ( len == 0 )
		return NULL;

	base = (maddr_t *)NEWBYTES( len*sizeof( maddr_t) );
	
	ptr = base;
	for ( n = list->head->next ; n != list->tail ; n=n->next )
	{
		*ptr = (maddr_t)n->objptr;
		ptr++;
	}
       
	return base;
}

/*
  ==============================
  U_ListBuildFlatCopyArray

  ==============================
*/
void * U_ListBuildFlatCopyArray( u_list_t *list, int *out_len, size_t objsize )
{
	int		len;
	char 		*base, *ptr;
	u_dln_t		*n;

	len = U_ListLength( list );
	*out_len = len;
	if ( len == 0 )
		return NULL;

	base = (char *)NEWBYTES( len*objsize );

	ptr = base;
	for ( n = list->head->next; n != list->tail ; n=n->next )
	{
		memcpy( ptr, n->objptr, objsize );
		ptr+=objsize;
	}
	
	return base;
}

/*
  ==================================================
  double linked list iterator

  ==================================================
*/

/*
  ==============================
  U_ListIterInit

  ==============================
*/

void U_ListIterInit( u_list_iter_t *iter, u_list_t *list )
{
	iter->list = list;
	iter->current = list->head;
}


/*
  ==============================
  U_ListIterToHead

  ==============================
*/
void U_ListIterToHead( u_list_iter_t *iter )
{
	iter->current = iter->list->head;
}



/*
  ==============================
  U_ListIterToTail

  ==============================
*/
void U_ListIterToTail( u_list_iter_t *iter )
{
	iter->current = iter->list->tail;
}


/*
  ==============================
  U_ListIterGetState

  ==============================
*/
listIterState U_ListIterGetState( u_list_iter_t *iter )
{
	if ( iter->current == iter->list->head )
		return iter_at_head;

	if ( iter->current == iter->list->tail )
		return iter_at_tail;

	return iter_at_item;
}



/*
  ==============================
  U_ListIterGetCurrent

  ==============================
*/
void * U_ListIterGetCurrent( u_list_iter_t *iter )
{
	return U_DLNGetObjPtr( iter->current );
}



/*
  ==============================
  U_ListIterNext

  ==============================
*/
void * U_ListIterNext( u_list_iter_t *iter )
{
	iter->current = iter->current->next;

	if ( iter->current == iter->list->tail )
	{
		return NULL;
	}

	return U_DLNGetObjPtr( iter->current );
}



/*
  ==============================
  U_ListIterPrev

  ==============================
*/

void * U_ListIterPrev( u_list_iter_t *iter )
{
	iter->current = iter->current->prev;

	if ( iter->current == iter->list->head )
	{
		return NULL;
	}

	return U_DLNGetObjPtr( iter->current );
}



/*
  ==============================
  U_ListIterInsertAsNext

  ==============================
*/
void U_ListIterInsertAsNext( u_list_iter_t *iter, void *objptr )
{
	u_dln_t		*node;

	node = U_NewDLN();
	U_DLNSetObjPtr( node, objptr );

	if ( iter->current == iter->list->tail )
	{
		// insert at (tail->prev, tail)
		U_DLNInsert( iter->current->prev, iter->current, node );
		iter->current = node;
		return;
	}

	U_DLNInsert( iter->current, iter->current->next, node );
	iter->current = node;
}



/*
  ==============================
  U_ListIterInsertAsPrev

  ==============================
*/
void U_ListIterInsertAsPrev( u_list_iter_t *iter, void *objptr )
{
	u_dln_t		*node;

	node = U_NewDLN();
	U_DLNSetObjPtr( node, objptr );

	if ( iter->current == iter->list->head )
	{
		// insert at (head, head->next)
		U_DLNInsert( iter->current, iter->current->next, node );
		iter->current = node;
		return;
	}

	U_DLNInsert( iter->current->prev, iter->current, node );
	iter->current = node;
}



/*
  ==============================
  U_ListIterRemoveGoNext

  ==============================
*/
void U_ListIterRemoveGoNext( u_list_iter_t *iter )
{
	if ( iter->current == iter->list->tail )
	{
		// at tail, do nothing
		return;
	}

	if ( iter->current == iter->list->head )
	{
		// at head, go next
		iter->current = iter->current->next;
		return;
	}

	U_DLNRemove( iter->current );
	iter->current = iter->current->next;

	// fix me: free dln
}




/*
  ==============================
  U_ListIterRemoveGoPrev

  ==============================
*/
void U_ListIterRemoveGoPrev( u_list_iter_t *iter )
{
	if ( iter->current == iter->list->head )
	{
		// at head, do nothing
		return;
	}

	if ( iter->current == iter->list->tail )
	{
		// at tail, go prev
		iter->current = iter->current->prev;
	}

	U_DLNRemove( iter->current );
	iter->current = iter->current->prev;

	// fix me: free dln
}



  
/*
  =============================================================================
  ring container

  =============================================================================
*/

/*
  ==============================
  U_NewRing

  ==============================
*/
u_ring_t * U_NewRing( void )
{
	u_ring_t	*ring;

//	ring = (u_ring_t *) malloc( sizeof( u_ring_t ) );
//	memset( ring, 0, sizeof( u_ring_t ) );

	ring = NEWTYPE( u_ring_t );

	return ring;
}


/*
  ==============================
  U_InitRing

  ==============================
*/
void U_InitRing( u_ring_t *ring )
{
	memset( ring, 0, sizeof( u_ring_t ) );
}


/*
  ==============================
  U_FreeRing

  ==============================
*/
void U_FreeRing( u_ring_t *ring )
{
	FREE( ring );
}


/*
  ==============================
  U_RingGetCurrent

  ==============================
*/
void * U_RingGetCurrent( u_ring_t *ring )
{
	return ring->current;
}


/*
  ==============================
  U_RingWalk

  ==============================
*/
void U_RingWalk( u_ring_t *ring, int dir )
{
	int		i;

	if ( dir == 0 || !ring->current )
	{
		return;
	}

	if ( dir < 0 )
	{
		// walk prev
		for ( i = dir; i < 0; i++ )
		{
			ring->current = ring->current->prev;
		}
	}

	if ( dir > 0 )
	{
		// walk next
		for ( i = 0; i < dir; i++ )
		{
			ring->current = ring->current->next;
		}
	}
}



/*
  ==============================
  U_RingInsertAsNext

  ==============================
*/
void U_RingInsertAsNext( u_ring_t *ring, void *objptr )
{
	u_dln_t		*node;

	node = U_NewDLN();
	U_DLNSetObjPtr( node, objptr );

	if ( !ring->current )
	{
		// first node
		node->next = node;
		node->prev = node;
		ring->current = node;
		return;
	}

	U_DLNInsert( ring->current, ring->current->next, node );
	ring->current = node;
}


/*
  ==============================
  U_RingInsertAsPrev

  ==============================
*/
void U_RingInsertAsPrev( u_ring_t *ring, void *objptr )
{
	u_dln_t		*node;

	node = U_NewDLN();
	U_DLNSetObjPtr( node, objptr );

	if ( !ring->current )
	{
		// first node
		node->next = node;
		node->prev = node;
		ring->current = node;
		return;		
	}

	U_DLNInsert( ring->current->prev, ring->current, node );
	ring->current = node;
}



/*
  ==============================
  U_RingRemoveGoNext

  ==============================
*/
void U_RingRemoveGoNext( u_ring_t *ring )
{
	if ( !ring->current )
		return;

	if ( ring->current->next == ring->current )
	{
		// last node
		ring->current = NULL;

		// fix me: free dln
		return;
	}

	U_DLNRemove( ring->current );
	ring->current = ring->current->next;

	// fix me: free dln
}



/*
  ==============================
  U_RingRemoveGoPrev

  ==============================
*/
void U_RingRemoveGoPrev( u_ring_t *ring )
{
	if ( !ring->current )
		return;

	if ( ring->current->next == ring->current )
	{
		// last node
		ring->current = NULL;

		// fix me: free dln
		return;
	}

	U_DLNRemove( ring->current );
	ring->current = ring->current->prev;

	// fix me: free dln
}



/*
  =============================================================================
  stack container -- not yet

  =============================================================================
*/

/*
  ==============================
  U_NewStack

  ==============================
*/
u_stack_t * U_NewStack( int fixsize )
{
//	u_stack_t	*stack;
//	int		size;

#if 0
	Error( "U_Stack: shame! not impl\n" );

	if ( fixsize > 0 )
	{
		// create a fix stack

		size = (int)(((u_stack_t *)0)->fix_stack[fixsize]);
//		stack = (u_stack_t *) malloc( size );
//		memset( stack, 0, size );
		stack = NEWBYTES( size );
	
		stack->fixsize = fixsize;
		stack->fixtop = -1;
	}
	else
	{
		// create a grow stack
		size = (int)(((u_stack_t *)0)->fix_stack[0]);
//		stack = (u_stack_t *) malloc( size );
//		memset( stack, 0, size );
		stack = NEWBYTES( size );

		stack->fixsize = 0;
		stack->fixtop = 0;
		
		stack->grow_stack = U_NewList();
	}

#endif	
	return NULL;
}


/*
  ==============================
  U_FreeStack

  ==============================
*/
void U_FreeStack( u_stack_t *stack )
{
	Error( "U_Stack: shame! not impl\n" );

	FREE( stack );
}



/*
  ==============================
  U_StackPush

  ==============================
*/
void U_StackPush( u_stack_t *stack, void *objptr )
{
	Error( "U_Stack: shame! not impl\n" );

	if ( stack->fixsize )
	{
		// fix stack
		stack->fixtop++;
		if ( stack->fixtop == stack->fixsize )
		{
			Error( "U_StackPush: fixstack overflow\n" );
		}
		stack->fix_stack[stack->fixtop] = objptr;
	}
	else
	{
		// grow stack
		U_ListInsertAtHead( stack->grow_stack, objptr );
	}
}



/*
  ==============================
  U_StackPop

  ==============================
*/
void* U_StackPop( u_stack_t *stack )
{
	Error( "U_Stack: shame! not impl\n" );

	if ( stack->fixsize )
	{
		// fix stack
		if ( stack->fixtop == -1 )
		{
			Error( "U_StackPop: fixstack underflow\n" );
		}
		stack->fixtop--;
		return stack->fix_stack[stack->fixtop+1];
	}
	else
	{

	}


	return NULL;
}



/*
  =============================================================================
  avl-tree container

  =============================================================================
*/

/*
  ==================================================
  avl-tree internal functions

  ==================================================
*/

static int	avl_success;	// operation succeed ?
static void *	avl_remove;	// set to the removed object

/*
  ==============================
  AVLRotateNodeLeft

  ==============================
*/
static __inline__ u_avln_t * AVLRotateNodeLeft( u_avln_t *n )
{
// 	u_avln_t		*c1;
    u_avln_t        *c2;
	int		s, t;

// 	c1 = n->child[0];
	c2 = n->child[1];

	n->child[1] = c2->child[0];
	c2->child[0] = n;

	s = n->balance;
	t = c2->balance;

	if ( t <= 0 )
	{
		if ( s >= 1 )
		{
			c2->balance = t - 1;
		}
		else
		{
			c2->balance = s + t - 2;
		}
		n->balance = s - 1;
	}
	else
	{
		if ( s <= t )
		{
			c2->balance = s - 2;
		}
		else
		{
			c2->balance = t - 1;
		}
		n->balance = s - t - 1;					
	}

	return c2;
}

/*
  ==============================
  AVLRotateNodeRight

  ==============================
*/
static __inline__ u_avln_t * AVLRotateNodeRight( u_avln_t *n )
{
	u_avln_t		*c1;/*, *c2;*/
	int		s, t;

	c1 = n->child[0];
// 	c2 = n->child[1];

	n->child[0] = c1->child[1];
	c1->child[1] = n;

	s = n->balance;
	t = c1->balance;

	if ( t <= 0 )
	{
		if ( t > s )
		{
			c1->balance = t + 1;
		}
		else
		{
			c1->balance = s + 2;
		}
		n->balance = s - t + 1;
	}
	else
	{
		if ( s <= -1 )
		{
			c1->balance = t + 1;
		}
		else
		{
			c1->balance = s + t + 2;
		}
		n->balance = s + 1;
	}

	return c1;
}

/*
  ==============================
  AVLBalanceNode

  ==============================
*/
static __inline__ u_avln_t * AVLBalanceNode( u_avln_t *n )
{
	if ( n->balance < -1 )
	{
		if ( n->child[0]->balance > 0 )
		{
			n->child[0] = AVLRotateNodeLeft( n->child[0] );
		}
		n = AVLRotateNodeRight( n );
	}
	else if ( n->balance > 1 )
	{
		if ( n->child[1]->balance < 0 )
		{
			n->child[1] = AVLRotateNodeRight( n->child[1] );
		}
		n = AVLRotateNodeLeft( n );
	}
	return n;
}

/*
  ==============================
  AVLRepairLeftBalance

  ==============================
*/
static __inline__ u_avln_t * AVLRepairLeftBalance( u_avln_t *n, int balance )
{
	if ( !n->child[0] )
	{
		n->balance += 1;
	}
	else if ( ( n->child[0]->balance != balance ) &&
		  n->child[0]->balance == 0 )
	{
		n->balance += 1;
	}

	if ( n->balance > 1 ) 
	{
		return AVLBalanceNode( n );
	}
	return n;							    
}

/*
  ==============================
  AVLRepairRightBalance

  ==============================
*/
static __inline__ u_avln_t * AVLRepairRightBalance( u_avln_t *n, int balance )
{
	if ( !n->child[1] )
	{
		n->balance -= 1;
	}
	else if ( ( n->child[1]->balance != balance ) &&
		  n->child[1]->balance == 0 )
	{
		n->balance -= 1;
	}

	if ( n->balance < -1 )
	{
		return AVLBalanceNode( n );
	}
	return n;
}

/*
  ==============================
  AVLRemoveLeftmostRecursive

  ==============================
*/
static __inline__ u_avln_t * AVLRemoveLeftmostRecursive( u_avln_t *n, u_avln_t **left_most )
{
	int	balance;

	if ( !n->child[0] )
	{
		*left_most = n;
		return n->child[1];
	}
	
	balance = n->child[0]->balance;
	n->child[0] = AVLRemoveLeftmostRecursive( n->child[0], left_most );
	return AVLRepairLeftBalance( n, balance );
}

/*
  ==============================
  AVLInsertRecursive

  ==============================
*/
static __inline__ u_avln_t * AVLInsertRecursive( u_avl_tree_t *tree, u_avln_t *n, void *obj, const void *key )
{
	int	balance;
	const void	*other_key;
	int	cmp;

	other_key = tree->get_key_func( n->objptr );
	cmp = tree->key_compare_func( key, other_key );
	
	if ( cmp == 0 )
	{
//		Error( "InsertKeyRecursive: key already in tree\n" );
//		insert_failed = 1;
		return n;
	}
	else if ( cmp < 0 )
	{
		if ( n->child[0] )
		{
			balance = n->child[0]->balance;
			n->child[0] = AVLInsertRecursive( tree, n->child[0], obj, key );
			if ( ( balance != n->child[0]->balance ) &&
			     n->child[0]->balance )
			{
				n->balance -= 1;
			}
		}
		else
		{
			n->child[0] = U_NewAVLN();
			n->child[0]->objptr = obj;
			n->balance -= 1;
			avl_success = 1;
		}
	}
	else if ( cmp > 0 )
	{
		if ( n->child[1] )
		{
			balance = n->child[1]->balance;
			n->child[1] = AVLInsertRecursive( tree, n->child[1], obj, key );
			if ( ( balance != n->child[1]->balance ) &&
			     n->child[1]->balance )
			{
				n->balance += 1;
			}

		}
		else
		{
			n->child[1] = U_NewAVLN();
			n->child[1]->objptr = obj;
			n->balance += 1;
			avl_success = 1;
		}
	}
	
	n = AVLBalanceNode( n );

	return n;
}

/*
  ==============================
  AVLRemoveRecursive

  ==============================
*/
static __inline__ u_avln_t * AVLRemoveRecursive( u_avl_tree_t *tree, u_avln_t *n, void *key )
{
	int		balance;
	u_avln_t		*left_most;
	u_avln_t		*destroy;
	const void	*other_key;
	int		cmp;

	other_key = tree->get_key_func( n->objptr );
	cmp = tree->key_compare_func( key, other_key );

	if ( cmp == 0 )
	{
		avl_remove = n->objptr;
		destroy = n;

		if ( !n->child[1] )
		{
			n = n->child[0];
		}
		else
		{
			balance = n->child[1]->balance;
			n->child[1] = AVLRemoveLeftmostRecursive( n->child[1], &left_most );
			left_most->child[0] = n->child[0];
			left_most->child[1] = n->child[1];
			left_most->balance = n->balance;
			n = AVLRepairRightBalance( left_most, balance );
		}

		U_FreeAVLN( destroy );
	}
	else if ( cmp < 0 )
	{
		if ( n->child[0] )
		{
			balance = n->child[0]->balance;
			n->child[0] = AVLRemoveRecursive( tree, n->child[0], key );
			n = AVLRepairLeftBalance( n, balance );
		}
	}
	else if ( cmp > 0 )
	{
		if ( n->child[1] )
		{
			balance = n->child[1]->balance;
			n->child[1] = AVLRemoveRecursive( tree, n->child[1], key );
			n = AVLRepairRightBalance( n, balance );
		}
	}

	return n;
}


/*
  ==============================
  AVLForEachRecursive

  ==============================
*/
void AVLForEachRecursive( u_avln_t *n, void (*action_func)(void *obj) )
{
	if ( n->child[0] )
		AVLForEachRecursive( n->child[0], action_func );

	action_func( n->objptr );

	if ( n->child[1] )
		AVLForEachRecursive( n->child[1], action_func );
}

/*
  ==================================================
  avl-tree public functions

  ==================================================
*/

/*
  ==============================
  U_NewAVLN

  ==============================
*/
u_avln_t * U_NewAVLN( void )
{
	u_avln_t	*n;

//	n = (u_avln_t *) malloc( sizeof( u_avln_t ) );
//	memset( n, 0, sizeof( u_avln_t ) );
	n = NEWTYPE( u_avln_t );

	return n;
}

/*
  ==============================
  U_FreeAVLN

  ==============================
*/
void U_FreeAVLN( u_avln_t *n )
{
	FREE( n );
}

/*
  ==============================
  U_NewAVLTree

  ==============================
*/
u_avl_tree_t * _U_NewAVLTree( int (*key_compare_func)(const void *k1, const void *k2), const void * (*get_key_func)(const void *obj) )
{
	u_avl_tree_t	*tree;

//	tree = (u_avl_tree_t *) malloc( sizeof( u_avl_tree_t ) );
//	memset( tree, 0, sizeof( u_avl_tree_t ) );
	tree = NEWTYPE( u_avl_tree_t );

	tree->key_compare_func = key_compare_func;
	tree->get_key_func = get_key_func;
	
	return tree;
}

/*
  ==============================
  U_InitAVLTree

  ==============================
*/
void  U_InitAVLTree( u_avl_tree_t *tree, int (*key_compare_func)(const void *k1, const void *k2), const void * (*get_key_func)(const void *obj) )
{
	INITTYPE( tree, u_avl_tree_t );

	tree->key_compare_func = key_compare_func;
	tree->get_key_func = get_key_func;
}

/*
  ==============================
  U_FreeAVLTree

  ==============================
*/
void _U_FreeAVLTree( u_avl_tree_t *tree )
{
	FREE( tree );
}

/*
  ==============================
  U_AVLTreeInsert

  ==============================
*/
int U_AVLTreeInsert( u_avl_tree_t *tree, void *obj )
{
	const void	*key;

	if ( !tree->topnode )
	{
		tree->topnode = U_NewAVLN();
 		tree->topnode->objptr = obj;
		return 1;
	}

	avl_success = 0;

	key = tree->get_key_func( obj );	// small speed up ?
	tree->topnode = AVLInsertRecursive( tree, tree->topnode, obj, key );
	      
	return avl_success;
}


/*
  ==============================
  U_AVLTreeRemove

  ==============================
*/
void * U_AVLTreeRemove( u_avl_tree_t *tree, void *key )
{
	if ( !tree->topnode )
	{
		return 0;
	}

	avl_remove = NULL;	// will be set to the removed object

	tree->topnode = AVLRemoveRecursive( tree, tree->topnode, key );
	return avl_remove; 
}

/*
  ==============================
  U_AVLTreeSearch

  ==============================
*/
void * U_AVLTreeSearch( u_avl_tree_t *tree, void *key )
{
	u_avln_t	*n;
	int		cmp;
	const void		*other_key;

	n = tree->topnode;
	for(;;)
	{
		if ( !n )
			break;

		other_key = tree->get_key_func( n->objptr );
		cmp = tree->key_compare_func( key, other_key );
		
		if ( cmp == 0 )
		{
			return n->objptr;
		}
		else if ( cmp < 0 )
		{
			n = n->child[0];
		}
		else if ( cmp > 0 )
		{
			n = n->child[1];
		}
	}
	return NULL;
}


/*
  ==============================
  U_AVLTreeForEach

  ==============================
*/
void U_AVLTreeForEach( u_avl_tree_t *tree, void (*action_func)(void *obj) )
{
	if ( !tree->topnode )
		return;

	AVLForEachRecursive( tree->topnode, action_func );
}

/*
  ==============================
  U_AVLTreeGetTopKey

  ==============================
*/
const void * U_AVLTreeGetTopKey( u_avl_tree_t *tree )
{	
	if ( !tree->topnode )
		return NULL;

	return tree->get_key_func( tree->topnode->objptr );
}

/*
  ==============================
  U_CleanUpAVLTree

  ==============================
*/
static void (*avl_cleanup_func)(void *obj) = NULL;

static void CleanUpAVLTreeRecursive( u_avln_t *node )
{
	if ( node->child[0] )
	{
		CleanUpAVLTreeRecursive( node->child[0] );
	}
	if ( node->child[1] )
	{
		CleanUpAVLTreeRecursive( node->child[1] );
	}

	if ( avl_cleanup_func )
	{
		if ( node->objptr )
			avl_cleanup_func( node->objptr );	
	}
	U_FreeAVLN( node );	
}

void U_CleanUpAVLTree( u_avl_tree_t *tree, void (*cleanup_func)(void *obj) )
{
	avl_cleanup_func = cleanup_func;
	if ( !tree->topnode )
		return;
	CleanUpAVLTreeRecursive( tree->topnode );
}

/*
  =============================================================================
  map container

  =============================================================================
*/


/*
  ==============================
  U_InitMap

  ==============================
*/
void U_InitMap( u_map_t *map, mapAlgorithm type, int (*key_compare_func)(const void *, const void *), const void * (*get_key_func)(const void *) )
{
	INITTYPE( map, u_map_t );

	if ( type == map_default )
	{
		type = map_avl_tree;
	}

	if ( type == map_linear )
	{
		map->type = map_linear;
		map->a.a_linear.list = U_NewList();

		map->key_compare_func = key_compare_func;
		map->get_key_func = get_key_func;
	}
	else if ( type == map_avl_tree )
	{
		map->type = map_avl_tree;

		map->a.a_avl_tree.tree = NEWTYPE( u_avl_tree_t );
		U_InitAVLTree( map->a.a_avl_tree.tree, key_compare_func, get_key_func );

		map->key_compare_func = key_compare_func;
		map->get_key_func = get_key_func;
	}
	else
	{
		__error( "U_NewMap: map algorithm not supported yet\n" );
	}
}

/*
  ==============================
  U_CleanUpMap

  ==============================
*/
void U_CleanUpMap( u_map_t *map, void (*cleanup_func)(void *obj) )
{
	if ( map->type == map_avl_tree )
	{
		U_CleanUpAVLTree( map->a.a_avl_tree.tree, cleanup_func );
		FREE( map->a.a_avl_tree.tree );
	}
	else
	{
		__error( "shame, not yet\n" );
	}	
}


/*
  ==============================
  U_MapInsert

  ==============================
*/
static int MapInsert_linear( u_map_t *map, void *obj )
{
	u_list_iter_t	iter;
	const void	*key;
	void	*other_obj;
	const void	*other_key;

	key = map->get_key_func( obj );

	U_ListIterInit( &iter, map->a.a_linear.list );
	for ( ; ( other_obj = U_ListIterNext( &iter ) ) ; )
	{
		other_key = map->get_key_func( other_obj );
		if ( !(map->key_compare_func( key, other_key ) ) )
		{
			return 0;	// already in map
		}		
	}

	U_ListInsertAtHead( map->a.a_linear.list, obj );
	
	return 1;
}

int U_MapInsert( u_map_t *map, void *obj )
{
	if ( map->type == map_linear )
	{
		return MapInsert_linear( map, obj );
	}
	else if ( map->type == map_avl_tree )
	{
		return U_AVLTreeInsert( map->a.a_avl_tree.tree, obj );
	}
	
	return 0;
}


/*
  ==============================
  U_MapSearch

  ==============================
*/

static void * MapSearch_linear( u_map_t *map, void *key )
{
	u_list_iter_t	iter;
	void		*obj;
	const void		*obj_key;

	U_ListIterInit( &iter, map->a.a_linear.list );
	for ( ; ( obj = U_ListIterNext( &iter ) ) ; )
	{
		obj_key = map->get_key_func( obj );
		if ( !(map->key_compare_func( obj_key, key )) )
		{
			// key found
			return obj;
		}
	}
	return NULL;
}

void * U_MapSearch( u_map_t* map, const void* key )
{
    // SIM: changed external intrface to const void *, but actual implementation still casts await the const!
    
    
	if ( map->type == map_linear )
	{
		return MapSearch_linear( map, (void *)key );
	}
	else if ( map->type == map_avl_tree )
	{
		return U_AVLTreeSearch( map->a.a_avl_tree.tree, (void *)key );
	}

	return NULL;
}


/*
  ==============================
  U_MapRemove

  ==============================
*/

static void * MapRemove_linear( u_map_t *map, void *key )
{
	u_list_iter_t	iter;
	void		*obj;
	const void		*obj_key;

	U_ListIterInit( &iter, map->a.a_linear.list );
	for ( ; ( obj = U_ListIterNext( &iter ) ) ; )
	{
		obj_key = map->get_key_func( obj );
		if ( !(map->key_compare_func( obj_key, key )) )
		{
			// key found
			U_ListIterRemoveGoNext( &iter );
			return obj;
		}
	}
	return NULL;	
}

void * U_MapRemove( u_map_t* map, const void* key )
{
    // SIM: changed external interface to const void *, but actual implementation still casts await the const!
	if ( map->type == map_linear )
	{
		return MapRemove_linear( map, (void *)key );
	}
	else if ( map->type == map_avl_tree )
	{
		return U_AVLTreeRemove( map->a.a_avl_tree.tree, (void *)key );
	}
	
	return NULL;
}


/*
  ==============================
  U_MapForEach

  ==============================
*/
void U_MapForEach( u_map_t *map, void (*action_func)(void *obj) )
{
	if ( map->type == map_linear )
	{
		U_ListForEach( map->a.a_linear.list, action_func );
	}
	else if ( map->type == map_avl_tree )
	{
		U_AVLTreeForEach( map->a.a_avl_tree.tree, action_func );
	}
}


/*
  ==============================
  U_MapGetKeyList

  ==============================
*/
u_list_t * U_MapGetKeyList( u_map_t *map, void * (*key_clone_func)(void *key ) )
{
	if ( map->type == map_avl_tree )
	{
		Error( "shame, not yet\n" );
	}
	else
	{
		Error( "shame, not yet\n" );
	}

	return NULL;
}

/*
  ==============================
  U_MapSize

  ==============================
*/
static size_t size_count = 0;

static void CountCallback( void *obj )
{
	size_count++;
}

int U_MapSize( u_map_t *map )
{
	size_count = 0;
	U_MapForEach( map, CountCallback );
	return size_count;
}
