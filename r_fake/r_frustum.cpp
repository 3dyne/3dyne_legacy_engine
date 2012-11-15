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



// r_frustum.c

/*
  =============================================================================
  frustum functions impl

  =============================================================================
*/
#include <string.h>
#include "shock.h"
#include "r_frustum.h"

/*
  ==================================================
  frustum setup 

  ==================================================
*/

/*
  ====================
  R_InitBaseFrustum

  ====================
*/
void R_InitBaseFrustum( frustum_t *f, fp_t width, fp_t height, fp_t viewdist )
{
	fp_t		wh, hh;

	//	printf( "R_InitBaseFrustum: %f %f %f\n", width, height, viewdist );
	//	memset( f, 0, sizeof( frustum_t ));
	wh = width * 0.5;
	hh = height * 0.5;

	Vec3dInit( f->viewcenter, 0.0, 0.0, -viewdist );
	Vec3dInit( f->topleft, -wh, hh, 0.0 );
	Vec3dInit( f->topright, wh, hh, 0.0 );
	Vec3dInit( f->bottomleft, -wh, -hh, 0.0 );
	Vec3dInit( f->bottomright, wh, -hh, 0.0 );
}

/*
  ====================
  R_FrustumCalcPlanes

  ====================
*/
void R_FrustumCalcPlanes( frustum_t *f )
{

	// left plane
	Vec3dInitPlane( f->planes[Frustum_left].norm, &f->planes[Frustum_left].dist, 
			f->viewcenter,
			f->bottomleft,
			f->topleft );

	// right plane
	Vec3dInitPlane( f->planes[Frustum_right].norm, &f->planes[Frustum_right].dist, 
			f->viewcenter,
			f->topright,
			f->bottomright );

	// top plane
	Vec3dInitPlane( f->planes[Frustum_top].norm, &f->planes[Frustum_top].dist, 
			f->viewcenter,
			f->topleft,
			f->topright );

	// bottom plane
	Vec3dInitPlane( f->planes[Frustum_bottom].norm, &f->planes[Frustum_bottom].dist, 
			f->viewcenter,
			f->bottomright,
			f->bottomleft );

	// near plane
	Vec3dInitPlane( f->planes[Frustum_near].norm, &f->planes[Frustum_near].dist, 
			f->topleft,
			f->topright,
			f->bottomright );
}

/*
  ====================
  R_FrustumGetNearpoly

  ====================
*/
void R_FrustumGetNearpoly( frustum_t *f, vec3d_t nearpoly[] )
{
	Vec3dCopy( nearpoly[0], f->topleft );
	Vec3dCopy( nearpoly[1], f->topright );
	Vec3dCopy( nearpoly[2], f->bottomright );
	Vec3dCopy( nearpoly[3], f->bottomleft );
}

/*
  ====================
  R_FrustumRotate

  ====================
*/
void R_FrustumRotate( frustum_t *f, matrix3_t rotmat )
{      
	Matrix3Vec3dRotate( f->topleft, f->topleft, rotmat );
	Matrix3Vec3dRotate( f->topright, f->topright, rotmat );
	Matrix3Vec3dRotate( f->bottomleft, f->bottomleft, rotmat );
	Matrix3Vec3dRotate( f->bottomright, f->bottomright, rotmat );
	Matrix3Vec3dRotate( f->viewcenter, f->viewcenter, rotmat );	
}

/*
  ====================
  R_FrustumTranslate

  ====================
*/
void R_FrustumTranslate( frustum_t *f, vec3d_t vec )
{
	Vec3dAdd( f->topleft, vec, f->topleft );
	Vec3dAdd( f->topright, vec, f->topright );
	Vec3dAdd( f->bottomleft, vec, f->bottomleft );
	Vec3dAdd( f->bottomright, vec, f->bottomright );
	Vec3dAdd( f->viewcenter, vec, f->viewcenter );
}

/*
  ====================
  R_FrustumDump

  ====================
*/
void R_FrustumDump( frustum_t *f )
{
	printf( "R_FrustumDump:\n" );
	printf( " topleft: " ); Vec3dPrint( f->topleft );
	printf( " topright: " ); Vec3dPrint( f->topright );
	printf( " bottomleft: " ); Vec3dPrint( f->bottomleft );
	printf( " bottomright: " ); Vec3dPrint( f->bottomright );
	printf( " viewcenter: " ); Vec3dPrint( f->viewcenter );
}



