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



// cl_util.c

#include "cl_state.h"
#include "cl_head.h"

#include "lib_hobj.h"
#include "lib_container.h"

#include "shock.h"

int Util_AtiCompareKeys( const void *key1, const void *key2 )
{
	unique_t	id1, id2;

	id1 = *(unique_t *)(key1);
	id2 = *(unique_t *)(key2);

	if ( id1 > id2 )
		return 1;
	else if ( id1 < id2 )
		return -1;
	return 0;
}

const void * Util_AtiGetKey( const void *obj )
{
	cl_ati_t	*at;

	at = (cl_ati_t *)(obj);

	return (void *)(&at->id);
}

void Util_AtiRegisterId( cl_ati_t *ati )
{
	cl_ati_t	*search;
	
	search = (cl_ati_t *) U_MapSearch( &cl_id_to_ati_map, (void *) &ati->id );
	
	if ( search )
	{
		__warning( "there is already an ati with id '#%u', ignore register\n", ati->id );
	}
	else
	{
		U_MapInsert( &cl_id_to_ati_map, (void *) ati );
	}
}

cl_ati_t * Util_AtiSearchById( unique_t id )
{
	cl_ati_t	*search;

	search = (cl_ati_t *) U_MapSearch( &cl_id_to_ati_map, (void *) &id );

	if ( !search )
	{
//		__warning( "can't find an ati with id '#%u'\n", id );
	}
	
	return search;
}

void Util_AtiUnregisterRegisterId( cl_ati_t *ati )
{
	const void	*key;
	void		*obj;

	key = Util_AtiGetKey( ati );

	obj = U_MapRemove( &cl_id_to_ati_map, key );
	if ( !obj )
	{
		__error( "client object '#%u' is not registered\n", ati->id );
	}
}

/*
  ==================================================
  client object switchable map

  ==================================================
*/

#define L_SWITCHABLEMAP_MAX_OBJECTS	( 1024 )

static int	 l_num_switchable_obj;
static cl_ati_t *l_switchable_obj_tbl[L_SWITCHABLEMAP_MAX_OBJECTS];
static unique_t  l_switch_id_tbl[L_SWITCHABLEMAP_MAX_OBJECTS];

/*
  ==============================
  CL_SwitchableMapReset

  ==============================
*/
void CL_SwitchableMapReset( void )
{
	l_num_switchable_obj = 0;
}

/*
  ==============================
  CL_SwitchableMapAddAtiWithSwitchId

  ==============================
*/
void CL_SwitchableMapAddAtiWithSwitchId( cl_ati_t *switchable, unique_t id_switch )
{
	__chkptr( switchable );

//	__named_message( "switchable '#%u' linked to switch '#%u'\n", switchable->id, id_switch );

	if ( l_num_switchable_obj >= L_SWITCHABLEMAP_MAX_OBJECTS )
	{
		__error( "reached L_SWITCHABLEMAP_MAX_OBJECTS\n" );
	}

	l_switchable_obj_tbl[l_num_switchable_obj] = switchable;
	l_switch_id_tbl[l_num_switchable_obj] = id_switch;
	
	l_num_switchable_obj++;
}

/*
  ==============================
  CL_SwitchableMapSelectAtiWhereSwitch

  ==============================
*/
void CL_SwitchableMapSelectAtiWhereSwitch( cl_ati_t *_switch, void (*select_func)(cl_ati_t *_switch, cl_ati_t *switchable ) )
{
	int		i;

	__chkptr( select_func );

	for ( i = 0; i < l_num_switchable_obj; i++ )
	{
		if ( l_switch_id_tbl[i] == _switch->id )
		{
			select_func( _switch, l_switchable_obj_tbl[i] );
		}
	}
}



/*
  ==================================================
  shape select

  ==================================================
*/

/*
  ==============================
  CL_SelectShapesInBoundBox

  ==============================
*/
static cl_ati_t *l_shape_selector;
static void (*l_shape_select_func)(cl_ati_t *selector, unique_t id_shape ) = NULL;

static void ShapeSelect_func( unique_t id_shape )
{
	l_shape_select_func( l_shape_selector, id_shape );
}

void CL_SelectShapesInBoundBox( cl_ati_t *selector, vec3d_t min, vec3d_t max, void (*select_func)(cl_ati_t *selector, unique_t id_shape ) )
{
	__chkptr( select_func );
	__chkptr( selector );

	l_shape_selector = selector;
	l_shape_select_func = select_func;

	g_api.Map_SelectShapeIDsInBoundBox( min, max, ShapeSelect_func );
}
