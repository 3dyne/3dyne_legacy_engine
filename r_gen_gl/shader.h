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



// shader.h

#ifndef __shader
#define __shader

#include "shader_defs.h"
#include "layers_defs.h"

// shader_mgr instance

shader_mgr_t *	Shader_NewMgr( void );
void		Shader_MgrCleanUp( shader_mgr_t *mgr );
void		Shader_FreeMgr( shader_mgr_t *mgr );
void		Shader_MgrRegisterShader( shader_mgr_t *mgr, shader_t *shd );
shader_t *	Shader_MgrSearchShader( shader_mgr_t *mgr, layers_characteristic_t *ca );

void		Shader_MgrDump( shader_mgr_t *mgr );

// shader instance
shader_t *			Shader_New( layers_characteristic_t *ca );	// shader is responsible for 'ca'
void				Shader_CleanUp( shader_t *shd );
void				Shader_Free( shader_t *shd );
layers_characteristic_t *	Shader_GetLayersCharacteristic( shader_t *shd );

void			Shader_Dump( shader_t *shd );

// shader_mgr frame ( public )
void	Shader_MgrFrameBegin( shader_mgr_t *mgr );
void	Shader_MgrFrameEnd( shader_mgr_t *mgr );
void	Shader_MgrRender( shader_mgr_t *mgr );

void	Shader_MgrFrameRegisterShape( shader_mgr_t *mgr, shape_t *shp );

// shader frame ( internal )
void	Shader_FrameBegin( shader_t *shd );
void	Shader_FrameEnd( shader_t *shd );
void	Shader_FrameRegisterShape( shader_t *shd, shape_t *shp );
void	Shader_SortShapes( shader_t *shd );


#endif
