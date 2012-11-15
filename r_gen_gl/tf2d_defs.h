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



// tf2d_defs.h

#ifndef __tf2d_defs
#define __tf2d_defs

#include "lib_math.h"
#include "lib_containerdefs.h"
#include "u_defs.h"	// color_rgba_t

union tf2d_u;

typedef enum
{
	tf2dType_none = 0,
	tf2dType_scale,
	tf2dType_shift,
	tf2dType_matrix,

	tf2dType_num
} tf2dType;

typedef struct tf2d_any_s
{
	tf2dType	type;
	void (*transform_func)( union tf2d_u *tf2d, vec2d_t out, vec2d_t in );
} tf2d_any_t;

typedef struct tf2d_scale_s
{
	tf2dType	type;
	void (*transform_func)( union tf2d_u *tf2d, vec2d_t out, vec2d_t in );
	vec2d_t		scale;
} tf2d_scale_t;

typedef struct tf2d_shift_s
{
	tf2dType	type;
	void (*transform_func)( union tf2d_u *tf2d, vec2d_t out, vec2d_t in );
	vec2d_t		shift;
} tf2d_shift_t;

typedef struct tf2d_matrix_s
{
	tf2dType	type;
	void (*transform_func)( union tf2d_u *tf2d, vec2d_t out, vec2d_t in );
	vec2d_t		vecs[2];
} tf2d_matrix_t;

typedef union tf2d_u
{
	tf2d_any_t	any;
	tf2d_scale_t	scale;
	tf2d_shift_t	shift;
	tf2d_matrix_t	matrix;
} tf2d_t;

typedef struct tf2d_stack_s
{
	u_list_t	tf2d_list;
} tf2d_stack_t;

#endif
