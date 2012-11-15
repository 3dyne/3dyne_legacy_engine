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

static void Delete( at_sound_t *self )
{
	L_ChangeToDead( &self->ati );
}

static void Destroy( at_sound_t *self )
{
	li.MM_Free( self );
}

static void GetOrigin( at_sound_t *self, vec3d_t vec )
{

	Vec3dCopy( vec, self->origin );
}

static void SetOrigin( at_sound_t *self, vec3d_t vec )
{
	Vec3dCopy( self->origin, vec );
}

static void Com( at_sound_t *self, char *in, char *out )
{
	li.SH_Printf( "got com string\n" );
}


static void Think1( at_sound_t *self )
{
	vec3d_t		porigin;
	vec3d_t		distvec;
	fp_t		dist;

//	__named_message( "sound\n" );
#if 1
	if( li.SND_JobIsActive( self->job )) // sound is already playing
		return;

	
	// get position of local player and test distance

	if( !at_players[local_player] )
	{
		__warning( "local_player == NULL\n" );
		return;
	}

	at_players[local_player]->ati.ftbl->GetOrigin( at_players[local_player], porigin );

	Vec3dSub( distvec, porigin, self->origin );
	dist = Vec3dLen( distvec );
	if( dist > 1024.0 )
		return;
	
//	printf( "dist: %f\n", dist );

	self->job = li.SND_AddJob( self->res, SND_TYPE_NORMAL, 1, self->origin );
#else
	if( self->job )
		if( li.SND_JobIsActive( self->job ))
			li.SND_JobChangeSound( self->job, NULL );
		else
			self->job = li.SND_AddJob( self->res, SND_TYPE_NORMAL, 1, self->origin );

	else
		self->job = li.SND_AddJob( self->res, SND_TYPE_NORMAL, 1, self->origin );

#endif
}




static void NotFound( char *name )
{
	__error( "key %s not found\n", name );
}

static void InitFromClass( at_sound_t *self, hobj_t *cls )
{
	hpair_t		*pair;
	hobj_t		*pref;
	char		resname[256];

	__named_message( "\n" );

	strncpy( self->ati.type, cls->type, ATI_STRINGSIZE );
	strncpy( self->ati.name, cls->name, ATI_STRINGSIZE );
	self->ati.type[ATI_STRINGSIZE-1] = '\0';
	self->ati.name[ATI_STRINGSIZE-1] = '\0';

	pair = FindHPair( cls, "origin" );
	if( !pair )
		NotFound( "origin" );
	else
		HPairCastToVec3d( self->origin, pair );

	pair = FindHPair( cls, "action" );
	if( !pair )
		NotFound( "action" );

	if( !strcmp( pair->value, "env" ))
		self->flag = 1;
	else
		__error( "no valid action\n" );
	    
	
	pair = FindHPair( cls, "sound" );
	if( !pair )
		NotFound( "sound" );

	HPairCastToString( resname, pair );

	self->res = li.GI_ResourceSearch( resname );
	self->job = 0;

}

/*
  ==============================
  NEW_sound

  ==============================
*/
static bool_t		ati_ftbl_sound_valid = false;
static ati_ftbl_t	ati_ftbl_sound;


at_sound_t *NEW_sound()
{
	at_sound_t	*obj;	

	if ( ati_ftbl_sound_valid == false )
	{
		ati_ftbl_sound_valid = true;	

		memset( &ati_ftbl_sound, 0, sizeof( ati_ftbl_t ) );
		ati_ftbl_sound.GetOrigin = GetOrigin;
		ati_ftbl_sound.SetOrigin = SetOrigin;
		ati_ftbl_sound.Delete = Delete;
		ati_ftbl_sound.Destroy = Destroy;
		ati_ftbl_sound.Com = Com;
		ati_ftbl_sound.Think1 = Think1;
		ati_ftbl_sound.InitFromClass = InitFromClass;
	}

	obj = NEWTYPE( at_sound_t );
	
	obj->ati.state = ATIState_none;
	obj->ati.ftbl = &ati_ftbl_sound;

	return obj;
}
