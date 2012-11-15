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



// u_va.h

#ifndef __u_va
#define __u_va

#include "defs.h"

void U_VACopy( vec3d_t out[], vec3d_t in[], int num );
void U_VAScale( vec3d_t inout[], int num, vec3d_t scale );
void U_VATranslate( vec3d_t inout[], int num, vec3d_t origin );
void U_VARotateMatrix( vec3d_t inout[], int num, matrix3_t mat );
void U_VARotateAxis( vec3d_t inout[], int num, vec3d_t axis[3] );
void U_VANormalize( vec3d_t inout[], int num );
void U_VACalcBB( vec3d_t in[], int num, vec3d_t min, vec3d_t max );

#endif
