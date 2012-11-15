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



// tf2d.h

#ifndef __tf2d
#define __tf2d

#include "tf2d_defs.h"

/*
  ==================================================
  transform 2d

  ==================================================
*/

tf2d_t * TF2D_CreateScale( void );
tf2d_t * TF2D_CreateShift( void );
tf2d_t * TF2D_CreateMatrix( void );

tf2d_scale_t * TF2D_IsScale( tf2d_t *tf2d );
tf2d_shift_t * TF2D_IsShift( tf2d_t *tf2d );
tf2d_matrix_t * TF2D_IsMatrix( tf2d_t *tf2d );

void TF2D_SetupMatrixByAngle( tf2d_matrix_t *tf2d, fp_t angle );
void TF2D_SetupMatrixByAxis( tf2d_matrix_t *tf2d, vec2d_t x, vec2d_t y );

void TF2D_Transfrom( tf2d_t *tf2d, vec2d_t out, vec2d_t in );

void TF2D_CalcScale( union tf2d_u *tf2d, vec2d_t out, vec2d_t in );
void TF2D_CalcShift( union tf2d_u *tf2d, vec2d_t out, vec2d_t in );
void TF2D_CalcMatrix( union tf2d_u *tf2d, vec2d_t out, vec2d_t in );

/*
  ==================================================
  transform 2d stack

  ==================================================
*/

void TF2D_InitStack( tf2d_stack_t *stack );
void TF2D_CleanUpStack( tf2d_stack_t *stack );
void TF2D_StackPush( tf2d_stack_t *stack, tf2d_t *tf2d );
tf2d_t * TF2D_StackPop( tf2d_stack_t *stack );
void TF2D_StackCalc( tf2d_stack_t *stack, vec2d_t out, vec2d_t in );	// from bottom to the top

#endif
