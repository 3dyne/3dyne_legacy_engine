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



// layer_instance.h

#ifndef __layer_instance
#define __layer_instance

#include "layer_defs.h"

void Layer_BeginInstance( layerMode mode );

void	Layer_SetAmbientColor( color_rgba_t color );

// projectiv transformation
void	Layer_SetProjection( layerProjection proj );
void	Layer_AddAxis( vec2d_t x, vec2d_t y );
void	Layer_AddRotate( fp_t angle );
void	Layer_AddShift( vec2d_t	shift );
void	Layer_AddScale( vec2d_t scale );

// transform 2d stack
void Layer_SetTF2DStack( tf2d_stack_t *stack );

// uv texgen
void	Layer_SetUVCtrl( vec2d_t cps[4] );

// layer image source
void	Layer_SetUniTex( unitex_t *uni );
void	Layer_SetLightmapHead( lightmap_t *lightmap_head );

// layer gl state
void	Layer_SetGLstate( layer_gl_state_t *state );

// layer ordinal
void	Layer_SetOrdinal( int ordinal );

layer_t	* Layer_EndInstance( void );

// destroy, old don't use it
void	Layer_Free( layer_t *layer );

// better
void Layer_CleanUp( layer_t *layer );

#endif
