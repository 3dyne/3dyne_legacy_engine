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



// g_trace.c

#include "r_interfaces.h"
#include "g_blockmap.h"
#include "g_trace.h"

#ifdef near
#undef near
#endif

#define TRACE_MAX_MAPOBJECTS		( 128 )

static int		 mobjnum;
static int		 testnum;
static mapobject_t	*mobjs[TRACE_MAX_MAPOBJECTS];

static ivec3d_t		 trymin, trymax;

static ivec3d_t		 btrymin, btrymax;

static blockmap_t	*blockmap;

static int		tracecount = 0;

static g_trace_t	trace;

void Vec3dPrintKeep( vec3d_t v, bool_t nl )
{
	printf( "(%.2f %.2f %.2f) ", v[0], v[1], v[2] ); 

	if ( nl )
		printf( "\n" );
}

void IVec3dPrintKeep( ivec3d_t v, bool_t nl )
{
	printf( "(%d %d %d) ", v[0], v[1], v[2] ); 

	if ( nl )
		printf( "\n" );
}

static void FindMapobjectIntersections( mapblock_t *block )
{
	mapln_t		*n;
	int		i;

	for ( n = block->head.nextb; n != &block->tail; n=n->nextb )
	{
		mapobject_t	*obj;

		obj = n->ref;

		if ( obj->lasttrace == tracecount )
			continue;
		obj->lasttrace = tracecount;
		
		testnum++;

//		printf( "obj_bound_box_check: obj %p ", obj );
//		IVec3dPrintKeep( obj->min, 0 );
//		IVec3dPrintKeep( obj->max, 0 );
//		IVec3dPrintKeep( trymin, 0 );
//		IVec3dPrintKeep( trymax, 1 );

		for ( i = 0; i < 3; i++ )
		{

			if ( obj->min[i] > trymax[i] ||
			     obj->max[i] < trymin[i] )
				break;
		}

		if ( i == 3 )
		{
			if ( mobjnum == TRACE_MAX_MAPOBJECTS )
				__error( "reached TRACE_MAX_MAPOBJECTS\n" );
//			printf( "add_obj_for_check:\n" );
			mobjs[mobjnum] = obj;
			mobjnum++;
		}
	}
}

static void FindMapblockIntersections( void )
{
	ivec3d_t	bpos;

	//
	// get all mapblocks, intersecting with btry bb
	//

	for ( bpos[0] = btrymin[0]; bpos[0] <= btrymax[0]; bpos[0]++ )
	{
		for ( bpos[1] = btrymin[1]; bpos[1] <= btrymax[1]; bpos[1]++ )
		{
			for ( bpos[2] = btrymin[2]; bpos[2] <= btrymax[2]; bpos[2]++ )
			{
				mapblock_t	*block;

				block = G_FindMapblock( blockmap, bpos );
				if ( !block )
				{
//					printf( "FindIntersections: no mapblock for this position.\n" );
					continue;
				}
	
				FindMapobjectIntersections( block );
			}
		}
	}
}

#define ZERO_EPSILON	( 0.0001 )

static void BoundBoxPlaneCheck( vec3d_t min, vec3d_t max, vec3d_t norm, fp_t dist, fp_t *dmin, fp_t *dmax )
{
	int		i;
	vec3d_t		bb[2];

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
	
//	Vec3dPrint( min );
//	Vec3dPrint( max );

	*dmin = Vec3dDotProduct( bb[0], norm ) - dist;
	*dmax = Vec3dDotProduct( bb[1], norm ) - dist;

	if ( *dmin > -ZERO_EPSILON && *dmin < ZERO_EPSILON )
		*dmin = 0.0;

	if ( *dmax > -ZERO_EPSILON && *dmax < ZERO_EPSILON )
		*dmax = 0.0;

	
//	printf( "dmin %f, dmax %f\n", *dmin, *dmax );
}

