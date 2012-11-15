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



// g_apidefs.h

#ifndef __g_apidefs
#define __g_apidefs

#include "lib_math.h"
#include "lib_modeldefs.h"
#include "g_resourcesdefs.h"
#include "lib_psysdefs.h"
#include "u_defs.h"

#define DI_TEXT_STRINGSIZE (256)

/*
  ==================================================
  render infos

  ==================================================
*/

typedef enum
{
	viewStyle_none,
	viewStyle_player,
	viewStyle_spectator,
	viewStyle_sky,

	viewStyle_num
} viewStyle;

typedef struct ri_view_s
{
	unique_t		id;	// view is setup for this object
	bool_t			take_view;	// set by render if view is bound to object

	viewStyle		style;
	vec3d_t			origin;
	fp_t			lat, lon, yaw;
} ri_view_t;

typedef struct ri_model_s
{
	md_t			*md;
	vec3d_t			ref_origin;
	vec3d_t			ref_axis[3];
} ri_model_t;

typedef struct ri_sprite_s
{
	vec3d_t			min, max;
	vec3d_t			origin;
	color_rgba_t		rgba;
	fp_t			width;
	fp_t			height;
	fp_t			rotate;
	g_resource_t		*gltex_res;	
} ri_sprite_t;

typedef struct ri_halo_s
{
	// render input
	vec3d_t			min, max;
	vec3d_t			origin;
	color_rgba_t		rgba;
	fp_t			width;
	fp_t			height;
	fp_t			rotate;
	g_resource_t		*gltex_res;

	// render output
	bool_t			was_visible;
} ri_halo_t;

typedef struct ri_psys_s
{
	vec3d_t		min, max;	// for bsp cull
	field_particle_system_t	fpsys;
} ri_psys_t;

typedef struct ri_local_light_s
{
	vec3d_t		min, max;
	vec3d_t		origin;
	vec3d_t		color;
	fp_t		value;
} ri_local_light_t;


/*
  =============================================================================
  draw infos

  =============================================================================
*/

typedef struct di_rect_s
{
	fp_t	x;
	fp_t	y;
	fp_t	w;
	fp_t	h;
	color_rgba_t	flat_color;
	
	g_resource_t	*gltex;
	fp_t	tx;
	fp_t	ty;
	fp_t	tw;
	fp_t	th;	
} di_rect_t;

typedef struct di_text_s
{
	fp_t	x;
	fp_t	y;

	char	string[DI_TEXT_STRINGSIZE];
	vec3d_t	color; // unused now.
} di_text_t;

#endif
