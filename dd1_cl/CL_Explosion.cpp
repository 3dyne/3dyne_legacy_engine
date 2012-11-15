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



// CL_Explosion.c

#include "cl_state.h"
#include "cl_head.h"

#include "s_mem.h"
#include "shock.h"

#include "events.h"
#include "g_apidefs.h"

typedef struct cl_at_explosion_s
{
	cl_ati_t	atif;

	// private data
	vec3d_t		origin;
	explosionStyle	style;
	bool_t		is_init;
	msec_t		time_start;

	// render info
	ri_sprite_t	ri_sprite;
	ri_sprite_t	ri_sprites[5];
	
	ri_local_light_t	ri_llight;
} cl_at_explosion_t;

/*
  ==============================
  RunExplosion

  ==============================
*/
static void RunExplosion( cl_ati_t *atif )
{
	cl_at_explosion_t	*explosion;
	msec_t			time_run;
	int		i;


	explosion = ADDR_OF_BASE( cl_at_explosion_t, atif, atif );

	time_run = cl_time - explosion->time_start;

	if ( explosion->style == explosionStyle_hot1 )
	{
		if ( explosion->is_init == false )
		{

			explosion->is_init = true;
			for ( i = 0; i < 5; i++ )
			{
				vec3d_t		v;

				v[0] = explosion->origin[0] + randomf()*32.0;
				v[1] = explosion->origin[1] + randomf()*32.0;
				v[2] = explosion->origin[2] + randomf()*32.0;	
				
				Vec3dCopy( explosion->ri_sprites[i].origin, v );
				explosion->ri_sprites[i].rgba[0] = 255;
				explosion->ri_sprites[i].rgba[1] = 127;
				explosion->ri_sprites[i].rgba[2] = 127;
				explosion->ri_sprites[i].rgba[3] = 255;
				explosion->ri_sprites[i].width = 2.0+(randomf()+1.0);
				explosion->ri_sprites[i].height = explosion->ri_sprites[i].width*1.33;
				explosion->ri_sprites[i].rotate = (randomf()+1.0)*180.0;
				explosion->ri_sprites[i].gltex_res = g_api.GI_ResourceSearch( "gltex.fx.expl1" );

			}
			
		}

		if ( time_run < 2000 )
		{
			
				// explosion goes on
			for ( i = 0; i < 5; i++ )
			{
				if ( time_run < 1000 )
				{

				}
				else if ( time_run < 2000 )
				{
					fp_t	t;
					t = (time_run - 1000)/1000.0;
					explosion->ri_sprites[i].rgba[0] = 255.0*(1.0-t);
					explosion->ri_sprites[i].rgba[1] = 255.0*(1.0-t);
					explosion->ri_sprites[i].rgba[2] = 255.0*(1.0-t);
				}			       

				explosion->ri_sprites[i].rotate = 1000.0+((time_run/10)%360)*((i&1)?1.0:-1.0);
				{
					// shift explosion upward
					
					explosion->ri_sprites[i].origin[1] += cl_time_delta*32.0;
					g_api.Render_Sprite( &explosion->ri_sprites[i] );
				}
			}
			
			{
				vec3d_t		rel_min = { -128, -128, -128 };
				vec3d_t		rel_max = { 128, 128, 128 };
				//
				// set a local light
				//
				
				Vec3dCopy( explosion->ri_llight.origin, explosion->origin );
				Vec3dInit( explosion->ri_llight.color, 1, 1, 0 );
				
				if ( time_run < 1000 )
				{
					explosion->ri_llight.value = 100;
				}
				else if ( time_run < 2000 )
				{
					fp_t	t;
					t = (time_run - 1000)/1000.0;	
					explosion->ri_llight.value = 100*(1.0-t) + 20.0;
				}

				
				Vec3dAdd( explosion->ri_llight.min, explosion->origin, rel_min );
				Vec3dAdd( explosion->ri_llight.max, explosion->origin, rel_max );
				
				g_api.Render_LocalLight( &explosion->ri_llight );
			}
			
			return;
		}
	}
	else if ( explosion->style == explosionStyle_smoke1 )
	{
		if ( time_run < 2000 )
		{
			Vec3dCopy( explosion->ri_sprite.origin, explosion->origin );
			explosion->ri_sprite.rgba[0] = 0xff;
			explosion->ri_sprite.rgba[1] = 0xff;
			explosion->ri_sprite.rgba[2] = 0xff;
			
			explosion->ri_sprite.width = 4.0;
			explosion->ri_sprite.height = 4.0;
			explosion->ri_sprite.rotate = (explosion->time_start+cl_time)/8;
			explosion->ri_sprite.gltex_res = g_api.GI_ResourceSearch( "gltex.fx.smoke2" );
			
			explosion->ri_sprite.rgba[3] = (1000-abs(time_run-1000))/4;
			
			g_api.Render_Sprite( &explosion->ri_sprite );
			
			return;
		}
	}
	else if ( explosion->style == explosionStyle_blood1 )
	{
		if ( time_run < 2000 )
		{
			Vec3dCopy( explosion->ri_sprite.origin, explosion->origin );
			explosion->ri_sprite.rgba[0] = 0xff;
			explosion->ri_sprite.rgba[1] = 0x00;
			explosion->ri_sprite.rgba[2] = 0x00;
			
			explosion->ri_sprite.width = 4.0;
			explosion->ri_sprite.height = 4.0;
			explosion->ri_sprite.rotate = (explosion->time_start+cl_time)/8;
			explosion->ri_sprite.gltex_res = g_api.GI_ResourceSearch( "gltex.fx.smoke2" );
			
			explosion->ri_sprite.rgba[3] = (1000-abs(time_run-1000))/4;
			
			g_api.Render_Sprite( &explosion->ri_sprite );
			
			return;
		}
	}
	
	else if ( explosion->style == explosionStyle_teleport_send )
	{
		if ( time_run < 1000 )
		{
			Vec3dCopy( explosion->ri_sprite.origin, explosion->origin );
			explosion->ri_sprite.rgba[0] = 0xff;
			explosion->ri_sprite.rgba[1] = 0xff;
			explosion->ri_sprite.rgba[2] = 0xff;
			
			if ( time_run < 100 )
			{
				fp_t	t;
				t = (time_run - 0)/100.0;
					
				explosion->ri_sprite.width = 4.0*(t);
				explosion->ri_sprite.height = 4.0*(t)*1.33;
				
			}
			else if ( time_run < 1000 )
			{
				fp_t	t;
				t = (time_run - 100)/900.0;
				
				explosion->ri_sprite.width = 4.0*(t-1.0);
				explosion->ri_sprite.height = 4.0*(t-1.0)*1.33;	
	
		}

			if ( time_run < 500 )
			{
				explosion->ri_sprite.gltex_res = g_api.GI_ResourceSearch( "gltex.fx.green1" );	
			}
			else
			{
				explosion->ri_sprite.gltex_res = g_api.GI_ResourceSearch( "gltex.fx.green2" );				
			}

			explosion->ri_sprite.rotate = 1000.0-(time_run/3.6);

			
			explosion->ri_sprite.rgba[3] = 0xff;
			
			g_api.Render_Sprite( &explosion->ri_sprite );

			{
				vec3d_t		rel_min = { -128, -128, -128 };
				vec3d_t		rel_max = { 128, 128, 128 };
				//
				// set a local light
				//
				
				Vec3dCopy( explosion->ri_llight.origin, explosion->origin );
				Vec3dInit( explosion->ri_llight.color, 0, 1, 0 );
				explosion->ri_llight.value = 100;
				
				Vec3dAdd( explosion->ri_llight.min, explosion->origin, rel_min );
				Vec3dAdd( explosion->ri_llight.max, explosion->origin, rel_max );
				
				g_api.Render_LocalLight( &explosion->ri_llight );
			}

			return;
		}			
	}
	
	else if ( explosion->style == explosionStyle_teleport_recv )
	{
		if ( time_run < 1000 )
		{
			Vec3dCopy( explosion->ri_sprite.origin, explosion->origin );
			explosion->ri_sprite.rgba[0] = 0xff;
			explosion->ri_sprite.rgba[1] = 0xff;
			explosion->ri_sprite.rgba[2] = 0xff;
			
			if ( time_run < 100 )
			{
				fp_t	t;
				t = (time_run - 0)/100.0;
					
				explosion->ri_sprite.width = 4.0*(t);
				explosion->ri_sprite.height = 4.0*(t)*1.33;
				
			}
			else if ( time_run < 1000 )
			{
				fp_t	t;
				t = (time_run - 100)/900.0;
				
				explosion->ri_sprite.width = 4.0*(t-1.0);
				explosion->ri_sprite.height = 4.0*(t-1.0)*1.33;	
	
		}

			if ( time_run < 500 )
			{
				explosion->ri_sprite.gltex_res = g_api.GI_ResourceSearch( "gltex.fx.blue1" );	
			}
			else
			{
				explosion->ri_sprite.gltex_res = g_api.GI_ResourceSearch( "gltex.fx.blue2" );				
			}

			explosion->ri_sprite.rotate = 1000.0-(time_run/3.6);

			
			explosion->ri_sprite.rgba[3] = 0xff;
			
			g_api.Render_Sprite( &explosion->ri_sprite );

			{
				vec3d_t		rel_min = { -128, -128, -128 };
				vec3d_t		rel_max = { 128, 128, 128 };
				//
				// set a local light
				//
				
				Vec3dCopy( explosion->ri_llight.origin, explosion->origin );
				Vec3dInit( explosion->ri_llight.color, 0, 0, 1 );
				explosion->ri_llight.value = 100;
				
				Vec3dAdd( explosion->ri_llight.min, explosion->origin, rel_min );
				Vec3dAdd( explosion->ri_llight.max, explosion->origin, rel_max );
				
				g_api.Render_LocalLight( &explosion->ri_llight );
			}

			return;
		}
	}

	else if ( explosion->style == explosionStyle_stardust )
	{
		if ( explosion->is_init == false )
		{

			explosion->is_init = true;
			for ( i = 0; i < 5; i++ )
			{
				vec3d_t		v;

				v[0] = explosion->origin[0] + randomf()*32.0;
				v[1] = explosion->origin[1] + randomf()*32.0;
				v[2] = explosion->origin[2] + randomf()*32.0;	
				
				Vec3dCopy( explosion->ri_sprites[i].origin, v );
				explosion->ri_sprites[i].rgba[0] = 255;
				explosion->ri_sprites[i].rgba[1] = 255;
				explosion->ri_sprites[i].rgba[2] = 255;
				explosion->ri_sprites[i].rgba[3] = 255;
				explosion->ri_sprites[i].width = 0.5;
				explosion->ri_sprites[i].height = 0.5;
				explosion->ri_sprites[i].rotate = (randomf()+1.0)*180.0;
				explosion->ri_sprites[i].gltex_res = g_api.GI_ResourceSearch( "gltex.fx.star1" );

			}
			
		}

		if ( time_run < 2000 )
		{
			
				// stardust goes on
			for ( i = 0; i < 5; i++ )
			{
				if ( time_run < 1000 )
				{

				}
				else if ( time_run < 2000 )
				{
					fp_t	t;
					t = (time_run - 1000)/1000.0;
					explosion->ri_sprites[i].rgba[0] = 255.0*(1.0-t);
					explosion->ri_sprites[i].rgba[1] = 255.0*(1.0-t);
					explosion->ri_sprites[i].rgba[2] = 255.0*(1.0-t);
				}			       

				explosion->ri_sprites[i].rotate = 1000.0+((time_run/10)%360)*((i&1)?1.0:-1.0);
				{
					// shift startdust upward
					
					explosion->ri_sprites[i].origin[1] += cl_time_delta*64.0;
					g_api.Render_Sprite( &explosion->ri_sprites[i] );
				}
			}
						
			return;
		}		
	}


	atif->state = clAtiState_dead;
}


/*
  ==============================
  DestroyExplosion

  ==============================
*/
static void DestroyExplosion( cl_ati_t *atif )
{
	cl_at_explosion_t	*explosion;

	explosion = ADDR_OF_BASE( cl_at_explosion_t, atif, atif );	
	FREE( explosion );	
}

/*
  ==================================================
  client explosion

  ==================================================
*/
static bool_t		ftbl_valid = false;
static cl_ati_ftbl_t	ftbl;

cl_ati_t * CL_CreateExplosion( vec3d_t origin, explosionStyle style  )
{
	cl_at_explosion_t		*explosion;

	if ( ftbl_valid == false )
	{
		ftbl_valid = true;
		INITTYPE( &ftbl, cl_ati_ftbl_t );

		ftbl.Destroy = DestroyExplosion;
		ftbl.Run = RunExplosion;
	}

	explosion = NEWTYPE( cl_at_explosion_t );
	explosion->atif.ftbl = &ftbl;

	//
	// init private data
	//

	Vec3dCopy( explosion->origin, origin );
	
	explosion->time_start = cl_time;
	explosion->style = style;

	return &explosion->atif;
}
