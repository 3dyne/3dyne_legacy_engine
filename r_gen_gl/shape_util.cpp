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



// shape_util.c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "lib_math.h"

#include "shape_util.h"

/*
  ==============================
  ShapeU_AreSamePoints

  ==============================
*/
bool_t ShapeU_AreSamePoints( vec3d_t p1, vec3d_t p2 )
{
	int		i;
	
	for ( i = 0; i < 3; i++ )
	{
		if ( fabs( p1[i]-p2[i] ) > SHAPEU_EQUAL_DIST )
			break;
	}

	if ( i != 3 )
		return false;
	return true;			
}


/*
  ==============================
  ShapeU_AreCollinearPoints

  ==============================
*/
bool_t ShapeU_AreCollinearPoints( vec3d_t p1, vec3d_t p2, vec3d_t t )
{
	vec3d_t		v1, v2;

//	Vec3dPrint( p1 );
//	Vec3dPrint( t );
//	Vec3dPrint( p2 );

	Vec3dSub( v1, p1, t );
	Vec3dSub( v2, t, p2 );
	Vec3dUnify( v1 );
	Vec3dUnify( v2 );
	
	if ( Vec3dDotProduct( v1, v2 ) > SHAPEU_ANGLE_COLLINEAR )
		return true;
	return false;
}



/*
  ==============================
  ShapeU_CalcCenter

  ==============================
*/
void ShapeU_VACalcCenter( vec3d_t center, int num, vec3d_t pts[] )
{
	int		i;
	fp_t		scale;

	Vec3dInit( center, 0, 0, 0 );

	for ( i = 0; i < num; i++ )
		Vec3dAdd( center, center, pts[i] );

	scale = 1.0/((fp_t)(num));
	Vec3dScale( center, scale, center );
}
