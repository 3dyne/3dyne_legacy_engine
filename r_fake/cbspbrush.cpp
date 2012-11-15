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



// cbspbrush.c

#include "cbspbrush.h"

cplane_t * NewCPlane( void )
{
	cplane_t	*pl;

	pl = (cplane_t *) malloc( sizeof( cplane_t ) );
	memset( pl, 0, sizeof( cplane_t ) );

	return pl;
}

ctexdef_t * NewCTexdef( void )
{
	ctexdef_t	*td;

	td = (ctexdef_t *) malloc( sizeof( ctexdef_t ) );
	memset( td, 0, sizeof( ctexdef_t ) );

	return td;
}


/*
  ====================
  NewBrush

  ====================
*/
cbspbrush_t * NewBrush( int surfnum )
{
	int		size;
	cbspbrush_t	*b;

	size = (int)&(((cbspbrush_t *)0)->surfaces[surfnum]);
	b = (cbspbrush_t *) malloc( size );
	memset( b, 0, size );

	return b;
}

/*
  ====================
  FreeBrush

  ====================
*/
void FreeBrush( cbspbrush_t *b )
{
	int		i;

	if ( !b )
		return;

	for ( i = 0; i < b->surfacenum; i++ )
	{
		if ( b->surfaces[i].p )
			FreePolygon( b->surfaces[i].p );
	}

	free( b );
}

/*
  ====================
  FreeBrushList

  ====================
*/
void FreeBrushList( cbspbrush_t *list )
{
	cbspbrush_t	*next;

	if ( !list )
		return;

	for ( ; list ;list = next )
	{
		next = list->next;
		FreeBrush( list );
	}
}

/*
  ====================
  CopyBrush

  ====================
*/
cbspbrush_t * CopyBrush( cbspbrush_t *in )
{
	int		i;
	int	size;
	cbspbrush_t	*b;

	size = (int)&(((cbspbrush_t *)0)->surfaces[in->surfacenum]);
	b = NewBrush( in->surfacenum );
	memcpy( b, in, size );

	for ( i = 0; i < in->surfacenum; i++ )
	{
		if ( in->surfaces[i].p )
			b->surfaces[i].p = CopyPolygon( in->surfaces[i].p );
	}

	return b;
}

  
/*
  ====================
  BrushListLength

  ====================
*/
int BrushListLength( cbspbrush_t *list )
{
	int		i;
	
	for ( i = 0; list; list=list->next, i++ )
	{ }
	
	return i;
}
  

/*
  ====================
  CreateBrushPolygons

  ====================
*/
void CreateBrushPolygons( cbspbrush_t *b )
{
	int		i, j;
	cplane_t		*pl;

	for( i = 0; i < b->surfacenum; i++ )
	{
		pl = b->surfaces[i].pl;
		b->surfaces[i].p = BasePolygonForPlane( pl->norm, pl->dist );

		for ( j = 0; j < b->surfacenum; j++ )
		{
			if ( i == j )
				continue;

			if ( b->surfaces[i].p )
			{
				pl = b->surfaces[j].pl;
				ClipPolygonInPlace( &b->surfaces[i].p, pl->norm, pl->dist );
			}
		}
		
		if ( !b->surfaces[i].p )
		{
			printf( "Warning: * CreateBrushPolygons: clipped away polygon in brush '%s'. *\n", b->original->name );
		}
	}

	CalcBrushBounds( b );
}


/*
  ====================
  CalcBrushBounds

  ====================
*/
void CalcBrushBounds( cbspbrush_t *b )
{
	int		i;
	int		j;

	Vec3dInitBB( b->min, b->max, 999999.9 );

	for ( i = 0; i < b->surfacenum; i++ )
	{
		if ( !b->surfaces[i].p )
			continue;

		for ( j = 0; j < b->surfaces[i].p->pointnum; j++ )
			Vec3dAddToBB( b->min, b->max, b->surfaces[i].p->p[j] );
	}      

	for ( i = 0; i < 3; i++ )
		if ( b->min[i] < -BASE_POLYGON_SIZE || b->max[i] > BASE_POLYGON_SIZE )
		{
			printf( " * CalcBrushBounds: critical big polygon in brush: %s\n", b->original->name );
			Vec3dPrint( b->min );
			Vec3dPrint( b->max );
			break;
		}
	
	for ( i = 0; i < 3; i++ )
		if ( b->min[i] > 99999.9 || b->max[i] < -99999.9 ) 
		{
			printf( " * CalcBrushBounds: odd bb for brush id: %s *\n", b->original->name );
			break;
		}
}


/*
  ====================
  CalcBrushListBounds

  ====================
*/
void CalcBrushListBounds( cbspbrush_t *list, vec3d_t min, vec3d_t max )
{
	Vec3dInitBB( min, max, 999999.9 );
	
	for ( ; list ; list=list->next )
	{
//		Vec3dPrint( head->min );
//		Vec3dPrint( head->max );
		Vec3dAddToBB( min, max, list->min );
		Vec3dAddToBB( min, max, list->max );
	}
}

