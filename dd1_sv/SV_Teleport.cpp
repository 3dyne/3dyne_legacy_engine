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



// SV_Teleport.c

#include "sv_state.h"

#include "l_head.h"
#include "l_defs.h"

#include "s_mem.h"
#include "lib_hobj.h"
#include "shock.h"

#include "events.h"

typedef struct at_teleport_s
{
	ati_t		atif;

	// private data
	vec3d_t		origin;
	vec3d_t		to_pos;

	vec3d_t		rel_min;
	vec3d_t		rel_max;
} at_teleport_t;

/*
  ==============================
  RunTeleport

  ==============================
*/
static void InitTeleportFromClass( at_teleport_t *tele, hobj_t *tele_obj )
{
	EasyFindVec3d( tele->origin, tele_obj, "origin" );
	EasyFindVec3d( tele->to_pos, tele_obj, "to_pos" );

	Vec3dInit( tele->rel_min, -32, -32, -32 );
	Vec3dInit( tele->rel_max, 32, 32, 32 );
}

static void TeleportCollide_func( ati_t *self, ati_t *other )
{
	byte_iter_t	frm;
	at_teleport_t	*tele;
	vec3d_t		stop={0,0,0};
	tele = ADDR_OF_BASE( at_teleport_t, atif, self );

	other->ftbl->Set3fv( other, atiP_origin3fv, tele->to_pos );
	other->ftbl->Set3fv( other, atiP_velocity3fv, stop );

	// teleport_send gfx
	PackFrame_Begin( bi_out, &frm );
	ByteIter_Packi( &frm, UNIQUE_INVALIDE );
	ByteIter_Packi( &frm, eventType_client_gfx_create );
	ByteIter_Packi( &frm, gfxStyle_teleport_send );
	ByteIter_Pack3fv( &frm, tele->origin );
	PackFrame_End( bi_out, &frm );

	// teleport_recv gfx
	PackFrame_Begin( bi_out, &frm );
	ByteIter_Packi( &frm, UNIQUE_INVALIDE );
	ByteIter_Packi( &frm, eventType_client_gfx_create );
	ByteIter_Packi( &frm, gfxStyle_teleport_recv );
	ByteIter_Pack3fv( &frm, tele->to_pos );
	PackFrame_End( bi_out, &frm );
}

static void RunTeleport( ati_t *atif, atiRun run )
{
	at_teleport_t	*tele;
	tele = ADDR_OF_BASE( at_teleport_t, atif, atif );

	if ( run == atiRun_think1 )
	{
		if ( atif->frame_count == 0 )
		{
			InitTeleportFromClass( tele, atif->ati_obj );
			L_AtiRegisterId( atif );
		}

		L_CollideMapAddAtiWithRelBoundBox( atif, tele->origin, tele->rel_min, tele->rel_max );
	}
	else if ( run == atiRun_think2 )
	{
		L_CollideMapSelectAllAtiInBoundBox( atif, TeleportCollide_func );
		
		atif->frame_count++;		
	}
	
	
}


/*
  ==============================
  DestroyTeleport

  ==============================
*/
static void DestroyTeleport( ati_t *atif )
{
	at_teleport_t	*tele;
	tele = ADDR_OF_BASE( at_teleport_t, atif, atif );

	DeepDestroyClass( atif->ati_obj );
	FREE( tele );
}

/*
  ==================================================
  ATI_CreateTeleport

  ==================================================
*/
static bool_t		ftbl_valid = false;
static ati_ftbl_t	ftbl;

ati_t * ATI_CreateTeleport( hobj_t *tele_obj )
{
	at_teleport_t	*tele;

	if ( ftbl_valid == false )
	{
		ftbl_valid = true;
		INITTYPE( &ftbl, ati_ftbl_t );

		ftbl.Destroy = DestroyTeleport;
		ftbl.Run = RunTeleport;
	}

	tele = NEWTYPE( at_teleport_t );

	tele->atif.ftbl = &ftbl;
	tele->atif.id = StringToUnique( tele_obj->name );
	tele->atif.ati_obj = tele_obj;
	tele->atif.frame_count = 0;

	return &tele->atif;
}
