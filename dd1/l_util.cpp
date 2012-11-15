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

#include "l_locals.h"

bool_t PositionEqual( vec3d_t p1, vec3d_t p2 )
{
	int		i;

	for ( i = 0; i < 3; i++ )
	{
		if ( fabs( p1[i] - p2[i] ) > 0.1 )
			return false;
	}
	return true;
}

/*
  ==================================================
  archetype list/state/change stuff

  ==================================================
*/
void *L_FindArcheT( char *type, char *name )
{
//	return NULL;
	ati_t		*ati;

	// search run list
	for ( ati = l_run.head.next ; ati != &l_run.tail ; ati=ati->next )
	{
		if ( !strcmp( ati->name, name ) )
			return ati;
	}

	// search sleep list
	for ( ati = l_sleep.head.next ; ati != &l_sleep.tail ; ati=ati->next )
	{
		if ( !strcmp( ati->name, name ) )
			return ati;
	}

	return NULL;
}
	
void L_InitATList( at_list_t *list )
{
	list->head.prev = &list->head;
	list->tail.next = &list->tail;
	list->head.next = &list->tail;
	list->tail.prev = &list->head;
}

bool_t L_ATListIsNotEmpty( at_list_t *list )
{
	if ( list->head.next == &list->tail )
		return false;

	return true;
}

void L_ATListInsertAtHead( at_list_t *list, ati_t *at )
{
	L_LinkAT( &list->head, list->head.next, at );
}

ati_t * L_ATListRemoveAtHead( at_list_t *list )
{
	if ( list->head.next == &list->tail )
		return NULL;
	
	return L_UnlinkAT( list->head.next );
}

ati_t * L_LinkAT( ati_t *prev, ati_t *next, ati_t *insert )
{
	prev->next = insert;
	insert->prev = prev;
	next->prev = insert;
	insert->next = next;

	return insert;
}

ati_t * L_UnlinkAT( ati_t *remove )
{
	remove->prev->next = remove->next;
	remove->next->prev = remove->prev;

	remove->next = NULL;
	remove->prev = NULL;

	return remove;
}

void L_ChangeToWakeUp( ati_t *at )
{
	if ( at->prev && at->next )
	{
		L_UnlinkAT( at );
	}

	at->state = ATIState_wakeup;
	L_ATListInsertAtHead( &l_wakeup, at );
}

void L_ChangeToRun( ati_t *at )
{
	// only a wakeup state can change to run state
	
	if ( at->state != ATIState_wakeup )
	{
		__warning( "ignore state change to 'run', current state is not 'wakeup'\n" );
		return;
	}

	if ( at->prev && at->next )
	{
		L_UnlinkAT( at );
	}

	at->state = ATIState_run;
	L_ATListInsertAtHead( &l_run, at );
}

void L_ChangeToSleep( ati_t *at )
{
	if ( at->prev && at->next )
	{
		L_UnlinkAT( at );
	}
	
	at->state = ATIState_sleep;
	L_ATListInsertAtHead( &l_sleep, at );
}

void L_ChangeToDead( ati_t *at )
{
	if ( at->prev && at->next )
	{
		L_UnlinkAT( at );
	}
	
	at->state = ATIState_dead;
	L_ATListInsertAtHead( &l_dead, at );
}

/*
  ==================================================
  move volume stuff

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
  move volume collide map

  ==================================================
*/

#define L_COLLIDEMAP_MAX_OBJECTS	( 1024 )

static int		l_collidemap_object_num;
static ati_t		*l_collidemap_owners[L_COLLIDEMAP_MAX_OBJECTS];
static move_volume_t	*l_collidemap_moves[L_COLLIDEMAP_MAX_OBJECTS];

/*
  ==============================
  L_BeginCollideMap

  ==============================
*/
void L_BeginCollideMap( void )
{
	l_collidemap_object_num = 0;	
}

/*
  ==============================
  L_EndCollideMap

  ==============================
*/
void L_EndCollideMap( void )
{
//	printf( " %d objects in collidemap\n", l_collidemap_object_num );
}

/*
  ==============================
  L_CollideMapAddMoveVolume

  ==============================
*/
void L_CollideMapAddMoveVolume( ati_t *owner, move_volume_t *vol )
{
	if ( l_collidemap_object_num >= L_COLLIDEMAP_MAX_OBJECTS )
	{
		__error( "reached L_COLLIDEMAP_MAX_OBJECTS\n" );
	}

	l_collidemap_owners[l_collidemap_object_num] = owner;
	l_collidemap_moves[l_collidemap_object_num] = vol;

	l_collidemap_object_num++;
}

