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



// lib_bezier.h

#ifndef __lib_bezier
#define __lib_bezier

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "lib_math.h"

typedef struct curve_ctrl_s
{
	int		order;
	vec3d_t		cp[3];		// variable sized
} curve_ctrl_t;

typedef struct curve_points_s
{
	int		pointnum;
	vec3d_t		p[10];			// variable sized
} curve_points_t;

typedef struct surface_ctrl_s
{
	int		uorder;
	int		vorder;
	vec3d_t		cp[12];		// variable sized
} surface_ctrl_t;

typedef struct surface_points_s 
{
	int		upointnum;
	int		vpointnum;
	vec3d_t		p[100];		// variable sized
} surface_points_t;

typedef struct uv_points_s
{
	int	upointnum;
	int	vpointnum;
	vec2d_t	p[16];
} uv_points_t;

curve_ctrl_t* NewBezierCurve( int order );
void FreeBezierCurve( curve_ctrl_t *c );
surface_ctrl_t* NewBezierSurface( int uorder, int vorder );
void FreeBezierSurface( surface_ctrl_t *sc );
void SetCurveCtrlPoint( curve_ctrl_t *c, int u, vec3d_t pos );
void SetCurveCtrlPoint3f( curve_ctrl_t *c, int u, fp_t x, fp_t y, fp_t z );
void SetSurfaceCtrlPoint( surface_ctrl_t *sc, int u, int v, vec3d_t pos );
void SetSurfaceCtrlPoint3f( surface_ctrl_t *sc, int u, int v, fp_t x, fp_t y, fp_t z );
void GetSurfaceCtrlPoint( surface_ctrl_t *sc, int u, int v, vec3d_t pos );
void EvalCurvePoint( curve_ctrl_t *c, fp_t u, vec3d_t out );
curve_points_t* EvalCurvePoints( curve_ctrl_t *cc, int unum );
void EvalSurfacePoint( surface_ctrl_t *sc, fp_t u, fp_t v, vec3d_t out );
surface_points_t* EvalSurfacePoints( surface_ctrl_t *sc, int unum, int vnum );
void GetSurfacePoint( surface_points_t *mesh, int u, int v, vec3d_t pos );
void GetCurvePoint( curve_points_t *curve, int u, vec3d_t pos );
void FreeCurvePoints( curve_points_t *c );
void FreeSurfacePoints( surface_points_t *sp );

uv_points_t * EvalUVPoints( surface_ctrl_t *sc, int unum, int vnum );
void GetUVPoint( uv_points_t *pts, int u, int v, vec2d_t p );
void FreeUVPoints( uv_points_t *pts );


void SetSurfacePoint( surface_points_t *sp, int u, int v, vec3d_t pos );
surface_points_t* NewSurfacePoints( int unum, int vnum );
surface_points_t* EvalSurfaceNormals( surface_points_t *in );

#ifdef __cplusplus
}
#endif

#endif
