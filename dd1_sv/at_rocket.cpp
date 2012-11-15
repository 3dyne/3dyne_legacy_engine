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



// at_rocket.c

#include "sv_state.h"

#include "l_head.h"
#include "l_defs.h"

#include "s_mem.h"
#include "lib_hobj.h"
#include "shock.h"

#include "events.h"

typedef struct at_rocket_s
{
	ati_t		atif;

	// private data
	vec3d_t		origin;
	vec3d_t		dir;	
	
	unique_t	id_owner;	// object, that launched the rocket

	bool_t		do_fly;
	bool_t		do_mapimpact;
	bool_t		do_objimpact;
	bool_t		do_timeout;

	// RocketMove calcs next position
	vec3d_t		pos_move;
	move_volume_t	move_vol;
	fp_t		speed;

	// update_ipo, send to client
	vec3d_t		ipo1;
	vec3d_t		ipo2;

	// rocket will die at this time
	msec_t		time_die;

} at_rocket_t;

/*
  ==============================
  ComRocket

  ==============================
*/
static const char * ComRocket( ati_t *atif, const char *in )
{
// 	at_rocket_t	*rocket;

// 	rocket = ADDR_OF_BASE( at_rocket_t, atif, atif );

	return "ok";
}

/*
  ==============================
  RunRocket

  ==============================
*/
static void InitRocketFromClass( at_rocket_t *rocket, hobj_t *obj )
{
	EasyFindVec3d( rocket->origin, obj, "origin" );
	EasyFindVec3d( rocket->dir, obj, "dir" );

	EasyFindClsrefAsUnique( &rocket->id_owner, obj, "owner" );

	rocket->speed = 256.0*4.0;
	
	// init rocket move_volume
	{
		vec3d_t		rel_min = { -16, -16, -16 };
		vec3d_t		rel_max = { 16, 16, 16 };
		
		L_MoveVolumeSetRelativeBoundBox( &rocket->move_vol, rel_min, rel_max );
	}

	// rockets will die after 3 sec if they don't hit something
	rocket->time_die = sv_time_current_frame + 3000;

	// rocket state
	rocket->do_fly = true;
}

static void MoveRocket( at_rocket_t *rocket )
{
	fp_t	u;

#if 0
	Vec3dMA( rocket->pos_move, sv_delta_sec*rocket->speed, rocket->dir, rocket->origin );

	L_MoveVolumeSetFromPos( &rocket->move_vol, rocket->origin );
	L_MoveVolumeSetToPos( &rocket->move_vol, rocket->pos_move );	

	L_ClipMoveVolume( &rocket->move_vol );

	L_MoveVolumeGetToPos( &rocket->move_vol, rocket->pos_move );

	Vec3dSub( v, rocket->pos_move, rocket->origin );
	u = Vec3dLen( v );

	time_done = (u*1.0)/(sv_delta_sec*rocket->speed);

	if ( time_done < 0.99 )
	{
		rocket->do_mapimpact = true;
		printf( " ************************ %f ***************************************", time_done );
	}

	printf( "fly %.2f%% of time\n", time_done*100.0 );
#else

	u = L_TraceClipLineMove( rocket->origin, rocket->dir, sv_delta_sec*rocket->speed );
	
	Vec3dMA( rocket->pos_move, u*sv_delta_sec*rocket->speed, rocket->dir, rocket->origin );
	
	if ( u < 0.99 )
	{
		rocket->do_mapimpact = true;
//		printf( " ************************ %f ***************************************", u );	
	}
#endif
}