/*
  ==============================
  L_CollideMapForEachInSpace

  ==============================
*/
void L_CollideMapForEachInSpace( ati_t *owner, move_volume_t *self, void (*action_func())(ati_t *caller, ati_t *collide_with) )
{
	int		i;

	vec3d_t		min, max;
	vec3d_t		min1, max1;
	vec3d_t		min2, max2;
	
	move_volume_t	*other_move;
	ati_t		*other_owner;

	L_MoveVolumeGetFromBoundBox( self, min1, max1 );
	L_MoveVolumeGetToBoundBox( self, min, max );
	Vec3dAddToBB( min1, max1, min );
	Vec3dAddToBB( min1, max1, max ); 

	for ( i = 0; i < l_collidemap_object_num; i++ )
	{
		other_owner = l_collidemap_owners[i];
		other_move = l_collidemap_moves[i];

		if ( other_move == self )
			continue;

		L_MoveVolumeGetFromBoundBox( other_move, min2, max2 );
		L_MoveVolumeGetToBoundBox( other_move, min, max );
		Vec3dAddToBB( min2, max2, min );
		Vec3dAddToBB( min2, max2, max ); 
	
		if ( Vec3dBBDoIntersect( min1, max1, min2, max2 ) )
		{
			action_func( owner, other_owner );	
		}
	}
}

/*
  ==================================================
  switchmap/switchable stuff

  ==================================================
*/

#define L_SWITCHMAP_MAX_OBJECTS		( 1024 )

static unique_t		l_switchmap_switchid[L_SWITCHMAP_MAX_OBJECTS];
static ati_t		*l_switchmap_owners[L_SWITCHMAP_MAX_OBJECTS];

/*
  ==============================
  L_InitSwitchMap

  ==============================
*/
void L_InitSwitchMap( void )
{
	int		i;

	for ( i = 0; i < L_SWITCHMAP_MAX_OBJECTS; i++ )
	{
		l_switchmap_owners[i] = NULL;
	}
}

/*
  ==============================
  L_SwitchMapInsertSwitchAble

  ==============================
*/
void L_SwitchMapInsertSwitchAble( ati_t *self, unique_t switch_id )
{
	int		i;

	for ( i = 0; i < L_SWITCHMAP_MAX_OBJECTS; i++ )
	{
		if ( !l_switchmap_owners[i] )
		{
			l_switchmap_owners[i] = self;
			l_switchmap_switchid[i] = switch_id;
			return;
		}
	}

	__error( "no space left in switchmap\n" );
}

/*
  ==============================
  L_SwitchMapRemoveSwitchAble

  ==============================
*/
void  L_SwitchMapRemoveSwitchAble( ati_t *self )
{
	int		i;

	for ( i = 0; i < L_SWITCHMAP_MAX_OBJECTS; i++ )
	{
		if ( l_switchmap_owners[i] == self )
		{
			l_switchmap_owners[i] = NULL;
			return;
		}
	}

	__warning( "archetype not found in switchmap\n" );
}

/*
  ==============================
  L_SwitchMapSelectSwitchAbleWhere_switchid

  ==============================
*/
void L_SwitchMapSelectSwitchAbleWhere_switchid( ati_t *self, unique_t switch_id, void (*action_func())(ati_t *caller, ati_t *switch_able ) )
{
	int		i;

	for ( i = 0; i < L_SWITCHMAP_MAX_OBJECTS; i++ )
	{
		if ( l_switchmap_switchid[i] == switch_id && l_switchmap_owners[i] )
		{
			action_func( self, l_switchmap_owners[i] );
		}
	}
}

/*
  ==================================================
  archetype message communication stuff

  ==================================================
*/

char *L_ComIterGetNext( comiter_t *iter )
{
	char	*rest;
	int	i, i2;

	if( iter->wasnull )
		return NULL;

	for( i = iter->pos, i2 = 0; ( i < iter->size - 1 ) && ( i2 < 255 ); i++, i2++ )
	{
		if( isspace( iter->ptr[i] ))
			break;

		if( iter->ptr[i] == '\0' )
		{
			iter->wasnull = 1;
			break;
		}
		iter->curtoken[i2] = iter->ptr[i];
	}
	iter->pos = i + 1;
	iter->curtoken[i2] = '\0';
	return iter->curtoken;
}	
	
void L_InitComIter( char *ptr, comiter_t *iter, int size )
{
	iter->pos = 0;
	iter->ptr = ptr;
	iter->size = size;
	iter->wasnull = 0;
	iter->GetNext = L_ComIterGetNext;
}

