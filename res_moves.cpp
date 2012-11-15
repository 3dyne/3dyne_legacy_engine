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



// res_moves.c

#include "interfaces.h"
#include "g_shared.h"  

#include "r_glbackenddefs.h"

#include "res_moves.h"
#include "res_movesdefs.h"

#define FIX_UNITS ( 64.0 )

static int MoveComparePrimaryKeys( const void *key1, const void *key2 )
{
	return strcmp( ((md_move_t*)(key1))->name, ((md_move_t*)(key2))->name );
}

static const void * MoveGetPrimaryKey( const void *obj )
{
	return (void *)obj;
}

res_moves_cache_t * Res_CacheInMOVES( res_moves_register_t *reg )
{
	res_moves_cache_t	*cache;
	hobj_t			*root;
	hpair_t			*pair;
	
	tokenstream_t		*ts;       
	ib_file_t		*h;
	int			filesize;

	hobj_search_iterator_t	iter;
	hobj_t			*move;

	cache = NEWTYPE( res_moves_cache_t );
	U_InitMap( &cache->moves_map, map_default, MoveComparePrimaryKeys, MoveGetPrimaryKey );
	
	//
	// load moves binary
	//
	
	pair = FindHPair( reg->resobj, "path_bin" );
	if ( !pair )
		__error( "missing key 'path_bin' in 'moves' resource '%s'\n", reg->resobj->name );

	h = IB_Open( pair->value );
	if ( !h )
	{
		__error( "can't open moves binary\n" );
	}

	filesize = IB_GetSize( h );
	cache->moves_bin = NEWBYTES( filesize );
	__chkptr( cache->moves_bin );

	IB_Read( cache->moves_bin, filesize, 1, h );
	IB_Close( h );

	// 
	// load moves class
	//

	pair = FindHPair( reg->resobj, "path_cls" );
	if ( !pair )
		__error( "missing key 'path_cls' in 'moves' resource '%s'\n", reg->resobj->name );

	ts = BeginTokenStream( pair->value );
	__chkptr( ts );

	root = ReadClass( ts );
	__chkptr( root );
	EndTokenStream( ts );

	InitClassSearchIterator( &iter, root, "move" );
	for ( ; ( move = SearchGetNextClass( &iter ) ) ; )
	{
		md_move_t		*md_move;
		int			param_num;
		int			ofs;

		md_move = NEWTYPE( md_move_t );
		strcpy( md_move->name, move->name );

		EasyFindInt( &md_move->step_num, move, "step_num" );
		EasyFindInt( &param_num, move, "param_num" );
		if ( param_num != 6 )
			__error( "expected param_num = 6\n" );
		EasyFindInt( &ofs, move, "ofs" );
		
		md_move->msecs_per_step = 16;

		md_move->steps = (void*)&(((char*)(cache->moves_bin))[ofs]);

		if ( !U_MapInsert( &cache->moves_map, md_move ) )
			__error( "there is already a move with name '%s'\n", md_move->name );
	}

	//
	// load coordsys class
	//
	
	pair = FindHPair( reg->resobj, "path_cs_cls" );
	if ( !pair )
		__error( "missing key 'path_cs_cls' in 'moves' resource '%s'\n", reg->resobj->name );

	ts = BeginTokenStream( pair->value );
	__chkptr( ts );

	cache->coordsys_cls = ReadClass( ts );
	__chkptr( cache->coordsys_cls );
	EndTokenStream( ts );

	//
	// load submodel class
	// 

	pair = FindHPair( reg->resobj, "path_sm_cls" );
	if ( !pair )
		__error( "missing key 'path_sm_cls' in 'moves' resource '%s'\n", reg->resobj->name );

	ts = BeginTokenStream( pair->value );
	__chkptr( ts );

	cache->submodel_cls = ReadClass( ts );
	__chkptr( cache->submodel_cls );
	EndTokenStream( ts );

	return cache;
}

/*
  ==================================================
  internal resource interface

  ==================================================
*/

g_resource_t * Res_RegisterMOVES( hobj_t *resobj )
{
	res_moves_register_t	*reg;
	g_resource_t		*r;
	hpair_t			*name;

	name = FindHPair( resobj, "name" );
	if ( !name )
		__error( "missing key 'name'\n" );

	r = G_NewResource( name->value, resobj->name );

	reg = NEWTYPE( res_moves_register_t );
	reg->resobj = resobj;

	r->res_register = reg;
	r->state = G_RESOURCE_STATE_REGISTERED;
	
	return r;
}


void Res_UnregisterMOVES( g_resource_t *res )
{
	printf( "Res_UnregisterMOVES: do nothing\n" );       
}

void Res_CacheMOVES( g_resource_t *res )
{
	res_moves_register_t	*res_register;

	if ( res->state != G_RESOURCE_STATE_REGISTERED )
		return;

	res_register = (res_moves_register_t *) res->res_register;

	res->res_cache = Res_CacheInMOVES( res_register );
	res->state = G_RESOURCE_STATE_CACHED;
}
	
void Res_UncacheMOVES( g_resource_t *res )
{
	printf( "Res_UncacheMOVES: do nothing\n" );
}
