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



// r_hvisclip.c

#include "render.h"
#include "r_state.h"

#define MAX_HVISCLIP_PLANES	( 128 )

static int		planenum;
static vec3d_t		norms[MAX_HVISCLIP_PLANES];
static fp_t		dists[MAX_HVISCLIP_PLANES];


bool_t R_HVIS_PortalInFrontOfNear( frustum_t *f, portal_t *pt )
{
	int		i;

	for ( i = 0; i < pt->pointnum; i++ )
	{
		fp_t		*v;
		fp_t		d;

		v = r_points[i+pt->startpoint];
		d = Vec3dDotProduct( v, f->planes[Frustum_near].norm ) - f->planes[Frustum_near].dist;

		if ( d < -FRUSTUM_ON_EPSILON )
			return true;;	// ok, at least on point backside near
	}
	return false;
}

int R_HVIS_SetupVisclip( frustum_t *f, portal_t *pt, bool_t flipclip )
{
	vec3d_t		from[4];
	int		tnum;
	vec3d_t		*through;

	int		i, j, k, l;
	vec3d_t		v1, v2;
	fp_t		d;
	fp_t		length;
	int		counts[3];
	bool_t		fliptest;

	R_FrustumGetNearpoly( f, from );
      
	through = (vec3d_t *)r_points[pt->startpoint];
	tnum = pt->pointnum;

	planenum = 0;
//	if ( !R_HVIS_PortalInFrontOfNear( f, pt ) )
//		return false;


	for ( i = 0; i < 4; i++ )
	{
		l = (i+1) % 4;
		Vec3dSub( v1, from[l], from[i] );
		
		for ( j = 0; j < tnum; j++ )
		{
			
			Vec3dSub( v2, through[j], from[i] );
			
			norms[planenum][0] = v1[1]*v2[2] - v1[2]*v2[1];
			norms[planenum][1] = v1[2]*v2[0] - v1[0]*v2[2];
			norms[planenum][2] = v1[0]*v2[1] - v1[1]*v2[0];

			length = norms[planenum][0]*norms[planenum][0]
				+norms[planenum][1]*norms[planenum][1] 
				+norms[planenum][2]*norms[planenum][2];

			if ( length < ON_EPSILON )
				continue;

			length = 1.0/sqrt(length);

			norms[planenum][0] *= length;
			norms[planenum][1] *= length;
			norms[planenum][2] *= length;

			dists[planenum] = Vec3dDotProduct( through[j], norms[planenum] );

			fliptest = false;
			for ( k = 0; k < 4; k++ )
			{
				if ( k == i || k == l )
					continue;

				d = Vec3dDotProduct( from[k], norms[planenum] ) - dists[planenum];
				if ( d < -ON_EPSILON )
				{
					fliptest = false;
					break;
				}
				else if ( d > ON_EPSILON )
				{
					fliptest = true;
					break;
				}
			}
			if ( k == 4 )
				continue;

			if ( fliptest )
			{
				Vec3dFlip( norms[planenum], norms[planenum] );
				dists[planenum] = -dists[planenum];
			}

			counts[0] = counts[1] = counts[2] = 0;
			for ( k = 0; k < tnum; k++ )
			{
				if ( k == j )
					continue;
				d = Vec3dDotProduct( through[k], norms[planenum] ) - dists[planenum];
				if ( d < -ON_EPSILON )
					break;
				else if ( d > ON_EPSILON )
					counts[0]++;
				else
					counts[2]++;
			}

			if ( k != tnum )
				continue;
			if ( !counts[0] )
				continue;

			if ( flipclip )
			{
				Vec3dFlip( norms[planenum], norms[planenum] );
				dists[planenum] = -dists[planenum];
			}

			planenum++;
		}
	}

	return planenum;
}


	
/*
  ==============================
  R_HVIS_VisclipTestBB

  ==============================
*/
bool_t R_HVIS_VisclipTestBB( vec3d_t min, vec3d_t max )
{
	int		i;

	for ( i = 0; i < planenum; i++ )
	{
		plane_t		pl;
		Vec3dCopy( pl.norm, norms[i] );
		pl.dist = dists[i];

		if ( PlaneTestBB( &pl, min, max ) == PlaneTest_front )
		{
			// bb clipped away
			return false;
		}
	}

	return true;
}
