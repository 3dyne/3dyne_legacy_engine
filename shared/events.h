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



// events.h

#ifndef __events
#define __events

#include "lib_math.h"
#include "lib_modeldefs.h"
#include "g_resourcesdefs.h"
#include "lib_psysdefs.h"

// client input event types

typedef enum
{
	eventType_none = 0,

/*
  --------------------------------------------------
  start of server events
  --------------------------------------------------
*/
	eventType_server_first,

	// server update events
	eventType_server_update_player,

	eventType_server_last,

/*
  --------------------------------------------------
  start of client events
  --------------------------------------------------
*/
  	eventType_client_first,

	// server frame to client time info
	eventType_client_info_first,
	eventType_client_info_time,		// msec_t begin, end
	eventType_client_info_last,

	// server to client proxy actions
	eventType_client_proxy_first,
	eventType_client_proxy_killbox,		// {vec3d_t origin,rel_min,rel_max}
	eventType_client_proxy_last,

	// server to client gfx actions	
	eventType_client_gfx_first,
	eventType_client_gfx_create,	// {enum gfxType, ... }
	eventType_client_gfx_last,

	// client object info events

	eventType_client_typeinfo_first,

	eventType_client_typeinfo_player,	// obsolet
	eventType_client_typeinfo_view,		// player, spectator, sky
	eventType_client_typeinfo_item,		// shows map item
	eventType_client_typeinfo_psys,		// shows map psys
	eventType_client_typeinfo_rocket,	// a flying rocket

	eventType_client_typeinfo_last,

	// client object update events
	eventType_client_obj_update_first,

	eventType_client_obj_update_ipo,	// {vec3d_t ipo1, ipo2 }, for whole server frame
	eventType_client_obj_update_pos,	// {vec3d_t}
	eventType_client_obj_update_style,	// {enum}
	eventType_client_obj_update_state,	// {int}
	eventType_client_obj_update_angles,	// {fp_t lon,lat,yaw}
	eventType_client_obj_update_move,	// {enum}
	eventType_client_obj_update_nickname,	// {char[32]}, player view only
	eventType_client_obj_update_hud,	// {int health, int numfrag}, player view only

	eventType_client_obj_update_last,

	// client object destroy events

	eventType_client_obj_destroy_first,

	eventType_client_obj_destroy,

	eventType_client_obj_destroy_last,

	//
	// end of client events
	//
	eventType_client_last,


/*
  --------------------------------------------------
  start of render events
  --------------------------------------------------
*/
	eventType_render_first,
	eventType_render_info_ptr,
	eventType_render_last,

	eventType_num
} eventType;

typedef enum
{
	itemStyle_none = 0,

	itemStyle_rocket,
	itemStyle_rlaunch,

	itemStyle_num
} itemStyle;

typedef enum
{
	psysStyle_none = 0,
	psysStyle_fire1,
	psysStyle_fire2,
	psysStyle_fire3,
	psysStyle_fire4,

	psysStyle_num
} psysStyle;

typedef enum
{
	rocketStyle_none,
	rocketStyle_fly,
	rocketStyle_impact,

	rocketStyel_num
} rocketStyle;

typedef enum
{
	playerMove_none = 0,
	playerMove_stan = 1,
	playerMove_walk = 2,
	playerMove_jump = 4,
	playerMove_attack = 8,
	playerMove_pain = 16,
	playerMove_dead = 32,
	playerMove_tribaldead = 64,

	playerMove_num
} playerMove;

typedef enum
{
	gfxStyle_none,
	gfxStyle_teleport_send,
	gfxStyle_teleport_recv,

	gfxStyle_num
} gfxStyle;

#endif
