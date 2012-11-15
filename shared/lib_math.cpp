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



// lib_math.h

#include <stdio.h>
#include <math.h>
#include <string.h>

#include "lib_math.h"

//
// print
//
 
void Vec2dPrint( vec2d_t in )
{
	printf( "(%.2f, %.2f)\n", in[0], in[1] );
}

void Vec3dPrint( vec3d_t in )
{
	printf( "(%.2f, %.2f, %.2f)\n", in[0], in[1], in[2] );
}

void IVec3dPrint( ivec3d_t in )
{
	printf( "(%d, %d, %d)\n", in[0], in[1], in[2] );
}

void VecndPrint( fp_t *in, int num )
{
	int		i;

	printf( "(" );
	for ( i = 0; i < num; i++ )
		printf( "%.2f, ", in[i] );
	printf( ")\n" );
}

//
// init
//

void Vec2dInit( vec2d_t out, fp_t in1, fp_t in2 )
{
	out[0] = in1;
	out[1] = in2;
}

void Vec3dInit( vec3d_t out, fp_t in1, fp_t in2, fp_t in3 )
{
	out[0] = in1;
	out[1] = in2;
	out[2] = in3;
}


//
// copy
//

void Vec2dCopy( vec2d_t out, vec2d_t in )
{
	out[0] = in[0];
	out[1] = in[1];
}

void Vec3dCopy( vec3d_t out, vec3d_t in )
{
	out[0] = in[0];
	out[1] = in[1];
	out[2] = in[2];
}

void Vec4dCopy( vec4d_t out, vec4d_t in )
{
	memcpy( out, in, sizeof( vec4d_t ) );
}

void Vec5dCopy( vec5d_t out, vec5d_t in )
{
	out[0] = in[0];
	out[1] = in[1];
	out[2] = in[2];
	out[3] = in[3];
	out[4] = in[4];
}

void VecndCopy( fp_t *out, fp_t *in, int num )
{
	int		i;

	for ( i = 0; i < num; i++ )
		out[i] = in[i];
}

void IVec3dCopy( ivec3d_t out, ivec3d_t in )
{
	out[0] = in[0];
	out[1] = in[1];
	out[2] = in[2];
}


//
// Rint
//

void Vec2dRint( vec2d_t out, vec2d_t in )
{
	out[0] = _Rint( in[0] );
	out[1] = _Rint( in[1] );
}

void Vec3dRint( vec3d_t out, vec3d_t in )
{
	out[0] = _Rint( in[0] );
	out[1] = _Rint( in[1] );
	out[2] = _Rint( in[2] );
} 

void IVec3dRint( ivec3d_t out, vec3d_t in )
{
	out[0] = (int)_Rint( in[0] );
	out[1] = (int)_Rint( in[1] );
	out[2] = (int)_Rint( in[2] );
} 

void IVec3dUnitSnap( ivec3d_t out, ivec3d_t in, int _snap )
{
	out[0] = _UnitSnap( in[0], _snap );
	out[1] = _UnitSnap( in[1], _snap );
	out[2] = _UnitSnap( in[2], _snap );
}

//
// add
//
void Vec2dAdd( vec2d_t out, vec2d_t in1, vec2d_t in2 )
{
	out[0] = in1[0] + in2[0];
	out[1] = in1[1] + in2[1];
}

void Vec3dAdd( vec3d_t out, vec3d_t in1, vec3d_t in2 )
{
	out[0] = in1[0] + in2[0];
	out[1] = in1[1] + in2[1];
	out[2] = in1[2] + in2[2];
}

void VecndAdd( fp_t *out, fp_t *in1, fp_t *in2, int num )
{
	int		i;

	for ( i = 0; i < num; i++ )
		out[i] = in1[i] + in2[i];
}


//
// sub
//
void Vec2dSub( vec2d_t out, vec2d_t in1, vec2d_t in2 )
{
	out[0] = in1[0] - in2[0];
	out[1] = in1[1] - in2[1];
}

void Vec3dSub( vec3d_t out, vec3d_t in1, vec3d_t in2 )
{
	out[0] = in1[0] - in2[0];
	out[1] = in1[1] - in2[1];
	out[2] = in1[2] - in2[2];
}

void VecndSub( fp_t *out, fp_t *in1, fp_t *in2, int num )
{
	int		i;

	for ( i = 0; i < num; i++ )
		out[i] = in1[i] - in2[i];
}


//
// scale
//
void Vec2dScale( vec2d_t out, fp_t scale, vec2d_t in )
{
	out[0] = scale * in[0];
	out[1] = scale * in[1];
}

void Vec3dScale( vec3d_t out, fp_t scale, vec3d_t in )
{
	out[0] = scale * in[0];
	out[1] = scale * in[1];
	out[2] = scale * in[2];
}

void VecndScale( fp_t *out, fp_t scale, fp_t *in, int num )
{
	int		i;

	for ( i = 0; i < num; i++ )
		out[i] = scale * in[i];
}

