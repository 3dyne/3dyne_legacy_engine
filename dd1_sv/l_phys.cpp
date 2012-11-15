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



// l_phys.c

#include "server.h"

#include "l_defs.h"
#include "l_head.h"

#include "shock.h"

planeSlope L_GetPlaneSlope( vec3d_t norm )
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

planeSlope L_TryMoveVolume( move_volume_t *vol )
{
	g_trace_t	tr;

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

	g_api.Map_TraceBB( &tr, min, max, trymin, trymax );	

	if ( tr.plane )
	{	   
//		printf( "trace hit!\n" );

		Vec3dCopy( norm, tr.plane->norm );

		slope = L_GetPlaneSlope( norm );

		dist = tr.plane->dist;

		Vec3dProjectOnPlane( p1, norm, dist, min );
		Vec3dProjectOnPlane( p2, norm, dist, trymin );

		Vec3dSub( vec, p2, p1 );

		Vec3dAdd( trymin, vec, min );
		Vec3dAdd( trymax, vec, max );

		g_api.Map_TraceBB( &tr, min, max, trymin, trymax );
		if ( tr.plane )                                                
                {			
			printf( "bounce again\n" );
//			Vec3dCopy( trypos, pos );
//			L_MoveVolumeSetToPos( vol, pos );
			L_MoveVolumeStay( vol );
			return L_GetPlaneSlope( tr.plane->norm );
		}
	}

	Vec3dCopy( min, trymin );
	Vec3dCopy( max, trymax );
	L_MoveVolumeGetRelativeBoundBox( vol, minspace, maxspace );

	Vec3dSub( trypos, trymin, minspace );

	L_MoveVolumeSetToPos( vol, trypos );

	return slope;	
}

void L_TryMoveVolumeToGround( move_volume_t *vol )
{
	vec3d_t		pos;

	L_MoveVolumeGetFromPos( vol, pos );
	pos[1]-=16.0;
	L_MoveVolumeSetToPos( vol, pos );

	L_TryMoveVolume( vol );       
}

void L_ClipMoveVolume( move_volume_t *vol )
{
	g_trace_t	tr;

// 	planeSlope	slope;

	vec3d_t		min;
	vec3d_t		max;

	vec3d_t		trymin;
	vec3d_t		trymax;

	vec3d_t		pos, trypos;
	
	fp_t		dist;
	vec3d_t		norm;
	vec3d_t		minspace;
	vec3d_t		maxspace;

// 	slope = PlaneSlope_none;
	
	L_MoveVolumeGetFromBoundBox( vol, min, max );
	L_MoveVolumeGetToBoundBox( vol, trymin, trymax );

	L_MoveVolumeGetFromPos( vol, pos );
	L_MoveVolumeGetToPos( vol, trypos );

	g_api.Map_TraceBB( &tr, min, max, trymin, trymax );	

	if ( tr.plane )
	{	   
		vec3d_t		dir;
		vec3d_t		clip_dir;

		Vec3dCopy( norm, tr.plane->norm );

		/*slope =*/ L_GetPlaneSlope( norm );

		dist = tr.plane->dist;

		Vec3dSub( dir, trymin, min );
		Vec3dClipDirection( clip_dir, min, dir, norm, dist );
		
		Vec3dAdd( trymin, clip_dir, min );
		Vec3dAdd( trymax, clip_dir, max );

	}

	Vec3dCopy( min, trymin );
	Vec3dCopy( max, trymax );
	L_MoveVolumeGetRelativeBoundBox( vol, minspace, maxspace );

	Vec3dSub( trypos, trymin, minspace );

	L_MoveVolumeSetToPos( vol, trypos );
}

/*
  ==================================================
  physic revison
  
  ==================================================
*/

#define ZERO_EPSILON	( 0.0001 )

