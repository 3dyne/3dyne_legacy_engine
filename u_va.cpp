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



// u_va.c
#include <string.h>
#include "u_va.h"

/*
  ==============================
  U_VACopy

  ==============================
*/
void U_VACopy( vec3d_t out[], vec3d_t in[], int num )
{
	memcpy( out, in, sizeof( vec3d_t ) * num );
}


/*
  ==============================
  U_VAScale

  ==============================
*/
void U_VAScale( vec3d_t inout[], int num, vec3d_t scale )
{
	int		i;

	for ( i = 0; i < num; i++ )
	{
		inout[i][0] *= scale[0];
		inout[i][1] *= scale[1];
		inout[i][2] *= scale[2];
	}
}


/*
  ==============================
  U_VATranslate

  ==============================
*/
void U_VATranslate( vec3d_t inout[], int num, vec3d_t origin )
{
	int		i;

	for ( i = 0; i < num; i++ )
	{
		inout[i][0] += origin[0];
		inout[i][1] += origin[1];
		inout[i][2] += origin[2];
	}
}

/*
  ==============================
  U_VARotateMatrix

  ==============================
*/
void U_VARotateMatrix( vec3d_t inout[], int num, matrix3_t mat )
{
	int		i;

	for ( i = 0; i < num; i++ )
	{
		Matrix3Vec3dRotate( inout[i], inout[i], mat );
	}
}


/*
  ==============================
  U_VARotateAxis

  ==============================
*/
void U_VARotateAxis( vec3d_t inout[], int num, vec3d_t axis[3] )
{
	int		i;
	vec3d_t		in;

	for ( i = 0; i < num; i++ )
	{
		Vec3dCopy( in, inout[i] );

		inout[i][0] = in[0]*axis[0][0] + in[1]*axis[1][0] + in[2]*axis[2][0];
		inout[i][1] = in[0]*axis[0][1] + in[1]*axis[1][1] + in[2]*axis[2][1];
		inout[i][2] = in[0]*axis[0][2] + in[1]*axis[1][2] + in[2]*axis[2][2];
	}	      
}


/*
  ==============================
  U_VANormalize

  ==============================
*/
void U_VANormalize( vec3d_t inout[], int num )
{
	int		i;
	vec3d_t		min, max;
	vec3d_t		delta;
	vec3d_t		center;
	fp_t		scale;

	Vec3dInitBB( min, max, 999999.9 );
	for ( i = 0; i < num; i++ )
	{
		Vec3dAddToBB( min, max, inout[i] );
	}
	
	Vec3dSub( delta, max, min );
	
	scale = 1.0;
	if ( delta[0] >= delta[1] && delta[0] >= delta[2] )
		scale = delta[0];
	else if ( delta[1] >= delta[0] && delta[1] >= delta[2] )
		scale = delta[1];
	else if ( delta[2] >= delta[0] && delta[2] >= delta[1] )
		scale = delta[2];

	Vec3dMA( center, 0.5, delta, min );


	scale = 1.0 / scale;
	for ( i = 0; i < num; i++ )
	{
		Vec3dSub( inout[i], center, inout[i] );
		Vec3dScale( inout[i], scale, inout[i] );
	}
}


/*
  ==============================
  U_VACalcBB

  ==============================
*/
void U_VACalcBB( vec3d_t in[], int num, vec3d_t min, vec3d_t max )
{
	int		i;

	Vec3dInitBB( min, max, 999999.9 );

	for ( i = 0; i < num; i++ )
	{
		Vec3dAddToBB( min, max, in[i] );
	}
}
