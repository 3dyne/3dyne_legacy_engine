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



#include "l_locals.h"



planeSlope GetPlaneSlope( vec3d_t norm )
{
	vec3d_t		up = { 0, 1, 0 };
	fp_t		dot;

	dot = Vec3dDotProduct( up, norm );

	if ( fabs( dot ) < 0.05 )
	{
//		printf( "GetPlaneSlope: wall\n" );
		return PlaneSlope_wall;
	}

	if ( fabs( dot ) > 0.95 )
	{
//		printf( "GetPlaneSlope: floor\n" );
		return PlaneSlope_floor;
	}

//	printf( "GetPlaneSlope: slope\n" );
	return PlaneSlope_slope;

}

void PlayerPhysic( arche_t *a )
{
#if 0	

	li.A_PlayerUpdateMovement( a );
	//li.SH_Printf( "speed: %f", a->player->speed );
	
	Vec3dMA( a->rinfo.origin, a->player->speed, a->player->dir, a->rinfo.origin );

	if ( a->player->speed )
		a->player->speed = 0;

	a->rinfo.yaw = a->player->yaw;
	a->rinfo.pitch = a->player->pitch;
#endif
}

planeSlope TryMoveVolume( move_volume_t *vol )
{
	int		i;
	g_trace_t	*tr;

	planeSlope	slope;

	vec3d_t		min;
	vec3d_t		max;

	vec3d_t		trymin;
	vec3d_t		trymax;

	vec3d_t		pos, trypos;
	
	fp_t		dist;
	vec3d_t		norm;
	vec3d_t		p1, p2, vec;
	vec3d_t		minspace;
	vec3d_t		maxspace;

	slope = PlaneSlope_none;
	
	L_MoveVolumeGetFromBoundBox( vol, min, max );
	L_MoveVolumeGetToBoundBox( vol, trymin, trymax );

	L_MoveVolumeGetFromPos( vol, pos );
	L_MoveVolumeGetToPos( vol, trypos );

	tr = li.TraceBB( min, max, trymin, trymax );	

	if ( tr->plane )
	{	   
		Vec3dCopy( norm, tr->plane->norm );

		slope = GetPlaneSlope( norm );

		dist = tr->plane->dist;

		Vec3dProjectOnPlane( p1, norm, dist, min );
		Vec3dProjectOnPlane( p2, norm, dist, trymin );

		Vec3dSub( vec, p2, p1 );

		Vec3dAdd( trymin, vec, min );
		Vec3dAdd( trymax, vec, max );

		tr = li.TraceBB( min, max, trymin, trymax );
		if ( tr->plane )                                                
                {			
			printf( "bounce again\n" );
//			Vec3dCopy( trypos, pos );
//			L_MoveVolumeSetToPos( vol, pos );
			L_MoveVolumeStay( vol );
			return GetPlaneSlope( tr->plane->norm );
		}
	}

	Vec3dCopy( min, trymin );
	Vec3dCopy( max, trymax );
	L_MoveVolumeGetRelativeBoundBox( vol, minspace, maxspace );

	Vec3dSub( trypos, trymin, minspace );

	L_MoveVolumeSetToPos( vol, trypos );

	return slope;	
}

planeSlope TryMove( vec3d_t pos, vec3d_t trypos )
{
	int		i;
	g_trace_t	*tr;

	planeSlope	slope;

	vec3d_t		min;
	vec3d_t		max;

	vec3d_t		trymin;
	vec3d_t		trymax;
	
	fp_t		dist;
	vec3d_t		norm;
	vec3d_t		p1, p2, vec;
	vec3d_t		minspace = { 16, 64, 16 };
	vec3d_t		maxspace = { 16, 0, 16 };

	Vec3dSub( min, pos, minspace );
	Vec3dAdd( max, pos, maxspace );
	Vec3dSub( trymin, trypos, minspace );
	Vec3dAdd( trymax, trypos, maxspace );

#if 0
	for ( i = 0; i < 3; i++ )
	{
		min[i] = pos[i] - 16.0;
		max[i] = pos[i] + 16.0;

		trymin[i] = trypos[i] - 16.0;
		trymax[i] = trypos[i] + 16.0;
	}
#endif

	slope = PlaneSlope_none;

	tr = li.TraceBB( min, max, trymin, trymax );	
	
	if ( tr->plane )
	{	   
		Vec3dCopy( norm, tr->plane->norm );

		slope = GetPlaneSlope( norm );

		dist = tr->plane->dist;

		Vec3dProjectOnPlane( p1, norm, dist, min );
		Vec3dProjectOnPlane( p2, norm, dist, trymin );

		Vec3dSub( vec, p2, p1 );

		Vec3dAdd( trymin, vec, min );
		Vec3dAdd( trymax, vec, max );

		tr = li.TraceBB( min, max, trymin, trymax );
		if ( tr->plane )                                                
                {			
			printf( "bounce again\n" );
			Vec3dCopy( trypos, pos );
			return GetPlaneSlope( tr->plane->norm );;
		}
	}

	Vec3dCopy( min, trymin );
	Vec3dCopy( max, trymax );
	
//	Vec3dAdd( vec, min, max );
//	Vec3dScale( trypos, 0.5, vec );	
	Vec3dAdd( trypos, trymin, minspace );
	return slope;
}

void TryMoveVolumeToGround( move_volume_t *vol )
{
	vec3d_t		pos;

	L_MoveVolumeGetFromPos( vol, pos );
	pos[1]-=16.0;
	L_MoveVolumeSetToPos( vol, pos );

	TryMoveVolume( vol );       
}


void L_ToGround( ati_t *ati )
{
	vec3d_t		pos, newpos;

	__chkptr( ati->ftbl->GetOrigin );
	__chkptr( ati->ftbl->SetOrigin );

	ati->ftbl->GetOrigin( ati, pos );
	Vec3dCopy( newpos, pos );

	newpos[1]-=8.0;

	TryMove( pos, newpos );
	
	ati->ftbl->SetOrigin( ati, newpos );
}
	

bool_t TryStraightMove( vec3d_t from, vec3d_t to )
{
	int		i;

	vec3d_t		min, max;
	vec3d_t		trymin, trymax;

	g_trace_t	*tr;

	for ( i = 0; i < 3; i++ )
	{
		min[i] = from[i] - 16.0;
		max[i] = from[i] + 16.0;

		trymin[i] = to[i] - 16.0;
		trymax[i] = to[i] + 16.0;
	}

	tr = li.TraceBB( min, max, trymin, trymax );

	if ( tr->plane )
	{
		return false;
	}

	return true;
}
	
