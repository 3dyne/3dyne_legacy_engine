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


static void Delete( at_dlight_t *self )
{
	L_ChangeToDead( &self->ati );
}

static void Destroy( at_dlight_t *self )
{
	li.MM_Free( self );
}

static void GetOrigin( at_dlight_t *self, vec3d_t vec )
{

	Vec3dCopy( vec, self->origin );
}

static void SetOrigin( at_dlight_t *self, vec3d_t vec )
{
	Vec3dCopy( self->origin, vec );
}

static void Com( at_dlight_t *self, char *in, char *out )
{
	li.SH_Printf( "got com string\n" );
}

static void Think1( at_dlight_t *self )
{
	vec3d_t	porigin;
	vec3d_t	trypos;
	vec3d_t	min = { -128, -128, -128 };
	vec3d_t	max = { 128, 128, 128 };

	fp_t	x, y;

	if ( self->wf_num_till_dead > 0 )
	{
		self->wf_num_till_dead--;
		
		if ( self->wf_num_till_dead == 0 )
		{
			self->ati.ftbl->Delete( self );
			return;
		}
	}
	
	x = sin( (world_frame*10.0)*D2R ) * 64.0;
	y = cos( (world_frame*10.0)*D2R ) * 64.0;

//	self->intensity = fabs((world_frame&15)-8)*20.0;
	self->intensity = 50.0 + self->wf_num_till_dead;

//	at_player[local_player]->ati.GetOrigin( at_player[local_player], porigin );
//	self->ati.SetOrigin( self, porigin );

	Vec3dMA( trypos, 64.0,  self->dir, self->origin );
	if ( !TryStraightMove( self->origin, trypos ) )
	{
		self->wf_num_till_dead = 4;
		Vec3dInit( self->dir, 0, 0, 0 );
	}
	else
	{
		Vec3dCopy( self->origin, trypos );
	}

	Vec3dCopy( self->wf_info.origin, self->origin );

//	self->wf_info.origin[0] += x;
//	self->wf_info.origin[2] += y;

	Vec3dCopy( self->wf_info.color, self->color );
	Vec3dAdd( self->wf_info.min, self->wf_info.origin, min );
	Vec3dAdd( self->wf_info.max, self->wf_info.origin, max );
	self->wf_info.value = self->intensity;

	
	if( ((sh_var_t*)li.SH_GetVar( "dlight_hack" )) ->ivalue )
	{
//		li.GI_RegisterDLightForWF( &self->wf_info );
		li.GI_RegisterWFInfo( WFInfoType_dlight, &self->wf_info );
//		li.GI_RegisterWFInfo( WFInfoType_dlight, &self->wf_info );
	}

	//
	// setup halo
	//
	Vec3dCopy( self->halo.origin, self->origin );
	Vec3dCopy( self->halo.color, self->wf_info.color );
	self->halo.gltex_res = li.GI_ResourceSearch( "gltex.fx.halo1" );
	self->halo.width = 4.0;
	self->halo.height = 4.0;
	self->halo.rotate = world_frame*16.0;
	if ( self->wf_num_till_dead <= 3 && self->wf_num_till_dead != 0 )
	{
		if( self->wf_num_till_dead == 3 )
			li.SND_AddJob( li.GI_ResourceSearch( "sound.env.test1" ), SND_TYPE_NORMAL, 1, self->origin );

		self->halo.width *= (self->wf_num_till_dead)*4.0;
		self->halo.height *= (self->wf_num_till_dead)*4.0;
	}
	if( ((sh_var_t*)li.SH_GetVar( "dlight_hack" )) ->ivalue )
	{
		li.GI_RegisterWFInfo( WFInfoType_sprite, &self->halo );
	}	
}

// this is only temporary
static void NotFound( char *name )
{
	__error( "key %s not found\n", name );
}

static void InitFromClass( at_dlight_t *self, hobj_t *cls )
{
	hpair_t		*pair;
	hobj_t		*pref;

	strncpy( self->ati.type, cls->type, ATI_STRINGSIZE );
	strncpy( self->ati.name, cls->name, ATI_STRINGSIZE );
	self->ati.type[ATI_STRINGSIZE-1] = '\0';
	self->ati.name[ATI_STRINGSIZE-1] = '\0';

	pref = FindClass( at_prefs, cls->type );

	pair = FindHPair( cls, "origin" );
	if( pair )
	{
		HPairCastToVec3d( self->origin, pair );
	} else
		NotFound( "origin" );

	pair = FindHPair( cls, "color" );
	if( pair )
	{
		HPairCastToVec3d( self->color, pair );
	} else
		NotFound( "color" );

	pair = FindHPair( cls, "value" );
	if( pair )
	{
		HPairCastToFloat( &self->intensity, pair );
	} else  
		NotFound( "value" );

}

/*
  ==============================
  NEW_dlight

  ==============================
*/
static bool_t		ati_ftbl_dlight_valid = false;
static ati_ftbl_t	ati_ftbl_dlight;

at_dlight_t *NEW_dlight()
{
	at_dlight_t	*obj;

	if ( ati_ftbl_dlight_valid == false )
	{
		ati_ftbl_dlight_valid = true;	

		memset( &ati_ftbl_dlight, 0, sizeof( ati_ftbl_t ) );
		ati_ftbl_dlight.GetOrigin = GetOrigin;
		ati_ftbl_dlight.SetOrigin = SetOrigin;
		ati_ftbl_dlight.Delete = Delete;
		ati_ftbl_dlight.Destroy = Destroy;
		ati_ftbl_dlight.Com = Com;
		ati_ftbl_dlight.Think1 = Think1;
		ati_ftbl_dlight.InitFromClass = InitFromClass;
	}
	
	obj = NEWTYPE( at_dlight_t );
	
	obj->ati.state = ATIState_none;
	obj->ati.ftbl = &ati_ftbl_dlight;

	// test
	obj->wf_num_till_dead = 0;
	
	return obj;
}




