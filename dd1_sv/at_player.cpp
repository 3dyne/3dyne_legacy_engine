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



// at_player.c

#include "sv_state.h"

#include "l_head.h"
#include "l_defs.h"

#include "s_mem.h"
#include "lib_hobj.h"
#include "shock.h"

#include "events.h"

#define SV_PLAYER_NICKNAME_SIZE		( 32 )

typedef struct at_player_s
{
	ati_t		atif;

	char		nickname[SV_PLAYER_NICKNAME_SIZE];

	// private data
	vec3d_t		origin;
	vec3d_t		dir;	// view direction

	vec3d_t		rel_min;
	vec3d_t		rel_max;

	// event
	bool_t		ev_valid;
	fp_t		ev_lon, ev_lat, ev_speed;
	fp_t		ev_strafespeed;
	int		ev_shoot, ev_use, ev_jump;

	// current
	fp_t		lon, lat, speed, strafespeed;

	fp_t		phi, theta;

	vec3d_t		vec_forward;
	vec3d_t		vec_right;

	fp_t		scale_forward;
	fp_t		scale_right;
	
	//
	// physic
	//
	move_volume_t	move_vol;

	vec3d_t		velocity;

	bool_t		is_dead;
	bool_t		is_ground;	// PlayerCheckGround found a surface
	bool_t		is_walking;	// if is_ground, the surface is not too steep
	vec3d_t		ground_norm;	// normal of ground surface

	vec3d_t		pos_move;	// position to move to after physic
	
	// 
	vec3d_t			delta;


	// player at work
	msec_t		time_last_shoot;
	msec_t		time_last_jump;
	msec_t		time_last_hurt;
	msec_t		time_last_dead;
	bool_t		is_tribaldead;

	// set by PlayerCheckMove
	playerMove	move;

	// player health
	int		health;
	int		numfrag;

} at_player_t;

/*
  ==================================================
  player modified by event

  ==================================================
*/
static void PlayerCheckGround( at_player_t *player )
{
	vec3d_t		v;

	fp_t		scale;
	vec3d_t		clip_pos;
	vec3d_t		clip_norm;

	Vec3dCopy( v, player->pos_move );

	// trace down
	v[1] -= 16.0;

	scale = L_TraceClipBoundBoxMove( player->rel_min, player->rel_max, player->pos_move, v, clip_pos, clip_norm );


	if ( scale == 1.0 )
	{
//		printf( "is_falling\n" );
		player->is_ground = false;
		player->is_walking = false;
	}	
	else if ( scale >= 0.0 )
	{
//		printf( "is_ground\n" );

		if ( player->velocity[1] > 0 )
		{
			player->is_ground = false;
			player->is_walking = false;
			return;
		}

		player->is_ground = true;
		Vec3dCopy( player->ground_norm, clip_norm );

		if ( clip_norm[1] > 0.7 )
		{
//			printf( "can_walk\n" );
			player->is_walking = true;
		}
		else
		{
			player->is_walking = false;
		}
	}
}

void PlayerFriction( at_player_t *player )
{
	if ( player->is_dead )
	{
			player->velocity[0] *= 0.7;
			player->velocity[2] *= 0.7;		
	}

	if ( player->is_ground )
	{
		if ( player->is_walking )		
		{
			player->velocity[0] *= 0.7;
			player->velocity[2] *= 0.7;
		}
	} 
	else
	{
		player->velocity[0] *= 0.9;
		player->velocity[2] *= 0.9;
	}
      
}

void PlayerAccelerate_test( vec3d_t velocity, vec3d_t accel_dir, fp_t accel, fp_t accel_time )
{
	vec3d_t		vel_dir;

	Vec3dCopy( vel_dir, velocity );
	Vec3dUnify( vel_dir );	
	/*dot =*/ Vec3dDotProduct( vel_dir, accel_dir ); // FIXME: remove, there should be no side effects!?

// 	extra = (2-dot);
	

	Vec3dMA( velocity, accel*accel_time, accel_dir, velocity );

//	vel_scale = Vec3dLen( velocity );
//	Vec3dScale( velocity, vel_scale, accel_dir );

//

}

