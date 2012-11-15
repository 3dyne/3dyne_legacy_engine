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



// l_util.c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "l_head.h"
#include "sv_state.h"

#include "lib_container.h"
#include "shock.h"


/*
  ==============================
  L_SpawnTmpObject

  ==============================
*/
void L_SpawnTmpObject( hobj_t *obj, ati_t * (*create_func)(hobj_t *obj) )
{
	ati_t	*atif;

//	__named_message( "create tmp object [%s,%s]\n", obj->type, obj->name );

	atif = create_func( obj );
	L_AtiChangeToWakeUp( atif );
}

/*
  ==================================================
  archetype list/state/change stuff

  ==================================================
*/
void L_InitAtiList( ati_list_t *list )
{
	list->head.prev = &list->head;
	list->tail.next = &list->tail;
	list->head.next = &list->tail;
	list->tail.prev = &list->head;
}

bool_t L_AtiListIsNotEmpty( ati_list_t *list )
{
	if ( list->head.next == &list->tail )
		return false;

	return true;
}

void L_AtiListInsertAtHead( ati_list_t *list, ati_t *at )
{
	L_LinkAti( &list->head, list->head.next, at );
}

ati_t * L_AtiListRemoveAtHead( ati_list_t *list )
{
	if ( list->head.next == &list->tail )
		return NULL;
	
	return L_UnlinkAti( list->head.next );
}

ati_t * L_LinkAti( ati_t *prev, ati_t *next, ati_t *insert )
{
	prev->next = insert;
	insert->prev = prev;
	next->prev = insert;
	insert->next = next;

	return insert;
}

ati_t * L_UnlinkAti( ati_t *remove )
{
	remove->prev->next = remove->next;
	remove->next->prev = remove->prev;

	remove->next = NULL;
	remove->prev = NULL;

	return remove;
}

void L_AtiChangeToWakeUp( ati_t *at )
{
	if ( at->prev && at->next )
	{
		L_UnlinkAti( at );
	}

	at->state = atiState_wakeup;
	L_AtiListInsertAtHead( &sv_wakeup, at );
}

void L_AtiChangeToRun( ati_t *at )
{
	// only a wakeup state can change to run state
	
	if ( at->state != atiState_wakeup )
	{
		__warning( "ignore state change to 'run', current state is not 'wakeup'\n" );
		return;
	}

	if ( at->prev && at->next )
	{
		L_UnlinkAti( at );
	}

	at->state = atiState_run;
	L_AtiListInsertAtHead( &sv_run, at );
}

void L_AtiChangeToSleep( ati_t *at )
{
	if ( at->prev && at->next )
	{
		L_UnlinkAti( at );
	}
	
	at->state = atiState_sleep;
	L_AtiListInsertAtHead( &sv_sleep, at );
}

void L_AtiChangeToDead( ati_t *at )
{
	if ( at->prev && at->next )
	{
		L_UnlinkAti( at );
	}
	
	at->state = atiState_dead;
	L_AtiListInsertAtHead( &sv_dead, at );
}

/*
  ==================================================
  ati id map

  ==================================================
*/
int L_AtiCompareKeys( const void *key1, const void *key2 )
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

const void * L_AtiGetKey( const void *obj )
{
	ati_t	*at;

	at = (ati_t *)(obj);

	return (void *)(&at->id);
}

void L_AtiRegisterId( ati_t *at )
{
	ati_t	*search;
	
	search = (ati_t *) U_MapSearch( &sv_id_to_ati_map, (void *) &at->id );
	
	if ( search )
	{
		__warning( "there is already an ati with id '#%u', ignore register\n", at->id );
	}
	else
	{
		U_MapInsert( &sv_id_to_ati_map, (void *) at );
	}
}

void L_AtiUnregisterId( ati_t *at )
{
	const void	*key;
	void		*obj;

	key = L_AtiGetKey( at );

	obj = U_MapRemove( &sv_id_to_ati_map, key );
	if ( !obj )
	{
		__error( "server object [%s,%s] is not registered\n", at->ati_obj->type, at->ati_obj->name );
	}       
}


ati_t * L_AtiSearchById( unique_t id )
{
	ati_t	*search;

	search = (ati_t *) U_MapSearch( &sv_id_to_ati_map, (void *) &id );

	if ( !search )
	{
//		__warning( "can't find an ati with id '#%u'\n", id );
	}
	
	return search;
}

