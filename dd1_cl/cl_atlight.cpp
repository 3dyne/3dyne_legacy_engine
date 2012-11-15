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



// cl_atlight.c

#include "cl_state.h"
#include "cl_head.h"

#include "s_mem.h"
#include "lib_hobj.h"
#include "shock.h"

#include "events.h"

typedef enum
{
	lightType_none,
	lightType_pointlight,
	lightType_spotlight,
	lightType_facelight,

	lightType_num
} lightType;

typedef enum
{
	lightStyle_none,
	lightStyle_sin_wave1,
	lightStyle_fade_in1,
	lightStyle_fade_out1,
	lightStyle_fade_in_out1,
	lightStyle_neon1,
	lightStyle_neon_broken1,
	lightStyle_arc1,
	lightStyle_fire1,

	lightStyle_num
} lightStyle;

static const char style_str_sin_wave1[] = "ABCDEFGHIJKLMNOPPONMLKJIHGFEDCBA";
static const char style_str_fade_in1[] = "ABCDEFGHIJKLMNOPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP";
static const char style_str_fade_out1[] = "PONMLKJIHGFEDCBAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
static const char style_str_neon_broken1[] = "PAAPAPPAPAAAPAPPAAAAPAAPPPPAPAAAPAAPAAAAPAAPAAAAAAAAAPPPP";
static const char style_str_arc1[] = "PAPAPAPAPAPAAAAAAAAAAAAAAAAAAAAAAAAPAPAPAPAAAAAAAAAAAAAPAPAAAAAAAAAAA";
static const char style_str_fire1[] = "FFFFFFFFFF8FFFFFFFF8FFFFF8F8FFF8FFFFFFFFFFFF8FFFFFFF";
#define NUM_NEON_STYLE	( 8 )
static const char *style_str_neon_styles[NUM_NEON_STYLE] = {
	"PAPAPPAPAAAPAPPAAAAPAAPPAPAAAPAAPAAAAPAAPAAAPAAPAAAP",
	"APPPAPPAPPAAAAAPAPPAAAAAAAAAAPAAAAPAAAAPAAAAAPAAAAPA",
	"AAAAAAAPAAAAAPAAAPPPPPPPPPPAAAAAAAPPPPPAPPPPPPPAAAAA",
	"APAPPPAPAAAAAPPAAAPAPPAAPAPAAAPAPPPAAAPAPPPAPAPAAAPA",
	"PPPPAPPPPPAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAPPAPPPPAPPPP",
	"PPPPPPPPPAPPPPPPPPPAPAAAAPPPPPPPAAAAAAPPPPAAAAPPPPPP",
	"AAPPPAPPAPPPAPAAPPPAPAAPAPAPAPAPPAPPPAAAPAPPAAPPAPAA",
	"PPAPAAPPAPAPAAAPAPPPAAPPAPAPAPPAPPPAPPPPAAPPAPAPAPPA" 
}; 

typedef struct cl_at_light_s
{
	cl_ati_t	atif;

	// private data	
	lightType	type;
	lightStyle	style;		

	bool_t		state;		// on/off
	bool_t		trigger;	// true if state changed by a message
	msec_t		time_last_change;	// time of the last state change, needed for style sequences
	
	int		random_choose;	// a random sub style
	
	unique_t	id_switch; // not really needed here, only used in switchable map

	// map lightsource of light
	vec3d_t		initial_color;		// original color of the lightsource
	lightsource_t	*map_lightsource;
	

	// facelight shape selection stuff
	vec3d_t		origin;
	vec3d_t		rel_min;
	vec3d_t		rel_max;

	// facelight shape id
	unique_t	id_shape;

	// halo test
	bool_t		have_halo;
	ri_halo_t	ri_halo;

} cl_at_light_t;

