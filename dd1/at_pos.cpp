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

static void Delete( at_pos_t *self )
{
	L_ChangeToDead( &self->ati );
}

static void Destroy( at_pos_t *self )
{
	li.MM_Free( self );
}

static void GetOrigin( at_pos_t *self, vec3d_t vec )
{

	Vec3dCopy( vec, self->origin );
}

static void SetOrigin( at_pos_t *self, vec3d_t vec )
{
	Vec3dCopy( self->origin, vec );
}

static void Com( at_pos_t *self, char *in, char *out )
{
	li.SH_Printf( "got com string\n" );
}

static void Think1( at_pos_t *self )
{
	__named_message( "fall to sleep\n" );

	L_ChangeToSleep( &self->ati );	
}

static void InitFromClass( at_pos_t *self, hobj_t *cls )
{

	hpair_t		*pair;


	strncpy( self->ati.type, cls->type, ATI_STRINGSIZE );
	strncpy( self->ati.name, cls->name, ATI_STRINGSIZE );
	self->ati.type[ATI_STRINGSIZE-1] = '\0';
	self->ati.name[ATI_STRINGSIZE-1] = '\0';

	pair = FindHPair( cls, "origin" );
	if( pair )
		HPairCastToVec3d( self->origin, pair );

}

/*
  ==============================
  NEW_pos

  ==============================
*/
static bool_t		ati_ftbl_pos_valid = false;
static ati_ftbl_t	ati_ftbl_pos;


at_pos_t *NEW_pos( void )
{
	at_pos_t	*obj;	

	if ( ati_ftbl_pos_valid == false )
	{
		ati_ftbl_pos_valid = true;	

		memset( &ati_ftbl_pos, 0, sizeof( ati_ftbl_t ) );
		ati_ftbl_pos.GetOrigin = GetOrigin;
		ati_ftbl_pos.SetOrigin = SetOrigin;
		ati_ftbl_pos.Delete = Delete;
		ati_ftbl_pos.Destroy = Destroy;
		ati_ftbl_pos.Com = Com;
		ati_ftbl_pos.Think1 = Think1;
		ati_ftbl_pos.InitFromClass = InitFromClass;
	}	

	obj = NEWTYPE( at_pos_t );
	
	obj->ati.state = ATIState_none;
	obj->ati.ftbl = &ati_ftbl_pos;

	return obj;
}


