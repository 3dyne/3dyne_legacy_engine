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



// at_spectator.c

#include <math.h>

#include "sv_state.h"

#include "l_head.h"
#include "l_defs.h"

#include "s_mem.h"
#include "lib_hobj.h"
#include "shock.h"

#include "events.h"

typedef struct at_spectator_s
{
	ati_t		atif;

	viewStyle	style;
	vec3d_t		origin;

//	vec3d_t		dir;
	fp_t		lon;
	fp_t		lat;
	fp_t		yaw;
     
} at_spectator_t;

/*
  ==============================
  ComSpectator

  ==============================
*/
static const char * ComSpectator( ati_t *atif, const char *in )
{
	return "ok";
}

/*
  ==============================
  RunSpectator

  ==============================
*/

static void InitSpectatorFromClass( at_spectator_t *spec, hobj_t *spec_obj )
{
	hpair_t		*pair;

	EasyFindVec3d( spec->origin, spec_obj, "origin" );
	
	pair = FindHPair( spec_obj, "style" );
	if ( !pair )
		__error( "missing key 'style' in spectator object\n" );
	
	spec->style = viewStyle_none;

	if ( !strcmp( pair->value, "free" ) )
	{
		vec3d_t		dir;

		spec->style = viewStyle_spectator;
		
		EasyFindVec3d( dir, spec_obj, "dir" );
		Vec3dUnify( dir );

		spec->lat = asin(-dir[1])*(180.0/M_PI);
		spec->lon = atan2(dir[0],dir[2])*(180.0/M_PI);

//		__named_message( "dir: %f %f\n", spec->lon, spec->lat );
	}
	else if ( !strcmp( pair->value, "sky" ) )
	{
		spec->style = viewStyle_sky;
	}
}

static void RunSpectator( ati_t *atif, atiRun run )
{
	at_spectator_t	*spec;

	spec = ADDR_OF_BASE( at_spectator_t, atif, atif );	

	if ( run == atiRun_think1 )
	{
		if ( atif->frame_count == 0 )
		{
			InitSpectatorFromClass( spec, atif->ati_obj );
			L_AtiRegisterId( atif );
		}
	}

	else if ( run == atiRun_think2 )
	{
		if ( ( atif->frame_count&31) == 0 )
		{
			if ( spec->style != viewStyle_none )
			{
				byte_iter_t	frm;
				
				PackFrame_Begin( bi_out, &frm );
				ByteIter_Packi( &frm, atif->id );
				ByteIter_Packi( &frm, eventType_client_typeinfo_view );
				PackFrame_End( bi_out, &frm );
				
				PackFrame_Begin( bi_out, &frm );
				ByteIter_Packi( &frm, atif->id );
				ByteIter_Packi( &frm, eventType_client_obj_update_style );
				ByteIter_Packi( &frm, spec->style );
				PackFrame_End( bi_out, &frm );

				PackFrame_Begin( bi_out, &frm );
				ByteIter_Packi( &frm, atif->id );
				ByteIter_Packi( &frm, eventType_client_obj_update_pos );
				ByteIter_Pack3fv( &frm, spec->origin );
				PackFrame_End( bi_out, &frm );

				PackFrame_Begin( bi_out, &frm );
				ByteIter_Packi( &frm, atif->id );
				ByteIter_Packi( &frm, eventType_client_obj_update_angles );
				ByteIter_Packf( &frm, spec->lon );
				ByteIter_Packf( &frm, spec->lat );
				ByteIter_Packf( &frm, spec->yaw );
				PackFrame_End( bi_out, &frm );	
			}
		}
		atif->frame_count++;
	}
}

/*
  ==============================
  DestroySpectator

  ==============================
*/
static void DestroySpectator( ati_t *atif )
{
	at_spectator_t	*spec;

	DeepDestroyClass( atif->ati_obj );

	spec = ADDR_OF_BASE( at_spectator_t, atif, atif );	
	FREE( spec );
}

/*
  ==================================================
  ATI_CreateSpectator

  ==================================================
*/
static bool_t		ftbl_valid = false;
static ati_ftbl_t	ftbl;

ati_t * ATI_CreateSpectator( hobj_t *spec_obj )
{
	at_spectator_t	*spec;

	if ( strcmp( spec_obj->type, "spectator" ) )
		__error( "no 'spectator' class\n" );

	if ( ftbl_valid == false )
	{
		ftbl_valid = true;
		INITTYPE( &ftbl, ati_ftbl_t );

		ftbl.Destroy = DestroySpectator;
		ftbl.Com = ComSpectator;
		ftbl.UnpackEvent = NULL;
		ftbl.Run = RunSpectator;
	}

	spec = NEWTYPE( at_spectator_t );

	spec->atif.ftbl = &ftbl;
	spec->atif.id = StringToUnique( spec_obj->name );
	spec->atif.ati_obj = spec_obj;
	spec->atif.frame_count = 0;

	return &spec->atif;
}
