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



// shape_instance.h

#ifndef __shape_instance
#define __shape_instance

#include "shape_defs.h"
#include "layer_defs.h"

void Shape_BeginInstance( void );

void Shape_SetPrimaryKey( unique_t self_id );
void Shape_SetUOCID( unique_t self_id );

void Shape_SetTesselation( shapeTesselation tess_name );
void Shape_SetPlaneHint( vec3d_t norm, fp_t dist );

// internal tesselation stuff
void Shape_SetCtrlPointNum( int unum, int vnum );

shape_ctrl_point_t * Shape_NewCtrlPointUV3fv( int u, int v, vec3d_t p );
void Shape_AddCtrlPoint( shape_ctrl_point_t *cp );

// external 
void Shape_SetGLMesh( hobj_t *glmesh, unsigned char *base );

// -

void Shape_AddLayer( layer_t *layer );

shape_t * Shape_EndInstance( void );

void Shape_CleanUp( shape_t *shp );

#endif
