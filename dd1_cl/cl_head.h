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



// cl_head.h

#ifndef __cl_head
#define __cl_head

#include "cl_defs.h"
#include "lib_math.h"
#include "u_defs.h"

#include "events.h"

#define OFS_FOR_BASE( _type, _member ) ( (maddr_t)(&(((_type *)0)->_member)) )
#define ADDR_OF_BASE( _type, _member, _member_addr ) ( (_type *) ((maddr_t)(_member_addr) - (maddr_t)(&(((_type *)0)->_member)) ) )

#define randomf() ( (rand()/(RAND_MAX*0.5))-1.0 )

//
// client objects created by server events
//


// server objects
cl_ati_t * CL_CreatePlayer( unique_t id, byte_iter_t *bi );
cl_ati_t * CL_CreateView( unique_t id, byte_iter_t *bi );
cl_ati_t * CL_CreateItem( unique_t id, byte_iter_t *bi );
cl_ati_t * CL_CreatePsys( unique_t id, byte_iter_t *bi );
cl_ati_t * CL_CreateRocket( unique_t id, byte_iter_t *bi );

// tmp objects
cl_ati_t * CL_CreateExplosion( vec3d_t origin, explosionStyle style );
cl_ati_t * CL_CreateDebris( vec3d_t origin, vec3d_t velocity, debrisStyle style );

//void Util_

// client objects created by CL_WorldSpring archetype classes
cl_ati_t * CL_CreateSwitch( hobj_t *switch_obj );
cl_ati_t * CL_CreateLight( hobj_t *light_obj );

md_t * CL_CreateModelFromResourceName( const char *name );
void CL_ModelSetMoveByName( md_t *md, const char *name );

int Util_AtiCompareKeys( const void *key1, const void *key2 );
const void * Util_AtiGetKey( const void *obj );

void Util_AtiRegisterId( cl_ati_t *at );
cl_ati_t * Util_AtiSearchById( unique_t id );
void Util_AtiUnregisterRegisterId( cl_ati_t *ati );

// switchable object map
void CL_SwitchableMapReset( void );
void CL_SwitchableMapAddAtiWithSwitchId( cl_ati_t *switchable, unique_t id_switch );
void CL_SwitchableMapSelectAtiWhereSwitch( cl_ati_t *_switch, void (*select_func)(cl_ati_t *_switch, cl_ati_t *switchable ) );


// shape select
void CL_SelectShapesInBoundBox( cl_ati_t *selector, vec3d_t min, vec3d_t max, void (*select_func)(cl_ati_t *selector, unique_t id_shape ) );


//
// ***** cl_proxy.c *****
//

void CL_ProxyKillBoxMapReset( void );
void CL_ProxyKillBoxMapAdd( byte_iter_t *bi );
bool_t CL_ProxyKillBoxMapTestBB( vec3d_t min, vec3d_t max, vec3d_t kill_center );
bool_t CL_ProxyKillBoxMapTestPoint( vec3d_t point, vec3d_t kill_center );

//
// ***** cl_gfx.c *****
//
void CL_CreateGfxFromEvent( byte_iter_t *bi );

#endif