/*
  ==================================================
  move volume

  ==================================================
*/

/*
  ==============================
  L_InitMoveVolume

  ==============================
*/
void L_InitMoveVolume( move_volume_t *vol )
{
	memset( vol, 0, sizeof( move_volume_t ) );
}

/*
  ==============================
  L_MoveVolumeSetFromPos

  ==============================
*/
void L_MoveVolumeSetFromPos( move_volume_t *vol, vec3d_t pos )
{
	Vec3dCopy( vol->from, pos );
}

/*
  ==============================
  L_MoveVolumeGetFromPos

  ==============================
*/
void L_MoveVolumeGetFromPos( move_volume_t *vol, vec3d_t pos )
{
	Vec3dCopy( pos, vol->from );
}


/*
  ==============================
  L_MoveVolumeSetToPos

  ==============================
*/
void L_MoveVolumeSetToPos( move_volume_t *vol, vec3d_t pos )
{
	Vec3dCopy( vol->to, pos );
}

/*
  ==============================
  L_MoveVolumeGetToPos

  ==============================
*/
void L_MoveVolumeGetToPos( move_volume_t *vol, vec3d_t pos )
{
	Vec3dCopy( pos, vol->to );
}

/*
  ==============================
  L_MoveVolumeSetRelativeBoundBox

  ==============================
*/
void L_MoveVolumeSetRelativeBoundBox( move_volume_t *vol, vec3d_t rel_min, vec3d_t rel_max )
{
	Vec3dCopy( vol->rel_min, rel_min );
	Vec3dCopy( vol->rel_max, rel_max );
}

/*
  ==============================
  L_MoveVolumeGetRelativeBoundBox

  ==============================
*/
void L_MoveVolumeGetRelativeBoundBox( move_volume_t *vol, vec3d_t rel_min, vec3d_t rel_max )
{
	Vec3dCopy( rel_min, vol->rel_min );
	Vec3dCopy( rel_max, vol->rel_max );
}

/*
  ==============================
  L_MoveVolumeGetFromBoundBox

  ==============================
*/
void L_MoveVolumeGetFromBoundBox( move_volume_t *vol, vec3d_t abs_min, vec3d_t abs_max )
{
	Vec3dAdd( abs_min, vol->from, vol->rel_min );
	Vec3dAdd( abs_max, vol->from, vol->rel_max );
}

/*
  ==============================
  L_MoveVolumeGetToBoundBox

  ==============================
*/
void L_MoveVolumeGetToBoundBox( move_volume_t *vol, vec3d_t abs_min, vec3d_t abs_max )
{
	Vec3dAdd( abs_min, vol->to, vol->rel_min );
	Vec3dAdd( abs_max, vol->to, vol->rel_max );
}

/*
  ==============================
  L_MoveVolumeGetBoundBox
  
  ==============================
*/
void L_MoveVolumeGetBoundBox( move_volume_t *vol, vec3d_t abs_min, vec3d_t abs_max )
{
	vec3d_t		min, max;

	//
	// get bound box of whole move
	//

	L_MoveVolumeGetToBoundBox( vol, min, max ); 
	L_MoveVolumeGetFromBoundBox( vol, abs_min, abs_max );

	Vec3dAddToBB( abs_min, abs_max, min ); 
	Vec3dAddToBB( abs_min, abs_max, max );
}

/*
  ==============================
  L_MoveVolumeFollow

  ==============================
*/
void L_MoveVolumeFollow( move_volume_t *vol )
{
	Vec3dCopy( vol->from, vol->to );
}

/*
  ==============================
  L_MoveVolumeStay

  ==============================
*/
void L_MoveVolumeStay( move_volume_t *vol )
{
	Vec3dCopy( vol->to, vol->from );
}


/*
  ==================================================
  server object collide map

  ==================================================
*/

#define L_COLLIDEMAP_MAX_OBJECTS	( 1024 )

static int	l_num_collide_obj;
static ati_t   *l_collide_obj_tbl[L_COLLIDEMAP_MAX_OBJECTS];
static vec3d_t	l_collide_min_tbl[L_COLLIDEMAP_MAX_OBJECTS];
static vec3d_t	l_collide_max_tbl[L_COLLIDEMAP_MAX_OBJECTS];

/*
  ==============================
  L_CollideMapReset

  ==============================
*/
void L_CollideMapReset( void )
{
	l_num_collide_obj = 0;
}

