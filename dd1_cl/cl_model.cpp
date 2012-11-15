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



// l_model.c

#include "lib_hobj.h"
#include "lib_math.h"
#include "lib_container.h"
#include "lib_model.h"

#include "g_resourcesdefs.h"                                                    
#include "res_submddefs.h"                                                      
#include "res_movesdefs.h"

#include "cl_state.h"
#include "cl_head.h"

#include "shock.h"

/*
  ==============================
  CL_CreateModelFromResourceName

  ==============================
*/
md_t * CL_CreateModelFromResourceName( const char *name )
{
	res_moves_cache_t	*md_cache;
	md_t			*md;

	hobj_search_iterator_t	iter;

	hobj_t			*coordsys;	
	u_list_iter_t		list1_iter;
	u_list_iter_t		list2_iter;
	md_coordsys_t		*cs1, *cs2;
	hpair_t			*pair1, *pair2;

	hobj_t			*submodel;
	md_submodel_t		*sm;	

	char			tt[256];

	md = NEWTYPE( md_t );

	U_InitList( &md->coordsys_list );
	U_InitList( &md->submodel_list );

	sprintf( tt, "moves.%s", name );
	md->md_res = g_api.GI_ResourceSearch( tt );
	if ( !md->md_res )
		__error( "can't find resource '%s'\n", tt );

	md_cache = ( res_moves_cache_t * ) md->md_res->res_cache;

	//
	// create all coordsys from class and insert into coordsys_list
	//

	InitClassSearchIterator( &iter, md_cache->coordsys_cls, "coordsys" );
	for ( ; ( coordsys = SearchGetNextClass( &iter ) ) ; )
	{
		md_coordsys_t	*cs;

		cs = NEWTYPE( md_coordsys_t );
		cs->coordsys_obj = coordsys;

		U_ListInsertAtHead( &md->coordsys_list, cs );
	}

	//
	// create all submodels from class and insert into submodel_list
	//

	InitClassSearchIterator( &iter, md_cache->submodel_cls, "submodel" );
	for ( ; ( submodel = SearchGetNextClass( &iter ) ) ; )
	{
		sm = NEWTYPE( md_submodel_t );
		sm->submodel_obj = submodel;

		U_ListInsertAtHead( &md->submodel_list, sm );
	}

	//
	// resolve coordsys 'parent' links
	//

	U_ListIterInit( &list1_iter, &md->coordsys_list );
	for ( ; ( cs1 = (md_coordsys_t*)U_ListIterNext( &list1_iter ) ) ; )
	{

		pair1 = FindHPair( cs1->coordsys_obj, "name" );
		if ( !pair1 )
			__error( "missing key 'name' in coordsys\n" );

		if ( !strcmp( pair1->value, "root" ) )
		{
			if ( md->cs_root )
				__error( "more than one root coordsys\n" );
			md->cs_root = cs1;

			// 'root' doesn't search parent
			continue;
		}

		pair1 = FindHPair( cs1->coordsys_obj, "parent" );
		if ( !pair1 )
			__error( "missing key 'parent' in coordsys\n" );
		
		// search parent for cs
		U_ListIterInit( &list2_iter, &md->coordsys_list );
		for ( ; ( cs2 = (md_coordsys_t*)U_ListIterNext( &list2_iter ) ) ; )
		{
			pair2 = FindHPair( cs2->coordsys_obj, "name" );
			if ( !pair2 )
				__error( "missing key 'name' in coordsys\n" );
			
			if ( !strcmp( pair1->value, pair2->value ) )
			{
				if ( cs1 == cs2 )
					__error( "coordsys is its own parent\n" );

				// insert cs1 as child of cs2
				MD_CoordsysInsertCoordsys( cs2, cs1 );
			}			
		}
	}

	//
	// reslove submodels 'coordsys' link
	//

	U_ListIterInit( &list1_iter, &md->submodel_list );
	for ( ; ( sm = (md_submodel_t*)U_ListIterNext( &list1_iter ) ) ; )
	{

		// first init submodel
		EasyFindVec3d( sm->origin, sm->submodel_obj, "origin" );
		EasyFindVec3d( sm->scale, sm->submodel_obj, "scale" );
		EasyFindFloat( &sm->lat, sm->submodel_obj, "latitude" );
		EasyFindFloat( &sm->lon, sm->submodel_obj, "longitude" );
		EasyFindFloat( &sm->yaw, sm->submodel_obj, "yaw" );

		Vec3dScale( sm->scale, 64.0, sm->scale );
		Vec3dScale( sm->origin, 64.0, sm->origin );

		pair1 = FindHPair( sm->submodel_obj, "res_submd" );
		if ( !pair1 )
			__error( "missing key 'res_submd' in submodel\n" );
		sm->res_submd = g_api.GI_ResourceSearch( pair1->value );
		if ( !sm->res_submd )
			__error( "can't find resource '%s'\n", pair1->value );
		
		pair1 = FindHPair( sm->submodel_obj, "res_gltex" );
		if ( !pair1 )
			__error( "missing key 'res_gltex' in submodel\n" );
		sm->res_gltex = g_api.GI_ResourceSearch( pair1->value );
		if ( !sm->res_gltex )
			__error( "can't find resource '%s'\n", pair1->value );
		
		pair1 = FindHPair( sm->submodel_obj, "coordsys_name" );
		if ( !pair1 )
			__error( "missing key 'coordsys' in submodel\n" );

		// search coordsys
		U_ListIterInit( &list2_iter, &md->coordsys_list );
		for ( ; ( cs1 = (md_coordsys_t*)U_ListIterNext( &list2_iter ) ) ; )
		{
			pair2 = FindHPair( cs1->coordsys_obj, "name" );
			if ( !pair2 )
				__error( "missing key 'name' in coordsys\n" );

			if ( !strcmp( pair1->value, pair2->value ) )
			{
				MD_CoordsysInsertSubmodel( cs1, sm );
			}

		}
	}

	return md;
}

/*
  ==============================
  CL_ModelSetMoveByName

  ==============================
*/

void CL_ModelSetMoveByName( md_t *md, const char *name )
{
	u_list_iter_t		iter;
	res_moves_cache_t	*md_cache;

	md_move_t		tmp;
	md_coordsys_t		*cs;
	hpair_t			*pair;
	md_move_t		*move;

	md_cache = ( res_moves_cache_t * ) md->md_res->res_cache;

	U_ListIterInit( &iter, &md->coordsys_list );
	for ( ; ( cs = (md_coordsys_t*)U_ListIterNext( &iter ) ) ; )
	{
		pair = FindHPair( cs->coordsys_obj, "name" );
		__chkptr( pair );

		// build potential move name
		sprintf( tmp.name, "%s.%s", name, pair->value );

		// search move name
		move = (md_move_t*)U_MapSearch( &md_cache->moves_map, &tmp );
		if ( !move )
			continue;

		__named_message( "found move: %s\n", tmp.name );

		cs->current_move = move;
	}
}
