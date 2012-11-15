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



// SV_Catapult.c

#include "sv_state.h"

#include "l_head.h"
#include "l_defs.h"

#include "s_mem.h"
#include "lib_hobj.h"
#include "shock.h"

#include "events.h"

typedef struct at_catapult_s
{
	ati_t		atif;

	// private data
	vec3d_t		origin;
	vec3d_t		velocity;

	vec3d_t		rel_min;
	vec3d_t		rel_max;
} at_catapult_t;

/*
  ==============================
  RunCatapult

  ==============================
*/
static void InitCatapultFromClass( at_catapult_t *cata, hobj_t *cata_obj )
{
	EasyFindVec3d( cata->origin, cata_obj, "origin" );
	EasyFindVec3d( cata->velocity, cata_obj, "velocity" );

	Vec3dInit( cata->rel_min, -8, -8, -8 );
	Vec3dInit( cata->rel_max, 8, 8, 8 );
} 

static void CatapultCollide_func( ati_t *self, ati_t *other )
{
	at_catapult_t *cata;
	cata = ADDR_OF_BASE( at_catapult_t, atif, self );

	// catapult has only effect if object is on ground to avoid
	// that it get stuck in due to set origin
	if ( other->ftbl->Geti( other, atiP_is_ground ) )
	{
		other->ftbl->Set3fv( other, atiP_origin3fv, cata->origin );
		other->ftbl->Set3fv( other, atiP_velocity3fv, cata->velocity );
	}
}

static void RunCatapult( ati_t *atif, atiRun run )
{
	at_catapult_t *cata;
	cata = ADDR_OF_BASE( at_catapult_t, atif, atif );

	if ( run == atiRun_think1 )
	{
		if ( atif->frame_count == 0 )
		{
			// first call, init at_item
			InitCatapultFromClass( cata, atif->ati_obj );
			
			// make id public
			L_AtiRegisterId( atif );			
		}

		//
		// add catapult to collide map
		// 
		L_CollideMapAddAtiWithRelBoundBox( atif, cata->origin, cata->rel_min, cata->rel_max );
	}
	else if ( run == atiRun_think2 )
	{
		L_CollideMapSelectAllAtiInBoundBox( atif, CatapultCollide_func );

		atif->frame_count++;
	}		
}

/*
  ==============================
  DestroyCatapult

  ==============================
*/
static void DestroyCatapult( ati_t *atif )
{
	at_catapult_t *cata;

	DeepDestroyClass( atif->ati_obj );

	cata = ADDR_OF_BASE( at_catapult_t, atif, atif );
	FREE( cata );
}

/*
  ==================================================
  ATI_CreateCatapult

  ==================================================
*/
static bool_t		ftbl_valid = false;
static ati_ftbl_t	ftbl;

ati_t * ATI_CreateCatapult( hobj_t *cata_obj )
{
	at_catapult_t	*cata;

	if ( ftbl_valid == false )
	{
		ftbl_valid = true;
		INITTYPE( &ftbl, ati_ftbl_t );

		ftbl.Destroy = DestroyCatapult;
		ftbl.Run = RunCatapult;
	}

	cata = NEWTYPE( at_catapult_t );

	cata->atif.ftbl = &ftbl;
	cata->atif.id = StringToUnique( cata_obj->name );
	cata->atif.ati_obj = cata_obj;
	cata->atif.frame_count = 0;

	return &cata->atif;
}