void PlayerAccelerate( at_player_t *player, vec3d_t dir, fp_t speed, fp_t accel )
{
#if 0
	vec3d_t		vel;
	vec3d_t		delta;

	Vec3dScale( vel, speed, dir );
	Vec3dSub( delta, vel, player->velocity  );

	Vec3dMA( player->velocity, accel, delta, player->velocity );
#else
//	printf( "velocity: %f\n", Vec3dLen( player->velocity ) );
//	PlayerAccelerate_test( player->velocity, dir, speed, sv_delta_sec );
	
	if ( 1 )
	{
		int                     i;                                                 
		float           addspeed, accelspeed, currentspeed;                        
		
//		static vec3d_t vel={0,0,0};
	   

//		printf( "speed: %f\n", speed );

		currentspeed = Vec3dDotProduct (player->velocity, dir);                     
		addspeed = speed - currentspeed;                                       
		if (addspeed <= 0) {  
//			printf( " =============================> old: " );
//			Vec3dPrint( player->velocity );
			return;                                                            
		}                                                                          
		accelspeed = accel*sv_delta_sec*speed;                                
		if (accelspeed > addspeed) {                                               
			accelspeed = addspeed;                                             
		}                                                                          
		
		for (i=0 ; i<3 ; i++) {                                                    
			player->velocity[i] += accelspeed*dir[i];                      
		} 		
	}
#endif
}

void PlayerDeadMove( at_player_t *player )
{
	PlayerFriction( player );

  	player->velocity[1] += -64.0;

	if ( player->is_ground )
	{
		L_ClipVelocity( player->velocity, player->velocity, player->ground_norm );
	}
	L_VelocityMove( player->rel_min, player->rel_max, player->pos_move, player->velocity, sv_delta_sec );

}


void PlayerAirMove( at_player_t *player )
{
//	printf( "air_move\n" );

//  	player->velocity[0] = 0.0;
//  	player->velocity[1] = -64.0;
//	player->velocity[2] = 0.0;


	PlayerFriction( player );

	PlayerAccelerate( player, player->vec_forward, player->scale_forward, 3.0 );

  	player->velocity[1] += -64.0;

	// fall out of the level
	if ( player->velocity[1] < -1000.0 )
	{
		player->health = 0;
		player->is_dead = true;
		player->time_last_dead = sv_time;
		return;
	}

	if ( player->is_ground )
	{
		L_ClipVelocity( player->velocity, player->velocity, player->ground_norm );
	}
	L_VelocityMove( player->rel_min, player->rel_max, player->pos_move, player->velocity, sv_delta_sec );

}

static bool_t PlayerCheckJump( at_player_t *player )
{
	if ( !player->ev_jump )
	{
		return false;
	}

	player->time_last_jump = sv_time;

	player->velocity[1] = 280.0;

	player->is_ground = false;
	player->is_walking = false;

	return true;
}

static bool_t PlayerCheckShock( at_player_t *player )
{
	if ( sv_time - player->time_last_hurt >= 500 )
	{
		return false;
	}
	
	// velocity is set by PlayerHurt

	player->is_ground = false;
	player->is_walking = false;

	return true;
}

