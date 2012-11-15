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



// lib_psysdefs.h

#ifndef __lib_psysdefs
#define __lib_psysdefs

#include "u_defs.h"
#include "lib_math.h"
#include "g_resourcesdefs.h"
#include "res_gltexdefs.h"
#include "res_lumpdefs.h"

/*
  =============================================================================
  generic particle system stuff

  =============================================================================
*/

#define PARTICLE_FACE_MAX_GRADIENTS		( 4 )

typedef enum
{
	particleFaceBlendFunc_addition,
	particleFaceBlendFunc_multiply,
	particleFaceBlendFunc_transparency,

	particleFaceBlendFunc_num
} particleFaceBlendFunc;

typedef struct particle_face_s
{
	int			gradient_num;
	particleFaceBlendFunc	blend_func;
	g_resource_t		*gltex_res;

	msec_t		msec_start[PARTICLE_FACE_MAX_GRADIENTS];
	msec_t		msec_stop[PARTICLE_FACE_MAX_GRADIENTS];
	vec3d_t		color_start[PARTICLE_FACE_MAX_GRADIENTS];
	vec3d_t		color_stop[PARTICLE_FACE_MAX_GRADIENTS];
	fp_t		size_start[PARTICLE_FACE_MAX_GRADIENTS];
	fp_t		size_stop[PARTICLE_FACE_MAX_GRADIENTS];
       	
} particle_face_t;

typedef struct particle_system_s
{
	int		max_particle_num;
	int		spawn_rate;
	int		particle_num;
} particle_system_t;

typedef enum
{
	ParticleSource_none,
	ParticleSource_point,
	ParticleSource_boundbox,

	ParticleSource_num
} particleSource;

typedef struct particle_source_s
{
	particleSource	type;
	vec3d_t		min, max;
	vec3d_t		origin;
} particle_source_t;


/*
  =============================================================================
  field particle system

  =============================================================================
*/

typedef struct field_particle_factory_s
{
	fp_t		speed_scale;
	msec_t		msec_life;	
} field_particle_factory_t;

typedef struct field_particle_s
{
	vec3d_t		pos;
	vec3d_t		dir;
	fp_t		speed_scale;
	msec_t		msec_left;
	particle_face_t	*face;
	struct field_particle_s		*next;
} field_particle_t;

typedef struct field_cell_s
{
	vec3d_t		vec;
} field_cell_t;

typedef struct field_space_s
{
	int	xsize;
	int	ysize;
	int	zsize;

	field_cell_t	cells[16];	// variable
} field_space_t;

typedef struct field_space_to_world_transform_s
{
	vec3d_t		scale;
	vec3d_t		shift;
} field_space_to_world_transform_t;

typedef struct field_particle_system_s
{
	// field stuff
	field_space_t				*fieldspace;
	field_particle_factory_t		factory;
	
	field_particle_t	*particle_head;

	field_space_to_world_transform_t	trans;
	
	// generic particle stuff
	particle_system_t			psys;
	particle_face_t				face;
	particle_source_t			source;
} field_particle_system_t;

#endif