/*
  ====================
  CalcBrushVolume

  ====================
*/
fp_t CalcBrushVolume( cbspbrush_t *in )
{
	int		i;
	fp_t		area, volume, d;
	polygon_t	*p;
	vec3d_t		corner;
	cplane_t		*pl;

	if ( !in )
		return 0.0;

	p = NULL;
	for ( i = 0; i < in->surfacenum; i++ )
	{
		p = in->surfaces[i].p;
		if ( p )
			break;
	}

	if ( !p )
		return 0.0;

	Vec3dCopy( corner, p->p[0] );

	volume = 0;
	for ( ; i < in->surfacenum; i++ )
	{
		p = in->surfaces[i].p;
		if ( !p )
			continue;
		pl = in->surfaces[i].pl;
		d = -(Vec3dDotProduct( corner, pl->norm ) - pl->dist );
		area = PolygonArea( p );
		volume += d*area;
	}
	volume /= 3;
	return volume;
}

/*
  =============================================================================
  a sane brush split imlp

  =============================================================================
*/



int CSG_IsExactOnPlane( cbspbrush_t *in, cplane_t *plane )
{
	int		i;
	cplane_t	*pl;
	
	for ( i = 0; i < in->surfacenum; i++ )
	{
		pl = in->surfaces[i].pl;
		if ( pl == plane )
		{
			// exact on plane, brush is back side
			return BRUSH_BACK_ON;
		}
		else if ( pl == plane->flipplane )
		{
			// exact on plane, brush is front side
			return BRUSH_FRONT_ON;
		}
	}
	return BRUSH_NONE;
}

void CSG_SplitBrush_new( cbspbrush_t *in, cplane_t *plane, cbspbrush_t **front, cbspbrush_t **back )
{
	int		i;
	int		exact;
	polygon_t	*splitpoly;
	cbspbrush_t	*b, *f;

	*front = *back = NULL;

	//
	// split plane part of brush ?
	//

	exact = CSG_IsExactOnPlane( in, plane );

	if ( exact == BRUSH_BACK_ON )
	{
		*back = CopyBrush( in );
		return;
	}
	if ( exact == BRUSH_FRONT_ON )
	{
		*front = CopyBrush( in );
		return;
	}

	//
	// real check
	//

	splitpoly = BasePolygonForPlane( plane->norm, plane->dist );

	for ( i = 0; i < in->surfacenum; i++ )
	{
		ClipPolygonInPlace( &splitpoly, in->surfaces[i].pl->norm, in->surfaces[i].pl->dist );
		if ( !splitpoly )
			break;
	}

	if ( !splitpoly )
	{
		// no splitpoly => brush is not split by plane
		// determine on which side of plane is the complete brush

		int	check = -1;

		for ( i = 0; i < in->surfacenum; i++ )
		{
			if ( !in->surfaces[i].p )
				continue;
			check = CheckPolygonWithPlane( in->surfaces[i].p, plane->norm, plane->dist );
			if ( check == POLY_BACK )
				break;
			else if ( check == POLY_FRONT )
				break;
			else if ( check == POLY_ON )
				continue;
			printf( "?" );
		}
		
		if ( check == POLY_BACK )
		{
			*back = CopyBrush( in );
			return;
		}
		else if ( check == POLY_FRONT )
		{
			*front = CopyBrush( in );
			return;
		}
		else
		{
			printf( "can't get planeside of brush.\n" );
			*front = NULL;
			*back = NULL;
			return;
		}
	}

	//
	// split input brush
	//
	
	f = NewBrush( in->surfacenum + 2 );
	b = NewBrush( in->surfacenum + 2 );
	f->surfacenum = b->surfacenum = 0;

	f->original = in->original;
	b->original = in->original;
	f->contents = in->contents;
	b->contents = in->contents;

	for( i = 0; i < in->surfacenum; i++ )
	{
		polygon_t	*fpoly, *bpoly;

		SplitPolygon( in->surfaces[i].p, plane->norm, plane->dist, &fpoly, &bpoly );

		if ( fpoly )
		{
			// add polygon to front brush and copy the rest
			if ( f->surfacenum == in->surfacenum + 2 )
				Error( "reached max surfs\n" );
			memcpy( &f->surfaces[f->surfacenum], &in->surfaces[i], sizeof( csurface_t ) );
			f->surfaces[f->surfacenum].p = fpoly;
			f->surfacenum++;
		}
		if ( bpoly )
		{
			// add polygon to back brush and copy the rest
			if ( b->surfacenum == in->surfacenum + 2 )
				Error( "reached max surfs\n" );
			memcpy( &b->surfaces[b->surfacenum], &in->surfaces[i], sizeof( csurface_t ) );
			b->surfaces[b->surfacenum].p = bpoly;
			b->surfacenum++;
		}
	}

	//
	// add split plane to front and back brush
	//

	// the backside brush gets the plane
	if ( f->surfacenum == in->surfacenum + 2 )
		Error( "reached max surfs\n" );
	b->surfaces[b->surfacenum].pl = plane;
	b->surfaces[b->surfacenum].td = NULL;
	b->surfaces[b->surfacenum].state = SURFACE_STATE_BYSPLIT;
	b->surfaces[b->surfacenum].contents = 0;
	b->surfaces[b->surfacenum].p = splitpoly;
	b->surfacenum++;

	// the frontside brush gets the flipplane
	if ( b->surfacenum == in->surfacenum + 2 )
		Error( "reached max surfs\n" );
	f->surfaces[f->surfacenum].pl = plane->flipplane;
	f->surfaces[f->surfacenum].td = NULL;
	f->surfaces[f->surfacenum].state = SURFACE_STATE_BYSPLIT;
	f->surfaces[f->surfacenum].contents = 0;
	f->surfaces[f->surfacenum].p = PolygonFlip( splitpoly );
	f->surfacenum++;
	
	if ( b->surfacenum < 4 )
	{
		printf( "no back %d\n", b->surfacenum );
		FreeBrush( b );
		b = NULL;
	}
	if ( f->surfacenum < 4 )
	{
		printf( "no front %d\n", f->surfacenum );
		FreeBrush( f );
		f = NULL;
	}

	if ( b )
		CalcBrushBounds( b );
	if ( f )
		CalcBrushBounds( f );

	*back = b;
	*front = f;
}