static void PlayerWalkMove( at_player_t *player )
{
	vec3d_t		vec_move;

	if ( !player->is_ground )
		return;
//	printf( "walk_move\n" );
       
	if ( PlayerCheckJump( player ) )	     
	{
		PlayerAirMove( player );
		return;
	}

	if ( PlayerCheckShock( player ) )
	{
		PlayerAirMove( player );
		return;
	}

	PlayerFriction( player );
	
	{
		vec3d_t		v;
		fp_t		speed;

		Vec3dScale( v, player->scale_forward, player->vec_forward );
		Vec3dMA( v, player->scale_right, player->vec_right, v );

		speed = Vec3dLen( v );
		Vec3dUnify( v );
		PlayerAccelerate( player, v, speed, 640.0 );
//	PlayerAccelerate( player, player->vec_forward, player->scale_forward*1.0, 640.0 );
	}

	L_ClipVelocity( player->velocity, player->velocity, player->ground_norm );

	{
		vec3d_t		pos_old;
		vec3d_t		vel_old;
		vec3d_t		first_try;
		vec3d_t		first_vel;

		fp_t		scale;
		vec3d_t		clip_pos;
		vec3d_t		clip_norm;
		
		vec3d_t		v;

		//
		// try a straight move
		//

		Vec3dCopy( pos_old, player->pos_move );
	     
//		vec_move[1] -= 256.0;
		Vec3dCopy( vel_old, player->velocity );
	       

//		printf( "first move: " );
//		Vec3dPrint( player->pos_move );
		if ( L_VelocityMove( player->rel_min, player->rel_max, player->pos_move, player->velocity, sv_delta_sec ) )
		{
//			printf( "first move successed\n" );
			return;
		}

//		printf( " ########################## micro jump ######################\n" );
//		return;


		Vec3dCopy( first_try, player->pos_move );
		Vec3dCopy( first_vel, player->velocity );
		Vec3dCopy( vec_move, vel_old );
		
		Vec3dCopy( player->pos_move, pos_old );
		Vec3dCopy( player->velocity, vel_old );
		Vec3dCopy( v, player->pos_move);
		v[1] += 24.0;
//		printf( "micro_jump_from: " );
//		Vec3dPrint( player->pos_move );

		scale = L_TraceClipBoundBoxMove( player->rel_min, player->rel_max, player->pos_move, v, clip_pos, clip_norm );
		Vec3dCopy( player->pos_move, clip_pos );
//		printf( "micro_jump_clip: " );
//		Vec3dPrint( player->pos_move );

//		printf( "micro_jump_scale: %f\n", scale );

		if ( scale < 1.0 )
		{
//			printf( "micro_jump failed\n" );
			Vec3dCopy( player->pos_move, first_try );
			Vec3dCopy( player->velocity, first_vel );
			return;
		}

		if ( !L_VelocityMove( player->rel_min, player->rel_max, player->pos_move, player->velocity, sv_delta_sec ) )
		{
//			printf( "second move clip !\n" );
		}

		Vec3dCopy( v, player->pos_move );
		v[1] -= 24.0;
		scale = L_TraceClipBoundBoxMove( player->rel_min, player->rel_max, player->pos_move, v, clip_pos, clip_norm );

		Vec3dCopy( player->pos_move, clip_pos );		

//		printf( "micro_fall_scale: %f ", scale );
//		Vec3dPrint( clip_norm );

//		printf( "micro_jump_to: " );
//		Vec3dPrint( player->pos_move );		
//

	}


	L_MoveVolumeSetFromPos( &player->move_vol, player->origin );
	L_MoveVolumeSetToPos( &player->move_vol, player->pos_move );
}