static bool_t RocketCheck_func( ati_t *self, ati_t *other )
{
	at_rocket_t	*rocket;
	
	rocket = ADDR_OF_BASE( at_rocket_t, atif, self );
	
//	__named_message( "object [%s:%s] collide with object [%s:%s]\n", self->ati_obj->type, self->ati_obj->name, other->ati_obj->type, other->ati_obj->name );

	if ( !strcmp( other->ati_obj->type, "player" ) )
	{
		if ( other->id != rocket->id_owner )
		{
			return true;
		}
		else
		{
//			__named_message( "rocket ignores its owner\n" );
			return false;
		}
	}
	else if ( !strcmp( other->ati_obj->type, "psys" ) )
	{
//		other->ftbl->Com( other, "psys_blow 
	}
	else 
	{
//		__named_message( "rocket ignores object\n" );
	}

	return false;
}


static void RunRocket( ati_t *atif, atiRun run )
{
	at_rocket_t	*rocket;

	rocket = ADDR_OF_BASE( at_rocket_t, atif, atif );

	if ( run == atiRun_think1 )
	{
		if ( atif->frame_count == 0 )
		{
			InitRocketFromClass( rocket, atif->ati_obj );
			L_AtiRegisterId( atif );
			
			// send: typeinfo for new client object
			{
				byte_iter_t	frm;
				
				PackFrame_Begin( bi_out, &frm );
				ByteIter_Packi( &frm, atif->id );
				ByteIter_Packi( &frm, eventType_client_typeinfo_rocket );
				PackFrame_End( bi_out, &frm );	
			}
			
			// send: update_style
			{
				byte_iter_t	frm;
				
				PackFrame_Begin( bi_out, &frm );
				ByteIter_Packi( &frm, atif->id );
				ByteIter_Packi( &frm, eventType_client_obj_update_style );
				ByteIter_Packi( &frm, rocketStyle_fly );
				PackFrame_End( bi_out, &frm );					
			}

			// send: update_angles
			{
				byte_iter_t	frm;
				
				fp_t	lon, lat, yaw;
				
				lat = asin(rocket->dir[1])*(180.0/M_PI)+0.0;
				lon = atan2(rocket->dir[0],rocket->dir[2])*(180.0/M_PI)-90.0;
				yaw = 0.0;

				PackFrame_Begin( bi_out, &frm );
				ByteIter_Packi( &frm, atif->id );
				ByteIter_Packi( &frm, eventType_client_obj_update_angles );
				ByteIter_Packf( &frm, lon );
				ByteIter_Packf( &frm, lat );
				ByteIter_Packf( &frm, yaw );
				PackFrame_End( bi_out, &frm );	
			}
		}

		if ( sv_time_current_frame < rocket->time_die )
		{
//			rocket->do_timeout = false;
		}
		else
		{
			rocket->do_timeout = true;
			rocket->do_fly = false;
			rocket->do_mapimpact = false;
			rocket->do_objimpact = false;
		}

		if ( rocket->do_fly )
		{
			//
			// move and check map impact
			//
			MoveRocket( rocket );
		}

		// add rocket move volume to collide map
#if 0
		{
			vec3d_t		mv_min;
			vec3d_t		mv_max;
			
			L_MoveVolumeGetBoundBox( &rocket->move_vol, mv_min, mv_max );
			L_CollideMapAddAtiWithBoundBox( atif, mv_min, mv_max );
		}
#endif
	}
	
	if ( run == atiRun_think2 )
	{		
		//
		// check for object imapct
		//
	       
		if ( rocket->do_fly )
		{
			fp_t		u;

			rocket->do_objimpact = false;

			u = L_CollideMapClipLine( atif, rocket->origin, rocket->dir, sv_delta_sec*rocket->speed, RocketCheck_func );

			if ( u < 0.99 && u >= 0.0 )
			{
//				printf( " ############################### objcollide %f ####################", u );
				Vec3dMA( rocket->pos_move, u*sv_delta_sec*rocket->speed, rocket->dir, rocket->origin );
				rocket->do_objimpact = true;
			}
			
		}

		if ( rocket->do_fly )
		{
			Vec3dCopy( rocket->ipo1, rocket->origin );
			Vec3dCopy( rocket->ipo2, rocket->pos_move );
			
			// send: update_ipo
			{
				byte_iter_t	frm;
				
				PackFrame_Begin( bi_out, &frm );
				ByteIter_Packi( &frm, atif->id );
				ByteIter_Packi( &frm, eventType_client_obj_update_ipo );
				ByteIter_Pack3fv( &frm, rocket->ipo1 );
				ByteIter_Pack3fv( &frm, rocket->ipo2 );
				PackFrame_End( bi_out, &frm );
			}
			
			// pos_move gets new origin
			Vec3dCopy( rocket->origin, rocket->pos_move );
		}

		if ( rocket->do_timeout )
		{
			// send: destroy
			{
				byte_iter_t	frm;
				
				PackFrame_Begin( bi_out, &frm );
				ByteIter_Packi( &frm, atif->id );
				ByteIter_Packi( &frm, eventType_client_obj_destroy );
				PackFrame_End( bi_out, &frm );	
			}

			//
			// rocket will die
			//
			L_AtiUnregisterId( atif );
			L_AtiChangeToDead( atif );
		}

#if 1
		if ( rocket->do_mapimpact || rocket->do_objimpact )
		{

			//
			// hurt all objects in the explosion area
			// 
			
			vec3d_t		rel_min = {-128,-128,-128};
			vec3d_t		rel_max = {128, 128, 128};

			ati_t		*owner;

			owner = L_AtiSearchById( rocket->id_owner );
			// the owner of the killbox is the owner of the rocket
			L_PlaceKillBox( owner, rocket->pos_move, rel_min, rel_max, 90.0 );
	
			// send: update_style
			{
				byte_iter_t	frm;
				
				PackFrame_Begin( bi_out, &frm );
				ByteIter_Packi( &frm, atif->id );
				ByteIter_Packi( &frm, eventType_client_obj_update_style );
				ByteIter_Packi( &frm, rocketStyle_impact );
				PackFrame_End( bi_out, &frm );					
			}

			// send: proxy_killbox
			{
				byte_iter_t	frm;
				
				PackFrame_Begin( bi_out, &frm );
				ByteIter_Packi( &frm, UNIQUE_INVALIDE );
				ByteIter_Packi( &frm, eventType_client_proxy_killbox );
				ByteIter_Pack3fv( &frm, rocket->pos_move );
				ByteIter_Pack3fv( &frm, rel_min );
				ByteIter_Pack3fv( &frm, rel_max );
				PackFrame_End( bi_out, &frm );	
			}

			rocket->do_fly = false;
			rocket->do_mapimpact = false;
			rocket->do_objimpact = false;
			rocket->do_timeout = true;
		}
#endif
		atif->frame_count++;
	}
}