/*
  =============================================================================
  brush class stuff

  =============================================================================
*/

cbspbrush_t * CompileCBspbrushClass( hobj_t *brush, hmanager_t *planehm, hmanager_t *texdefhm )
{
	int		i;
	cbspbrush_t	*bb;
	hobj_search_iterator_t	surfiter;
	hobj_t			*surface;
	hobj_t			*plane;
	hobj_t			*texdef;
	hpair_t			*pair;
	

	InitClassSearchIterator( &surfiter, brush, "surface" );
	for ( i = 0; ( surface = SearchGetNextClass( &surfiter ) ); i++ )
	{ }

	bb = NewBrush( i );
	bb->surfacenum = i;

	pair = FindHPair( brush, "content" );
	if ( !pair )
		Error( "missing 'contents' in brush '%s'.\n", brush->name );
	HPairCastToInt_safe( &bb->contents, pair );

	InitClassSearchIterator( &surfiter, brush, "surface" );
	for ( i = 0; ( surface = SearchGetNextClass( &surfiter ) ); i++ )
	{
		// contents
		pair = FindHPair( surface, "content" );
		if ( !pair )
			Error( "missing 'content' in surface '%s'.\n", surface->name );
		HPairCastToInt( &bb->surfaces[i].contents, pair );

		// clsref plane		
		pair = FindHPair( surface, "plane" );
		if ( !pair )
			Error( "missing 'plane' in surface '%s'.\n", surface->name );
		plane = HManagerSearchClassName( planehm, pair->value );
		bb->surfaces[i].pl = GetClassExtra( plane );

		// clsref texdef
		pair = FindHPair( surface, "texdef" );
		if ( !texdefhm || !pair )
		{
			bb->surfaces[i].td = NULL;
		}
		else
		{
			texdef = HManagerSearchClassName( texdefhm, pair->value );
			bb->surfaces[i].td = GetClassExtra( texdef );
		}

		bb->original = brush;
		SetClassExtra( brush, bb );		
	}
	
	return bb;
}

hobj_t * BuildCBspbrushClass( cbspbrush_t *b, hmanager_t *planehm, hmanager_t *texdefhm )
{
	int		i;
	hpair_t		*pair;
	hobj_t		*brush;
	hobj_t		*surface;
	char		tt[256];

	sprintf( tt, "#%u", HManagerGetFreeID() );
	brush = NewClass( "bspbrush", tt );
	
	sprintf( tt, "%u", b->contents );
	pair = NewHPair2( "int", "content", tt );
	InsertHPair( brush, pair );

	pair = FindHPair( b->original, "original" );
	if ( pair )
	{
		pair = NewHPair2( "ref", "original", pair->value );
		InsertHPair( brush, pair );
	}

	for ( i = 0; i < b->surfacenum; i++ )
	{
		sprintf( tt, "#%u", HManagerGetFreeID() );
		surface = NewClass( "surface", tt );

		// clsref plane
		pair = NewHPair2( "ref", "plane", b->surfaces[i].pl->self->name );
		InsertHPair( surface, pair );

		// clsref texdef
		if ( b->surfaces[i].td )
		{
			pair = NewHPair2( "ref", "texdef", b->surfaces[i].td->self->name );
			InsertHPair( surface, pair );
		}

		// contents
		sprintf( tt, "%u", b->surfaces[i].contents );
		pair = NewHPair2( "int", "content", tt );
		InsertHPair( surface, pair );

		InsertClass( brush, surface );
	}
	return brush;
}