void ModifyPlayer( at_player_t *player )
{
	vec3d_t		old_origin;
// 	fp_t		old_lat;
// 	fp_t		old_lon;
	
	Vec3dCopy( old_origin, player->origin );
// 	old_lat = player->lat;
// 	old_lon = player->lon;

	

	if ( player->ev_speed == 0.0 && fabs( player->speed ) < 0.5 )
	{
		player->speed = 0.0;	
	}
	else if ( fabs( player->ev_speed - player->speed ) < 0.5 )
	{
		player->speed = player->ev_speed;
	}
	else
	{
		player->speed += (player->ev_speed - player->speed ) * 0.2 ;
	}

	if ( player->ev_strafespeed == 0.0 && fabs( player->strafespeed ) < 0.5 )
	{
		player->strafespeed = 0.0;	
	}
	else if ( fabs( player->ev_strafespeed - player->strafespeed ) < 0.5 )
	{
		player->strafespeed = player->ev_strafespeed;
	}
	else
	{
		player->strafespeed += (player->ev_strafespeed - player->strafespeed ) * 0.2 ;
	}
	
	player->lon = player->ev_lon;
	player->lat = player->ev_lat;

	if( player->lat < -90 )
		player->lat = -90;

	if( player->lat > 90 )
		player->lat = 90;

	if( player->lon < -180 )
		player->lon += 360;

	if( player->lon > 180 )
		player->lon -= 360;

	player->phi = player->lat * ( M_PI / 180.0 );
	player->theta = player->lon * ( M_PI / 180.0 );
	
	player->dir[0] = sin( player->theta )* cos( player->phi );
	player->dir[1] = -sin( player->phi );
	player->dir[2] = cos( player->theta ) * cos( player->phi );

	player->vec_forward[0] = sin( player->theta );
	player->vec_forward[1] = 0.0;
	player->vec_forward[2] = cos( player->theta );
	
	player->vec_right[0] = sin( player->theta + M_PI/2.0 );
	player->vec_right[1] = 0.0;
	player->vec_right[2] = cos( player->theta + M_PI/2.0 );
	
	player->scale_forward = player->ev_speed;
	player->scale_right = player->ev_strafespeed;

	if ( player->scale_forward < 0.0 )
	{
		player->scale_forward = -player->scale_forward;
		Vec3dScale( player->vec_forward, -1.0, player->vec_forward );
	}

	if ( player->scale_right < 0.0 )
	{
		player->scale_right = -player->scale_right;
		Vec3dScale( player->vec_right, -1.0, player->vec_right );
	}
	
	Vec3dCopy( player->pos_move, player->origin );
	
	PlayerCheckGround( player );

	if ( player->is_dead )
	{
		PlayerDeadMove( player );
		return;
	}
	
	if ( player->is_walking )
	{
		PlayerWalkMove( player );
//		printf( " remaining speed: %f\n", Vec3dLen( player->velocity ) );
	}
	else 
	{
		PlayerAirMove( player );
	}

	PlayerCheckGround( player );

}


/*
  ==============================
  PlayerCheckRespawn

  ==============================
*/

static void PlayerGetStartPlace( at_player_t *player )
{
	hobj_search_iterator_t	iter;
	hobj_t		*obj;

	int		num_place;
	hobj_t		*places[16];
	
	InitClassSearchIterator( &iter, sv_at_root, "*" );
	
	num_place = 0;

	for ( ; ( obj = SearchGetNextClass( &iter ) ) ; )
	{
		if ( !strcmp( obj->type, "pos_player" ) )
		{
			if ( num_place >= 16 )
			{
				break;
			}
			places[num_place] = obj;
			num_place++;
		}
	}

	if ( num_place == 0 )
	{
		__error( "no 'pos_player' found in ats.hobj\n" );
	}
	
	obj = places[rand()%num_place];
	
	__chkptr( obj );

	EasyFindVec3d( player->dir, obj, "dir" );
	EasyFindVec3d( player->origin, obj, "origin" );

	//
	// jitter origin a little bit
	//
	
	player->origin[0] += ((rand()%32)-16);
	player->origin[2] += ((rand()%32)-16);
}

static bool_t PlayerCheckRespawn( at_player_t *player )
{
	if ( !player->is_dead )
	{
		return false;
	}
	
	if ( sv_time - player->time_last_dead < 1500 )
	{
		return false;
	}
/*
	if ( !player->ev_jump && !player->ev_use && !player->ev_shoot )
	{
		return false;
	}
*/
	return true;
}

/*
  ==============================
  PlayerRespawn

  ==============================
*/
static void PlayerRespawn( at_player_t *player )
{
	vec3d_t		rel_min = { -24, -64, -24 };
	vec3d_t		rel_max = { 24, 16, 24 };	

	player->ev_shoot = false;
	player->ev_jump = false;
	player->ev_use = false;

	player->health = 100;
	player->is_dead = false;
	player->is_ground = false;
	player->is_walking = false;
	PlayerGetStartPlace( player );
	Vec3dInit( player->velocity, 0, 0, 0 );
	
	Vec3dCopy( player->rel_min, rel_min );
	Vec3dCopy( player->rel_max, rel_max );
	
	// teleport_recv gfx
	{
		byte_iter_t	frm;
		PackFrame_Begin( bi_out, &frm );
		ByteIter_Packi( &frm, UNIQUE_INVALIDE );
		ByteIter_Packi( &frm, eventType_client_gfx_create );
		ByteIter_Packi( &frm, gfxStyle_teleport_recv );
		ByteIter_Pack3fv( &frm, player->origin );
		PackFrame_End( bi_out, &frm );
	}
}