/*
  ==============================
  DestroyRocket

  ==============================
*/
static void DestroyRocket( ati_t *atif )
{
	at_rocket_t	*rocket;

	DeepDestroyClass( atif->ati_obj );

	rocket = ADDR_OF_BASE( at_rocket_t, atif, atif );
	FREE( rocket );
}

/*
  ==================================================
  ATI_CreateRocket

  ==================================================
*/
static bool_t		ftbl_valid = false;
static ati_ftbl_t	ftbl;

ati_t * ATI_CreateRocket( hobj_t *rocket_obj )
{
	at_rocket_t	*rocket;

	if ( strcmp( rocket_obj->type, "rocket" ) )
		__error( "not 'rocket' class\n" );

	if ( ftbl_valid == false )
	{
		ftbl_valid = true;
		INITTYPE( &ftbl, ati_ftbl_t );

		ftbl.Destroy = DestroyRocket;
		ftbl.Com = ComRocket;
		ftbl.Run = RunRocket;
	}

	rocket = NEWTYPE( at_rocket_t );

	rocket->atif.ftbl = &ftbl;
	rocket->atif.id = StringToUnique( rocket_obj->name );
	rocket->atif.ati_obj = rocket_obj;
	rocket->atif.frame_count = 0;

	return &rocket->atif;
}
