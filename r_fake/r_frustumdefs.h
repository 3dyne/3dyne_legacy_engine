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



// r_frustumdefs.h

#ifndef _r_frustumdefs
#define _r_frustumdefs

#include "lib_math.h"
#include "r_math.h"

typedef enum {
	Frustum_left = 0,
	Frustum_right,
	Frustum_top,
	Frustum_bottom,
	Frustum_near
} frustumPlanes;

typedef enum {
	Frustum_cnone = 0,
	Frustum_cleft = 1,
	Frustum_cright = 2,
	Frustum_ctop = 4,
	Frustum_cbottom = 8,
	Frustum_cnear = 16,
	Frustum_ctrue = 31,
	Frustum_call = 32
} frustumClipMask;

typedef struct 
{
	vec3d_t		topleft;
	vec3d_t		topright;
	vec3d_t		bottomleft;
	vec3d_t		bottomright;
	vec3d_t		viewcenter;

	plane_t		planes[5];
} frustum_t;


#endif
