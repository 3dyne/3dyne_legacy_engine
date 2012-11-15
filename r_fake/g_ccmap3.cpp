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



// g_ccmap3.c

#include "r_interfaces.h"
#include "g_ccmap3.h"

/*
  ==============================
  G_NewCCMap3

  ==============================
*/
ccmap3_t * G_NewCCMap3( int hashsize, int clustersize, int cellbytes )
{
	ccmap3_t	*map;
	size_t		size;
	int		i;

	for ( i = 0; i < 32; i++ )
	{
		if ( (1<<i) == hashsize )
			break;
	}

	if ( i == 32 )
		__error( "G_NewCCMap3: invalid hash table size\n ");

	size = (size_t)&(((ccmap3_t *)0)->hash[hashsize]);
	map = (ccmap3_t *) NEWBYTES( size );
	
	map->hashsize = hashsize;
	map->clustersize = clustersize;
	map->cellbytes = cellbytes;

	map->clustercellnum = clustersize * clustersize * clustersize;
	
	map->hashshift = i;
	map->keyshift = i / 3;
	map->keymask = hashsize-1;
	
	return map;
}



void G_FreeCCMap3( ccmap3_t *map )
{
	FREE( map );
}

void G_DumpCCMap3( ccmap3_t *map )
{
	int		i;

	printf( "G_DumpCCMap3: \n" );
	printf( " %d hashtable size, %d hashshift, %d keyshift, %d keymask\n",
		map->hashsize, map->hashshift, map->keyshift, map->keymask );
	printf( " %d clusters, %d clustersize, %d cells pre cluster\n",
		map->clusternum, map->clustersize, map->clustercellnum );

	for ( i = 0; i < map->hashsize; i++ )
	{
		int		j;
		ccluster_t	*c;
		for ( j = 0, c = map->hash[i]; c ; c=c->next, j++ )
		{ }
		printf( " %d", j );
	}
	printf( "\n" );
}

/*
  ==============================
  G_CCMap3CalcHashkey

  ==============================
*/
int G_CCMap3CalcHashkey( ccmap3_t *map, ivec3d_t pos )
{
	int		key;

	key = (pos[2] << (map->keyshift))+pos[1];
	key <<= (map->keyshift);
	key += pos[0];
	key &= map->keymask;

	return key;
}

/*
  ==============================
  G_CCMap3InsertCCluster 

  ==============================
*/
void G_CCMap3InsertCCluster( ccmap3_t *map, ccluster_t *cc )
{
	int		key;

	key = G_CCMap3CalcHashkey( map, cc->pos );
	
	cc->next = map->hash[key];
	map->hash[key] = cc;	

	map->clusternum++;
}

/*
  ==============================
  G_CCMap3FindCCluster

  ==============================
*/
ccluster_t * G_CCMap3FindCCluster( ccmap3_t *map, ivec3d_t pos )
{
	int		key;
	ccluster_t	*cc;

	key = G_CCMap3CalcHashkey( map, pos );
	
	for ( cc = map->hash[key]; cc ; cc=cc->next )
	{
		if ( cc->pos[0] == pos[0] &&
		     cc->pos[1] == pos[1] &&
		     cc->pos[2] == pos[2] )
		{
			return cc;
		}
	}

	return NULL;
}

/*
  ==============================
  G_CCMap3RemoveCCluster

  ==============================
*/
void G_CCMap3RemoveCCluster( ccmap3_t *map, ccluster_t *cc )
{
	int		key;
	ccluster_t	*c, *next, *head;

	key = G_CCMap3CalcHashkey( map, cc->pos );
	head = NULL;

	for ( c = map->hash[key]; c ; c=next )
	{
		next = c->next;

		if ( c == cc )
		{
			map->clusternum--;
			continue;
		}
		c->next = head;
		head = c;
	}
	map->hash[key] = head;	
}

/*
  ==============================
  G_Vec3dCalcCCMap3Units

  ==============================
*/
void G_Vec3dToCCMap3Units( ccmap3_t *map, ivec3d_t out, ivec3d_t in )
{
//	out[0] = (int)floor( 1.0*in[0]/map->clusterunits );
//	out[1] = (int)floor( 1.0*in[1]/map->clusterunits );
//	out[2] = (int)floor( 1.0*in[2]/map->clusterunits );
	IVec3dUnitSnap( out, in, map->clusterunits );
}

/*
  ==============================
  G_NewCCluster

  ==============================
*/
ccluster_t * G_NewCCluster( int datasize )
{
	ccluster_t	*cc;
	size_t		size;

	size = (size_t)&(((ccluster_t *)0)->cdata[datasize]);
	cc = (ccluster_t *) NEWBYTES( size );

	return cc;
}

void G_FreeCCluster( ccluster_t *cc )
{
	FREE( cc );
}

/*
  ==============================
  G_LoadCCMap3Binary

  ==============================
*/

void G_CCMap3LoadCCluster( ccmap3_t *map, FILE *h )
{
	int		size;
	short		s;	
	ccluster_t	*cc;

	size = map->cellbytes * map->clustercellnum;
	cc = G_NewCCluster( size );

	// read cluster pos
	fread( &s, 2, 1, h );
	cc->pos[0] = (int) s;
	fread( &s, 2, 1, h );
	cc->pos[1] = (int) s;
	fread( &s, 2, 1, h );
	cc->pos[2] = (int) s;

	// read compressed data
	cc->cinfo.size = size;
	fread( cc->cdata, size, 1, h );

	G_CCMap3InsertCCluster( map, cc );
}

ccmap3_t * G_LoadCCMap3Binary( char *name )
{
	int		i;
	FILE		*h;
	int		clusternum;
	int		clustersize;
	int		cellbytes;
	ccmap3_t	*map;

	h = fopen( name, "r" );
	if ( !h )
	{
		return NULL;
//		__error( "G_LoadCCMap3Binary: can't open file\n" );
	}

	fread( &clusternum, 4, 1, h );
	fread( &clustersize, 4, 1, h );

	cellbytes = 12;	// hack, sizeof vec3d_t

	map = G_NewCCMap3( 1024, clustersize, cellbytes );

	// hack
	map->clusterunits = 64;
	map->cellunits = map->clusterunits / clustersize;

	for ( i = 0; i < clusternum; i++ )
	{
		G_CCMap3LoadCCluster( map, h );
	}
	fclose( h );

//	printf( " %d clusters\n", map->clusternum );
//	G_DumpCCMap3( map );	

	return map;
}