/*
  ==================================================
  at standart interface

  ==================================================
*/

/*
  ==============================
  ComPlayer

  ==============================
*/
static const char * ComPlayer( ati_t *atif, const char *in )
{
// 	at_player_t	*player;
	
// 	player = ADDR_OF_BASE( at_player_t, atif, atif );

	return "ok";
}

/*
  ==============================
  UnpackEventPlayer

  ==============================
*/
static void UnpackEventPlayer( ati_t *atif, unique_t id, eventType type, byte_iter_t *bi )
{
	at_player_t	*player;
	
	player = ADDR_OF_BASE( at_player_t, atif, atif );
	
	if ( player->ev_valid )
	{
		__warning( "override unhandled event\n" );
	}

	if ( type != eventType_server_update_player )
	{
		__error( "unknown event\n" );
	}

	player->ev_speed = ByteIter_Unpackf( bi );
	player->ev_strafespeed = ByteIter_Unpackf( bi );
	player->ev_lon = ByteIter_Unpackf( bi );
	player->ev_lat = ByteIter_Unpackf( bi );
	player->ev_shoot = ByteIter_Unpacki( bi );
	player->ev_use = ByteIter_Unpacki( bi );
	player->ev_jump = ByteIter_Unpacki( bi );

	player->ev_valid = true;
}

/*
  ==============================
  RunPlayer

  ==============================
*/


static void InitPlayerFromClass( at_player_t *player, hobj_t *player_obj )
{
	hpair_t		*pair;

	pair = FindHPair( player_obj, "nickname" );
	if ( pair )
	{
		strncpy( player->nickname, pair->value, SV_PLAYER_NICKNAME_SIZE-1);
	}

	player->numfrag = 0;
}

static void PlayerEmitTypeInfoEvent( ati_t *atif )
{
	byte_iter_t	frm;
	
	PackFrame_Begin( bi_out, &frm );
	ByteIter_Packi( &frm, atif->id );
//	ByteIter_Packi( &frm, eventType_client_typeinfo_player );
	ByteIter_Packi( &frm, eventType_client_typeinfo_view );
	PackFrame_End( bi_out, &frm );
}

static void PlayerEmitUpdateEvent( ati_t *atif )
{
	byte_iter_t	frm;
	at_player_t	*player;

	player = ADDR_OF_BASE( at_player_t, atif, atif );

	PackFrame_Begin( bi_out, &frm );
	ByteIter_Packi( &frm, atif->id );
	ByteIter_Packi( &frm, eventType_client_obj_update_ipo );
//	ByteIter_Packi( &frm, sv_time );
	ByteIter_Pack3fv( &frm, player->origin );
	ByteIter_Pack3fv( &frm, player->pos_move );
	PackFrame_End( bi_out, &frm );
}

static void PlayerEmitStyleEvent( ati_t *atif )
{
	byte_iter_t	frm;
// 	at_player_t	*player;

// 	player = ADDR_OF_BASE( at_player_t, atif, atif );

	PackFrame_Begin( bi_out, &frm );
	ByteIter_Packi( &frm, atif->id );
	ByteIter_Packi( &frm, eventType_client_obj_update_style );
	ByteIter_Packi( &frm, viewStyle_player );
	PackFrame_End( bi_out, &frm );
}

