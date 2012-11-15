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


static void Delete( at_waypoint_t *self )
{
	L_ChangeToDead( &self->ati );
}

static void Destroy( at_waypoint_t *self )
{
	li.MM_Free( self );
}

static void GetOrigin( at_waypoint_t *self, vec3d_t vec )
{

	Vec3dCopy( vec, self->origin );
}

static void SetOrigin( at_waypoint_t *self, vec3d_t vec )
{
	Vec3dCopy( self->origin, vec );
}

static void Com( at_waypoint_t *self, char *in, char *out )
{
	li.SH_Printf( "got com string\n" );
}

static void Think1( at_waypoint_t *self )
{


}


static void NotFound( char *name )
{
	__error( "key %s not found\n", name );
}

static void InitFromClass( at_waypoint_t *self, hobj_t *cls )
{
	hpair_t		*pair;
	hobj_t		*pref;

	__named_message( "\n" );

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
	

	L_InsertWaypoint( self );
}

/*
  ==============================
  NEW_waypoint

  ==============================
*/

static bool_t		ati_ftbl_waypoint_valid = false;
static ati_ftbl_t	ati_ftbl_waypoint;

at_waypoint_t *NEW_waypoint()
{
	at_waypoint_t	*obj;	

	if ( ati_ftbl_waypoint_valid == false )
	{
		ati_ftbl_waypoint_valid = true;	

		memset( &ati_ftbl_waypoint, 0, sizeof( ati_ftbl_t ) );
		ati_ftbl_waypoint.GetOrigin = GetOrigin;
		ati_ftbl_waypoint.SetOrigin = SetOrigin;
		ati_ftbl_waypoint.Delete = Delete;
		ati_ftbl_waypoint.Destroy = Destroy;
		ati_ftbl_waypoint.Com = Com;
		ati_ftbl_waypoint.Think1 = Think1;
		ati_ftbl_waypoint.InitFromClass = InitFromClass;
	}

	obj = NEWTYPE( at_waypoint_t );
	
	obj->ati.state = ATIState_none;
	obj->ati.ftbl = &ati_ftbl_waypoint;

	return obj;
}
