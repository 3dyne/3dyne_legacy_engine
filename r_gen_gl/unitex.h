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



// unitex.h

#ifndef __unitex
#define __unitex

#include "imgcache_defs.h"
#include "g_resourcesdefs.h"

#include "unitex_defs.h"

unitex_t * UniTex_NewFrom_gltexres( g_resource_t *gltex );
unitex_t * UniTex_NewFrom_subimage( ic_subimage_t *sub );

g_resource_t * UniTex_IsReal_gltexres( unitex_t *uni );
ic_subimage_t * UniTex_IsReal_subimage( unitex_t *uni );

bool_t UniTex_IsValid( unitex_t *uni );
void	UniTex_TryToValidate( unitex_t *uni );
void	UniTex_TryToInvalidate( unitex_t *uni );

int UniTex_GetGLtexobj( unitex_t *uni );

void UniTex_RealTexCoord_01( unitex_t *uni, vec2d_t out, vec2d_t in );
void UniTex_RealTexCoord_0N( unitex_t *uni, vec2d_t out, vec2d_t in );

#endif
