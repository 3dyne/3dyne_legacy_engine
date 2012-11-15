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



// r_data.c

#include "r_interfaces.h"
#include "interfaces.h"



/*
  ==================================================
  mapnode rep

  ==================================================
*/

// typedef of face_t moved to r_defs.h


#define MAX_MAPNODES	( 8192 )
#define MAX_LEAFREFS	( 32000 )

int		r_leafrefnum = 0;
leafref_t	r_leafrefs[MAX_LEAFREFS];

int		r_mapnodenum = 0;
mapnode_t	r_mapnodes[MAX_MAPNODES];

int		r_visleafnum = 0;
int		r_leafbitpos[MAX_MAPNODES];

void CompileMapnodeClass( hmanager_t *mapnodehm, hmanager_t *planehm )
{
	int		i;
	hmanager_type_iterator_t	iters[3];
	hpair_search_iterator_t		iter;
	hobj_t		*mapnode;
	hobj_t		*leaf, *plane, *child;
	hpair_t		*pair;
	int		index;
	int		bitpos;

	memset( r_leafrefs, 0, sizeof( r_leafrefs ) );
	memset( r_mapnodes, 0, sizeof( r_mapnodes ) );
	memset( r_leafbitpos, 0, sizeof( r_leafbitpos ) );

	r_leafrefnum = 0;
	r_mapnodenum = 0;
	r_visleafnum = 0;

	HManagerIndexClassesOfType( mapnodehm, &r_mapnodenum, "mapnodes" );
	HManagerIndexClassesOfType( mapnodehm, &r_mapnodenum, "mapnode_front" );
	HManagerIndexClassesOfType( mapnodehm, &r_mapnodenum, "mapnode_back" );

	printf( " %d mapnodes\n", r_mapnodenum );

	HManagerInitTypeSearchIterator( &iters[0], mapnodehm, "mapnodes" );
	HManagerInitTypeSearchIterator( &iters[1], mapnodehm, "mapnode_front" );
	HManagerInitTypeSearchIterator( &iters[2], mapnodehm, "mapnode_back" );

	for ( i = 0; i < 3; i++ )
	{
		for ( ; ( mapnode = HManagerGetNextClass( &iters[i] ) ); )
		{
			pair = FindHPair( mapnode, "index" );
			if ( !pair )
				__error( "missing 'index' in mapnode '%s'.\n", mapnode->name );
			HPairCastToInt_safe( &index, pair );

			r_mapnodes[index].self = mapnode;

			pair = FindHPair( mapnode, "plane" );
			if ( pair )
			{
				// it's a node
				plane = HManagerSearchClassName( planehm, pair->value );
				if ( !plane )
					__error( "mapnode '%s' can't find plane '%s'.\n", mapnode->name, pair->value );
				r_mapnodes[index].pl = (cplane_t *)GetClassExtra( plane );

				child = FindClassType( mapnode, "mapnode_front" );
				if ( !child )
					__error( "missing 'mapnode_front' in mapnode '%s'.\n", mapnode->name );
				pair = FindHPair( child, "index" );
				if ( !pair )
					__error( "mapnode '%s' missing 'index' of child '%s'.\n", mapnode->name, child->name );
				HPairCastToInt( &r_mapnodes[index].child[0], pair );


				child = FindClassType( mapnode, "mapnode_back" );
				if ( !child )
					__error( "missing 'mapnode_back' in mapnode '%s'.\n", mapnode->name );
				pair = FindHPair( child, "index" );
				if ( !pair )
					__error( "mapnode '%s' missing 'index' of child '%s'.\n", mapnode->name, child->name );
				HPairCastToInt( &r_mapnodes[index].child[1], pair );				
			}
			else
			{
				// it's a leaf
				r_mapnodes[index].visleaf = -1;	// for debug draw
				r_mapnodes[index].pl = NULL;
				r_mapnodes[index].child[0] = -1;
				r_mapnodes[index].child[1] = -1;

				// get pvs
				pair = FindHPair( mapnode, "bitpos" );
				if ( pair )
				{
					HPairCastToInt( &bitpos, pair );
					r_leafbitpos[bitpos] = index;
					
					r_visleafnum++;
					r_mapnodes[index].visinfo = true;

				// get center, min, max
					pair = FindHPair( mapnode, "center" );
					if ( !pair )
						__error( "missing 'center' in mapnode '%s'.\n", mapnode->name );
					HPairCastToVec3d_safe( r_mapnodes[index].center, pair );
					
					pair = FindHPair( mapnode, "min" );
					if ( !pair )
						__error( "missing 'min' in mapnode '%s'.\n", mapnode->name );
					HPairCastToVec3d_safe( r_mapnodes[index].min, pair );
					
					pair = FindHPair( mapnode, "max" );
					if ( !pair )
						__error( "missing 'max' in mapnode '%s'.\n", mapnode->name );
					HPairCastToVec3d_safe( r_mapnodes[index].max, pair );
				}
				else
				{
					r_mapnodes[index].visinfo = false;
				}

				// get touchleafs
				r_mapnodes[index].leafrefnum = 0;
				r_mapnodes[index].startleafref = r_leafrefnum;
				InitHPairSearchIterator( &iter, mapnode, "touchleaf" );
				for ( ; ( pair = SearchGetNextHPair( &iter ) ); )
				{
					leaf = HManagerSearchClassName( mapnodehm, pair->value );
					if ( !leaf )
						__error( "mapnode '%s' can't find brush '%s'.\n", mapnode->name, pair->value );
					
					pair = FindHPair( leaf, "index" );
					if ( !pair )
						__error( "missing 'index' in mapnode '%s'.\n", leaf->name );
					HPairCastToInt_safe( &r_leafrefs[r_leafrefnum++].mapnode, pair );
					r_mapnodes[index].leafrefnum++;
				}


			}
			
		}
	}
}



