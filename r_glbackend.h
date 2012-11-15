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



// r_glbackend.h

#ifndef __r_glbackend
#define __r_glbackend


void R_BE_SetVertexMatrix( matrix3_t *mat );
void R_BE_SetVertexOrigin( vec3d_t origin );

// global va
void R_BE_AppendVertexArray( int vertexnum, vec3d_t vertices[] );
int R_BE_GetVertexOfs( void );
void R_BE_SetVertexOfs( int ofs );

void R_BE_LockVertexArray( void );
void R_BE_UnlockVertexArray( void );

// locals
void R_BE_ResetLocalBuffers( void );
void R_BE_SetCMDBuffer( int cmdnum, int cmd_buf[] );
void R_BE_SetVrefArray( int vrefnum, int vref_array[] );
void R_BE_SetColorArray( int colornum, vec3d_t color_array[] );
void R_BE_SetTexcoordArray( int texcoordnum, vec2d_t texcoord_array[] );

void R_BE_ProcessCMDBuffer( void );

void R_BE_ResetCounters( void );
void R_BE_DumpCounters( void );

void R_BE_ResetTriangleCounter( void );
int R_BE_GetTriangleCounter( void );

#endif