/*
  ==============================
  ComLight

  ==============================
*/
static const char * ComLight( cl_ati_t *atif, const char *in )
{
	cl_at_light_t	*light;

	light = ADDR_OF_BASE( cl_at_light_t, atif, atif );
	
//	__named_message( "message '%s'\n", in );
	
	if ( !strcmp( in, "is_on" ) )
	{
		if ( light->state == false )
		{
			light->state = true;
			light->trigger = true;
			light->time_last_change = cl_time_begin_frame;
		}
	}
	else if ( !strcmp( in, "is_off" ) )
	{
		if ( light->state == true )
		{
			light->state = false;
			light->trigger = true;
			light->time_last_change = cl_time_begin_frame;
		}
	}

	return "ok";
}

/*
  ==============================
  RunLight

  ==============================
*/
static void InitLightFromClass( cl_at_light_t *light, hobj_t *light_obj )
{
	hpair_t		*pair;

	if ( !strcmp( light_obj->type, "light" ) )
	{
		light->type = lightType_pointlight;
	}
	else if ( !strcmp( light_obj->type, "spotlight" ) ) 
	{
		light->type = lightType_spotlight;
	}
	else if ( !strcmp( light_obj->type, "facelight" ) )
	{
		light->type = lightType_facelight;
	}
	else
	{
		__error( "unkown light type in client object '%s'\n", light_obj->name );
	}
	
	EasyFindVec3d( light->origin, light_obj, "origin" );
	
	light->id_switch = UNIQUE_INVALIDE;
	pair = FindHPair( light_obj, "switch" );
	if ( pair )
	{
		light->id_switch = StringToUnique( pair->value );
	}

	//
	// light style
	//

	pair = FindHPair( light_obj, "style" );
	if ( !pair )
	{
		light->style = lightStyle_none;
	}
	else
	{
		// fixme: use style names
		light->style = (lightStyle)atoi( pair->value );
	}


	//
	// facelight select bound box
	//
	pair = FindHPair( light_obj, "dist" );
	if ( !pair )
	{
		Vec3dInit( light->rel_min, -24, -24, -24 );
		Vec3dInit( light->rel_max, 24, 24, 24 );
	}
	else
	{
		fp_t	d;
		HPairCastToFloat( &d, pair );

		Vec3dInit( light->rel_min, -d, -d, -d );
		Vec3dInit( light->rel_max, d, d, d );		
	}

	//
	// halo style
	//
	
	pair = FindHPair( light_obj, "halo" );
	if ( !pair )
	{
		light->have_halo = false;
	}
	else
	{
		if ( !strcmp( pair->value, "none" ) )
		{
			light->have_halo = false;
		}
		else
		{
			light->have_halo = true;
		}
	}
}

static void ShapeSelect_func( cl_ati_t *atif, unique_t id_shape )
{
	cl_at_light_t	*light;

	light = ADDR_OF_BASE( cl_at_light_t, atif, atif );
	
	// if we ain't got a map_lightsource search database
	// if id_shape is registered as id_any_light

	if( light->map_lightsource )
	{
		return;
	}
	
	light->map_lightsource = g_api.Map_SelectLightSourceWhereAnyLightID( id_shape );

	if ( light->map_lightsource )
	{
		light->id_shape = id_shape;
	}
}

static void InitLightSource( cl_at_light_t *light )
{
	light->id_shape = UNIQUE_INVALIDE;
	light->map_lightsource = NULL;

	if ( light->type == lightType_pointlight ||
	     light->type == lightType_spotlight )
	{
		light->map_lightsource = g_api.Map_SelectLightSourceWhereAnyLightID( light->atif.id );
	}
	else if ( light->type == lightType_facelight )
	{
		vec3d_t min, max;

		Vec3dAdd( min, light->origin, light->rel_min );
		Vec3dAdd( max, light->origin, light->rel_max );
		
		CL_SelectShapesInBoundBox( &light->atif, min, max, ShapeSelect_func );
		if ( !light->map_lightsource )
		{
			__warning( "no shape found for facelight '#%u'\n", light->atif.id );
		}
	}
	else
	{
		__error( "no light type\n" );
	}

	if ( !light->map_lightsource )
	{
		__warning( "can't find map lightsource for client light '#%u'\n", light->atif.id );
	}
	else 
	{
		printf( "initial lightsource color is " );
		Vec3dPrint( light->map_lightsource->color );
		Vec3dCopy( light->initial_color, light->map_lightsource->color );
	}
}