//
// flip
//
void Vec2dFlip( vec2d_t out, vec2d_t in )
{
	out[0] = -in[0];
	out[1] = -in[1];
}

void Vec3dFlip( vec3d_t out, vec3d_t in )
{
	out[0] = -in[0];
	out[1] = -in[1];
	out[2] = -in[2];
}

//
// MA
//
void Vec2dMA( vec2d_t out, fp_t scale, vec2d_t in1, vec2d_t in2 )
{
	out[0] = scale * in1[0] + in2[0];
	out[1] = scale * in1[1] + in2[1];
}

void Vec3dMA( vec3d_t out, fp_t scale, vec3d_t in1, vec3d_t in2 )
{
	out[0] = scale * in1[0] + in2[0];
	out[1] = scale * in1[1] + in2[1];
	out[2] = scale * in1[2] + in2[2];
}

//
// Length
//

fp_t Vec2dLen( vec2d_t in )
{
	fp_t		len;

	len = ( in[0]*in[0] + in[1]*in[1] );
	if ( len <= 0.0 )
		return 0.0;
	len = sqrt( len );	

	return len;
}


fp_t Vec3dLen( vec3d_t in )
{
	fp_t		len;

	len = ( in[0]*in[0] + in[1]*in[1] + in[2]*in[2] );
	if ( len <= 0.0 )
		return 0.0;
	len = sqrt( len );
	return len;
}

fp_t VecndLen( fp_t *in, int num )
{
	int		i;
	fp_t		len = 0.0;

	for ( i = 0; i < num; i++ )
		len+=in[i]*in[i];

	if ( len <= 0.0 )
		return 0.0;

	len = sqrt( len );
	return len;
}

//
// normalize
//
void Vec2dUnify( vec2d_t out, vec2d_t in )
{
	fp_t		len;

	len = ( in[0]*in[0] + in[1]*in[1] );
	
//	if ( len <= 0.0 )
//		len = 1.0;
	
	len = sqrt( len );

	out[0] = in[0] / len;
	out[1] = in[1] / len;
}

fp_t Vec3dUnify( vec3d_t inout )
{
	fp_t		len;

	len = sqrt( inout[0]*inout[0] + inout[1]*inout[1] + inout[2]*inout[2] );

	if ( len == 0.0 )
		return len;

	inout[0]/=len;
	inout[1]/=len;
	inout[2]/=len;

	return len;
}

void Vec3dUnify2( vec3d_t out, vec3d_t in )
{
	fp_t		len;

	len = sqrt( in[0]*in[0] + in[1]*in[1] + in[2]*in[2] );

	out[0]=in[0]/len;
	out[1]=in[1]/len;
	out[2]=in[2]/len;	
}


//
// dot product
//
fp_t Vec2dDotProduct( vec2d_t in1, vec2d_t in2 )
{
	fp_t		scale;

	scale = in1[0]*in2[0] + in1[1]*in2[1];
	return scale;
}

fp_t Vec3dDotProduct( vec3d_t in1, vec3d_t in2 )
{
	fp_t		scale;

	scale = in1[0]*in2[0] + in1[1]*in2[1] + in1[2]*in2[2];
	return scale;
}


//
// cross product
//
void Vec2dLeft( vec2d_t out, vec2d_t in )
{
	out[0] = -in[1];
	out[1] =  in[0];
}

void Vec3dCrossProduct( vec3d_t out, vec3d_t in1, vec3d_t in2 )
{
	out[0] = in1[1]*in2[2] - in1[2]*in2[1];
	out[1] = in1[2]*in2[0] - in1[0]*in2[2];
	out[2] = in1[0]*in2[1] - in1[1]*in2[0];
}

//
// colinear check
// 
bool_t	Vec3dCheckColinear( vec3d_t p1, vec3d_t p2, vec3d_t t )
{
	vec3d_t		v1, v2;

	Vec3dSub( v1, p1, t );
	Vec3dSub( v2, t, p2 );
	if ( Vec3dUnify( v1 ) < 0.01 ) return true;
	if ( Vec3dUnify( v2 ) < 0.01 ) return true;
	if ( Vec3dDotProduct( v1, v2 ) > 0.999 )
		return true;
	return false;	
}

//
// init planes
//

fp_t Vec3dInitPlane2( vec3d_t norm, vec3d_t pos )
{
	fp_t		d;
	d = Vec3dDotProduct( norm, pos );
	return d;
}

bool_t Vec3dInitPlane( vec3d_t norm, fp_t *dist, vec3d_t in1, vec3d_t in2, vec3d_t in3 )
{
	bool_t		good;
	vec3d_t 	delta1, delta2;
	fp_t		len;

	Vec3dSub( delta1, in2, in1 );
	Vec3dSub( delta2, in3, in1 );

	Vec3dCrossProduct( norm, delta1, delta2 );

	len = norm[0]*norm[0] + norm[1]*norm[1] + norm[2]*norm[2];

	if ( len < ON_EPSILON )
		good = false;
	else
		good = true;

	len = 1.0/sqrt(len);
	norm[0]*=len;
	norm[1]*=len;
	norm[2]*=len;

	*dist = Vec3dDotProduct( in1, norm );

	return good;
}