/*
  ==================================================
  frustum test

  ==================================================
*/

/*
  ====================
  R_FrustumTestPoint

  ====================
*/
bool_t R_FrustumTestPoint( frustum_t *f, vec3d_t p )
{
	// test near plane
	if ( PlaneTestPoint( &f->planes[Frustum_near], p ) != PlaneTest_back )
		return false; // in front

	// test right plane
	if ( PlaneTestPoint( &f->planes[Frustum_right], p ) != PlaneTest_back )
		return false; // in front	

	// test left plane
	if ( PlaneTestPoint( &f->planes[Frustum_left], p ) != PlaneTest_back )
		return false; // in front	

	// test top plane
	if ( PlaneTestPoint( &f->planes[Frustum_top], p ) != PlaneTest_back )
		return false; // in front	

	// test bottom plane
	if ( PlaneTestPoint( &f->planes[Frustum_bottom], p ) != PlaneTest_back )
		return false; // in front	
	
	return true;
}


/*
  ====================
  R_FrustumTestBB

  ====================
*/

extern frustum_t r_frustum;
frustumClipMask R_FrustumTestBB( frustum_t *f, vec3d_t min, vec3d_t max )
{
	int	mask = Frustum_cnone;
	planeTestState	state;

	//	return mask;
	// test near plane
	//	printf( "minmax: (%f %f %f) (%f %f %f)\n", min[0], min[1], min[2], max[0], max[1], max[2] );


	//	printf( "ptr here: %p\n", f );
	//	printf( ">>>>>\n" );
	state = PlaneTestBB( &f->planes[Frustum_near], min, max );
	//printf( "<<<<<\n" );
	if ( state == PlaneTest_front )
		return Frustum_call;
	else if ( state == PlaneTest_split )
		mask |= Frustum_cnear;

	// test left plane
	state = PlaneTestBB( &f->planes[Frustum_left], min, max );
	if ( state == PlaneTest_front )
		return Frustum_call;
	else if ( state == PlaneTest_split )
		mask |= Frustum_cleft;

	// test right plane
	state = PlaneTestBB( &f->planes[Frustum_right], min, max );
	if ( state == PlaneTest_front )
		return Frustum_call;
	else if ( state == PlaneTest_split )
		mask |= Frustum_cright;

	// test top plane
	state = PlaneTestBB( &f->planes[Frustum_top], min, max );
	if ( state == PlaneTest_front )
		return Frustum_call;
	else if ( state == PlaneTest_split )
		mask |= Frustum_ctop;

	// test bottom plane
	state = PlaneTestBB( &f->planes[Frustum_bottom], min, max );
	if ( state == PlaneTest_front )
		return Frustum_call;
	else if ( state == PlaneTest_split )
		mask |= Frustum_cbottom;

	return (frustumClipMask)mask;
}

/*
  ==================================================
  fixpolygon test

  ==================================================
*/

#ifdef SIDE_FRONT
#undef SIDE_FRONT
#endif
#ifdef SIDE_BACK
#undef SIDE_BACK
#endif
#ifdef SIDE_ON
#undef SIDE_ON
#endif

#define		SIDE_FRONT		( 0 )
#define		SIDE_BACK		( 1 )
#define		SIDE_ON			( 2 )

#define _FP_SIGN_IS_NEG( x )		( *((unsigned int*)(&x))&(1<<31) )