static void PlayerEmitHudEvent( ati_t *atif )
{
	byte_iter_t	frm;
	at_player_t	*player;

	player = ADDR_OF_BASE( at_player_t, atif, atif );

	PackFrame_Begin( bi_out, &frm );
	ByteIter_Packi( &frm, atif->id );
	ByteIter_Packi( &frm, eventType_client_obj_update_hud );
	ByteIter_Packi( &frm, player->health );
	ByteIter_Packi( &frm, player->numfrag );
	PackFrame_End( bi_out, &frm );
}

static void PlayerEmitNicknameEvent( ati_t *atif )
{
	byte_iter_t	frm;
	at_player_t	*player;
	
	player = ADDR_OF_BASE( at_player_t, atif, atif );
	
	PackFrame_Begin( bi_out, &frm );
	ByteIter_Packi( &frm, atif->id );
	ByteIter_Packi( &frm, eventType_client_obj_update_nickname );
	ByteIter_Packnbv( &frm, player->nickname, SV_PLAYER_NICKNAME_SIZE );
	PackFrame_End( bi_out, &frm );
}


static void PlayerEmitDestroyEvent( ati_t *atif )
{
	byte_iter_t	frm;
	
	PackFrame_Begin( bi_out, &frm );
	ByteIter_Packi( &frm, atif->id );
	ByteIter_Packi( &frm, eventType_client_obj_destroy );
	PackFrame_End( bi_out, &frm );	
}

static void PlayerEmitMoveEvent( ati_t *atif )
{
	byte_iter_t	frm;
	at_player_t	*player;

	player = ADDR_OF_BASE( at_player_t, atif, atif );
	
	PackFrame_Begin( bi_out, &frm );
	ByteIter_Packi( &frm, atif->id );
	ByteIter_Packi( &frm, eventType_client_obj_update_move );
	ByteIter_Packi( &frm, player->move );
	PackFrame_End( bi_out, &frm );
}

static void PlayerCheckMove( at_player_t *player )
{
	player->move = (playerMove)0;

//	player->move |= playerMove_walk;

	if ( player->is_dead )
	{
		player->move = (playerMove)(player->move | playerMove_dead);

		if ( player->is_tribaldead )
		{
			player->move = (playerMove)(player->move | playerMove_tribaldead);
		}
		return;
	}

	if ( !player->is_walking )
	{
		// jump move
		player->move = (playerMove)(player->move | playerMove_jump);
	}
	else
	{
		player->move = (playerMove)(player->move | playerMove_walk);
	}

	if ( sv_time - player->time_last_shoot < 500 )
	{
		// shoot move
		player->move = (playerMove)(player->move |playerMove_attack);
	}

	if ( sv_time - player->time_last_hurt < 800 )
	{
		// pain move
		player->move = (playerMove)(player->move | playerMove_pain);
	}
}

static void PlayerCollide_func( ati_t *self, ati_t *other )
{
// 	const char	*com_res;

	at_player_t	*player;
       
	player = ADDR_OF_BASE( at_player_t, atif, self );

//	__named_message( "object [%s:%s] collide with object [%s:%s]\n", self->ati_obj->type, self->ati_obj->name, other->ati_obj->type, other->ati_obj->name );

	if ( !strcmp( other->ati_obj->type, "switch" ) )
	{
		/*com_res =*/ other->ftbl->Com( other, "inside" );
		if ( player->ev_use )
		{
			/*com_res =*/ other->ftbl->Com( other, "use" );
		}
	}
}


