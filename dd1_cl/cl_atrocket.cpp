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



// cl_atrocket.c

#include "cl_state.h"
#include "cl_head.h"

#include "s_mem.h"
#include "shock.h"

#include "events.h"

typedef struct cl_at_rocket_s
{
	cl_ati_t	atif;

	// first run setup model ...
	bool_t		init;

	// origin, setup by ipo
	vec3d_t		origin;

	// events from server
	bool_t		u_ipo_mask;
	vec3d_t		u_ipo1;
	vec3d_t		u_ipo2;

	bool_t		u_angles_mask;
	fp_t		u_lon, u_lat, u_yaw;

	bool_t		u_style_mask;
	rocketStyle	u_style;

	// ipo
	vec3d_t		ipo1;
	vec3d_t		ipo2;

	// angles
	fp_t		lon;
	fp_t		lat;
	fp_t		yaw;	// spin

	// style
	rocketStyle	style;

	// render info
	ri_sprite_t	ri_sprite;
	ri_model_t	ri_model;
	ri_local_light_t	ri_llight;
	
} cl_at_rocket_t;

/*
  ==============================
  UnpackEventRocket

  ==============================
*/
static void UnpackEventRocket( cl_ati_t *atif, unique_t id, eventType type, byte_iter_t *bi )
{
	cl_at_rocket_t	*rocket;

	rocket = ADDR_OF_BASE( cl_at_rocket_t, atif, atif );	
	
	if ( type == eventType_client_obj_update_ipo )
	{
		rocket->u_ipo_mask = true;
		ByteIter_Unpack3fv( bi, rocket->u_ipo1 );
		ByteIter_Unpack3fv( bi, rocket->u_ipo2 );
	}
	else if ( type == eventType_client_obj_update_angles )
	{
		rocket->u_angles_mask = true;
		rocket->u_lon = ByteIter_Unpackf( bi );
		rocket->u_lat = ByteIter_Unpackf( bi );
		rocket->u_yaw = ByteIter_Unpackf( bi );
	}
	else if ( type == eventType_client_obj_update_style )
	{
		rocket->u_style_mask = true;
		rocket->u_style = (rocketStyle)ByteIter_Unpacki( bi );
	}
	else
	{
		__error( "unkown event %d for client object '#%u'\n", type, id );
	}	
}