static void CalcBoundBoxPlaneDists ( vec3d_t min, vec3d_t max, vec3d_t norm, fp_t dist, fp_t *d1, fp_t *d2 )
{
	int             i;                                                      
        vec3d_t         bb[2];                                                  
                                                                                
        for ( i = 0; i < 3; i++ )                                               
        {                                                                       
                if ( norm[i] < 0.0 )                                        
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
                                                                                
        *d1 = Vec3dDotProduct( bb[0], norm ) - dist;                   
        *d2 = Vec3dDotProduct( bb[1], norm ) - dist; 

	if ( *d1 > -ZERO_EPSILON && *d1 < ZERO_EPSILON )
		*d1 = 0.0;

	if ( *d2 > -ZERO_EPSILON && *d2 < ZERO_EPSILON )
		*d2 = 0.0;


}

fp_t L_TraceClipBoundBoxMove( vec3d_t rel_min, vec3d_t rel_max, vec3d_t from, vec3d_t to, vec3d_t clip_pos, vec3d_t clip_norm )
{
	vec3d_t		min_to;
	vec3d_t		max_to;
	vec3d_t		min_from;
	vec3d_t		max_from;
	
	g_trace_t	tr;
	fp_t		d1, d2;
	static fp_t		scale;
	fp_t		d_from, d_to;
	vec3d_t		delta;

//	printf( " ----- Bound Box Move ---\n" );

//	printf( "move_bound_box_from: " );
//	Vec3dPrint( from );
//	printf( "move_bound_box_to: " );
//	Vec3dPrint( to );

	Vec3dAdd( min_to, to, rel_min );
	Vec3dAdd( max_to, to, rel_max );

	Vec3dAdd( min_from, from, rel_min );
	Vec3dAdd( max_from, from, rel_max );

	tr.surf_flags = BM_SURF_MAP_WALL;
	g_api.Map_TraceBB( &tr, min_from, max_from, min_to, max_to );

	if ( !tr.plane )
	{
		Vec3dCopy( clip_pos, to );
		Vec3dInit( clip_norm, 0, 0, 0 );
		return 1.0;
	}

//	printf( "plane: %p\n", tr.plane );

	CalcBoundBoxPlaneDists ( min_from, max_from, tr.plane->norm, tr.plane->dist, &d1, &d2 );
	d_from = d1<d2?d1:d2;
	CalcBoundBoxPlaneDists ( min_to, max_to, tr.plane->norm, tr.plane->dist, &d1, &d2 );
	d_to = d1<d2?d1:d2;

	if ( d_from*d_to > 0.0 )
	{
		__warning( "trace: 'from' positon not in empty space ?\n" );
	}
	
	scale = d_from/(d_from-d_to);	

	scale = tr.scale;

	if ( scale < 0.0 || scale > 1.0 )
	{
		__error( "got invalide scale %f from trace\n", scale );
	}

	Vec3dSub( delta, to, from );
	Vec3dMA( clip_pos, scale, delta, from );
	Vec3dCopy( clip_norm, tr.plane->norm );

//	printf( "d1 %f, d2 %f, scale %f\n", d_from, d_to, scale );

	return scale;
}

fp_t L_TraceClipLineMove( vec3d_t from, vec3d_t dir, fp_t len )
{
	g_trace_t	tr;
	vec3d_t		to;
	
	fp_t		d1, d2;
	fp_t		scale;

	Vec3dMA( to, len, dir, from );

	tr.surf_flags = BM_SURF_MAP_WALL;
	g_api.Map_TraceLine( &tr, from, to );

	if ( !tr.plane )
	{
		return 1.0;
	}
	
	d1 = Vec3dDotProduct( from, tr.plane->norm ) - tr.plane->dist;
	d2 = Vec3dDotProduct( to, tr.plane->norm ) - tr.plane->dist;	

	scale = d1/(d1-d2);

	return scale;
}

/*
  ==================================================
  higher level moves

  ==================================================
*/
#define VELOCITY_MOVE_MAX_CLIP_NORMS	( 6 )

void L_ClipVelocity( vec3d_t out, vec3d_t in, vec3d_t norm )
{
	fp_t	dot;

	dot = Vec3dDotProduct( in, norm );

	out[0] = in[0] - norm[0]*dot;
	out[1] = in[1] - norm[1]*dot;
	out[2] = in[2] - norm[2]*dot;	
}

bool_t L_VelocityMove( vec3d_t rel_min, vec3d_t rel_max, vec3d_t origin, vec3d_t velocity, fp_t time )
{
	int		i, j;
	int		count;
	fp_t		scale;
	int	num_norm;
	vec3d_t	norms[VELOCITY_MOVE_MAX_CLIP_NORMS];
	vec3d_t	clip_norm;

	vec3d_t	initial_velocity;
	vec3d_t	ref_velocity;
	vec3d_t	velocity2;

	vec3d_t	to;
	vec3d_t	clip_pos;

	num_norm = 0;

//	printf( " ----------- Velocity Move ---\n" );

	Vec3dCopy( initial_velocity, velocity );
	Vec3dCopy( ref_velocity, velocity );

	for( count = 0; count < 5; count++ )
	{
//		printf( "time: %.2f\n", time );
		Vec3dMA( to, time, velocity, origin );
		
//		printf( "from: " );
//		Vec3dPrint( origin );
//		printf( "to: " );
//		Vec3dPrint( to );

		scale = L_TraceClipBoundBoxMove( rel_min, rel_max, origin, to, clip_pos, clip_norm );

		if ( scale > 0.0 )
		{
			Vec3dCopy( origin, clip_pos );
			Vec3dCopy( ref_velocity, velocity );
			num_norm = 0;
		}

		if ( scale == 1.0 )
		{
//			printf( "complete_trace\n" );
			break;
		}

		if ( scale == 0.0 )
		{ 			
			//
			// trace was blocked at all
			//

//			printf( "trace_total_blocked\n" );
		}
		else 
		{
//			printf( "trace_frac_blocked\n" );
		}

		time -= time*scale;

		//
		// add clip_norm to norm table
		//

		if ( num_norm >= VELOCITY_MOVE_MAX_CLIP_NORMS )
		{
			__warning( "reached VELOCITY_MOVE_MAX_CLIP_NORMS\n" );
			Vec3dInit( velocity, 0, 0, 0 );
			break;
		}
		Vec3dCopy( norms[num_norm], clip_norm );
		num_norm++;

		//
		// calc a better velocity from norm table
		//
	       
		for ( i = 0; i < num_norm; i++ )
		{
			L_ClipVelocity( velocity2, ref_velocity, norms[i] );
			for ( j = 0; j < num_norm; j++ )
			{
				if ( i != j )
				{
					if ( Vec3dDotProduct( velocity2, norms[j] ) < 0 )
					{
						break;
					}
				}
			}
			if ( j == num_norm )
			{
				break;
			}
		}

		if ( i != num_norm )
		{
			//
			// found a velocity2 that is ok for all norms
			//
//			printf( "found velocity2\n" );
			
//			Vec3dPrint( velocity );
//			Vec3dPrint( velocity2 );
			Vec3dCopy( velocity, velocity2 );
		}
		else
		{
			vec3d_t		tmp;
			fp_t		dot;

//			printf( "use_cross\n" );

			if ( num_norm != 2 )
			{
//				printf( "failed num_norm %d\n", num_norm );
				Vec3dInit( velocity, 0, 0, 0 );
				break;
			}

			Vec3dCrossProduct( tmp, norms[0], norms[1] );
			dot = Vec3dDotProduct( tmp, velocity );
			Vec3dScale( velocity, dot, tmp );
		}

		if ( Vec3dDotProduct( velocity, initial_velocity ) <= 0 )
		{
//			printf( "backward\n" );
			Vec3dInit( velocity, 0, 0, 0 );
			break;
		}
	}

	if ( count == 0 )
		return true;

	return false;
}
