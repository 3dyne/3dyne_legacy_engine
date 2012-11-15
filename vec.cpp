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



// vec.c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "vec.h"


// 5D stuff
void Vec5dCopy( vec5d_t out, vec5d_t in )
{
//	int	i;

	memcpy( out, in, sizeof(vec5d_t) );
//	for ( i = 0; i < 5; i++ )
//		out[i] = in[i];
}

// **************************************************
// 2D stuff
// **************************************************

void Vec2dPrint( vec2d_t v ) {
	
	printf("(%.2f, %.2f)\n", v[0], v[1] );

}

void Vec2dInit( vec2d_t out , float x0, float x1 )
{
	out[0] = x0;
	out[1] = x1;
}

void Vec2dCopy( vec2d_t out, vec2d_t in )
{
	out[0] = in[0];
	out[1] = in[1];
}

// **************************************************
// 3D stuff
// **************************************************

void Vec3dPrint( vec3d_t v )
{
	
	printf("(%.6f, %.6f, %.6f)\n", v[0], v[1], v[2] );

}

void Vec3dInit( vec3d_t v, float x0, float x1, float x2 )
{
	v[0] = x0;
	v[1] = x1;
	v[2] = x2;
}


void Vec3dCopy( vec3d_t vout, vec3d_t vin )
{
	// vout = vin;
	memcpy( vout, vin, sizeof( vec3d_t ) );
}

void Vec3dAdd( vec3d_t v0, vec3d_t v1, vec3d_t v2 )
{
	// v0 = v1 + v2
	v0[0] = v1[0] + v2[0];
	v0[1] = v1[1] + v2[1];
	v0[2] = v1[2] + v2[2];
}

void Vec3dSub( vec3d_t v0, vec3d_t v1, vec3d_t v2 )
{
	// v0 = v1 - v2;

	v0[0] = v1[0] - v2[0];
	v0[1] = v1[1] - v2[1];
	v0[2] = v1[2] - v2[2];

}

void Vec3dScale( vec3d_t vout, float s, vec3d_t vin )
{
	// vout = s * vin
	vout[0] = s * vin[0];
	vout[1] = s * vin[1];
	vout[2] = s * vin[2];
}

void Vec3dMA( vec3d_t vout, float s, vec3d_t v, vec3d_t vin )
{
	// vout = s * v + vin
	vout[0] = s * v[0] + vin[0];
	vout[1] = s * v[1] + vin[1];
	vout[2] = s * v[2] + vin[2];
}

float Vec3dLen( vec3d_t v )
{
	float		len;

	len = sqrt( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );
	return len;
}

void Vec3dUnify( vec3d_t v )
{
	float		len;

	len = sqrt( v[0]*v[0] + v[1]*v[1] + v[2]*v[2] );

	v[0] /= len;
	v[1] /= len;
	v[2] /= len;
}

float Vec3dDotProduct( vec3d_t v0, vec3d_t v1 )
{
	float		sc;
	sc = v0[0]*v1[0] + v0[1]*v1[1] + v0[2]*v1[2];
	return sc;
}

void Vec3dCrossProduct( vec3d_t vout, vec3d_t v1, vec3d_t v2 )
{
	// vout = v1 x v2

	vout[0] = v1[1]*v2[2] - v1[2]*v2[1];
	vout[1] = v1[2]*v2[0] - v1[0]*v2[2];
	vout[2] = v1[0]*v2[1] - v1[1]*v2[0];
}



// complex 3D

float SolveLineAndPlane( vec3d_t l0, vec3d_t l1, vec3d_t e, vec3d_t n )
{

	// l0, l1 legen gerade fest ( l0 ist aufpunkt ).
	// e      ist aufpunkt der ebene
	// n      ist normalenvektor der ebene
	// return lambda der gerade

	float		dvd;
	float		dvr;
	float		lambda;

	dvd = n[0]*( e[0]  - l0[0] ) + n[1]*( e[1]  - l0[1] ) + n[2]*( e[2]  - l0[2] );

	dvr = n[0]*( l1[0] - l0[0] ) + n[1]*( l1[1] - l0[1] ) + n[2]*( l1[2] - l0[2] );
	
	lambda = dvd / dvr;

	return lambda;
}

//
// plane
//

void Vec3dInitPlane( plane_t *p, vec3d_t v0, vec3d_t v1, vec3d_t v2 )
{
	vec3d_t		tv1, tv2;

	Vec3dSub( tv1, v1, v0 );
	Vec3dSub( tv2, v2, v0 );

	Vec3dCrossProduct( p->norm, tv1, tv2 );
	Vec3dUnify( p->norm );
	
	p->dist = Vec3dDotProduct( v0, p->norm );
}

