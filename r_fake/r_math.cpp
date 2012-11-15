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



// r_math.c

/*
  =============================================================================
  math functions impl

  =============================================================================
*/

#include "r_math.h"
#include <math.h>
#include <string.h>

/*
  ==================================================
  plane stuff

  ==================================================
*/

/*
  ====================
  PlaneTestPoint

  ====================
*/
planeTestState PlaneTestPoint( plane_t *pl, vec3d_t p )
{
	fp_t		d;

	d = Vec3dDotProduct( pl->norm, p ) - pl->dist;
	if ( d > PLANETEST_EPSILON )
		return PlaneTest_front;
	if ( d < -PLANETEST_EPSILON )
		return PlaneTest_back;
	return PlaneTest_on;
}

/*
  ====================
  PlaneTestBB

  ====================
*/
planeTestState PlaneTestBB( plane_t *pl, vec3d_t min, vec3d_t max )
{
	int		i;
	vec3d_t		bb[2];
	fp_t		dmin, dmax;

	for ( i = 0; i < 3; i++ )
	{
		if ( pl->norm[i] < 0.0 )
		{
			bb[0][i] = min[i];
			bb[1][i] = max[i];
		}
		else
		{
			bb[1][i] = min[i];
			bb[0][i] = max[i];
		}
	}

	dmin = Vec3dDotProduct( bb[0], pl->norm ) - pl->dist;
	dmax = Vec3dDotProduct( bb[1], pl->norm ) - pl->dist;
//	printf( "%f %f\n", dmin, dmax );

	if ( dmin <= 0 && dmax <= 0)
		return PlaneTest_back;
	else if ( dmin > 0 && dmax > 0 )
		return PlaneTest_front;
	return PlaneTest_split;
}


/*
  ==================================================
  matrix stuff

  ==================================================
*/

/*
  ====================
  Matrix3Transpose

  ====================
*/
void Matrix3Transpose( matrix3_t out, matrix3_t in )
{
	
	out[0][0] = in[0][0];
	out[1][1] = in[1][1];
	out[2][2] = in[2][2];
	out[0][1] = in[1][0];
	out[0][2] = in[2][0];
	out[1][0] = in[0][1];
	out[1][2] = in[2][1];
	out[2][0] = in[0][2];
	out[2][1] = in[1][2];
}

/*
  ====================
  Matrix3Multiply

  ====================
*/
void Matrix3Multiply( matrix3_t out, matrix3_t in1, matrix3_t in2 )
{
	int		i, j;
	
	for ( i = 0; i < 3; i++ )
		for ( j = 0; j < 3; j++ )
			out[i][j] = in1[i][0]*in2[0][j] + in1[i][1]*in2[1][j] + in1[i][2]*in2[2][j];
}

/*
  ====================
  Matrix3Identity

  ====================
*/
void Matrix3Identity( matrix3_t out )
{
	matrix3_t	identity3 = { { 1.0, 0.0, 0.0 },
				      { 0.0, 1.0, 0.0 },
				      { 0.0, 0.0, 1.0 } };
	
	memcpy( out, identity3, sizeof ( matrix3_t ) );
}

/*
  ====================
  Matrix3Rotate

  ====================
*/
void Matrix3Rotate( matrix3_t out, fp_t roll, fp_t pitch, fp_t yaw )
{
        matrix3_t       mroll   = { {1, 0, 0}, {0, 1, 0}, {0, 0, 1} };          
        matrix3_t       mpitch  = { {1, 0, 0}, {0, 1, 0}, {0, 0, 1} };          
        matrix3_t       myaw    = { {1, 0, 0}, {0, 1, 0}, {0, 0, 1} };          
                                                                                
        matrix3_t       mtemp1;                                                 
                                                                                
        float   s, c;                                                           
                                                                                
        s = sin( roll );                                                        
        c = cos( roll );                                                        
        mroll[0][0] = c;                                                        
        mroll[0][1] = s;                                                        
        mroll[1][0] = -s;                                                       
        mroll[1][1] = c;                                                        
                                                                                
        s = sin( pitch );                                                       
        c = cos( pitch );                                                       
        mpitch[1][1] = c;                                                       
        mpitch[1][2] = s;                                                       
        mpitch[2][1] = -s;                                                      
        mpitch[2][2] = c;                                                       
                                                                                
        s = sin( yaw );                                                         
        c = cos( yaw );                                                         
        myaw[0][0] = c;                                                         
        myaw[0][2] = -s;                                                        
        myaw[2][0] = s;                                                         
        myaw[2][2] = c;                                                         
                                                                                
        Matrix3Concat( mtemp1, mroll, myaw );                                  
        Matrix3Concat( out, mpitch, mtemp1 );	
}


void Matrix3TransformPoint( vec3d_t out, vec3d_t in, matrix3_t matrix )
{
	vec3d_t		tmp;

	Vec3dCopy( tmp, in );

        out[0] = tmp[0]*matrix[0][0] + tmp[1]*matrix[0][1] + tmp[2]*matrix[0][2];
        out[1] = tmp[0]*matrix[1][0] + tmp[1]*matrix[1][1] + tmp[2]*matrix[1][2];
        out[2] = tmp[0]*matrix[2][0] + tmp[1]*matrix[2][1] + tmp[2]*matrix[2][2];
}

