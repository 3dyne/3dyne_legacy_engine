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



// cl_proxy.c

#include "cl_state.h"
#include "cl_head.h"

#include "lib_math.h"
#include "shock.h"

#define CL_PROXY_KILLBOX_MAX	( 128 )

static int	num_killbox;
static vec3d_t		mins[CL_PROXY_KILLBOX_MAX];
static vec3d_t		maxs[CL_PROXY_KILLBOX_MAX];
static vec3d_t		centers[CL_PROXY_KILLBOX_MAX];

static fp_t Vec3dCalcBBVolume( vec3d_t min, vec3d_t max )
{
	return (max[0]-min[0])*(max[1]-min[1])*(max[2]-min[2]);
}

static fp_t Vec3dCalcBBIntersection( vec3d_t min1, vec3d_t max1, vec3d_t min2, vec3d_t max2 )
{
	int		i;
	vec3d_t		min, max;

	for ( i = 0; i < 3; i++ )
	{
		if ( min1[i] > max2[i] || max1[i] < min2[i] )
			return 0.0;
		if ( min2[i] > min1[i] )
			min[i] = min2[i];
		else
			min[i] = min1[i];

		if ( max2[i] < max1[i] )
			max[i] = max2[i];
		else
			max[i] = max1[i];
	}

	return Vec3dCalcBBVolume( min, max );
}

static void Vec3dCalcBBCenter( vec3d_t center, vec3d_t min, vec3d_t max )
{
	vec3d_t		delta;

	Vec3dSub( delta, max, min );
	Vec3dMA( center, 0.5, delta, min );
}

static bool_t Vec3dBBTestPoint( vec3d_t min, vec3d_t max, vec3d_t p )
{
	int		i;

	for ( i = 0; i < 3; i++ )
	{
		if ( p[i] < min[i] || p[i] > max[i] )
		{
			return false;
		}
	}

	return true;
}

/*
  ==============================
  CL_ProxyKillBoxMapReset

  ==============================
*/
void CL_ProxyKillBoxMapReset( void )
{
	num_killbox = 0;
}

/*
  ==============================
  CL_ProxyKillBoxMapAdd

  ==============================
*/
void CL_ProxyKillBoxMapAdd( byte_iter_t *bi )
{
	if ( num_killbox >= CL_PROXY_KILLBOX_MAX )
	{
		__error( "reached CL_PROXY_KILLBOX_MAX\n" );
	}
	
	ByteIter_Unpack3fv( bi, centers[num_killbox] );
	ByteIter_Unpack3fv( bi, mins[num_killbox] );	// rel_min
	ByteIter_Unpack3fv( bi, maxs[num_killbox] );	// rel_max

	Vec3dAdd( mins[num_killbox], centers[num_killbox], mins[num_killbox] );
	Vec3dAdd( maxs[num_killbox], centers[num_killbox], maxs[num_killbox] );

	num_killbox++;
}

/*
  ==============================
  CL_ProxyKillBoxMapTestBB

  ==============================
*/
bool_t CL_ProxyKillBoxMapTestBB( vec3d_t min, vec3d_t max, vec3d_t kill_center )
{
	int		i;
	fp_t		vol, best_vol;
// 	int		best_i;

	best_vol = -999999.9;	// maximum search;
// 	best_i = -1;

	for ( i = 0; i < num_killbox; i++ )
	{
		vol = Vec3dCalcBBIntersection( min, max, mins[i], maxs[i] );

		if ( vol == 0.0 )
			continue;

		if ( vol > best_vol )
		{
			best_vol = vol;
// 			best_i = i;
		}
	}

	if ( i == -1 )
	{
		return false;
	}

	Vec3dCalcBBCenter( kill_center, min, max );
	return true;
}


/*
  ==============================
  CL_ProxyKillBoxMapTestPoint

  ==============================
*/
bool_t CL_ProxyKillBoxMapTestPoint( vec3d_t point, vec3d_t kill_center )
{
	int		i;
	fp_t		dist, best_dist;
	int		best_i;
	vec3d_t		delta;

	best_dist = 999999.9;	// minimum search;
	best_i = -1;

	for ( i = 0; i < num_killbox; i++ )
	{
		if ( !Vec3dBBTestPoint( mins[i], maxs[i], point ) )
		{
			continue;
		}
		
		Vec3dSub( delta, centers[i], point );
		dist = Vec3dLen( delta );

		if ( dist < best_dist )
		{
			best_dist = dist;
			best_i = i;

			Vec3dCopy( kill_center, centers[i] );
		}
	}
	
	if ( best_i == -1 )
	{
		return false;
	}

	return true;
}