void TestMapobject( mapobject_t *obj, vec3d_t minf, vec3d_t maxf, vec3d_t mint, vec3d_t maxt )
{
	int		i;
	bmplane_t	*pl;
	fp_t		d1, d2;
	fp_t		t1, t2;
	fp_t		near = 0.0;

	fp_t		old_nearest;
	bmplane_t	*old_plane;
	fp_t		old_scale;

	old_nearest = trace.nearest;
	old_plane = trace.plane;
	old_scale = trace.scale;

	for ( i = 0; i < obj->planenum; i++ )
	{
		pl = &obj->planes[i];
	       
		
		BoundBoxPlaneCheck( minf, maxf, pl->norm, pl->dist, &d1, &d2 );

//		printf( "test_obj_plane_from: plane %p of obj %p, from %f %f ", pl, obj, d1, d2 );
//		Vec3dPrint( pl->norm );

		if ( d1 >= 0.0 && d2 >= 0.0 )
		{
			fp_t	scale;


			BoundBoxPlaneCheck( mint, maxt, pl->norm, pl->dist, &t1, &t2 );

//			printf( "test_obj_plane_to: %f %f\n", t1, t2 );

			{
				fp_t	x1, x2;

				x1 = d1<d2?d1:d2;
				x2 = t1<t2?t1:t2;
				
				if ( x1-x2 != 0.0 )
					scale = x1/(x1-x2);
				else
					scale = -1.0;

//				printf( "scale: %f\n", scale );
			}
#if 0
			if ( t1 >= 0.0 && t2 >= 0.0 )				
				continue;
#else
			if ( t1 >= 0.0 && t2 >= 0.0 )				
			{
//				printf( "outside !\n" );
				trace.plane = old_plane;
				trace.nearest = old_nearest;
				trace.scale = old_scale;
				return;
			}	
#endif	
		
			else if ( t1 < 0.0 && t2 < 0.0 )
				if ( t1 > t2 )
					near = t1;
				else
					near = t2;
			
			else if ( t1 >= 0.0 && t2 < 0.0 )
				near = t2;
			else if ( t2 >= 0.0 && t1 < 0.0 )
				near = t1;
			else
				__error( "TestMapobject: odd case\n" );

			if ( near < 0.0 )
			{		
//				printf( "canditate !!!\n" );
#if 0
				if ( trace.nearest < near )
				{
					trace.nearest = near;
					trace.plane = pl;
				}
#else
				if ( scale >= 0.0 && scale <= 1.0 && scale < trace.scale )
				{
//					printf( " >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> take it\n" );
					trace.scale = scale;
					trace.plane = pl;
				} 
#endif
			}

		}		
	}
}

/*
  ==============================
  G_TraceBoundBox

  ==============================
*/

g_trace_t * G_TraceBoundBox( blockmap_t *map, vec3d_t minf, vec3d_t maxf, vec3d_t mint, vec3d_t maxt )
{
	int			i;

	tracecount++;
	blockmap = map;
	mobjnum = 0;
	testnum = 0;
	trace.nearest = -9999.9;
	trace.plane = NULL;
	trace.scale = 9999999.9;	// smallest scale greater or equale 0 search

//	printf( " --- enter trace\n" );

//	printf( " from: " );
//	Vec3dPrintKeep( minf, 0 );
//	Vec3dPrintKeep( maxf, 1 );
//	printf( " to: " );
//	Vec3dPrintKeep( mint, 0 );
//	Vec3dPrintKeep( maxt, 1 );	

	IVec3dRint( trymin, mint );
	IVec3dRint( trymax, maxt );

	G_IVec3dToBlockmapUnits( btrymin, trymin );
	G_IVec3dToBlockmapUnits( btrymax, trymax );

	FindMapblockIntersections();
	
	for ( i = 0; i < mobjnum; i++ )
	{
		TestMapobject( mobjs[i], minf, maxf, mint, maxt );
	}

//	printf( " --- leave trace\n" );

	return &trace;
}


/*
  ==================================================
  trace line

  ==================================================
*/

static void TraceLineFindObjectsInBB( mapblock_t *block, ivec3d_t min, ivec3d_t max )
{
	int		i;
	mapln_t		*n;

	for ( n = block->head.nextb; n != &block->tail; n=n->nextb )
	{
		mapobject_t	*obj;

		obj = n->ref;

		if ( obj->lasttrace == tracecount )
			continue;
		obj->lasttrace = tracecount;
		
		for ( i = 0; i < 3; i++ )
		{
			if ( min[i] > obj->max[i] ||
			     max[i] < obj->min[i] )
			{
				break;
			}
		}

		if ( i == 3 )
		{
			if ( mobjnum == TRACE_MAX_MAPOBJECTS )
				__error( "reached TRACE_MAX_MAPOBJECTS\n" );
			mobjs[mobjnum] = obj;
			mobjnum++;
		}
	}
}