/*
  ==============================
  L_CollideMapAddAtiWithBoundBox

  ==============================
*/
void L_CollideMapAddAtiWithBoundBox( ati_t *ati, vec3d_t min, vec3d_t max )
{
	if ( l_num_collide_obj >= L_COLLIDEMAP_MAX_OBJECTS )
	{
		__error( "reached L_COLLIDEMAP_MAX_OBJECTS\n" );
	}

	l_collide_obj_tbl[l_num_collide_obj] = ati;
	Vec3dCopy( l_collide_min_tbl[l_num_collide_obj], min );
	Vec3dCopy( l_collide_max_tbl[l_num_collide_obj], max );

	l_num_collide_obj++;
}

/*
  ==============================
  L_CollideMapAddAtiWithRelBoundBox

  ==============================
*/
void L_CollideMapAddAtiWithRelBoundBox( ati_t *ati, vec3d_t origin, vec3d_t rel_min, vec3d_t rel_max )
{
	vec3d_t		min;
	vec3d_t		max;

	Vec3dAdd( min, origin, rel_min );
	Vec3dAdd( max, origin, rel_max );

	L_CollideMapAddAtiWithBoundBox( ati, min, max );
}

/*
  ==============================
  L_CollideMapSelectAllAtiInBoundBox

  ==============================
*/
void L_CollideMapSelectAllAtiInBoundBox( ati_t *self, void (*select_func)(ati_t *self, ati_t *other ) )
{
	int		i;
	int		self_i;

	__chkptr( select_func );

	for ( i = 0; i < l_num_collide_obj; i++ )
	{
		if ( l_collide_obj_tbl[i] == self )
		{
			break;
		}
	}

	if ( i == l_num_collide_obj )
	{
		__warning( "object is not in collide map\n" );
		return;
	}

	self_i = i;

	for ( i = 0; i < l_num_collide_obj; i++ )
	{
		if ( i == self_i )
			continue;

		if ( Vec3dBBDoIntersect( l_collide_min_tbl[self_i], l_collide_max_tbl[self_i],
					 l_collide_min_tbl[i], l_collide_max_tbl[i] ) )
		{
			select_func( self, l_collide_obj_tbl[i] );
		}
	}
}


/*
  ==============================
  L_CollideMapSelectAllAtiInSpecialBoundBox

  ==============================
*/
void L_CollideMapSelectAllAtiInSpecialRelBoundBox( ati_t *self, vec3d_t origin, vec3d_t rel_min, vec3d_t rel_max, void (*select_func)(ati_t *self, ati_t *other ) )
{
	int		i;

	vec3d_t		min;
	vec3d_t		max;

	__chkptr( select_func );

	Vec3dAdd( min, origin, rel_min );
	Vec3dAdd( max, origin, rel_max );

	for ( i = 0; i < l_num_collide_obj; i++ )
	{
		if ( Vec3dBBDoIntersect( min, max,
					 l_collide_min_tbl[i], l_collide_max_tbl[i] ) )
		{
			select_func( self, l_collide_obj_tbl[i] );
		}
	}	
}

void L_PlaceKillBox( ati_t *self, vec3d_t origin, vec3d_t rel_min, vec3d_t rel_max, fp_t destruction )
{
	int		i;

	vec3d_t		min;
	vec3d_t		max;

	Vec3dAdd( min, origin, rel_min );
	Vec3dAdd( max, origin, rel_max );

	for ( i = 0; i < l_num_collide_obj; i++ )
	{
		if ( Vec3dBBDoIntersect( min, max,
					 l_collide_min_tbl[i], l_collide_max_tbl[i] ) )
		{
			if ( l_collide_obj_tbl[i]->ftbl->Hurt )
				l_collide_obj_tbl[i]->ftbl->Hurt( l_collide_obj_tbl[i], self, origin, destruction );
		}
	}	
}