static void RunPlayer( ati_t *atif, atiRun run )
{
	at_player_t	*player;

//	__named_message( "\n" );

	player = ADDR_OF_BASE( at_player_t, atif, atif );

	if ( run == atiRun_think1 )
	{
		if ( atif->frame_count == 0 )
		{
			// first call, init at_player
			InitPlayerFromClass( player, atif->ati_obj );
			PlayerRespawn ( player );

			// make id public
			L_AtiRegisterId( atif );

			{
//				vec3d_t		rel_min = { -16, -64, -16 };
//				vec3d_t		rel_max = { 16, 16, 16 };	
	
				// smilie test


				L_MoveVolumeSetRelativeBoundBox( &player->move_vol, player->rel_min, player->rel_max );
			}		

		}

		if ( PlayerCheckRespawn( player ) )
		{
			PlayerRespawn ( player );
		}
		
		if ( player->ev_valid == false )
		{
			player->ev_speed = 0.0;	
			player->ev_strafespeed = 0.0;
			player->ev_shoot = 0;
			player->ev_use = 0;
			player->ev_jump = 0;
		}

		player->ev_valid = false;

		ModifyPlayer( player );
		
		{
			vec3d_t		mv_min;
			vec3d_t		mv_max;

			L_MoveVolumeSetFromPos( &player->move_vol, player->origin );
			L_MoveVolumeSetToPos( &player->move_vol, player->pos_move );

			L_MoveVolumeGetBoundBox( &player->move_vol, mv_min, mv_max );
			L_CollideMapAddAtiWithBoundBox( atif, mv_min, mv_max );
		}
	}

	else if ( run == atiRun_think2 )
	{	
		if ( player->ev_use )
		{
//			__named_message( "use\n" );
		}
		if ( player->ev_shoot )
		{
			// shoots every 500 msec
			if ( sv_time - player->time_last_shoot > 500 && !player->is_dead )
			{
				unique_t	id_rocket;
				hobj_t		*obj_rocket;

//				__named_message( "shoot accepted\n" );
				
				id_rocket = g_api.Map_GetUniqueID();
				obj_rocket = EasyNewClassFromID( "rocket", id_rocket );
				EasyNewVec3d( obj_rocket, "origin", player->origin );
				EasyNewVec3d( obj_rocket, "dir", player->dir );
				EasyNewClsref( obj_rocket, "owner", atif->ati_obj );
				
				L_SpawnTmpObject( obj_rocket, ATI_CreateRocket );

				player->time_last_shoot = sv_time;
				
				
			}
			else
			{
//				__named_message( "shoot ignored\n" );
			}
		}
		if ( player->ev_jump )
		{
//			__named_message( "jump\n" );
		}

		L_CollideMapSelectAllAtiInBoundBox( atif, PlayerCollide_func );

		PlayerCheckMove( player );
			
		if ( (atif->frame_count&31) == 0 )
		{
			PlayerEmitTypeInfoEvent( atif );
			PlayerEmitStyleEvent( atif );
		}
		if ( (atif->frame_count&127) )
		{
			PlayerEmitNicknameEvent( atif );
		}


		PlayerEmitUpdateEvent( atif );
		PlayerEmitMoveEvent( atif );
		PlayerEmitHudEvent( atif );
		//
		// set player to new origin
		//
		Vec3dCopy( player->origin, player->pos_move );

		atif->frame_count++;
	}
}

/*
  ==============================
  HurtPlayer

  ==============================
*/
static void HurtPlayer( ati_t *self, ati_t *other, vec3d_t kill_center, fp_t destruction )
{
	at_player_t	*player;
	vec3d_t		shock_vec;

	fp_t		dist;
	int		hurt;

	player = ADDR_OF_BASE( at_player_t, atif, self );

	// already dead
	if ( player->is_dead )
	{
		return;
	}

	Vec3dSub( shock_vec, player->origin, kill_center );
	dist = Vec3dLen( shock_vec );
	Vec3dUnify( shock_vec );
	PlayerAccelerate( player, shock_vec, 700.0, 640.0 );

	//
	// calc hurt
	// 
	hurt = destruction - dist;
	if ( hurt < 0 )
		hurt = 0;
	
	hurt += 5;

	player->health -= hurt;

	if ( player->health <= 0 )
	{
		vec3d_t		rel_min = { -24, -24, -24 };
		vec3d_t		rel_max = { 24, 24, 24 };	

		Vec3dCopy( player->rel_min, rel_min );
		Vec3dCopy( player->rel_max, rel_max );

		player->health = 0;
		player->is_dead = true;
		player->time_last_dead = sv_time;

		if ( hurt > 30 )
		{
			player->is_tribaldead = true;
		}
		else
		{
			player->is_tribaldead = false;
		}
	}

	// handle frags
	if ( player->is_dead )
	{		
		if ( self == other )
		{
			// suicide ...
			printf( " ************************** SUICIDE ! *********\n" );
			player->numfrag--;
		}
		else
		{
			other->ftbl->Seti( other, atiP_fragadd, 1 );
		}
	}

	// let the player fly
	player->time_last_hurt = sv_time;
}

