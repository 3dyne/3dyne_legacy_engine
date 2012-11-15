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



#include "l_locals.h"



void L_ModifyPlayer( at_player_t *at )
{
	fp_t	phi, theta;

	vec3d_t		old_origin;
	fp_t		old_lat;
	fp_t		old_lon;
	
//	printf( "at: %p\n", at );
	
//	li.SH_Printf( "speedscale: %f", at->pmod->speedscale );
	Vec3dCopy( old_origin, at->origin );
	old_lat = at->lat;
	old_lon = at->lon;


	if ( at->pmod.speedscale == 0.0 && fabs( at->speedscale ) < 0.5 )
	{
		at->speedscale = 0.0;	
	}
	else if ( fabs( at->pmod.speedscale - at->speedscale ) < 0.5 )
	{
		at->speedscale = at->pmod.speedscale;
	}
	else
	{
		at->speedscale += (at->pmod.speedscale - at->speedscale ) * 0.2 ;
	}
	
	at->lon = at->pmod.lon;
	at->lat = at->pmod.lat;

	if( at->lat < -90 )
		at->lat = -90;

	if( at->lat > 90 )
		at->lat = 90;

	if( at->lon < -180 )
		at->lon += 360;

	if( at->lon > 180 )
		at->lon -= 360;

	phi = at->lat * ( M_PI / 180.0 );
	theta = at->lon * ( M_PI / 180.0 );

//	at->dir[0] = cos( phi ) * cos( theta );
//	at->dir[2] = sin( phi ) * cos( theta );
//	at->dir[1] = sin( theta );

	at->dir[0] = sin( theta );
	at->dir[1] = 0.0;
	at->dir[2] = cos( theta );

 


	{
		vec3d_t		trymove;
		vec3d_t		tryres;

		vec3d_t		start;

		Vec3dCopy( start, at->origin );
		
		Vec3dMA( trymove, at->speedscale, at->dir, at->origin );

		L_MoveVolumeSetFromPos( &at->move_vol, at->origin );
		L_MoveVolumeSetToPos( &at->move_vol, trymove );
		
		if ( TryMoveVolume( &at->move_vol ) == PlaneSlope_wall )
		{
			// move was clipped by a wall


			// try micro jump
			
			vec3d_t		v1, v2;

			// jump
			Vec3dCopy( v1, at->origin );
			v1[1] += 24.0;
			L_MoveVolumeSetFromPos( &at->move_vol, at->origin );
			L_MoveVolumeSetToPos( &at->move_vol, v1 );
			TryMoveVolume( &at->move_vol );
			L_MoveVolumeGetToPos( &at->move_vol, v1 );

			// move
			Vec3dMA( v2, at->speedscale, at->dir, v1 );
			L_MoveVolumeSetFromPos( &at->move_vol, v1 );
			L_MoveVolumeSetToPos( &at->move_vol, v2 );
			TryMoveVolume( &at->move_vol );
			L_MoveVolumeGetToPos( &at->move_vol, v1 );

			// fall
			Vec3dCopy( v2, v1 );
			v2[1] -= 24.0;
			L_MoveVolumeSetFromPos( &at->move_vol, v1 );
			L_MoveVolumeSetToPos( &at->move_vol, v2 );
			TryMoveVolume( &at->move_vol );
		}

//		L_MoveVolumeGetToPos( &at->move_vol, tryres );
//		Vec3dCopy( at->origin, tryres );

//		L_ToGround( &at->ati );
		
		L_MoveVolumeFollow( &at->move_vol );

		TryMoveVolumeToGround( &at->move_vol );

		// set move_volume's 'from' back to the old position, so that we get the effectiv move_volume
		L_MoveVolumeSetFromPos( &at->move_vol, start );
//		L_MoveVolumeGetToPos( &at->move_vol, at->origin );
	}
		
//	li.SH_Printf( "speedscale: %f", at->speedscale );

//	if( at->pmod->forward )
//		li.SH_Printf( "forward" );

	at->dir[0] = sin( theta )* cos( phi );
	at->dir[1] = -sin( phi );
	at->dir[2] = cos( theta ) * cos( phi );

}


static void Delete( at_player_t *self )
{
	L_ChangeToDead( &self->ati );
}

static void Destroy( at_player_t *self )
{
	li.MM_Free( self );
}

static void GetOrigin( at_player_t *self, vec3d_t vec )
{

	Vec3dCopy( vec, self->origin );
}

static void SetOrigin( at_player_t *self, vec3d_t vec )
{
	Vec3dCopy( self->origin, vec );
}

static void Com( at_player_t *self, char *in, char *out )
{
	li.SH_Printf( "got com string\n" );
}

/*
  ==================================================
  Think1 stuff

  ==================================================
*/

