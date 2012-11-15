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



// at_psys.c

#include "sv_state.h"

#include "l_head.h"
#include "l_defs.h"

#include "s_mem.h"
#include "lib_hobj.h"
#include "shock.h"

#include "events.h"

typedef struct at_psys_s
{
	ati_t		atif;

	// private data
	psysStyle	style;
	vec3d_t		origin;
	
	vec3d_t		rel_min;
	vec3d_t		rel_max;

} at_psys_t;

/*
  ==============================
  ComPsys

  ==============================
*/
static const char * ComPsys( ati_t *atif, const char *in )
{
// 	at_psys_t	*psys;

// 	psys = ADDR_OF_BASE( at_psys_t, atif, atif );

	return "ok";
}

/*
  ==============================
  RunPsys

  ==============================
*/
static void InitPsysFromClass( at_psys_t *psys, hobj_t *psys_obj )
{
	hpair_t		*pair;
	int		i;

	static struct { const char *key; psysStyle style; } resolves[] = {
		{ "fire1", psysStyle_fire1 },
		{ "fire2", psysStyle_fire2 },
		{ "fire3", psysStyle_fire3 },
		{ "fire4", psysStyle_fire4 },
		{ NULL, psysStyle_none }
	};
	
	EasyFindVec3d( psys->origin, psys_obj, "origin" );
	
	pair = FindHPair( psys_obj, "style" );
	if ( !pair )
		__error( "missing key 'style' in psys_obj '%s'\n", psys_obj->name );
	
	for ( i = 0; resolves[i].key ; i++ )
		if ( !strcmp( resolves[i].key, pair->value ) ) break;

	if ( !resolves[i].key )
	{
		__error( "unknown item style '%s'\n", pair->value );
	}
	else
	{
		psys->style = resolves[i].style;
	}

	Vec3dInit( psys->rel_min, -32.0, -32.0, -32.0 );
	Vec3dInit( psys->rel_max, 32.0, 32.0, 32.0 );
	
}

static void RunPsys( ati_t *atif, atiRun run )
{
	at_psys_t	*psys;

	psys = ADDR_OF_BASE( at_psys_t, atif, atif );
	
	if ( run == atiRun_think1 )
	{
		if ( atif->frame_count == 0 )
		{
			// first call, init at_psys
			InitPsysFromClass( psys, atif->ati_obj );

			// make id public
			L_AtiRegisterId( atif );
		}

		//
		// add psys object to collide map for some nice fx
		//
//		L_CollideMapAddAtiWithRelBoundBox( atif, psys->origin, psys->rel_min, psys->rel_max );
	}
	else if ( run == atiRun_think2 )
	{
		if ( !( atif->frame_count&31 ) )
		{
			byte_iter_t	frm;

			// emit client typeinfo event			
			PackFrame_Begin( bi_out, &frm );
			ByteIter_Packi( &frm, atif->id );
			ByteIter_Packi( &frm, eventType_client_typeinfo_psys );
			PackFrame_End( bi_out, &frm );

			// emit client item style update event
			PackFrame_Begin( bi_out, &frm );
			ByteIter_Packi( &frm, atif->id );
			ByteIter_Packi( &frm, eventType_client_obj_update_style );
			ByteIter_Packi( &frm, psys->style );
			PackFrame_End( bi_out, &frm );			
		}
		
		{
			byte_iter_t	frm;
			
			// emit client item pos update event
			PackFrame_Begin( bi_out, &frm );
			ByteIter_Packi( &frm, atif->id );
			ByteIter_Packi( &frm, eventType_client_obj_update_pos );
			ByteIter_Pack3fv( &frm, psys->origin );
			PackFrame_End( bi_out, &frm ); 
		}

		atif->frame_count++;
	}
}

/*
  ==============================
  DestroyPsys

  ==============================
*/
static void DestroyPsys( ati_t *atif )
{
	at_psys_t	*psys;

	DeepDestroyClass( atif->ati_obj );

	psys = ADDR_OF_BASE( at_psys_t, atif, atif );
	FREE( psys );
}
  
/*
  ==================================================
  ATI_CreatePsys

  ==================================================
*/
static bool_t		ftbl_valid = false;
static ati_ftbl_t	ftbl;

ati_t * ATI_CreatePsys( hobj_t *psys_obj )
{
	at_psys_t		*psys;

	if ( strcmp( psys_obj->type, "psys" ) )
	{
		__error( "type is not 'psys'\n" );
	}

	if ( ftbl_valid == false )
	{
		ftbl_valid = true;
		INITTYPE( &ftbl, ati_ftbl_t );

		ftbl.Destroy = DestroyPsys;
		ftbl.Com = ComPsys;
		ftbl.UnpackEvent = NULL;
		ftbl.Run = RunPsys;
	}
	
	psys = NEWTYPE( at_psys_t );

	psys->atif.ftbl = &ftbl;
	psys->atif.id = StringToUnique( psys_obj->name );
	psys->atif.ati_obj = psys_obj;
	psys->atif.frame_count = 0;

	return &psys->atif;
}
