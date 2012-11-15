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



// at_light.c

#include "sv_state.h"

#include "l_head.h"
#include "l_defs.h"

#include "s_mem.h"
#include "lib_hobj.h"
#include "shock.h"

#include "events.h"

/*
  =============================================================================
  lights don't do anything on the server side for the moment
  lightsource/lightmap/shape_ambient stuff is on the client side

  they just recive is_on/is_off from connected switches 

  =============================================================================
*/

typedef enum
{
	lightStyle_none,

	lightStyle_num
} lightStyle;

typedef struct at_light_s
{
	ati_t		atif;

	// private data	
	lightStyle	style;
	bool_t		state;		// on/off
	unique_t	id_switch;	// controls light state
	vec3d_t		origin;

} at_light_t;

/*
  ==============================
  ComLight

  ==============================
*/
static const char * ComLight( ati_t *atif, const char *in )
{
// 	at_light_t	*light;

// 	light = ADDR_OF_BASE( at_light_t, atif, atif );

//	__named_message( "message '%s'\n", in );

	return "ok";
}

/*
  ==============================
  RunLight

  ==============================
*/
static void InitLightFromClass( at_light_t *light, hobj_t *light_obj )
{
	hpair_t		*pair;

	EasyFindVec3d( light->origin, light_obj, "origin" );
	
	light->id_switch = UNIQUE_INVALIDE;
	pair = FindHPair( light_obj, "switch" );
	if ( pair )
	{
		light->id_switch = StringToUnique( pair->value );
	}	
}

static void RunLight( ati_t *atif, atiRun run )
{
	at_light_t	*light;

	light = ADDR_OF_BASE( at_light_t, atif, atif );	

	if ( run == atiRun_think1 )
	{
		if ( atif->frame_count == 0 )
		{
			// first call, init light object
			InitLightFromClass( light, atif->ati_obj );

			// make id public
			L_AtiRegisterId( atif );

			//
			// add light to switchable map if it has a valid id_switch
			//
			if ( light->id_switch != UNIQUE_INVALIDE )
			{
				L_SwitchableMapAddAtiWithSwitchId( atif, light->id_switch );
			}	       
		}
	}
	else if ( run == atiRun_think2 )
	{
		atif->frame_count++;
	}
}

/*
  ==============================
  DestroyLight

  ==============================
*/
static void DestroyLight( ati_t *atif )
{
	at_light_t	*light;

	DeepDestroyClass( atif->ati_obj );

	light = ADDR_OF_BASE( at_light_t, atif, atif );	
	FREE( light );
}


/*
  ==================================================
  ATI_CreateLight

  ==================================================
*/
static bool_t		ftbl_valid = false;
static ati_ftbl_t	ftbl;

ati_t * ATI_CreateLight( hobj_t *light_obj )
{
	at_light_t	*light;

	if ( strcmp( light_obj->type, "light" ) )
	{
		__error( "light_obj is not of type 'light'\n" );
	}

	if ( ftbl_valid == false )
	{
		ftbl_valid = true;
		INITTYPE( &ftbl, ati_ftbl_t );

		ftbl.Destroy = DestroyLight;
		ftbl.Com = ComLight;
		ftbl.UnpackEvent = NULL;
		ftbl.Run = RunLight;
	}

	light = NEWTYPE( at_light_t );

	light->atif.ftbl = &ftbl;
	light->atif.id = StringToUnique( light_obj->name );
	light->atif.ati_obj = light_obj;
	light->atif.frame_count = 0;

	return &light->atif;
}	