bool_t Vec2dInitStraight( vec2d_t norm, fp_t *dist, vec2d_t in1, vec2d_t in2 )
{
	vec2d_t		delta;
	fp_t		len;
	bool_t		good;

	Vec2dSub( delta, in2, in1 );
	Vec2dLeft( norm, delta );

	len = norm[0]*norm[0] + norm[1]*norm[1];
	if ( len < ON_EPSILON )
		good = false;
	else
		good = true;

	len = 1.0/sqrt(len);

	norm[0]*=len;
	norm[1]*=len;

	*dist = Vec2dDotProduct( in1, norm );

	return good;
}

//
// project
//

fp_t Vec3dProjectOnPlane( vec3d_t out, vec3d_t norm, fp_t dist, vec3d_t in )
{
	fp_t		d;
	
	d = Vec3dDotProduct( in, norm ) - dist;
	Vec3dMA( out, -d, norm, in );

	return d;
}


//
// clip
//
void Vec3dClipLine( vec3d_t clip, vec3d_t p1, vec3d_t p2, vec3d_t norm, fp_t dist )
{
	fp_t	d1, d2;
	fp_t	scale;
	
	d1 = Vec3dDotProduct( p1, norm ) - dist;
	d2 = Vec3dDotProduct( p2, norm ) - dist;

	scale = d1 / (d1-d2);

	clip[0] = p1[0]+scale*(p2[0]-p1[0]);
	clip[1] = p1[1]+scale*(p2[1]-p1[1]);
	clip[2] = p1[2]+scale*(p2[2]-p1[2]);
}

void Vec3dClipDirection( vec3d_t clip_dir, vec3d_t p, vec3d_t dir, vec3d_t norm, fp_t dist )
{
	fp_t	d1, d2;
	fp_t	scale;

	vec3d_t		p2;

	Vec3dAdd( p2, p, dir );

	d1 = Vec3dDotProduct( p, norm ) - dist;
	d2 = Vec3dDotProduct( p2, norm ) - dist;

	scale = d1 / (d1-d2);
	
	Vec3dScale( clip_dir, scale, dir );
}


//
// bounding box
//
void Vec2dInitBB( vec2d_t min, vec2d_t max, fp_t pos )
{
	int		i;
	for ( i = 0; i < 2; i++ )
	{
		min[i] = pos;
		max[i] = -pos;
	}
}

void Vec2dAddToBB( vec2d_t min, vec2d_t max, vec2d_t in )
{
	int		i;
	for ( i = 0; i < 2; i++ )
	{
		if ( in[i] < min[i] )
			min[i] = in[i];
		if ( in[i] > max[i] )
			max[i] = in[i];
	}
}


void Vec3dInitBB( vec3d_t min, vec3d_t max, fp_t pos )
{
	int		i;
	for ( i = 0; i < 3; i++ )
	{
		min[i] = pos;
		max[i] = -pos;
	}
}

void Vec3dAddToBB( vec3d_t min, vec3d_t max, vec3d_t in )
{
	int		i;
	for ( i = 0; i < 3; i++ )
	{
		if ( in[i] < min[i] )
			min[i] = in[i];
		if ( in[i] > max[i] )
			max[i] = in[i];
	}
}

bool_t Vec3dBBDoIntersect( vec3d_t min1, vec3d_t max1, vec3d_t min2, vec3d_t max2 )
{
	int		i;

	for ( i = 0; i < 3; i++ )
	{
		if ( min1[i] > max2[i] || max1[i] < min2[i] )
			return false;
	}
	return true;
}


//
// matrix
//

void Matrix3Concat( matrix3_t out, matrix3_t in1, matrix3_t in2 )
{
	int		i, j;

	for ( i = 0; i < 3; i++ )
		for ( j = 0; j < 3; j++ )
			out[i][j] = in1[i][0]*in2[0][j] + in1[i][1]*in2[1][j] + in1[i][2]*in2[2][j];
}

void Matrix3SetupRotate( matrix3_t out, fp_t roll, fp_t pitch, fp_t yaw )
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

void Matrix3Vec3dRotate( vec3d_t out, vec3d_t in, matrix3_t matrix )
{
//        Vec3dSub( v, in, r_origin );     
	vec3d_t		tmp;
	Vec3dCopy( tmp, in );
        out[0] = tmp[0]*matrix[0][0] + tmp[1]*matrix[0][1] + tmp[2]*matrix[0][2];
        out[1] = tmp[0]*matrix[1][0] + tmp[1]*matrix[1][1] + tmp[2]*matrix[1][2];
        out[2] = tmp[0]*matrix[2][0] + tmp[1]*matrix[2][1] + tmp[2]*matrix[2][2];
}

//#if defined (win32_x86)
//fp_t rint( fp_t in )
//{
//    return floor( in + 0.5 );
//}
//#endif
