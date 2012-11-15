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



// tf2d.c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "lib_math.h"
#include "lib_container.h"
#include "shock.h"

#include "s_mem.h"

#include "tf2d.h"

/*
  ==============================
  TF2D_CreateScale

  ==============================
*/
tf2d_t * TF2D_CreateScale( void )
{
	tf2d_scale_t	*tf2d;

	tf2d = NEWTYPE( tf2d_scale_t );
	tf2d->type = tf2dType_scale;
	tf2d->transform_func = TF2D_CalcScale;
	
	return (tf2d_t *) tf2d;
}

/*
  ==============================
  TF2D_CreateShift

  ==============================
*/
tf2d_t * TF2D_CreateShift( void )
{
	tf2d_shift_t	*tf2d;

	tf2d = NEWTYPE( tf2d_shift_t );
	tf2d->type = tf2dType_shift;
	tf2d->transform_func = TF2D_CalcShift;

	return (tf2d_t *) tf2d;
}

/*
  ==============================
  TF2D_CreateMatrix

  ==============================
*/
tf2d_t * TF2D_CreateMatrix( void )
{
	tf2d_matrix_t	*tf2d;

	tf2d = NEWTYPE( tf2d_matrix_t );
	tf2d->type = tf2dType_matrix;
	tf2d->transform_func = TF2D_CalcMatrix;

	return (tf2d_t *) tf2d;	
}

/*
  ==============================
  TF2D_IsScale

  ==============================
*/
tf2d_scale_t * TF2D_IsScale( tf2d_t *tf2d )
{
	if ( (maddr_t)tf2d->any.transform_func == (maddr_t)TF2D_CalcScale )
		return &tf2d->scale;
	return NULL;
}

/*
  ==============================
  TF2D_IsShift

  ==============================
*/
tf2d_shift_t * TF2D_IsShift( tf2d_t *tf2d )
{
	if ( (maddr_t)tf2d->any.transform_func == (maddr_t)TF2D_CalcShift )
		return &tf2d->shift;
	return NULL;
}

/*
  ==============================
  TF2D_IsMatrix

  ==============================
*/
tf2d_matrix_t * TF2D_IsMatrix( tf2d_t *tf2d )
{
	if ( (maddr_t)tf2d->any.transform_func == (maddr_t)TF2D_CalcMatrix )
		return &tf2d->matrix;
	return NULL;
}

/*
  ==============================
  TF2D_CalcScale

  ==============================
*/
void TF2D_CalcScale( tf2d_t *tf2d, vec2d_t out, vec2d_t in )
{
	out[0] = tf2d->scale.scale[0] * in[0];
	out[1] = tf2d->scale.scale[1] * in[1];
}

/*
  ==============================
  TF2D_CalcShift

  ==============================
*/
void TF2D_CalcShift( tf2d_t *tf2d, vec2d_t out, vec2d_t in )
{
	Vec2dAdd( out, in, tf2d->shift.shift );
}

/*
  ==============================
  TF2D_CalcMatrix

  ==============================
*/
void TF2D_CalcMatrix( tf2d_t *tf2d, vec2d_t out, vec2d_t in )
{
	vec2d_t		res;

	res[0] = in[0]*tf2d->matrix.vecs[0][0] + in[1]*tf2d->matrix.vecs[0][1];
	res[1] = in[0]*tf2d->matrix.vecs[1][0] + in[1]*tf2d->matrix.vecs[1][1];

	out[0] = res[0];
	out[1] = res[1];
}

/*
  ==============================
  TF2D_Transform

  ==============================
*/
void TF2D_Transform( tf2d_t *tf2d, vec2d_t out, vec2d_t in )
{
	tf2d->any.transform_func( tf2d, out, in );
}

/*
  ==============================
  TF2D_SetupMatrixByAxis

  ==============================
*/
void TF2D_SetupMatrixByAxis( tf2d_matrix_t *tf2d, vec2d_t x, vec2d_t y )
{
	tf2d->vecs[0][0] = x[0];
	tf2d->vecs[0][1] = x[1];
	tf2d->vecs[1][0] = y[0];
	tf2d->vecs[1][1] = y[1];
}

/*
  ==============================
  TF2D_SetupMatrixByAngle

  ==============================
*/
void TF2D_SetupMatrixByAngle( tf2d_matrix_t *tf2d, fp_t angle )
{
	fp_t		s, c;
	fp_t		alpha;

	alpha = angle * D2R;
	
	s = sin( alpha );
	c = cos( alpha );
	
	tf2d->vecs[0][0] = c;
	tf2d->vecs[0][1] = -s;
	tf2d->vecs[1][0] = s;
	tf2d->vecs[1][1] = c;	
}


/*
  ==================================================
  transfrom 2d stack

  ==================================================
*/

/*
  ==============================
  TF2D_InitStack

  ==============================
*/
void TF2D_InitStack( tf2d_stack_t *stack )
{
	U_InitList( &stack->tf2d_list );
}

/*
  ==============================
  TF2D_CleanUpStack

  ==============================
*/
void TF2D_CleanUpStack( tf2d_stack_t *stack )
{
	tf2d_t		*tf2d;

	for ( ; ( tf2d = TF2D_StackPop( stack ) ) ; )
	{
		FREE( tf2d );
	}
	
	U_CleanUpList( &stack->tf2d_list, NULL );
}

/*
  ==============================
  TF2D_StackPush

  ==============================
*/
void TF2D_StackPush( tf2d_stack_t *stack, tf2d_t *tf2d )
{
	U_ListInsertAtTail( &stack->tf2d_list, tf2d );
}

/*
  ==============================
  TF2D_StackPop

  ==============================
*/
tf2d_t * TF2D_StackPop( tf2d_stack_t *stack )
{
	return (tf2d_t *)U_ListRemoveAtTail( &stack->tf2d_list );
}

/*
  ==============================
  TF2D_StackCalc

  ==============================
*/
void TF2D_StackCalc( tf2d_stack_t *stack, vec2d_t out, vec2d_t in )
{
	u_list_iter_t		iter;
	tf2d_t			*tf2d;
	vec2d_t			tmp;

	Vec2dCopy( tmp, in );

	U_ListIterInit( &iter, &stack->tf2d_list );
	for ( ; ( tf2d = (tf2d_t *)U_ListIterNext( &iter ) ) ; )
	{
		TF2D_Transform( tf2d, tmp, tmp );		
	}
	
	Vec2dCopy( out, tmp );
}
