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



#include <stdio.h>
#include <stdlib.h>

#include "s_config.h"
#include "s_mem.h"

#include "lib_hobj.h"
#include "lib_hmanager.h"

#include "shock.h"

/*
  ==================================================
  hmanager stuff

  ==================================================
*/
hmanager_t * NewHManager( void )
{
	hmanager_t *hm;

	hm = (hmanager_t *) malloc( sizeof( hmanager_t ) );
	memset( hm, 0, sizeof( hmanager_t ) );

	return hm;
}

hmanager_t * NewHManagerFromClass( hobj_t *root )
{
	hmanager_t *hm;

	hm = (hmanager_t *) malloc( sizeof( hmanager_t ) );
	memset( hm, 0, sizeof( hmanager_t ) );	

	hm->root = root;

	if ( !hm->root )
	{
		__warning( "null class.\n" );
		return NULL;
	}

	HManagerRebuildHash( hm );
	return hm;
}

hmanager_t * NewFlatHManagerFromClass( hobj_t *root )
{
	hmanager_t *hm;

	hm = (hmanager_t *) malloc( sizeof( hmanager_t ) );
	memset( hm, 0, sizeof( hmanager_t ) );	

	hm->root = root;

	if ( !hm->root )
	{
		__warning( "null class.\n" );
		return NULL;
	}

	HManagerRebuildHash( hm );
	return hm;
}

void FreeHManager( hmanager_t *hm )
{
	free( hm );
}

void DumpHManager( hmanager_t *hm, bool_t verbose )
{
	hmanager_listnode_t	*n;
	int		i, num;
	int		min_entries, max_entries;

	printf( "DumpHManager:\n" );

	min_entries = 1<<30;
	max_entries = -min_entries;
	for ( i = 0; i < HMANAGER_HASH_SIZE; i++ )
	{
		for ( n = hm->hash[i], num = 0; n ; n=n->next, num++ )
		{ }
		if ( verbose )
			printf( " hash[%d]: %d entries\n", i, num );
		if ( num < min_entries )
			min_entries = num;
		if ( num > max_entries )
			max_entries = num;
	}
	printf( "hash entries min: %d, max: %d\n", min_entries, max_entries );
}

/*
  ====================
  HManagerGetRootClass

  ====================
*/
hobj_t * HManagerGetRootClass( hmanager_t *hm )
{
	return hm->root;
}

/*
  ====================
  HManagerSetRootClass

  ====================
*/
void HManagerSetRootClass( hmanager_t *hm, hobj_t *obj )
{
	hm->root = obj;
}

/*
  ====================
  HManagerCalcHashkey

  ====================
*/
int HManagerCalcHashkey( char *name )
{
	int	key;

	for ( key = 0; *name ; name++ )
	{
		key <<= 2;
		key += (int)(*name);		
	}

//	key *= key;
//	key >>= 4;
	
	return abs( key % HMANAGER_HASH_SIZE );
}

/*
  ====================
  HManagerRebuildHash

  ====================
*/
static void HManagerRebuildHashRecursive( hmanager_t *hm, hobj_t *obj )
{
	hobj_t		*o;
//	int		id;

	HManagerHashClass( hm, obj );

	for ( o = obj->hobjs; o ; o=o->next )
		HManagerRebuildHashRecursive( hm, o );
}

void HManagerRebuildHash( hmanager_t *hm )
{
	int		i;
	for ( i = 0; i < HMANAGER_HASH_SIZE; i++ )
		hm->hash[i] = NULL;

	HManagerRebuildHashRecursive( hm, hm->root );
} 

void HManagerRebuildHashFlat( hmanager_t *hm )
{
	int		i;
	hobj_t		*o;

	for ( i = 0; i < HMANAGER_HASH_SIZE; i++ )
		hm->hash[i] = NULL;

	for ( o = hm->root->hobjs; o ; o=o->next )
	{
		HManagerHashClass( hm, o );
	}
}

/*
  ====================
  HManagerHashClass

  ====================
*/
void HManagerHashClass( hmanager_t *hm, hobj_t *obj )
{
	int	key;
	hmanager_listnode_t	*node;

	key = HManagerCalcHashkey( obj->name );
//	printf( "key: %d\n", key );
	node = (hmanager_listnode_t *) malloc( sizeof( hmanager_listnode_t ) );

	node->obj = obj;
	node->next = hm->hash[key];
	hm->hash[key] = node;
}

/*
  ====================
  HManagerUnhashClass

  ====================
*/
void HManagerUnhashClass( hmanager_t *hm, hobj_t *obj )
{
	int		key;
	hmanager_listnode_t	*n, *next, *head;

	key = HManagerCalcHashkey( obj->name );

	head = NULL;
	for ( n = hm->hash[key]; n ; n=next )
	{
		next = n->next;
		if ( n->obj == obj )
		{
			free( n );
			continue;
		}
		n->next = head;
		head = n;
	}
	hm->hash[key] = head;
}

/*
  ====================
  HManagerSearchClassName

  ====================
*/
hobj_t * HManagerSearchClassName( hmanager_t *hm, char *name )
{
	int		key;
	hmanager_listnode_t	*n;

	key = HManagerCalcHashkey( name );
	
	for ( n = hm->hash[key]; n ; n=n->next )
	{		
		if ( !n->obj )
			__error( "null\n" );

		if ( !strcmp( n->obj->name, name ) )
		{
			return n->obj;
		}
	}
	return NULL;
}

