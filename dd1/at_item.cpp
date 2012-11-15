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



// at_item.c

#include "l_locals.h"

/*
  ==============================
  Delete

  ==============================
*/
static void Delete( at_item_t *self )
{
	L_ChangeToDead( &self->ati );
}

/*
  ==============================
  Destroy

  ==============================
*/
static void Destroy( at_item_t *self )
{
	FREE( self );
}


/*
  ==============================
  GetOrigin

  ==============================
*/
static void GetOrigin( at_item_t *self, vec3d_t vec )
{
	Vec3dCopy( vec, self->origin );
}


/*
  ==============================
  SetOrigin

  ==============================
*/
static void SetOrigin( at_item_t *self, vec3d_t vec )
{
	Vec3dCopy( self->origin, vec );
}


/*
  ==============================
  Com

  ==============================
*/
static void Com( at_item_t *self, char *in, char *out )
{
	li.SH_Printf( "got com string\n" );
}


/*
  ==============================
  Think1

  ==============================
*/
static void Think1( at_item_t *self )
{
	int		i;
	vec3d_t		axis[3] = { {1,0,0}, {0,1,0}, {0,0,1} };

	self->wf_info.cs = self->md->cs_root;

	Vec3dCopy( self->wf_info.ref_origin, self->origin );
	for ( i = 0; i < 3; i++ )
		Vec3dCopy( self->wf_info.ref_axis[i], axis[i] );

	li.GI_RegisterWFInfo( WFInfoType_model, &self->wf_info );
}


/*
  ==============================
  Think2

  ==============================
*/
static void Think2( at_item_t *self )
{

}

/*
  ==============================
  InitFromClass

  ==============================
*/
static void NotFound( char *name )
{
	__error( "key %s not found\n", name );
}

static void InitFromClass( at_item_t *self, hobj_t *obj )
{
	hpair_t		*pair;

	pair = FindHPair( obj, "style" );
	if ( !pair )
	{
		__error( "missing key 'style' in item '%s'\n", self->ati.name );
	}
	else
	{
		if ( !strcmp( "rocket", pair->value ) )
		{
			self->md = L_CreateModelFromResourceName( "rocket1" );
			L_ModelSetMoveByName( self->md, "base_frame" );
		}
		else if ( !strcmp( "rlaunch", pair->value ) )
		{
			self->md = L_CreateModelFromResourceName( "rlaunch1" );
			L_ModelSetMoveByName( self->md, "base_frame" );
		}
		else
		{
			__error( "unknown style '%s' for item '%s'\n", pair->value, self->ati.name );
		}
	}

	strncpy( self->ati.type, obj->type, ATI_STRINGSIZE );
	strncpy( self->ati.name, obj->name, ATI_STRINGSIZE );
	self->ati.type[ATI_STRINGSIZE-1] = '\0';
	self->ati.name[ATI_STRINGSIZE-1] = '\0';

	pair = FindHPair( obj, "origin" );
	if( pair )
	{
		HPairCastToVec3d( self->origin, pair );
	} else
		NotFound( "origin" );	
}

/*
  ==============================
  NEW_item

  ==============================
*/

static bool_t	        ftbl_valid = false;
static ati_ftbl_t	ftbl;

at_item_t * NEW_item( void )
{
	at_item_t	*obj;

	if ( ftbl_valid == false )
	{
		ftbl_valid = true;

		memset( &ftbl, 0, sizeof( ati_ftbl_t ) );

		ftbl.GetOrigin = GetOrigin;
		ftbl.SetOrigin = SetOrigin;
		ftbl.Delete = Delete;
		ftbl.Destroy = Destroy;
		ftbl.Com = Com;
		ftbl.Think1 = Think1;
		ftbl.InitFromClass = InitFromClass;
	}

	obj = NEWTYPE( at_item_t );

	obj->ati.state = ATIState_none;
	obj->ati.ftbl = &ftbl;

	return obj;
}