static void Think1( at_player_t *self )
{
	static int	last_dlight = -1;

	// carbide bb
	vec3d_t	min = { -128, -128, -128 };
	vec3d_t	max = { 128, 128, 128 };

	// extract pmod from event
	if ( self->ev )
	{
		unique_t		id;

		U_BeginUnpack( U_PACKED_BIN, self->ev->buf, 256 );
		U_Unpacks32( &id );
		U_Unpackfp32( &self->pmod.speedscale );
		U_Unpackfp32( &self->pmod.lon );
		U_Unpackfp32( &self->pmod.lat );
		U_Unpacks32( &self->pmod.shoot );
		U_Unpacks32( &self->pmod.use );
		U_EndUnpack();	

		FREE( self->ev );
		self->ev = NULL;
	}	
	else
	{
		__warning( "no event for player '%s'\n", self->ati.name );
	}

	if( self->pmod.use )
		li.SH_Printf( "use\n" );

	if( self->pmod.shoot && last_dlight < 0 )
	{
#if 0
		at_dlight_t	*dl;
		
		dl = NEW_dlight();
		Vec3dCopy( dl->origin, self->origin );
		Vec3dPrint( self->dir );
		Vec3dCopy( dl->dir, self->dir );
		dl->intensity = 100;
		dl->wf_num_till_dead = 50;
		Vec3dInit( dl->color, (random()%4)/3.0,(random()%4)/3.0,(random()%4)/3.0 );

//		L_RegisterNewArcheType( dl );
		L_ChangeToWakeUp( (ati_t *) dl );
		last_dlight = 0;
#else
		at_rocket_t	*rocket;
		hobj_t		*rocket_obj;

		rocket_obj = NewClass( "rocket", "#9999" );
		EasyNewVec3d( rocket_obj, "origin", self->origin );
		EasyNewVec3d( rocket_obj, "dir", self->dir );
		EasyNewFloat( rocket_obj, "speedscale", 64.0 );
		
		rocket = NEW_rocket();

		rocket->ati.ftbl->InitFromClass( &rocket->ati, rocket_obj );
		DeepDestroyClass( rocket_obj );

		L_ChangeToWakeUp( &rocket->ati );
		last_dlight = 5;
#endif

	}
	else
	{
		last_dlight--;
	}

	L_ModifyPlayer( self );
	L_MoveVolumeGetToPos( &self->move_vol, self->origin );

	L_CollideMapAddMoveVolume( (ati_t *) self, &self->move_vol );

	{
		vec3d_t		from, to;
		vec3d_t		delta;

		L_MoveVolumeGetFromPos( &self->move_vol, from );
		L_MoveVolumeGetToPos( &self->move_vol, to );

		Vec3dSub( delta, to, from );

#if 0		
		self->mark.gltex_res = li.GI_ResourceSearch( "gltex.fx.cross_red" );
		Vec3dCopy( self->mark.origin, self->origin );
		Vec3dInit( self->mark.color, 1, 1, 1 );
		self->mark.width = 0.25;
		self->mark.height = 0.25;
		self->mark.rotate = world_frame*16.0;
		li.GI_RegisterWFInfo( WFInfoType_sprite, &self->mark );
#endif
	}

	//
	// init carbide
	//
	self->carbide.value = 64.0;
	Vec3dCopy( self->carbide.origin, self->origin );
	Vec3dInit( self->carbide.color, 1.0, 1.0, 0.8 );
	Vec3dAdd( self->carbide.min, self->carbide.origin, min );
	Vec3dAdd( self->carbide.max, self->carbide.origin, max );

	if( ((sh_var_t*)li.SH_GetVar( "dlight_hack" )) ->ivalue )
		li.GI_RegisterDLightForWF( &self->carbide );
}

/*
  ==================================================
  Think2 stuff

  ==================================================
*/

void AT_PlayerCollideWith( at_player_t *self, ati_t *collide_with )
{
	char	send[256];
	char	recv[256];

//	__named_message( "[%s,%s]\n", collide_with->type, collide_with->name );

	if ( self->pmod.use )
	{
		sprintf( send, "use", self->ati.type, self->ati.name );	
		collide_with->ftbl->Com( collide_with, send, recv );
	}
	
	sprintf( send, "inside", self->ati.type, self->ati.name );	
	collide_with->ftbl->Com( collide_with, send, recv );	
}

static void Think2( at_player_t *self )
{
	L_CollideMapForEachInSpace( (ati_t *) self, &self->move_vol, (void *) AT_PlayerCollideWith );	
}

/*
  ==============================
  New

  ==============================
*/

static bool_t		ati_ftbl_player_valid = false;
static ati_ftbl_t	ati_ftbl_player;

static at_player_t *New()
{
	at_player_t	*obj;

	if ( ati_ftbl_player_valid == false )
	{
		ati_ftbl_player_valid = true;	

		memset( &ati_ftbl_player, 0, sizeof( ati_ftbl_t ) );
		ati_ftbl_player.GetOrigin = (void *) GetOrigin;
		ati_ftbl_player.SetOrigin = (void *) SetOrigin;
		ati_ftbl_player.Delete = (void *) Delete;
		ati_ftbl_player.Destroy = (void *) Destroy;
		ati_ftbl_player.Com = (void *) Com;
		ati_ftbl_player.Think1 = (void *) Think1;
		ati_ftbl_player.Think2 = (void *) Think2;
		ati_ftbl_player.InitFromClass = NULL;
	}
	
	
	obj = NEWTYPE( at_player_t );

	obj->ati.state = ATIState_none;
	obj->ati.ftbl = &ati_ftbl_player;

	return obj;
}


/*
  ==============================
  NEW_player

  ==============================
*/
at_player_t * NEW_player( char *name )
{
	at_player_t	*obj;
	vec3d_t		pos;
	ati_t		*start;

	obj = New();

	strcpy( obj->ati.type, "player" );
//	sprintf( obj->ati.name, "player_%d", num );
	strcpy( obj->ati.name, name );

	obj->ev = NULL;
	
	start = L_FindArcheT( NULL, "playerstart" );
	__chkptr( start );

	start->ftbl->GetOrigin( start, pos );

	obj->ati.ftbl->SetOrigin( obj, pos );

	__named_message( "startpos: " );
	Vec3dPrint( pos );

	//
	// fixme: don't init the object here
	//

	{
		vec3d_t		minspace = { -16, -64, -16 };
		vec3d_t		maxspace = { 16, 0, 16 };	

		L_MoveVolumeSetRelativeBoundBox( &obj->move_vol, minspace, maxspace );
	}

	return obj;
}