static void SetLightSource( cl_at_light_t *light, fp_t scale, vec3d_t color )
{
	if ( light->id_shape != UNIQUE_INVALIDE )
	{
		vec3d_t		flat_color;

		Vec3dInit( flat_color, scale, scale, scale );
		g_api.Map_ShapeIDSetFlatColor( light->id_shape, flat_color );
	}
	
	Vec3dScale( light->map_lightsource->color, scale, color );
	g_api.Map_LightSourceUpdate( light->map_lightsource );
}

static void RunLight( cl_ati_t *atif )
{
	cl_at_light_t	*light;
	msec_t		time_since_last_change;
	
	light = ADDR_OF_BASE( cl_at_light_t, atif, atif );
	
	if ( atif->frame_count == 0 )
	{
		// first call, init light object
		InitLightFromClass( light, atif->ati_obj );

		// search render side lightsource that is controlled by the light object
		InitLightSource( light );

		if ( !light->map_lightsource )
		{
			atif->state = clAtiState_dead;
			return;
		}

		//
		// add light to switchable map if it has a valid id_switch
		//
		if ( light->id_switch != UNIQUE_INVALIDE )
		{
			CL_SwitchableMapAddAtiWithSwitchId( atif, light->id_switch );
		}		

		// initital light state is on
		light->state = true;
	}

	time_since_last_change = cl_time_begin_frame - light->time_last_change;

	if ( light->state == true )
	{
		// check for switch on style sequence

		//
		// switch on default: style
		//		
		if ( light->style == lightStyle_none )
		{
			// just switch on the light on trigger without any style
			if ( light->trigger )
			{
				SetLightSource( light, 1.0, light->initial_color );
			}
		}

		//
		// switch on style: sin_wave1
		//
		else if ( light->style == lightStyle_sin_wave1 )
		{
			// cycle till switch off
			int	style_len;
			int	scale;

			style_len = strlen( style_str_sin_wave1 );			
			scale = (int)(style_str_sin_wave1[(time_since_last_change/32)%style_len] - 'A');
			SetLightSource( light, scale/15.0, light->initial_color );
		}

		//
		// switch on style: fade_in1, fade_in_out1
		//
		else if ( light->style == lightStyle_fade_in_out1 ||
			  light->style == lightStyle_fade_in1 )
		{
			// modulate lightsource for 1000msec with a fade in
			if ( time_since_last_change < 1000 )
			{
				int	style_len;
				int	scale;

				style_len = strlen( style_str_fade_in1 );
				scale = (int)(style_str_fade_in1[(time_since_last_change/32)%style_len] - 'A');
				SetLightSource( light, scale/15.0, light->initial_color );
			}
		}
		
		//
		// switch on style: arc1
		//
		else if ( light->style == lightStyle_arc1 )
		{
			// cycle till switch off
			int	style_len;
			int	scale;

			style_len = strlen( style_str_arc1 );			
			scale = (int)(style_str_arc1[(time_since_last_change/32)%style_len] - 'A');
			SetLightSource( light, scale/15.0, light->initial_color );
		}

		//
		// switch on style: arc1
		//
		else if ( light->style == lightStyle_fire1 )
		{
			// cycle till switch off
			int	style_len;
			int	scale;

			style_len = strlen( style_str_fire1 );			
			scale = (int)(style_str_fire1[(time_since_last_change/32)%style_len]);
			if ( scale < 'A' )
				scale -= '0';
			else
				scale -= 'A'-10;
			SetLightSource( light, scale/15.0, light->initial_color );
		}

		//
		// switch on style: neon_broken1
		//
		else if ( light->style == lightStyle_neon_broken1 )
		{
			// cycle till switch off
			int	style_len;
			int	scale;

			style_len = strlen( style_str_neon_broken1 );			
			scale = (int)(style_str_neon_broken1[(time_since_last_change/32)%style_len] - 'A');
			SetLightSource( light, scale/15.0, light->initial_color );
		}	

		//
		// switch on style: neon1
		//
		else if ( light->style == lightStyle_neon1 )
		{
			if ( light->trigger )
			{
				// choose a random neon style
				light->random_choose = rand()%NUM_NEON_STYLE;
			}

			// modulate lightsource for 1000msec with neon style
			if ( time_since_last_change < 1000 )
			{
				int	style_len;
				int	scale;
				
				style_len = strlen( style_str_neon_styles[light->random_choose] );
				scale = (int)(style_str_neon_styles[light->random_choose][(time_since_last_change/32)%style_len] - 'A');
				SetLightSource( light, scale/15.0, light->initial_color );
			}
			// then switch it on
			else if ( time_since_last_change < 2000 )
			{
				SetLightSource( light, 1.0, light->initial_color );
			}
		}		
	}
	else
	{
		// check for switch off style sequence

		//
		// switch off style: default, sin_wave1, arc1, neon_broken1, neon1
		//
		if ( light->style == lightStyle_none ||
		     light->style == lightStyle_sin_wave1 ||
		     light->style == lightStyle_fade_in1 ||
		     light->style == lightStyle_arc1 ||
		     light->style == lightStyle_fire1 ||
		     light->style == lightStyle_neon_broken1 ||
		     light->style == lightStyle_neon1 )
		{
			// just switch off the light on trigger without any style
			if ( light->trigger )
			{
				SetLightSource( light, 0.0, light->initial_color );
			}
		}

		//
		// switch off: fade_in_out1, fade_out1
		//
		else if ( light->style == lightStyle_fade_in_out1 ||
			  light->style == lightStyle_fade_out1 )
		{
			// modulate lightsource for 1000msec with a fade out
			if ( time_since_last_change < 1000 )
			{
				int	style_len;
				int	scale;

				style_len = strlen( style_str_fade_out1 );				
				scale = (int)(style_str_fade_out1[(time_since_last_change/32)%style_len] - 'A');
				SetLightSource( light, scale/15.0, light->initial_color );
			}
		}
	}

	//
	// handle halo
	//

	if ( light->have_halo )
	{
		Vec3dCopy( light->ri_halo.origin, light->origin );
		light->ri_halo.rgba[0] = 255;
		light->ri_halo.rgba[1] = 255;
		light->ri_halo.rgba[2] = 255;
		light->ri_halo.rgba[3] = 255;
		light->ri_halo.width = 1.0*0.15;
		light->ri_halo.height = 1.33*0.15;
		light->ri_halo.rotate = 0.0;
		g_api.Render_Halo( &light->ri_halo );
	}

	// clear trigger flag for 'state change' Com messages
	light->trigger = false;
	atif->frame_count++;
}

/*
  ==============================
  DestroyLight

  ==============================
*/
static void DestroyLight( cl_ati_t *atif )
{
	cl_at_light_t	*light;

	DeepDestroyClass( atif->ati_obj );

	light = ADDR_OF_BASE( cl_at_light_t, atif, atif );
	FREE( light );	
}

/*
  ==================================================
  create client light

  ==================================================
*/
static bool_t		ftbl_valid = false;
static cl_ati_ftbl_t	ftbl;

cl_ati_t * CL_CreateLight( hobj_t *obj_light )
{
	cl_at_light_t	*light;

	if ( ftbl_valid == false )
	{
		ftbl_valid = true;
		INITTYPE( &ftbl, cl_ati_ftbl_t );

		ftbl.Destroy = DestroyLight;
		// light got only local messages from switches
		ftbl.Com = ComLight;
		ftbl.Run = RunLight;
	}

	light = NEWTYPE( cl_at_light_t );
	light->atif.ftbl = &ftbl;
	light->atif.id = StringToUnique( obj_light->name );
	light->atif.ati_obj = obj_light;

	light->atif.frame_count = 0;

	return &light->atif;
}