static void TraceLineFindNearest( g_trace_t *tr, vec3d_t from, vec3d_t to )
{
	int		i, j;

	mapobject_t	*obj;
	bmplane_t	*pl;

	fp_t		best_d1;
	bmplane_t	*best_pl;

	best_d1 = 999999.9;	// minimum search
	best_pl = NULL;

	for ( j = 0; j < mobjnum; j++ )
	{
		fp_t		best_obj_d1;
		bmplane_t	*best_obj_pl;
		
		vec3d_t		p1, p2;

		obj = mobjs[j];

		// search best plane of object

		best_obj_pl = NULL;
		
		Vec3dCopy( p1, from );
		Vec3dCopy( p2, to );
		
		for ( i = 0; i < obj->planenum; i++ )
		{
			fp_t	d1, d2;
			fp_t		scale;

			pl = &obj->planes[i];

			d1 = Vec3dDotProduct( p1, pl->norm ) - pl->dist;
			d2 = Vec3dDotProduct( p2, pl->norm ) - pl->dist;
		       
			if ( d1 > 0 && d2 > 0 )
			{
				// object not hit
				break;
			}

			if ( d1 > 0 && d2 < 0 )
			{
				// ray enter brush
				scale = d1/(d1-d2);
				
				p1[0] = p1[0]+scale*(p2[0]-p1[0]);
				p1[1] = p1[1]+scale*(p2[1]-p1[1]);
				p1[2] = p1[2]+scale*(p2[2]-p1[2]);

				best_obj_pl = pl;
			}

			if ( d1 < 0 && d2 > 0 )
			{
				// ray leave brush
				scale = d1/(d1-d2);

				p2[0] = p1[0]+scale*(p2[0]-p1[0]);
				p2[1] = p1[1]+scale*(p2[1]-p1[1]);
				p2[2] = p1[2]+scale*(p2[2]-p1[2]);				
			}
		}
		
		// object not hit
		if ( i != obj->planenum || !best_obj_pl )
		{
			continue;
		}
		
		//
		// should we ignore the surface ?
		//
		if ( ! (best_obj_pl->surf_flags & tr->surf_flags ) )
		{
			continue;
		}
		
		best_obj_d1 = Vec3dDotProduct( from, best_obj_pl->norm ) - best_obj_pl->dist;

		// is best plane of object although the best of all

		if ( best_obj_d1 < best_d1 )
		{
			best_d1 = best_obj_d1;
			best_pl = best_obj_pl;
		}
	}

	tr->nearest = best_d1;
	tr->plane = best_pl;
}

void G_TraceLine( blockmap_t *map, g_trace_t *tr, vec3d_t from, vec3d_t to )
{
	vec3d_t		min, max;
	ivec3d_t	mini, maxi;
	ivec3d_t	minb, maxb;

	ivec3d_t		posb;

	__chkptr( tr );
	__chkptr( map );
	
	tracecount++;
	blockmap = map;
	mobjnum = 0;
	testnum = 0;

	tr->nearest = -9999.9;
	tr->plane = NULL;	

//	Vec3dPrint( from );
//	Vec3dPrint( to );

	// get bound box of line
	Vec3dInitBB( min, max, 999999.9 );
	Vec3dAddToBB( min, max, from );
	Vec3dAddToBB( min, max, to );

	// get int bound box
	IVec3dRint( mini, min );
	IVec3dRint( maxi, max);

	// get blockmap units from bound box
	G_IVec3dToBlockmapUnits( minb, mini );	
	G_IVec3dToBlockmapUnits( maxb, maxi );	

//	fprintf( stderr, "b_units: %d %d\n", minb, maxb );
       

	// search blocks in bound box
	for ( posb[0] = minb[0]; posb[0] <= maxb[0]; posb[0]++ )
	{
		for ( posb[1] = minb[1]; posb[1] <= maxb[1]; posb[1]++ )
		{
			for ( posb[2] = minb[2]; posb[2] <= maxb[2]; posb[2]++ )
			{
				mapblock_t	*block;

//				fprintf( stderr, "scan b_units: %d %d %d\n", posb[0], posb[1], posb[2] );
				block = G_FindMapblock( map, posb );
				if ( block )
				{
					// search objects bound box
					TraceLineFindObjectsInBB( block, mini, maxi );	
				}
			}
		}
	}

	TraceLineFindNearest( tr, from, to );
}
