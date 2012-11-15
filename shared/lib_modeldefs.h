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



// lib_modeldefs.h

#ifndef __lib_modeldefs
#define __lib_modeldefs

#include "lib_math.h"
#include "lib_containerdefs.h"

#include "g_resourcesdefs.h"

typedef struct g_dkeys_s
{
	char	name[64];
	int	step_num;
	int	msecs_per_step;
	void	*steps;
} md_move_t;

typedef struct md_submodel_s
{
	hobj_t		*submodel_obj;

	vec3d_t		origin;
	vec3d_t		scale;
	fp_t		lat, lon, yaw;

	g_resource_t	*res_submd;
	g_resource_t	*res_gltex;
} md_submodel_t;

typedef struct md_coordsys_s
{
	hobj_t		*coordsys_obj;	// property of resource

	// relative parameters, set by life
	md_move_t	*current_move;
	unsigned int		current_step;

	// absolute parameters, set by SetupModel
	vec3d_t		origin;
	vec3d_t		axis[3];

	u_list_t	*child_list;
	u_list_t	*submodel_list;
} md_coordsys_t;

typedef struct md_s
{
	md_coordsys_t	*cs_root;
	u_list_t	coordsys_list;
	u_list_t	submodel_list;
	
	g_resource_t	*md_res;
} md_t;

#endif
