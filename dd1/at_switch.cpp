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



// at_switch.c

#include "l_locals.h"

#define AT_SWITCH_STYLE_USE		( 0 )
#define AT_SWITCH_STYLE_INSIDE		( 1 )

/*
  ==============================
  AT_SwitchDelete

  ==============================
*/
void AT_SwitchDelete( at_switch_t *self )
{
	L_ChangeToDead( &self->ati );
}

/*
  ==============================
  AT_SwitchDestroy

  ==============================
*/
void AT_SwitchDestroy( at_switch_t *self )
{
	li.MM_Free( self );
}

/*
  ==============================
  AT_SwitchGetOrigin

  ==============================
*/
void AT_SwitchGetOrigin( at_switch_t *self, vec3d_t vec )
{
	Vec3dCopy( vec, self->origin );
}

/*
  ==============================
  AT_SwitchSetOrigin

  ==============================
*/
void AT_SwitchSetOrigin( at_switch_t *self, vec3d_t vec )
{
	Vec3dCopy( self->origin, vec );
}

/*
  ==============================
  AT_SwitchCom

  ==============================
*/
void AT_SwitchSelectCallback( at_switch_t *self, ati_t *switch_able )
{
	char		send[256];
	char		recv[256];
//	__named_message( "[%s,%s] found switchable [%s,%s]\n", self->ati.type, self->ati.name, switch_able->type, switch_able->name );

	sprintf( send, "%d", self->state );
	switch_able->ftbl->Com( switch_able, send, recv );
}

void AT_SwitchCom( at_switch_t *self, char *in, char *out )
{
//	printf( "COM: reciever [%s,%s] got '%s'\n", self->ati.type, self->ati.name, in );

	if ( self->style == AT_SWITCH_STYLE_USE )
	{
		if ( !strcmp( in, "use" ) )
		{
			if ( self->wf_suspended != 0 )
			{
				return;
			}

			if ( self->state != 1 )
			{
				self->state = 1;
			}
			else
			{
				self->state = 0;
			}
			L_SwitchMapSelectSwitchAbleWhere_switchid( (ati_t *) self, self->self_id, (void *) AT_SwitchSelectCallback );
			self->wf_suspended = 5;
		}
	}
	else if ( self->style == AT_SWITCH_STYLE_INSIDE )
	{
		if ( !strcmp( in, "inside" ) )
		{
			self->wf_suspended |= 1; 
		}
	}
}

/*
  ==============================
  AT_SwitchThink1

  ==============================
*/
void AT_SwitchThink1( at_switch_t *self )
{
	if ( self->wf_count == 0 )
	{
		self->state = 0;
	}

	if ( self->wf_count == 1 )
	{
		L_SwitchMapSelectSwitchAbleWhere_switchid( (ati_t *) self, self->self_id, (void *) AT_SwitchSelectCallback );
	}

	if ( self->style == AT_SWITCH_STYLE_INSIDE )
	{
		if ( self->wf_suspended == 1 || self->wf_suspended == 2 )
		{
			if ( self->state != 1 )
			{
				self->state = 1;
			}
			else
			{
				self->state = 0;
			}
			L_SwitchMapSelectSwitchAbleWhere_switchid( (ati_t *) self, self->self_id, (void *) AT_SwitchSelectCallback );
		}

		if ( self->wf_suspended & 1 )
		{
			self->wf_suspended = 2;
		}
		else
		{
			self->wf_suspended = 0;
		}
	}

	else if ( self->style == AT_SWITCH_STYLE_USE )
	{
		if ( self->wf_suspended > 0 )
		{
			self->wf_suspended--;
		}	
	}

	L_CollideMapAddMoveVolume( (ati_t *) self, &self->move_vol );
	self->wf_count++;
}

/*
  ==============================
  AT_SwitchInitFromClass

  ==============================
*/

void AT_SwitchInitFromClass( at_switch_t *self, hobj_t *cls )
{
	hpair_t		*pair;

	strncpy( self->ati.type, cls->type, ATI_STRINGSIZE );
	strncpy( self->ati.name, cls->name, ATI_STRINGSIZE );
	self->ati.type[ATI_STRINGSIZE-1] = '\0';
	self->ati.name[ATI_STRINGSIZE-1] = '\0';

	self->self_id = StringToUnique( self->ati.name );

	EasyFindVec3d( self->origin, cls, "origin" );
	EasyFindInt( &self->state, cls, "state" );


	pair = FindHPair( cls, "style" );
	if ( !pair )
	{
		self->style = AT_SWITCH_STYLE_USE;	// change state on 'use' message
	}
	else if ( !strcmp( pair->value, "default" ) )
	{
		self->style = AT_SWITCH_STYLE_USE;	// change state on 'use' message
	}
	else if ( !strcmp( pair->value, "inside" ) )
	{
		self->style = AT_SWITCH_STYLE_INSIDE;	// change state as long as inside the bb
	}
	else
	{
		__warning( "unknown switch style\n" );
		self->style = AT_SWITCH_STYLE_USE;	// change state on 'use' message
	}

	pair = FindHPair( cls, "dist" );
	if ( !pair )
	{
		__warning( "no 'dist' key found, use default\n" );
		self->dist = 64.0;
	}
	else
	{
		HPairCastToFloat( &self->dist, pair );
	}

	{
		// init move_volume
		vec3d_t		min = { -1, -1, -1 };
		vec3d_t		max = { 1, 1, 1 };
		
		Vec3dScale( min, self->dist, min );
		Vec3dScale( max, self->dist, max );
		
		L_MoveVolumeSetRelativeBoundBox( &self->move_vol, min, max );
		L_MoveVolumeSetFromPos( &self->move_vol, self->origin );
		L_MoveVolumeSetToPos( &self->move_vol, self->origin );
	}

	self->state = 0;
}

/*
  ==============================
  NEW_switch

  ==============================
*/
static bool_t		ati_ftbl_switch_valid = false;
static ati_ftbl_t	ati_ftbl_switch;

at_switch_t * NEW_switch( void )
{
	at_switch_t		*obj;

	if ( ati_ftbl_switch_valid == false )
	{
		ati_ftbl_switch_valid = true;

		memset( &ati_ftbl_switch, 0, sizeof( ati_ftbl_t ) );
		ati_ftbl_switch.GetOrigin = AT_SwitchGetOrigin;
		ati_ftbl_switch.SetOrigin = AT_SwitchSetOrigin;
		ati_ftbl_switch.Delete = AT_SwitchDelete;
		ati_ftbl_switch.Destroy = AT_SwitchDestroy;
		ati_ftbl_switch.Com = AT_SwitchCom;
		ati_ftbl_switch.Think1 = AT_SwitchThink1;
		ati_ftbl_switch.InitFromClass = AT_SwitchInitFromClass;
	}

	obj = NEWTYPE( at_switch_t );

	obj->ati.state = ATIState_none;
	obj->ati.ftbl = &ati_ftbl_switch;

	return obj;
}
