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



// cl_atplayer.c

#include "cl_state.h"
#include "cl_head.h"

#include "s_mem.h"
#include "shock.h"

#include "events.h"

typedef struct cl_at_player_s
{
	cl_ati_t	atif;
	
	vec3d_t		origin_current;
	msec_t		time_current;

	vec3d_t		origin_next;
	msec_t		time_next;

	msec_t		time_rel;
	msec_t		duration;

	// event from server
	bool_t		ev_valid;
	msec_t		ev_time;
	vec3d_t		ev_origin;

	// render info
	ri_view_t	ri_view;
	ri_sprite_t	ri_sprite;
} cl_at_player_t;


/*
  ==============================
  UnpackEventPlayer

  ==============================
*/
static void UnpackEventPlayer( cl_ati_t *atif, unique_t id, eventType type, byte_iter_t *bi )
{
	cl_at_player_t	*player;

//	__named_message( "\n" );

	player = ADDR_OF_BASE( cl_at_player_t, atif, atif );	

	if ( player->ev_valid )
	{
		__warning( "override event of '#%u'\n", atif->id );
	}
	
	if ( type == eventType_client_obj_update_ipo )
	{
		player->ev_time = ByteIter_Unpacki( bi );
		ByteIter_Unpack3fv( bi, player->ev_origin );
	}
	else
	{
		__warning( "unknown event for client object '#%u'\n", id );
	}

	player->ev_valid = true;

}

/*
  ==============================
  RunPlayer

  ==============================
*/
static void RunPlayer( cl_ati_t *atif )
{
	cl_at_player_t	*player;

//	__named_message( "\n" );

	player = ADDR_OF_BASE( cl_at_player_t, atif, atif );	

	if ( player->ev_valid )
	{
		player->ev_valid = false;
		
		player->time_rel = 0;

		player->time_current = player->time_next;
		Vec3dCopy( player->origin_current, player->origin_next );

		player->time_next = player->ev_time;
		Vec3dCopy( player->origin_next, player->ev_origin );

		player->duration = player->time_next - player->time_current;
	}

//	printf( "time_rel %d, time_dur %d\n", player->time_rel, player->duration );
//	Vec3dPrint( player->origin_current );
//	Vec3dPrint( player->origin_next );

	if ( player->time_rel <= player->duration )
	{
		fp_t		scale;
		vec3d_t		delta;
		vec3d_t		origin;

		scale = (1.0*player->time_rel)/(1.0*player->duration);

		Vec3dSub( delta, player->origin_next, player->origin_current );
		Vec3dMA( origin, scale, delta, player->origin_current );


		// render info view
		player->ri_view.id = atif->id;
		player->ri_view.style = viewStyle_player;
		Vec3dCopy( player->ri_view.origin, origin );
		
		g_api.Render_View( &player->ri_view );

		// render info sprite
		Vec3dCopy( player->ri_sprite.origin, origin );
		player->ri_sprite.rgba[0] = 255;
		player->ri_sprite.rgba[1] = 255;
		player->ri_sprite.rgba[2] = 255;
		player->ri_sprite.rgba[3] = 255;
		player->ri_sprite.width = 0.5;
		player->ri_sprite.height = 0.5;
		player->ri_sprite.rotate = 0;
		player->ri_sprite.gltex_res = g_api.GI_ResourceSearch( "gltex.fx.cross_red" );
	
		g_api.Render_Sprite( &player->ri_sprite );
	}

	player->time_rel += cl_time_delta;
}

/*
  ==============================
  DestroyPlayer

  ==============================
*/
static void DestroyPlayer( cl_ati_t *atif )
{
	cl_at_player_t	*player;

	player = ADDR_OF_BASE( cl_at_player_t, atif, atif );	
	FREE( player );
}

/*
  ==================================================
  client player

  ==================================================
*/
static bool_t		ftbl_valid = false;
static cl_ati_ftbl_t	ftbl;

cl_ati_t * CL_CreatePlayer( unique_t id, byte_iter_t *bi )
{
	cl_at_player_t		*player;

	if ( ftbl_valid == false )
	{
		ftbl_valid = true;
		INITTYPE( &ftbl, cl_ati_ftbl_t );

		ftbl.Destroy = DestroyPlayer;
		ftbl.UnpackEvent = UnpackEventPlayer;
		ftbl.Run = RunPlayer;
	}

	player = NEWTYPE( cl_at_player_t );
	player->atif.ftbl = &ftbl;
	player->atif.id = id;

	// init private data
	
	// player needs nothing special from byte_iter event ...

	return &player->atif;
}
