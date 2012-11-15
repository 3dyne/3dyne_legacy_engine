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



// l_head.h

#ifndef __l_head
#define __l_head

#include "l_defs.h"
#include "lib_math.h"

#define OFS_FOR_BASE( _type, _member ) ( (maddr_t)(&(((_type *)0)->_member)) )
#define ADDR_OF_BASE( _type, _member, _member_addr ) ( (_type *) ((maddr_t)(_member_addr) - (maddr_t)(&(((_type *)0)->_member)) ) )

//
// create ati
//

// map objects
ati_t * ATI_CreateSpectator( hobj_t *player_obj );
ati_t * ATI_CreateItem( hobj_t *item_obj );
ati_t * ATI_CreatePsys( hobj_t *psys_obj );
ati_t * ATI_CreateSwitch( hobj_t *switch_obj );
ati_t * ATI_CreateLight( hobj_t *light_obj );
ati_t * ATI_CreateCatapult( hobj_t *catapult_obj );
ati_t * ATI_CreateTeleport( hobj_t *teleport_obj );

// tmp objects
ati_t * ATI_CreatePlayer( hobj_t *player_obj );
ati_t * ATI_CreateRocket( hobj_t *rocket_obj );


void L_SpawnTmpObject( hobj_t *obj, ati_t * (*create_func)(hobj_t *obj) );

//
// ati lists
//
void L_InitAtiList( ati_list_t *list );
bool_t L_AtiListIsNotEmpty( ati_list_t *list );
void L_AtiListInsertAtHead( ati_list_t *list, ati_t *at );
ati_t * L_AtiListRemoveAtHead( ati_list_t *list );
ati_t * L_LinkAti( ati_t *prev, ati_t *next, ati_t *insert );
ati_t * L_UnlinkAti( ati_t *remove );

void L_AtiChangeToWakeUp( ati_t *at );
void L_AtiChangeToRun( ati_t *at );
void L_AtiChangeToSleep( ati_t *at );
void L_AtiChangeToDead( ati_t *at );

// ati maps
int L_AtiCompareKeys( const void *key1, const void *key2 );
const void * L_AtiGetKey( const void *obj );
void L_AtiRegisterId( ati_t *at );
ati_t * L_AtiSearchById( unique_t id );
void L_AtiUnregisterId( ati_t *at );

// move volume
void L_InitMoveVolume( move_volume_t *vol );
void L_MoveVolumeSetFromPos( move_volume_t *vol, vec3d_t pos );
void L_MoveVolumeGetFromPos( move_volume_t *vol, vec3d_t pos );
void L_MoveVolumeSetToPos( move_volume_t *vol, vec3d_t pos );
void L_MoveVolumeGetToPos( move_volume_t *vol, vec3d_t pos );
void L_MoveVolumeSetRelativeBoundBox( move_volume_t *vol, vec3d_t rel_min, vec3d_t rel_max );
void L_MoveVolumeGetRelativeBoundBox( move_volume_t *vol, vec3d_t rel_min, vec3d_t rel_max );
void L_MoveVolumeGetFromBoundBox( move_volume_t *vol, vec3d_t abs_min, vec3d_t abs_max );
void L_MoveVolumeGetToBoundBox( move_volume_t *vol, vec3d_t abs_min, vec3d_t abs_max );
void L_MoveVolumeGetBoundBox( move_volume_t *vol, vec3d_t abs_min, vec3d_t abs_max );

void L_MoveVolumeFollow( move_volume_t *vol );
void L_MoveVolumeStay( move_volume_t *vol );

// server object collide map
void L_CollideMapReset( void );
void L_CollideMapAddAtiWithBoundBox( ati_t *ati, vec3d_t min, vec3d_t max );
void L_CollideMapAddAtiWithRelBoundBox( ati_t *atif, vec3d_t origin, vec3d_t rel_min, vec3d_t rel_max );
void L_CollideMapSelectAllAtiInBoundBox( ati_t *self, void (*select_func)(ati_t *self, ati_t *other ) );

void L_CollideMapSelectAllAtiInSpecialRelBoundBox( ati_t *self, vec3d_t origin, vec3d_t rel_min, vec3d_t rel_max, void (*select_func)(ati_t *self, ati_t *other ) );

bool_t L_ClipLineByBoundBox( vec3d_t p1, vec3d_t p2, vec3d_t min, vec3d_t max, vec3d_t clip_norm, fp_t *clip_dist );
fp_t L_CollideMapClipLine( ati_t *ati, vec3d_t from, vec3d_t dir, fp_t len, bool_t (*check_func)( ati_t *self, ati_t *other) );

void L_PlaceKillBox( ati_t *ati, vec3d_t origin, vec3d_t rel_min, vec3d_t rel_max, fp_t destruction );

// switchable object map
void L_SwitchableMapReset( void );
void L_SwitchableMapAddAtiWithSwitchId( ati_t *switchable, unique_t id_switch );
void L_SwitchableMapSelectAtiWhereSwitch( ati_t *_switch, void (*select_func)(ati_t *_switch, ati_t *switchable ) );

//
// physic
//
planeSlope L_GetPlaneSlope( vec3d_t norm );
planeSlope L_TryMoveVolume( move_volume_t *vol );
void L_TryMoveVolumeToGround( move_volume_t *vol );
bool_t L_TryStraightMove( vec3d_t from, vec3d_t to );
void L_ClipMoveVolume( move_volume_t *vol );

// should be used for bound box objects
fp_t L_TraceClipBoundBoxMove( vec3d_t rel_min, vec3d_t rel_max, vec3d_t from, vec3d_t to, vec3d_t clip_pos, vec3d_t clip_norm );

// should be used for rockets and bullets 
fp_t L_TraceClipLineMove( vec3d_t from, vec3d_t dir, fp_t len );

void L_ClipVelocity( vec3d_t out, vec3d_t in, vec3d_t norm );
bool_t L_VelocityMove( vec3d_t rel_min, vec3d_t rel_max, vec3d_t origin, vec3d_t velocity, fp_t time );
#endif