/*
  =============================================================================
  visleaf 

  =============================================================================
*/


#define MAX_POINTS	( 32000 )
#define MAX_PORTALS	( 16000 )
#define MAX_VISLEAFS	( 1024 )

int	r_pointnum = 0;
int	r_portalnum = 0;


vec3d_t	r_points[MAX_POINTS];
portal_t r_portals[MAX_PORTALS];
visleaf_t r_visleafs[MAX_VISLEAFS];


void CompileVisleafClass( hmanager_t *visleafhm, hmanager_t *planehm )
{
	hobj_search_iterator_t	iter;
	hobj_t		*visleaf;
	hobj_t		*plane;
	hpair_t		*pair;
	int		i, num;

	memset( r_points, 0, sizeof( r_points ) );
	memset( r_portals, 0, sizeof( r_portals ) );
	memset( r_visleafs, 0, sizeof( r_visleafs ) );

	r_pointnum = 0;
	r_portalnum = 0;
	r_visleafnum = 0;

	InitClassSearchIterator( &iter, HManagerGetRootClass( visleafhm ), "visleaf" );
	for ( ; ( visleaf = SearchGetNextClass( &iter ) ) ; )
	{
		hobj_search_iterator_t	portaliter;		
		hobj_t		*portal;
		char		tt[256];

		pair = FindHPair( visleaf, "bitpos" );
		if ( !pair )
			__error( "missing 'bitpos' in visleaf '%s'.\n", visleaf->name );
		HPairCastToInt_safe( &r_visleafs[r_visleafnum].bitpos, pair );
		
		// set own index
		sprintf( tt, "%d", r_visleafnum );
		pair = NewHPair2( "int", "index", tt );
		InsertHPair( visleaf, pair );

		r_visleafs[r_visleafnum].self = visleaf;
		r_visleafs[r_visleafnum].startportal = r_portalnum;
		r_visleafs[r_visleafnum].portalnum = 0;
		r_visleafs[r_visleafnum].count = 0;

		InitClassSearchIterator( &portaliter, visleaf, "portal" );
		for ( ; ( portal = SearchGetNextClass( &portaliter ) ) ; )
		{

			// portal plane
			pair = FindHPair( portal, "plane" );
			if ( !pair )
				__error( "missing 'plane' in portal '%s'.\n", portal->name );
			plane = HManagerSearchClassName( planehm, pair->value );
			if ( !plane )
				__error( "portal '%s' can't find plane '%s'.\n", portal->name, pair->value );
			r_portals[r_portalnum].pl = (cplane_t *)GetClassExtra( plane );
			
			// through_num
			pair = FindHPair( portal, "through_num" );
//			if ( pair )
//				__error( "missing 'through_num' in portal '%s'.\n", portal->name );
//			HPairCastToInt_safe( &r_portals[r_portalnum].through_num, pair );

			// complex_num
			pair = FindHPair( portal, "complex_num" );
//			if ( pair ) 
//				__error( "missing 'complex_num' in portal '%s'.\n", portal->name );
//			HPairCastToInt_safe( &r_portals[r_portalnum].complex_num, pair );
			

			// see_through1 ( trivial )
			pair = FindHPair( portal, "through_see1" );
			if ( pair )
			{
				int		see_buffer_size;

				see_buffer_size = strlen(pair->value)/2 + 2;
				r_portals[r_portalnum].see_through = (unsigned char*) NEWBYTES( see_buffer_size );
				HPairCastToBstring_safe( r_portals[r_portalnum].see_through, &see_buffer_size, pair );				
			}
			
			
			// see_through ( complex )
			pair = FindHPair( portal, "through_see" );
			if ( pair )
			{
				int		see_buffer_size;

				see_buffer_size = strlen(pair->value)/2 + 2;
				r_portals[r_portalnum].see_through = (unsigned char*) NEWBYTES( see_buffer_size );
				HPairCastToBstring_safe( r_portals[r_portalnum].see_through, &see_buffer_size, pair );				
			}

			r_portals[r_portalnum].startpoint = r_pointnum;
			r_portals[r_portalnum].pointnum = 0;
			
			pair = FindHPair( portal, "pointnum" );
			if ( !pair ) 
				__error( "missing 'pointnum' in portal '%s'.\n", portal->name );
			HPairCastToInt_safe( &num, pair );

			for ( i = 0; i < num; i++ )
			{
				sprintf( tt, "%d", i );

				pair = FindHPair( portal, tt );
				if ( !pair )
					__error( "missing point '%s' in portal '%s'.\n", tt, portal->name );
				HPairCastToVec3d_safe( r_points[r_pointnum++], pair );
				r_portals[r_portalnum].pointnum++;
			}
			r_visleafs[r_visleafnum].portalnum++;
			r_portalnum++;
		}
		r_visleafnum++;
	}

	printf( " %d visleafs\n", r_visleafnum );
	printf( " %d visportals\n", r_portalnum );
	printf( " %d visportalpoints\n", r_pointnum );


	//
	// set visleafs in mapnodes
	//

	for ( i = 0; i < r_mapnodenum; i++ )
	{
		pair = FindHPair( r_mapnodes[i].self, "visleaf" );
		if ( !pair )
			continue;
		visleaf = HManagerSearchClassName( visleafhm, pair->value );
		if ( !visleaf )
			__error( "mapnode '%s' can't find visleaf '%s'.\n", r_mapnodes[i].self->name, pair->value );

		pair = FindHPair( visleaf, "index" );
		if ( !pair )
			__error( "missing 'index' in visleaf '%s'.\n", visleaf->name );
		HPairCastToInt_safe( &r_mapnodes[i].visleaf, pair );				
	}
}