#if 0
frustumClipMask R_FrustumTestDedicatedPlanes( frustum_t *f, fixpolygon_t *fix, frustumClipMask mask )
{
	int		i, j;
	int		bit;
	plane_t	*pl;
	int		counts[3];
	frustumClipMask mask2;

	mask2 = Frustum_cnone;
	for ( i = Frustum_near; i >= Frustum_left; i-- )	// check near first
	{
		bit = 1 << i;

		if ( ! ( bit & mask ) )
			continue;

		pl = &f->planes[i]; 

		// check points
		counts[0]=counts[1]=counts[2] = 0;
		for ( j = 0; j < fix->pointnum; j++ )
		{
			fp_t		*v;		     
			fp_t		d;

			v = r_vertices[r_vertexrefs[j+fix->startvertexref].vertex].v;
			
			d = _Vec3dDotProduct( v, pl->norm ) - pl->dist;
			
			if ( _FP_SIGN_IS_NEG( d ) )
				counts[SIDE_BACK]++;
			else 
				counts[SIDE_FRONT]++;

#if 0
			if ( d > FRUSTUM_ON_EPSILON )
				counts[SIDE_FRONT]++;
			else if ( d < -FRUSTUM_ON_EPSILON )
				counts[SIDE_BACK]++;
			else
				counts[SIDE_ON]++;
#endif
		}

//		printf( "%d %d %d\n", counts[SIDE_FRONT], counts[SIDE_BACK], counts[SIDE_ON] );

		if ( !counts[SIDE_BACK] )
			// plane clipped every thing
			return Frustum_call;
		if ( counts[SIDE_FRONT] )
			// plane split
			mask2 |= bit;
	}
	return mask2;
}
#endif

/*
  ==================================================
  frustum clip

  ==================================================
*/

/*
  ====================
  R_FrustumClipVertex3Buffer_devel

  for development use only
  ====================
*/

#define		CLIP_BUFFER_SIZE	( 256 )

static int Clip3Buffer( plane_t *pl, vec3d_t *outbuf, int innum, vec3d_t *inbuf )
{
	fp_t	dists[CLIP_BUFFER_SIZE];
	int	sides[CLIP_BUFFER_SIZE];
	int	counts[3];
	fp_t	d;
	int	i, j;
	int	num;
	fp_t	*p1, *p0;

	counts[0] = counts[1] = counts[2] = 0;
	
	for ( i = 0; i < innum; i++ )
	{
		d = Vec3dDotProduct( inbuf[i], pl->norm ) - pl->dist;
		dists[i] = d;

		if ( d > FRUSTUM_ON_EPSILON )
			sides[i] = SIDE_FRONT;
		else if ( d < -ON_EPSILON )
			sides[i] = SIDE_BACK;
		else
			sides[i] = SIDE_ON;

		counts[sides[i]]++;
	}

	if ( !counts[SIDE_FRONT] )
	{
		return -1;
	}

	if ( !counts[SIDE_BACK] )
	{
		return 0;
	}

	dists[i] = dists[0];
	sides[i] = sides[0];

	num = 0;
	for ( i = 0; i < innum; i++ )
	{
		p0 = inbuf[i];

		if ( sides[i] == SIDE_ON )
		{
			Vec3dCopy( outbuf[num++], p0 );
			continue;
		}
		
		if ( sides[i] == SIDE_BACK )
			Vec3dCopy( outbuf[num++], p0 );

		if ( sides[i] == SIDE_ON || sides[i+1] == sides[i] )
			continue;

		j = ((i+1)==innum) ? 0 : (i+1);
		p1 = inbuf[j];

		d = dists[i] / ( dists[i] - dists[i+1] );
		for ( j = 0; j < 3; j++ )
		{
			outbuf[num][j] = p0[j] + d*(p1[j]-p0[j]);
		}	      	
		num++;
	}

	return num;
}

