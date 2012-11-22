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



// cl_atview.c

#include "cl_state.h"
#include "cl_head.h"

#include "s_mem.h"
#include "shock.h"

#include "events.h"

#include "hud_player.h"

#ifdef win32_x86
#include <stdlib.h>
//static float randomf() {
//  return float(rand()) / RAND_MAX;
//}

#endif

typedef struct cl_at_view_s
{
	cl_ati_t	atif;
	
	msec_t		time_nickname;

	// event from server
	bool_t		u_style_mask;
	viewStyle	u_style;

	bool_t		u_angles_mask;
	fp_t		u_lon, u_lat, u_yaw;

	bool_t		u_pos_mask;
	vec3d_t		u_pos;

	bool_t		u_ipo_mask;
	vec3d_t		u_ipo1;
	vec3d_t		u_ipo2;

       	bool_t		u_move_mask;	// for player views
	playerMove	u_move;

	// spectator angle modulation
	fp_t		lon, lat, yaw;

	// ipo
	vec3d_t		ipo1;
	vec3d_t		ipo2;

	vec3d_t		origin;

	// render info      

	ri_view_t	ri_view;
	ri_sprite_t	ri_sprite;

	// player hud
	hud_player_t	hud;

} cl_at_view_t;


/*
  ==============================
  UnpackEventView

  ==============================
*/
static void UnpackEventView( cl_ati_t *atif, unique_t id, eventType type, byte_iter_t *bi )
{
	cl_at_view_t	*view;

//	__named_message( "\n" );

	view = ADDR_OF_BASE( cl_at_view_t, atif, atif );	
	
	if ( type == eventType_client_obj_update_ipo )
	{
		view->u_ipo_mask = true;
		ByteIter_Unpack3fv( bi, view->u_ipo1 );
		ByteIter_Unpack3fv( bi, view->u_ipo2 );
	}
	else if ( type == eventType_client_obj_update_style )
	{
		view->u_style_mask = true;
		view->u_style = (viewStyle)ByteIter_Unpacki( bi );
	}
	else if ( type == eventType_client_obj_update_angles )
	{
		view->u_angles_mask = true;
		view->u_lon = ByteIter_Unpackf( bi );
		view->u_lat = ByteIter_Unpackf( bi );
		view->u_yaw = ByteIter_Unpackf( bi );
	}
	else if ( type == eventType_client_obj_update_pos )
	{
		view->u_pos_mask = true;
		ByteIter_Unpack3fv( bi, view->u_pos );
	}
	else if ( type == eventType_client_obj_update_move )
	{
		view->u_move_mask = true;
		view->u_move = (playerMove)ByteIter_Unpacki( bi );
	}
	else if ( type == eventType_client_obj_update_nickname )
	{
		char	tmp[CL_SV_PLAYER_NICKNAME_SIZE];
		
		ByteIter_Unpacknbv( bi, tmp, CL_SV_PLAYER_NICKNAME_SIZE );
		
		if ( strcmp( tmp, view->hud.nickname ) )
		{
			strncpy( view->hud.nickname, tmp, CL_SV_PLAYER_NICKNAME_SIZE-1 );
			view->time_nickname = cl_time;
		}
	}
	else if ( type == eventType_client_obj_update_hud )
	{
		int	tmp;

		tmp = ByteIter_Unpacki( bi );
		if ( tmp != view->hud.health )
		{
			view->hud.health = tmp;
			view->hud.time_health = cl_time;
		}

		tmp = ByteIter_Unpacki( bi );
		if ( tmp != view->hud.numfrag )
		{
			view->hud.numfrag = tmp;
			view->hud.time_numfrag = cl_time;
		}

	}

	else
	{
		__warning( "unknown event for client object '#%u'\n", id );
	}
}

