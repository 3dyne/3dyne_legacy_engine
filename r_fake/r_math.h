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



// r_math.h

/*
  =============================================================================
  some math functions

  =============================================================================
*/

#ifndef __r_math
#define __r_math

//#include "render.h"
#include "lib_math.h"
//#include "r_state.h"

#ifndef M_PI
#define M_PI	( 3.14159265358979323846 )
#endif

/*
  ==================================================
  plane stuff

  ==================================================
*/
#define PLANETEST_EPSILON	( 0.01 )

typedef enum
{
	PlaneTest_on = 0,
	PlaneTest_front,
	PlaneTest_back,
	PlaneTest_split
} planeTestState;

typedef struct
{
	vec3d_t		norm;
	fp_t		dist;
} plane_t;

planeTestState PlaneTestPoint( plane_t *pl, vec3d_t p );
planeTestState PlaneTestBB( plane_t *pl, vec3d_t min, vec3d_t max );


/*
  ==================================================
  matrix stuff

  ==================================================
*/

void Matrix3Transpose( matrix3_t out, matrix3_t in );
void Matrix3Multiply( matrix3_t out, matrix3_t in1, matrix3_t in2 );
void Matrix3Identity( matrix3_t out );

void Matrix3Rotate( matrix3_t out, fp_t roll, fp_t pitch, fp_t yaw );

//void Matrix3ApplyRotate( matrix3_t inout, fp_t roll, fp_t pitch, fp_t yaw );

//void Matrix3ApplyTranslate( matrix3_t inout, vec3d_t vec );

void Matrix3TransformPoint( vec3d_t out, vec3d_t in, matrix3_t matrix );

/*
  ==================================================
  misc math stuff
  in r_misc.c

  ==================================================
*/
// r_misc.c
typedef enum
{
	ProjectionType_X = 0,
	ProjectionType_Y = 1,
	ProjectionType_Z = 2,
	ProjectionType_Mask = 3,

	ProjectionType_Vecs = 4,
	ProjectionType_Shift = 8
} projectionType;

#endif