bool_t L_ClipLineByBoundBox( vec3d_t from, vec3d_t to, vec3d_t min, vec3d_t max, vec3d_t clip_norm, fp_t *clip_dist )
{
	vec3d_t		norms[6] = { { 1, 0, 0 },
				     { 0, 1, 0 },
				     { 0, 0, 1 },
				     { -1, 0, 0 },
				     { 0, -1, 0 },
				     { 0, 0, -1 } };
	fp_t		dists[6];

	int	i;
	int	best_plane;

	vec3d_t		p1, p2;
	fp_t		scale;

	Vec3dCopy( p1, from );
	Vec3dCopy( p2, to );

	dists[0] = max[0];
	dists[1] = max[1];
	dists[2] = max[2];
	dists[3] = -min[0];
	dists[4] = -min[1];
	dists[5] = -min[2];

	best_plane = -1;

	for ( i = 0; i < 6; i++ )
	{
		fp_t	d1, d2;

		d1 = Vec3dDotProduct( p1, norms[i] ) - dists[i];
		d2 = Vec3dDotProduct( p2, norms[i] ) - dists[i];
	
		if ( d1 > 0 && d2 > 0 )
		{
			break;
		}

		if ( d1 > 0 && d2 < 0 )
		{
			// ray enter brush
			
			scale = d1/(d1-d2);
			
			p1[0] = p1[0]+scale*(p2[0]-p1[0]);
			p1[1] = p1[1]+scale*(p2[1]-p1[1]);
			p1[2] = p1[2]+scale*(p2[2]-p1[2]);

			best_plane = i;
		}
		
		if ( d1 < 0 && d2 > 0 )
		{
				// ray leave brush
			scale = d1/(d1-d2);
			
			p2[0] = p1[0]+scale*(p2[0]-p1[0]);
			p2[1] = p1[1]+scale*(p2[1]-p1[1]);
			p2[2] = p1[2]+scale*(p2[2]-p1[2]);				
		}
	}

	if ( i != 6 || best_plane == -1 )
	{
		return false;
	}

	Vec3dCopy( clip_norm, norms[best_plane] );
	*clip_dist = dists[best_plane];
	
	return true;	
}

fp_t L_CollideMapClipLine( ati_t *ati, vec3d_t from, vec3d_t dir, fp_t len, bool_t (*check_func)( ati_t *self, ati_t *other) )
{
	int		i;
	vec3d_t		to;
	vec3d_t		min, max;
	fp_t		best_scale;

	__chkptr( check_func );

	Vec3dMA( to, len, dir, from );

	Vec3dCopy( min, from );
	Vec3dCopy( max, from );
	Vec3dAddToBB( min, max, to );
       
	best_scale = 1.0;	// minimum search

	for ( i = 0; i < l_num_collide_obj; i++ )
	{
		if ( Vec3dBBDoIntersect( min, max,
					 l_collide_min_tbl[i], l_collide_max_tbl[i] ) )
		{
			vec3d_t		clip_norm;
			fp_t		clip_dist;

			if ( l_collide_obj_tbl[i] == ati )
			{
				continue;
			}
			
			if ( !check_func( ati, l_collide_obj_tbl[i] ) )
			{
				continue;
			}

			if ( L_ClipLineByBoundBox( from, to, l_collide_min_tbl[i], l_collide_max_tbl[i], clip_norm, &clip_dist ) )
			{
				fp_t		d1, d2, scale;

				d1 = Vec3dDotProduct( from, clip_norm ) - clip_dist;
				d2 = Vec3dDotProduct( to, clip_norm ) - clip_dist;
				
				scale = d1/(d1-d2);

				if ( scale < best_scale )
				{
					best_scale = scale;
				}
			}
		}
	}

	return best_scale;
}

/*
  ==================================================
  server object switchable map

  ==================================================
*/

#define L_SWITCHABLEMAP_MAX_OBJECTS	( 1024 )

static int	l_num_switchable_obj;
static ati_t   *l_switchable_obj_tbl[L_SWITCHABLEMAP_MAX_OBJECTS];
static unique_t l_switch_id_tbl[L_SWITCHABLEMAP_MAX_OBJECTS];

/*
  ==============================
  L_SwitchableMapReset

  ==============================
*/
void L_SwitchableMapReset( void )
{
	l_num_switchable_obj = 0;
}

/*
  ==============================
  L_SwitchableMapAddAtiWithSwitchId

  ==============================
*/
void L_SwitchableMapAddAtiWithSwitchId( ati_t *switchable, unique_t id_switch )
{
	__chkptr( switchable );

	__named_message( "switchable '#%u' linked to switch '#%u'\n", switchable->id, id_switch );

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
  L_SwitchableMapSelectAtiWhereSwitch

  ==============================
*/
void L_SwitchableMapSelectAtiWhereSwitch( ati_t *_switch, void (*select_func)(ati_t *_switch, ati_t *switchable ) )
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
