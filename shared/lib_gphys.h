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



// lib_gphys.h

#include "lib_gphysdefs.h"

void GP_Init( gphys_t *obj );
void GP_CleanUp( gphys_t *obj );

void GP_SetFuncTraceBB( gphys_t *obj, void (*TraceBB)(g_trace_t *tr, vec3d_t from, vec3d_t to, vec3d_t rel_min, vec3d_t rel_max) );
void GP_SetFuncTraceLine( gphys_t *obj, void (*TraceLine)(g_trace_t *tr, vec3d_t from, vec3d_t to) );

void GP_SetOrigin( gphys_t *obj, vec3d_t origin );
void GP_GetOrigin( gphys_t *obj, vec3d_t origin );
void GP_SetVelocity( gphys_t *obj, vec3d_t velocity );
void GP_GetVelocity( gphys_t *obj, vec3d_t velocity );
void GP_SetRelBB( gphys_t *obj, vec3d_t rel_min, vec3d_t rel_max );
void GP_GetAbsBB( gphys_t *obj, vec3d_t min, vec3d_t max );

