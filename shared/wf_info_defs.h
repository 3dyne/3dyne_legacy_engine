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



// wf_info_defs.h

#ifndef __wf_info_defs
#define __wf_info_defs

#include "lib_math.h"
#include "lib_modeldefs.h"
#include "g_resourcesdefs.h"
#include "lib_psysdefs.h"

typedef enum
{
	WFInfoType_model,
	WFInfoType_dlight,
	WFInfoType_sprite,
	WFInfoType_fpsys,
	WFInfoType_light,
	WFInfoType_shape,

	WFInfoType_num
} wfInfoType;

typedef struct wf_model_info_s
{
	md_coordsys_t	*cs;
	vec3d_t		ref_origin;
	vec3d_t		ref_axis[3];
} wf_model_info_t;

#if 1
typedef struct wf_dlight_info_s
{
	vec3d_t		min, max;	// for fast distance cull, bsptree cull
	vec3d_t		origin;
	vec3d_t		color;	
	fp_t		value;
} wf_dlight_info_t;
#endif

typedef struct wf_sprite_info_s
{
	vec3d_t		min, max;
	vec3d_t		origin;
	vec3d_t		color;
	fp_t		width, height;
	fp_t		rotate;		// 0..359
	g_resource_t	*gltex_res;
} wf_sprite_info_t;

typedef struct wf_fpsys_info_s
{
	vec3d_t		min, max;	// for fast bsptree cull
	field_particle_system_t		*fpsys;
} wf_fpsys_info_t;

typedef struct wf_light_info_s
{
	unique_t	lightsource_id;
	int		state;		// 0: off / 1: on
	char		*style_string;	// style
} wf_light_info_t;

typedef struct wf_shape_info_s
{
	int		ref_count;		// number of jobs still running with this info
	unique_t	shape_id;
	int		order_count;		// set by customer, jobs will run till order_id changes
	vec3d_t		color;
	char		*style_string;
} wf_shape_info_t;

#endif
