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


static void Delete( at_testobj_t *self )
{
	L_ChangeToDead( &self->ati );
}

static void Destroy( at_testobj_t *self )
{
	li.MM_Free( self );
}

static void GetOrigin( at_testobj_t *self, vec3d_t vec )
{

	Vec3dCopy( vec, self->origin );
}

static void SetOrigin( at_testobj_t *self, vec3d_t vec )
{
	Vec3dCopy( self->origin, vec );
}

static void Com( at_testobj_t *self, char *in, char *out )
{
	li.SH_Printf( "got com string\n" );
}

static void Think1( at_testobj_t *self )
{
//	__named_message( "testobj\n" );
	int		i;
	vec3d_t		axis[3] = { {1,0,0}, {0,1,0}, {0,0,1} };
	vec3d_t		porigin;
	at_waypoint_t	*p;

	if( !at_players[local_player] )
	{
		__warning( "local_player == NULL\n" );
		return;
	}	
	at_players[local_player]->ati.ftbl->GetOrigin( at_players[local_player], porigin );

#if 0

	if( fabs( ( porigin[0] - 128.0 ) - self->origin[0] ) <= 8.0 )
		self->origin[0] = porigin[0] - 128.0;
	else if( porigin[0] - 128.0 > self->origin[0] )
		self->origin[0] += 8.0;
	else if( porigin[0] - 128.0 < self->origin[0] )
		self->origin[0] -= 8.0;
	
	if( fabs( porigin[1] - self->origin[1] ) <= 8.0 )
		self->origin[1] = porigin[1];
	else if( porigin[1] > self->origin[1] )
		self->origin[1] += 8;
	else if( porigin[1] < self->origin[1] )
		self->origin[1] -= 8;
	
	if( fabs( porigin[2] - self->origin[2] ) <= 8.0 )
		self->origin[2] = porigin[2];
	else if( porigin[2] > self->origin[2] )
		self->origin[2] += 8.0;
	else if( porigin[2] < self->origin[2] )
		self->origin[2] -= 8.0;
	
	
#endif

	p = WayNetFindBest( porigin );

	if( p )
	{
		vec3d_t	vec;
//		li.SH_Printf( "waypoint: %s", p->ati.name );
		p->ati.ftbl->GetOrigin( p, vec );
		Vec3dCopy( self->origin, vec );
	}

	self->wf_info.cs = self->md->cs_root;

	Vec3dCopy( self->wf_info.ref_origin, self->origin );
	for ( i = 0; i < 3; i++ )
		Vec3dCopy( self->wf_info.ref_axis[i], axis[i] );
//	li.GI_RegisterWFInfo( WFInfoType_model, &self->wf_info );
}



static void NotFound( char *name )
{
	__error( "key %s not found\n", name );
}

static void InitFromClass( at_testobj_t *self, hobj_t *cls )
{
	hpair_t		*pair;
	hobj_t		*pref;

	self->md = L_CreateModelFromResourceName( "testguy1" );
	L_ModelSetMoveByName( self->md, "base_frame" );
	L_ModelSetMoveByName( self->md, "run1_lower_part" );
	L_ModelSetMoveByName( self->md, "run1_upper_part" );

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
	
}

/*
  ==============================
  NEW_testobj

  ==============================
*/

static bool_t		ati_ftbl_testobj_valid = false;
static ati_ftbl_t	ati_ftbl_testobj;


at_testobj_t *NEW_testobj()
{
	at_testobj_t	*obj;	

	if ( ati_ftbl_testobj_valid == false )
	{
		ati_ftbl_testobj_valid = true;	

		memset( &ati_ftbl_testobj, 0, sizeof( ati_ftbl_t ) );
		ati_ftbl_testobj.GetOrigin = GetOrigin;
		ati_ftbl_testobj.SetOrigin = SetOrigin;
		ati_ftbl_testobj.Delete = Delete;
		ati_ftbl_testobj.Destroy = Destroy;
		ati_ftbl_testobj.Com = Com;
		ati_ftbl_testobj.Think1 = Think1;
		ati_ftbl_testobj.InitFromClass = InitFromClass;
	}

	obj = NEWTYPE( at_testobj_t );
	
	obj->ati.state = ATIState_none;
	obj->ati.ftbl = &ati_ftbl_testobj;

	return obj;
}