/*
  ==============================
  GetiPlayer
  
  ==============================
*/
static int GetiPlayer( ati_t *self, atiP param )
{
	at_player_t	*player;
	player = ADDR_OF_BASE( at_player_t, atif, self );
	
	switch( param )
	{
	case atiP_is_ground:
		return player->is_ground;				
	default:
		__error( "param = %d\n", param );
	}

	return 0;
}

/*
  ==============================
  SetiPlayer
  
  ==============================
*/
static void SetiPlayer( ati_t *self, atiP param, int i )
{
	at_player_t	*player;
	player = ADDR_OF_BASE( at_player_t, atif, self );
	
	switch( param )
	{
	case atiP_fragadd:
		player->numfrag += i;
		break;
	default:
		__error( "param = %d\n", param );
	}
}


/*
  ==============================
  Get3fvPlayer

  ==============================
*/
static void Get3fvPlayer( ati_t *self, atiP param, fp_t fv[3] )
{
	at_player_t	*player;
	player = ADDR_OF_BASE( at_player_t, atif, self );
	
	switch( param )
	{
	case atiP_origin3fv:
		Vec3dCopy( fv, player->origin );
		break;
	case atiP_velocity3fv:
		Vec3dCopy( fv, player->velocity );
		break;
	default:
		__error( "param = %d\n", param );
	}
}

/*
  ==============================
  Set3fvPlayer

  ==============================
*/
static void Set3fvPlayer( ati_t *self, atiP param, fp_t fv[3] )
{
	at_player_t	*player;
	player = ADDR_OF_BASE( at_player_t, atif, self );
	
	switch( param )
	{
	case atiP_origin3fv:
		Vec3dCopy( player->origin, fv );
		Vec3dCopy( player->pos_move, fv );
		break;
	case atiP_velocity3fv:
		Vec3dCopy( player->velocity, fv );
		break;
	default:
		__error( "param = %d\n", param );
	}
}

/*
  ==============================
  DestroyPlayer

  ==============================
*/
static void DestroyPlayer( ati_t *atif )
{
	at_player_t	*player;

	PlayerEmitDestroyEvent( atif );

	L_AtiUnregisterId( atif );	
	DeepDestroyClass( atif->ati_obj );

	player = ADDR_OF_BASE( at_player_t, atif, atif );
	FREE( player );
}

/*
  ==================================================
  ATI_CreatePlayer

  ==================================================
*/
static bool_t		ftbl_valid = false;
static ati_ftbl_t	ftbl;

ati_t * ATI_CreatePlayer( hobj_t *player_obj )
{
	at_player_t		*player;

	if ( strcmp( player_obj->type, "player" ) )
		__error( "player_obj is not of type 'player'\n" );


	if ( ftbl_valid == false )
	{
		ftbl_valid = true;
		INITTYPE( &ftbl, ati_ftbl_t );
		
		ftbl.Destroy = DestroyPlayer;
		ftbl.Com = ComPlayer;
		ftbl.UnpackEvent = UnpackEventPlayer;
		ftbl.Run = RunPlayer;

		ftbl.Hurt = HurtPlayer;

		ftbl.Geti = GetiPlayer;
		ftbl.Seti = SetiPlayer;
		ftbl.Get3fv = Get3fvPlayer;
		ftbl.Set3fv = Set3fvPlayer;
	}
	
	player = NEWTYPE( at_player_t );
	
	player->atif.ftbl = &ftbl;
	player->atif.id = StringToUnique( player_obj->name );
	player->atif.ati_obj = player_obj;
	player->atif.frame_count = 0;

	return &player->atif;
}
