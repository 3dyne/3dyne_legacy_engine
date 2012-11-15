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



// lib_containerdefs.h

#ifndef __lib_containerdefs
#define __lib_containerdefs


/* double linked list node */
typedef struct u_dln_s
{
	void	*objptr;
	struct u_dln_s	*prev;
	struct u_dln_s	*next;
} u_dln_t;

typedef struct u_list_s
{
	int		free_self;	// true = dynamic, false = static
	u_dln_t		*head;
	u_dln_t		*tail;
} u_list_t;

typedef enum
{
	iter_at_head,
	iter_at_tail,
	iter_at_item
} listIterState;

typedef struct u_list_base_iter_s
{
	u_list_t	*list;
	u_dln_t		*current;
} u_list_iter_t;



/* ring container */

typedef struct u_ring_s
{
	u_dln_t		*current;
} u_ring_t;



/* stack container */

typedef struct u_stack_s
{
	int		fixsize;	// 0 is grow_stack, > 0 is fix stack_size
	int		fixtop;
	u_list_t	*grow_stack;
	void		*fix_stack[16];	// variable size
} u_stack_t;


/* avl-tree container */
typedef struct u_avln_s
{
	void	*objptr;
	int	balance;

	struct u_avln_s	*child[2];
} u_avln_t;

typedef struct u_avl_tree_s
{
	u_avln_t	*topnode;
	
	int (*key_compare_func)(const void *k1, const void *k2);
	const void * (*get_key_func)(const void *obj );
} u_avl_tree_t;

/* map container */
typedef enum
{
	map_default,
	map_linear,
	map_hash,
	map_avl_tree
} mapAlgorithm;

typedef struct u_map_s
{
	mapAlgorithm	type;

	int (*key_compare_func)(const void *k1, const void *k2);
	const void * (*get_key_func)(const void *obj);
	
	union algo_u
	{
		struct a_linear_s
		{
			u_list_t	*list;
		} a_linear;
		struct a_avl_tree_s
		{
			u_avl_tree_t	*tree;
		} a_avl_tree;
	} a;
	
} u_map_t;

#endif