/*
  ====================
  HManagerIndexClassOfType

  ====================
*/
void HManagerIndexClassesOfType( hmanager_t *hm, int *start, const char *type )
{
	int		i;
	hmanager_listnode_t	*n;
	hpair_t		*pair;
	char		tt[256];

	for ( i = 0; i < HMANAGER_HASH_SIZE; i++ )
	{
		for ( n = hm->hash[i]; n ; n=n->next )
		{
			if ( !n->obj )
				continue;

			if ( !strcmp( n->obj->type, type ) )
			{
				sprintf( tt, "%d", (*start) );
				pair = NewHPair2( "int", "index", tt );
				InsertHPair( n->obj, pair );
				(*start)++;
			}
		}
	}
}

/*
  ====================
  HManagerSearchClassName_linear

  ====================
*/
hobj_t * HManagerSearchClassNameRecursive( hobj_t *obj, char *name )
{
	hobj_t	*o;
	hobj_t	*found;

	if ( !strcmp( name, obj->name ) )
		return obj;

	for ( o = obj->hobjs; o ; o=o->next )
	{
		found = HManagerSearchClassNameRecursive( o, name );
		if ( found )
			return found;
	}

	return NULL;
}

hobj_t * HManagerSearchClassName_linear( hmanager_t *hm, char * name )
{
	return HManagerSearchClassNameRecursive( hm->root, name );
}

/*
  ====================
  HManagerInsertClass

  ====================
*/
void HManagerInsertClass( hmanager_t *hm, hobj_t *parent, hobj_t *obj )
{
	InsertClass( parent, obj );
	HManagerHashClass( hm, obj );
}

/*
  ====================
  HManagerRemoveClass

  ====================
*/
void HManagerRemoveClass( hmanager_t *hm, hobj_t *obj )
{
	HManagerUnhashClass( hm, obj );
	RemoveClass2( obj );
}

/*
  ====================
  HManagerDeepDestroyClass

  ====================
*/
static void DestroyClassRecursive( hmanager_t *hm, hobj_t *obj )
{
	hobj_t		*o, *next;
	hpair_t		*p, *pnext;

	for ( o = obj->hobjs; o ; o=next )
	{
		next = o->next;
		DestroyClassRecursive( hm, o );		
	}
	for ( p = obj->pairs; p ; p=pnext )
	{
		pnext = p->next;
		FreeHPair( p );
	}

	HManagerUnhashClass( hm, obj );
	FreeClass( obj );
}

void HManagerDeepDestroyClass( hmanager_t *hm, hobj_t *obj )
{
	DestroyClassRecursive( hm, obj );
}

/*
  ====================
  RemoveAllClassTypes

  ====================
*/
void HManagerRemoveAndDestroyAllClassesOfType( hmanager_t *hm, hobj_t *obj, char *type )
{
	hobj_t		*o, *next, *list;
	
	list = NULL;
	for ( o = obj->hobjs; o ; o=next )
	{
		next = o->next;
		if ( !strcmp( type, o->type ) )
		{
			HManagerDeepDestroyClass( hm, o );
			continue;
		}
		o->next = list;
		list = o;
	}
	obj->hobjs = list;
}


/*
  ====================
  HManagerCheckClassConsistancy

  ====================
*/
bool_t HManagerCheckClassConsistancy( hmanager_t *hm )
{
	int		i;
	hmanager_listnode_t	*n1, *n2;
	bool_t		success;

	printf( "Check class consistancy ...\n" );

	//
	// check hash tabel
	//
	success = true;
	for ( i = 0; i < HMANAGER_HASH_SIZE; i++ )
	{
		for ( n1 = hm->hash[i]; n1 ; n1=n1->next )
		{
			for ( n2 = hm->hash[i]; n2 ; n2=n2->next )
			{
				if ( n1 == n2 )
					continue;
				if ( !strcmp( n1->obj->name, n2->obj->name ) )
				{
					printf( " failed: name \"%s\" found in class types \"%s\" and \"%s\"\n", 
						n1->obj->name, n1->obj->type, n2->obj->type );
					success = false;
				}
			}
		}
	}
	return success;
}

/*
  ====================
  HManagerInitTypeSearchIterator

  ====================
*/
void HManagerInitTypeSearchIterator( hmanager_type_iterator_t *iter, hmanager_t *hm, const char *type )
{
	int		i;

	iter->hm = hm;

	for ( i = 0; i < HMANAGER_HASH_SIZE; i++ )
	{
		if ( hm->hash[i] )
			break;
	}
	
	if ( i == HMANAGER_HASH_SIZE )
	{
		iter->current = NULL;
	}
	else
	{
		iter->current = hm->hash[i];
	}

	iter->hash = i;

	strcpy( iter->type, type );
}

/*
  ====================
  HManagerGetNextClass

  ====================
*/
hobj_t * HManagerGetNextClass( hmanager_type_iterator_t *iter )
{
	hmanager_listnode_t		*cur;

	cur = NULL;

	if ( iter->hash == HMANAGER_HASH_SIZE )
		return NULL;

again:
	for ( ; iter->current; iter->current=iter->current->next )
	{
		if ( !iter->current->obj )
			continue;

		if ( !strcmp( iter->current->obj->type, iter->type ) )
		{
			cur = iter->current;
			break;
		}
	}
	
	if ( !iter->current )
	{
		iter->hash++;
		if ( iter->hash == HMANAGER_HASH_SIZE )
			return NULL;
		iter->current = iter->hm->hash[iter->hash];
		goto again;
	}

	iter->current = iter->current->next;

	return cur->obj;
}