/*
  ==============================
  RunRocket

  ==============================
*/
static void RunRocket( cl_ati_t *atif )
{
	cl_at_rocket_t	*rocket;

	rocket = ADDR_OF_BASE( cl_at_rocket_t, atif, atif );	

	if ( !rocket->init )
	{
		int		i;
		vec3d_t         axis[3] = { {1,0,0}, {0,1,0}, {0,0,1} };                
		
		rocket->init = true;

		rocket->ri_model.md = CL_CreateModelFromResourceName( "rocket1" );		
		for ( i = 0; i < 3; i++ )                                               
			Vec3dCopy( rocket->ri_model.ref_axis[i], axis[i] );                
	}

	if ( rocket->u_ipo_mask )
	{
		rocket->u_ipo_mask = false;
		Vec3dCopy( rocket->ipo1, rocket->u_ipo1 );
		Vec3dCopy( rocket->ipo2, rocket->u_ipo2 );

		{
			cl_ati_t	*expl;
			expl = CL_CreateExplosion( rocket->ipo1, explosionStyle_smoke1 );
			U_ListInsertAtHead( &cl_ati_wakeup_list, expl );
		}
	}

	if ( rocket->u_angles_mask )
	{
		rocket->u_angles_mask = false;
		
		rocket->lon = rocket->u_lon;
		rocket->lat = rocket->u_lat;		
	}

	if ( rocket->u_style_mask )
	{
		rocket->u_style_mask = false;
		rocket->style = rocket->u_style;
	}

	// do spin
	{
		Matrix3SetupRotate( rocket->ri_model.ref_axis, rocket->u_lat*D2R, (cl_time_begin_frame/3.6)*D2R, rocket->u_lon*D2R );
	}

	//
	// do ipo
	//
	{
		fp_t		scale;
		vec3d_t		delta;

//		Vec3dPrint( view->ipo1 );
//		Vec3dPrint( view->ipo2 );

		scale = (cl_sv_time)/(cl_sv_time_delta);

//		printf( "scale: %f\n", scale );

		Vec3dSub( delta, rocket->ipo2, rocket->ipo1 );
		Vec3dMA( rocket->origin, scale, delta, rocket->ipo1 );		
	}
	

	if ( rocket->style == rocketStyle_fly )
	{
		// model
		Vec3dCopy( rocket->ri_model.ref_origin, rocket->origin );                    
		g_api.Render_Model( &rocket->ri_model );

		// nozzle
		{
			Vec3dMA( rocket->ri_sprite.origin, -16.0, rocket->ri_model.ref_axis[0], rocket->origin );
		}
		rocket->ri_sprite.rgba[0] = 0xaf;
		rocket->ri_sprite.rgba[1] = 0xaf;
		rocket->ri_sprite.rgba[2] = 0xff;
		rocket->ri_sprite.rgba[3] = 255;
		rocket->ri_sprite.width = 2.0;
		rocket->ri_sprite.height = 2.0;
		rocket->ri_sprite.rotate = cl_time_begin_frame;
		rocket->ri_sprite.gltex_res = g_api.GI_ResourceSearch( "gltex.fx.halo1" );

		g_api.Render_Sprite( &rocket->ri_sprite );

		// local light
		{

			vec3d_t		rel_min = { -128, -128, -128 };
			vec3d_t		rel_max = { 128, 128, 128 };
				//
				// set a local light
				//
		       
			
			Vec3dCopy( rocket->ri_llight.origin, rocket->origin );
			Vec3dInit( rocket->ri_llight.color, 1.0, 0.7, 0.0 );
			rocket->ri_llight.value = 100;
			
			Vec3dAdd( rocket->ri_llight.min, rocket->origin, rel_min );
			Vec3dAdd( rocket->ri_llight.max, rocket->origin, rel_max );
			
			g_api.Render_LocalLight( &rocket->ri_llight );	
		}
	}

	if ( rocket->style == rocketStyle_impact )
	{
		{
			cl_ati_t	*expl;
			vec3d_t		fly_dir;
			vec3d_t		expl_pos;

			Vec3dSub( fly_dir, rocket->ipo2, rocket->ipo1 );

			Vec3dUnify( fly_dir );
			Vec3dMA( expl_pos, -64.0, fly_dir, rocket->ipo2 );

			expl = CL_CreateExplosion( expl_pos, explosionStyle_hot1 );
			U_ListInsertAtHead( &cl_ati_wakeup_list, expl );
//			Util_SpawnTmpObject( expl );
		}		
		rocket->style = rocketStyle_none;
	}
	
}

/*
  ==============================
  DestroyRocket

  ==============================
*/
static void DestroyRocket( cl_ati_t *atif )
{
	cl_at_rocket_t	*rocket;

	rocket = ADDR_OF_BASE( cl_at_rocket_t, atif, atif );	
	FREE( rocket );
}

/*
  ==================================================
  client rocket

  ==================================================
*/
static bool_t		ftbl_valid = false;
static cl_ati_ftbl_t	ftbl;

cl_ati_t * CL_CreateRocket( unique_t id, byte_iter_t *bi )
{
	cl_at_rocket_t		*rocket;

	if ( ftbl_valid == false )
	{
		ftbl_valid = true;
		INITTYPE( &ftbl, cl_ati_ftbl_t );

		ftbl.Destroy = DestroyRocket;
		ftbl.UnpackEvent = UnpackEventRocket;
		ftbl.Run = RunRocket;
	}

	rocket = NEWTYPE( cl_at_rocket_t );
	rocket->atif.ftbl = &ftbl;
	rocket->atif.id = id;

	return &rocket->atif;
}
