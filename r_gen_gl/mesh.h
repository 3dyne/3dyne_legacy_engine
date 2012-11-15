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



// mesh.h

#ifndef __mesh
#define __mesh

#include "mesh_defs.h"

void Mesh_BeginInstance( void );
void Mesh_AddIntArray( int size );
void Mesh_AddFloatArray( int size );
void Mesh_AddVec2dArray( int size );
void Mesh_AddVec3dArray( int size );

void Mesh_AddExternIntArray( int size, void *ptr );
void Mesh_AddExternFloatArray( int size, void *ptr );
void Mesh_AddExternVec2dArray( int size, void *ptr );
void Mesh_AddExternVec3dArray( int size, void *ptr );

mesh_t * Mesh_EndInstance( void );

unique_t Mesh_GetName( mesh_t *mesh );
bool_t Mesh_CheckName( mesh_t *mesh, unique_t name );

void Mesh_GetIntBase( mesh_t *mesh, int array, int *size, int **ptr );
void Mesh_GetFloatBase( mesh_t *mesh, int array, int *size, fp_t **ptr );
void Mesh_GetVec2dBase( mesh_t *mesh, int array, int *size, vec2d_t **ptr );
void Mesh_GetVec3dBase( mesh_t *mesh, int array, int *size, vec3d_t **ptr );

void Mesh_Free( mesh_t *mesh );

#endif
