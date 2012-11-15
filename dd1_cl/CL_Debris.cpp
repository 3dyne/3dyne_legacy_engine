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



// CL_Debris.c

#include "cl_state.h"
#include "cl_head.h"

#include "s_mem.h"
#include "shock.h"

#include "events.h"
#include "g_apidefs.h"

typedef struct cl_at_debris_s
{
	cl_ati_t	atif;

	// private data
	debrisStyle	style;
	vec3d_t		origin;
	vec3d_t		velocity;
	msec_t		time_start;

	// render info
	ri_sprite_t	ri_sprite;

} cl_at_debris_t;

/*
  ==============================
  RunDebris

  ==============================
*/
static void RunDebris( cl_ati_t *atif )
{
	cl_at_debris_t		*debris;
	msec_t		time_run;
	vec3d_t			grav = { 0, -1, 0 };

	debris = ADDR_OF_BASE( cl_at_debris_t, atif, atif );

	time_run = cl_time - debris->time_start;

	if ( debris->style == debrisStyle_ham1 )
	{
		if ( time_run < 2000 )
		{		
			
			Vec3dMA( debris->origin, cl_time_delta, debris->velocity, debris->origin );
			Vec3dMA( debris->velocity, cl_time_delta*256.0, grav, debris->velocity );
			
			
			Vec3dCopy( debris->ri_sprite.origin, debris->origin );
			debris->ri_sprite.rgba[0] = 255;
			debris->ri_sprite.rgba[1] = 255;
			debris->ri_sprite.rgba[2] = 255;
			debris->ri_sprite.rgba[3] = 255;
			debris->ri_sprite.width = 0.25;
			debris->ri_sprite.height = 0.25;
			debris->ri_sprite.rotate = 0;
			debris->ri_sprite.gltex_res = g_api.GI_ResourceSearch( "gltex.fx.blood1" );
			g_api.Render_Sprite( &debris->ri_sprite );		

			return;
		}
	}
	else if ( debris->style == debrisStyle_tribalsmiliedead1 )
	{
		if ( time_run < 2000 )
		{		
			
			Vec3dMA( debris->origin, cl_time_delta, debris->velocity, debris->origin );
			Vec3dMA( debris->velocity, cl_time_delta*256.0, grav, debris->velocity );
			
			
			Vec3dCopy( debris->ri_sprite.origin, debris->origin );
			debris->ri_sprite.rgba[0] = 255;
			debris->ri_sprite.rgba[1] = 255;
			debris->ri_sprite.rgba[2] = 255;
			debris->ri_sprite.rgba[3] = 255;
			debris->ri_sprite.width = 2.0;
			debris->ri_sprite.height = 2.0*1.33;
			debris->ri_sprite.rotate = 0.0;
			debris->ri_sprite.gltex_res = g_api.GI_ResourceSearch( "gltex.smilie.tribal_dead1" );
			g_api.Render_Sprite( &debris->ri_sprite );		

			return;
		}		
	}
	else if ( debris->style == debrisStyle_tribalsmiliedead2 )
	{
		if ( time_run < 2000 )
		{		
			
			Vec3dMA( debris->origin, cl_time_delta, debris->velocity, debris->origin );
			Vec3dMA( debris->velocity, cl_time_delta*256.0, grav, debris->velocity );
			
			
			Vec3dCopy( debris->ri_sprite.origin, debris->origin );
			debris->ri_sprite.rgba[0] = 255;
			debris->ri_sprite.rgba[1] = 255;
			debris->ri_sprite.rgba[2] = 255;
			debris->ri_sprite.rgba[3] = 255;
			debris->ri_sprite.width = 2.0;
			debris->ri_sprite.height = 2.0*1.33;
			debris->ri_sprite.rotate = 0.0;
			debris->ri_sprite.gltex_res = g_api.GI_ResourceSearch( "gltex.smilie.tribal_dead2" );
			g_api.Render_Sprite( &debris->ri_sprite );		

			return;
		}		
	}
	else if ( debris->style == debrisStyle_tribalsmiliedead3 )
	{
		if ( time_run < 2000 )
		{		
			
			Vec3dMA( debris->origin, cl_time_delta, debris->velocity, debris->origin );
			Vec3dMA( debris->velocity, cl_time_delta*256.0, grav, debris->velocity );
			
			
			Vec3dCopy( debris->ri_sprite.origin, debris->origin );
			debris->ri_sprite.rgba[0] = 255;
			debris->ri_sprite.rgba[1] = 255;
			debris->ri_sprite.rgba[2] = 255;
			debris->ri_sprite.rgba[3] = 255;
			debris->ri_sprite.width = 2.0;
			debris->ri_sprite.height = 2.0*1.33;
			debris->ri_sprite.rotate = 0.0;
			debris->ri_sprite.gltex_res = g_api.GI_ResourceSearch( "gltex.smilie.tribal_dead3" );
			g_api.Render_Sprite( &debris->ri_sprite );		

			return;
		}		
	}
	else if ( debris->style == debrisStyle_tribalsmiliedead4 )
	{
		if ( time_run < 2000 )
		{		
			
			Vec3dMA( debris->origin, cl_time_delta, debris->velocity, debris->origin );
			Vec3dMA( debris->velocity, cl_time_delta*256.0, grav, debris->velocity );
			
			
			Vec3dCopy( debris->ri_sprite.origin, debris->origin );
			debris->ri_sprite.rgba[0] = 255;
			debris->ri_sprite.rgba[1] = 255;
			debris->ri_sprite.rgba[2] = 255;
			debris->ri_sprite.rgba[3] = 255;
			debris->ri_sprite.width = 2.0;
			debris->ri_sprite.height = 2.0*1.33;
			debris->ri_sprite.rotate = 0.0;
			debris->ri_sprite.gltex_res = g_api.GI_ResourceSearch( "gltex.smilie.tribal_dead4" );
			g_api.Render_Sprite( &debris->ri_sprite );		

			return;
		}		
	}

	atif->state = clAtiState_dead;

}

/*
  ==============================
  DestroyDebris

  ==============================
*/
static void DestroyDebris( cl_ati_t *atif )
{
	cl_at_debris_t		*debris;

	debris = ADDR_OF_BASE( cl_at_debris_t, atif, atif );
	FREE( debris );
}

/*
  ==================================================
  client debris

  ==================================================
*/
static bool_t		ftbl_valid = false;
static cl_ati_ftbl_t	ftbl;

cl_ati_t * CL_CreateDebris( vec3d_t origin, vec3d_t velocity, debrisStyle style )
{
	cl_at_debris_t		*debris;

	if ( ftbl_valid == false )
	{
		ftbl_valid = true;
		INITTYPE( &ftbl, cl_ati_ftbl_t );

		ftbl.Destroy = DestroyDebris;
		ftbl.Run = RunDebris;
	}

	debris = NEWTYPE( cl_at_debris_t );
	debris->atif.ftbl = &ftbl;

	//
	// init private data
	//

	Vec3dCopy( debris->origin, origin );
	Vec3dCopy( debris->velocity, velocity );
	debris->style = style;

	debris->time_start = cl_time;

	return &debris->atif;	
}