/*
  ==================================================
  plane class

  ==================================================
*/

#define MAX_PLANES	( 8192 )

int			r_planenum;
cplane_t		*r_planes;

#if 0
void R_CreatePlanesFromPool( cp_if_t *cpif )
{
	cp_dword_t		id_root;
	cp_dword_t		x_plane;
	cp_dword_t		x_flipplane;
	cp_dword_t		x_type;
	cp_dword_t		x_dist;
	cp_dword_t		x_norm;
	cp_dword_t		x_index;

	cp_stack_info_t		stack;
	cp_pool_t		*pool;

	cp_key_iter_t		iter;

	int		num;
	int		i;

	SHV_Printf( "R_CreatePlanesFromPool:" );

	pool = cpif->pool;

	x_plane = CP_WhatIs( cpif, "plane" );
	x_flipplane = CP_WhatIs( cpif, "flipplane" );
	x_type = CP_WhatIs( cpif, "type" );
	x_dist = CP_WhatIs( cpif, "dist" );
	x_norm = CP_WhatIs( cpif, "norm" );	
	x_index = CP_WhatIsCreate( cpif, "index" );

	id_root = CP_GetEntry( cpif, "plane" );

	CP_GetStackInfo( &stack );

	// push plane-root-class
	CP_PushClass( pool, id_root );

	// iterator for local 'plane' clsref keys
	CP_InitKeyIter( &iter, x_plane );
	for ( num = 0; ( CP_PushNextKey( &iter ) ) ; num++ )
	{
		cp_dword_t	id_plane;

		int		pl_type;
		float		pl_dist;
		vec3d_t		pl_norm;

		GC_GiveBackTime();

 		// pushed a plane-lclsref-key

		// push plane-class
 		CP_CastLclsref( &id_plane, CP_STACKTOP );
		CP_PushClass( pool, id_plane );

		// insert index-key into class
		CP_InsertKeyInClass( CP_STACKTOP, x_index, CP_NewScalarInt( &num ) );

		// get type-key
		CP_PushKey( x_type );
		CP_CastInt( &pl_type, CP_STACKTOP );
		CP_PopKey();

		// get dist-key
		CP_PushKey( x_dist );
		CP_CastFloat( &pl_dist, CP_STACKTOP );
		CP_PopKey();

		// get norm-key
		CP_PushKey( x_norm );
		CP_CastVec3d( pl_norm, CP_STACKTOP );
		CP_PopKey();

		// pop plane-class
		CP_PopClass();

		// pop plane-lclsref-key
		CP_PopKey();

		r_planes[num].id_self = id_plane;
		r_planes[num].type = pl_type;
		r_planes[num].dist = pl_dist;
		Vec3dCopy( r_planes[num].norm, pl_norm );

	}

	r_planenum = num;

	// plane-root-class
	CP_PopClass();

	// set flipplane
	for ( i = 0; i < num; i++ )
	{
		int	flipindex;
		cp_dword_t	pl_id_flipplane;

		CP_PushClass( pool, r_planes[i].id_self );
		CP_PushKey( x_flipplane );
		CP_CastClsref( &pl_id_flipplane, CP_STACKTOP );
		CP_PopKey();

		CP_PushClass( pool, pl_id_flipplane );
		CP_PushKey( x_index );
		CP_CastInt( &flipindex, CP_STACKTOP );
		r_planes[i].flipplane = &r_planes[flipindex];
		CP_PopKey();
		CP_PopClass();		

		CP_PopClass();
	}

	CP_CheckStackInfo( &stack );

	__named_message( " %d planes\n", num );
	SHV_Printf( " %d planes\n", num );
	
	GC_GiveBackTime();
}
#endif