static int Clip5Buffer( plane_t *pl, vec5d_t *outbuf, int innum, vec5d_t *inbuf )
{
	fp_t	dists[CLIP_BUFFER_SIZE];
	int	sides[CLIP_BUFFER_SIZE];
	int	counts[3];
	fp_t	d;
	int	i, j;
	int	num;
	fp_t	*p1, *p0;

	counts[0] = counts[1] = counts[2] = 0;
	
	for ( i = 0; i < innum; i++ )
	{
		d = Vec3dDotProduct( inbuf[i], pl->norm ) - pl->dist;
		dists[i] = d;

		if ( d > FRUSTUM_ON_EPSILON )
			sides[i] = SIDE_FRONT;
		else if ( d < -ON_EPSILON )
			sides[i] = SIDE_BACK;
		else
			sides[i] = SIDE_ON;

		counts[sides[i]]++;
	}

	if ( !counts[SIDE_FRONT] )
	{
		return -1;
	}

	if ( !counts[SIDE_BACK] )
	{
		return 0;
	}

	dists[i] = dists[0];
	sides[i] = sides[0];

	num = 0;
	for ( i = 0; i < innum; i++ )
	{
		p0 = inbuf[i];

		if ( sides[i] == SIDE_ON )
		{
			Vec5dCopy( outbuf[num++], p0 );
			continue;
		}
		
		if ( sides[i] == SIDE_BACK )
			Vec5dCopy( outbuf[num++], p0 );

		if ( sides[i] == SIDE_ON || sides[i+1] == sides[i] )
			continue;

		j = ((i+1)==innum) ? 0 : (i+1);
		p1 = inbuf[j];

		d = dists[i] / ( dists[i] - dists[i+1] );
		for ( j = 0; j < 5; j++ )
		{
			outbuf[num][j] = p0[j] + d*(p1[j]-p0[j]);
		}	      	
		num++;
	}

	return num;
}


void R_FrustumClipVertex3Buffer_devel( frustum_t *f, void *out, void *in )
{
  __error("meeeeep\n");
	int	pointnum;
	int		i, num;

	vec3d_t		clipbuf[2][CLIP_BUFFER_SIZE];
	vec3d_t		*tmp, *inbuf, *outbuf;

	inbuf = clipbuf[0];
	outbuf = clipbuf[1];

	//
	// setup clipbuf
	//

	int *in_i = (int *)in;
	
	//pointnum = *((int*)(in))++;
	pointnum = *in_i;
	in_i++;
	memcpy( inbuf, in_i, 12*pointnum );

	//
	// clip with all frustum planes
	//

	for ( i = 0; i<5; i++ )
	{
		num = Clip3Buffer( &f->planes[i], outbuf, pointnum, inbuf );
		if ( num == -1 )
		{
			// plane doesn't clip, don't swap clipbuffers
			continue;
		}
		else if ( num == 0 )
		{
			// clipped away
			pointnum = 0;
			break;
		}

		tmp = inbuf;
		inbuf = outbuf;
		outbuf = tmp;
		pointnum = num;
	}
	
#if 0
	printf( "after frustum clip: %d\n", pointnum );
	for ( j = 0; j < pointnum; j++ )
		Vec3dPrint( inbuf[j] );
#endif

	//
	// setup out
	//

	int *out_i = (int *)out;
	//*((int*)(out))++ = pointnum;
	*out_i = pointnum;
	out_i++;
	memcpy( out_i, inbuf, 12*pointnum );
}


void R_FrustumClipVertex5Buffer_devel( frustum_t *f, void *out, void *in )
{
	int	pointnum;
	int		i, num;

	vec5d_t		clipbuf[2][CLIP_BUFFER_SIZE];
	vec5d_t		*tmp, *inbuf, *outbuf;

	inbuf = clipbuf[0];
	outbuf = clipbuf[1];

	//
	// setup clipbuf
	//
	int * in_i = (int*)in;
	pointnum = *in_i;
	in_i++;
	//pointnum = *((int*)(in))++;
	memcpy( inbuf, in_i, (5*4)*pointnum );

	//
	// clip with all frustum planes
	//

	for ( i = 0; i<5; i++ )
	{
		num = Clip5Buffer( &f->planes[i], outbuf, pointnum, inbuf );
		if ( num == -1 )
		{
			// plane doesn't clip, don't swap clipbuffers
			continue;
		}
		else if ( num == 0 )
		{
			// clipped away
			pointnum = 0;
			break;
		}

		tmp = inbuf;
		inbuf = outbuf;
		outbuf = tmp;
		pointnum = num;
	}
	
#if 0
	printf( "after frustum clip: %d\n", pointnum );
	for ( j = 0; j < pointnum; j++ )
		Vec3dPrint( inbuf[j] );
#endif

	//
	// setup out
	//
	int *out_i = (int*)out;
	*out_i = pointnum;
	out_i++;
	//*((int*)(out))++ = pointnum;
	memcpy( out_i, inbuf, (5*4)*pointnum );
}