/*
  ==============================
  RunView

  ==============================
*/
static void RunView( cl_ati_t *atif )
{
	cl_at_view_t	*view;

//	__named_message( "\n" );

	view = ADDR_OF_BASE( cl_at_view_t, atif, atif );	

	if( view->u_style_mask )
	{
		view->u_style_mask = false;

		// if style is changed from none to player init the player hud
		if ( view->ri_view.style == viewStyle_none &&
		     view->u_style == viewStyle_player )
		{
			HUD_PlayerInit( &view->hud );
		}
		view->ri_view.style = view->u_style;
	}

	if ( view->u_pos_mask )
	{
		view->u_pos_mask = false;
//		Vec3dCopy( view->ri_view.origin, view->u_pos );
		Vec3dCopy( view->origin, view->u_pos );
	}

	if ( view->u_angles_mask )
	{
		view->u_angles_mask = false;
		view->lon = view->u_lon;
		view->lat = view->u_lat;
		view->yaw = view->u_yaw;
	}

	if ( view->u_ipo_mask )
	{
		view->u_ipo_mask = false;
		Vec3dCopy( view->ipo1, view->u_ipo1 );
		Vec3dCopy( view->ipo2, view->u_ipo2 );
	}

	if ( cl_time - view->time_nickname < 1000 )
	{
//		__named_message( "player nickname: '%s'\n", view->nickname );
	}

	//
	// if spectator modulate angles
	//
	if ( view->ri_view.style == viewStyle_spectator )
	{
		view->ri_view.lon = view->lon+sin(((cl_time_begin_frame%10000)/10000.0)*2*M_PI)*2.5;
		view->ri_view.lat = view->lat+sin(((cl_time_begin_frame%15000)/15000.0)*2*M_PI)*2.5;;
		view->ri_view.yaw = view->yaw;	
	}


	//
	// do ipo
	//
	if ( view->ri_view.style == viewStyle_player )
	{
		fp_t		scale;
		vec3d_t		delta;
		vec3d_t		origin;

//		Vec3dPrint( view->ipo1 );
//		Vec3dPrint( view->ipo2 );

		scale = (cl_sv_time)/(cl_sv_time_delta);

//		printf( "scale: %f\n", scale );

		Vec3dSub( delta, view->ipo2, view->ipo1 );
		Vec3dMA( origin, scale, delta, view->ipo1 );

//		Vec3dCopy( view->ri_view.origin, origin );
//		Vec3dCopy( view->ri_sprite.origin, origin );
		Vec3dCopy( view->origin, origin );
	}
	
	
	//
	// special player view
	//
	if ( view->ri_view.style == viewStyle_player )
	{

		vec3d_t		kill_center;

		if ( CL_ProxyKillBoxMapTestPoint( view->origin, kill_center ) )
		{
			vec3d_t		shock_dir;
			cl_ati_t	*debris;
			cl_ati_t	*expl;
			int		i;
			
			Vec3dSub( shock_dir, view->origin, kill_center );
			Vec3dUnify( shock_dir );

			for ( i = 0; i < 10; i++ )
			{
				vec3d_t		v;
				v[0] = 256.0*((rand()%1000)/1000.0+0.5)*shock_dir[0];
				v[1] = 256.0*((rand()%1000)/1000.0+0.5)*shock_dir[1];
				v[2] = 256.0*((rand()%1000)/1000.0+0.5)*shock_dir[2];

//				Vec3dScale( v, 256.0, shock_dir );

				debris = CL_CreateDebris( view->origin, v, debrisStyle_ham1 );
				U_ListInsertAtHead( &cl_ati_wakeup_list, debris );
			}

			if ( view->u_move & playerMove_tribaldead )
			{
				for ( i = 0; i < 4; i++ )
				{
					vec3d_t		v;
					v[0] = 64.0*((rand()%1000)/1000.0+0.5)*shock_dir[0];
					v[1] = 64.0*((rand()%1000)/1000.0+0.5)*shock_dir[1];
					v[2] = 64.0*((rand()%1000)/1000.0+0.5)*shock_dir[2];
					
					
					debris = CL_CreateDebris( view->origin, v, (debrisStyle)(debrisStyle_tribalsmiliedead1+i) );
					U_ListInsertAtHead( &cl_ati_wakeup_list, debris );				
				}				
			}
			
			for ( i = 0; i < 3; i++ )
			{
				vec3d_t	v;
				v[0] = view->origin[0] + randomf()*32.0;
				v[1] = view->origin[1] + randomf()*32.0;
				v[2] = view->origin[2] + randomf()*32.0;
//				Vec3dPrint( v );
				expl = CL_CreateExplosion( v, explosionStyle_blood1 );
				U_ListInsertAtHead( &cl_ati_wakeup_list, expl );
			}

			// if player is hurt during a jump draw some extra gfx
			if ( view->u_move & playerMove_jump )
			{
				expl = CL_CreateExplosion( view->origin, explosionStyle_stardust );
				U_ListInsertAtHead( &cl_ati_wakeup_list, expl );
			}					

//			printf( " ======================== in proxy killbox ============\n" );
		}
		
		
	
		{
			// render info sprite
			view->ri_sprite.rgba[0] = 255;
			view->ri_sprite.rgba[1] = 255;
			view->ri_sprite.rgba[2] = 255;
			view->ri_sprite.rgba[3] = 255;
			view->ri_sprite.width = 2.0;
			view->ri_sprite.height = 2.0*1.33;
			view->ri_sprite.rotate = 0;
			if ( view->u_move & playerMove_dead )
			{
				view->ri_sprite.gltex_res = g_api.GI_ResourceSearch( "gltex.smilie.dead" );
				
			}
			else if ( view->u_move & playerMove_pain )
			{
				view->ri_sprite.gltex_res = g_api.GI_ResourceSearch( "gltex.smilie.pain" );
				
			}
			else if ( view->u_move & playerMove_attack )
			{
				view->ri_sprite.gltex_res = g_api.GI_ResourceSearch( "gltex.smilie.attack" );
			}
			else if ( view->u_move & playerMove_jump )
			{
				view->ri_sprite.gltex_res = g_api.GI_ResourceSearch( "gltex.smilie.jump" );
			}
			else
			{
				view->ri_sprite.gltex_res = g_api.GI_ResourceSearch( "gltex.smilie.nice" );
			}
			//
			// draw player only if not the local view is bound
			//


			if ( !view->ri_view.take_view )
			{

				// test: tribal smilie dead
				if ( view->u_move & playerMove_tribaldead )
				{
					// don't draw the smile, debris is running
				}
				else
				{
					Vec3dCopy( view->ri_sprite.origin, view->origin );
					g_api.Render_Sprite( &view->ri_sprite );
				}
			}
			
			view->hud.di_face.gltex = view->ri_sprite.gltex_res;
		}



		//
		// draw hud only on local view
		//

		if ( view->ri_view.take_view )
		{			
			HUD_PlayerDraw( &view->hud );
		}

		if ( cl_num_hud < CL_MAX_HUD )
		{
			cl_huds[cl_num_hud++] = &view->hud;
		}
	}
		

	// render info view
	view->ri_view.id = atif->id;	
	view->ri_view.take_view = false;
	Vec3dCopy( view->ri_view.origin, view->origin );
	g_api.Render_View( &view->ri_view );
	

//	view->time_rel += cl_time_delta;
}

/*
  ==============================
  DestroyView

  ==============================
*/
static void DestroyView( cl_ati_t *atif )
{
	cl_at_view_t	*view;

	view = ADDR_OF_BASE( cl_at_view_t, atif, atif );	
	FREE( view );
}

/*
  ==================================================
  client view

  ==================================================
*/
static bool_t		ftbl_valid = false;
static cl_ati_ftbl_t	ftbl;

cl_ati_t * CL_CreateView( unique_t id, byte_iter_t *bi )
{
	cl_at_view_t		*view;

	if ( ftbl_valid == false )
	{
		ftbl_valid = true;
		INITTYPE( &ftbl, cl_ati_ftbl_t );

		ftbl.Destroy = DestroyView;
		ftbl.UnpackEvent = UnpackEventView;
		ftbl.Run = RunView;
	}

	view = NEWTYPE( cl_at_view_t );
	view->atif.ftbl = &ftbl;
	view->atif.id = id;

	// init private data
	
	// view needs nothing special from byte_iter event ...

	return &view->atif;
}
