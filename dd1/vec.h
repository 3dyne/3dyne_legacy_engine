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



// vec.h

#ifndef		__vec_h_included
#define		__vec_h_included

#ifdef __cplusplus                                                              
extern "C"                                                                      
{                                                                               
#endif                                                                          

typedef float	vec5d_t[5]; 
typedef float	vec3d_t[3];
typedef float	vec2d_t[2];

typedef struct {
	float		dist;
	vec3d_t		norm;
} plane_t;

void Vec5dCopy( vec5d_t out, vec5d_t in );
       
void Vec2dPrint( vec2d_t v );
void Vec2dInit( vec2d_t out, float x0, float x1 );
void Vec2dCopy( vec2d_t out, vec2d_t in );	

void Vec3dPrint( vec3d_t v );
void Vec3dInit( vec3d_t v, float x0, float x1, float x2 );
void Vec3dCopy( vec3d_t vout, vec3d_t vin );
void Vec3dAdd( vec3d_t v0, vec3d_t v1, vec3d_t v2 ); // v0 = v1 + v2
void Vec3dSub( vec3d_t v0, vec3d_t v1, vec3d_t v2 ); // v0 = v1 - v2;
void Vec3dScale( vec3d_t vout, float s, vec3d_t vin ); // vout = s * vin
void Vec3dMA( vec3d_t vout, float s, vec3d_t v, vec3d_t vin ); // vout = s * v + vin
float Vec3dLen( vec3d_t v );
void Vec3dUnify( vec3d_t v );
float Vec3dDotProduct( vec3d_t v0, vec3d_t v1 ); // float = v0 o v1
void Vec3dCrossProduct( vec3d_t vout, vec3d_t v1, vec3d_t v2 ); // vout = v1 x v2

float SolveLineAndPlane( vec3d_t l0, vec3d_t l1, vec3d_t e, vec3d_t n );

void Vec3dInitPlane( plane_t *p, vec3d_t v0, vec3d_t v1, vec3d_t v2 );

#ifdef __cplusplus                                                              
}                                                                               
#endif

#endif
