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


// CHANGED FILES:
// db_lightmap.cpp
// db_lightmap.h
// db_lightmapdefs.h
// g_defs.h
// l_wrap.cpp
// r_fake/render3.cpp
// r_fake/r_shapes.cpp
// r_fake/r_shpctrljobs.cpp

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "ib_service.h"
#include "u_token.h"
#include "u_hobj.h"
#include "lib_math.h"
#include "lib_hobj.h"
#include "lib_container.h"
#include "lib_digest.h"
#include "la_service.h"

#include "db_lightmap.h"
#include "shock.h"

static bool_t	db_internal_init = false;

static fp_t	db_hitrate;
static int	db_datamem;

static unsigned char	db_ramps[256][256];

void X_CacheInit( x_cache_t *c, int max_mem, int highwater_mem, int lowwater_mem );
void X_CacheCleanUp( x_cache_t *c );


static void VV_LightCC( int flag, char *string )
{
	sprintf( string, "light combine cache: hitrate: %.2f%%, %f.2 KB", db_hitrate, db_datamem/1024.0 );
}

static void LightmapDB_InternalSetupRamps( void )
{
	int		i, j;

	for ( i = 0; i < 256; i++ )
	{
		for ( j = 0; j < 256; j++ )
		{
			fp_t	b;
			int	c;

			b = ((fp_t) j * (i/255.0));
			c = (int) b;
			db_ramps[i][j] = (unsigned char) c;
		}
	}
}

/*
  =============================
  LightmapDB_Init

  ==============================
*/
#if 0
void LightmapDB_Init( db_lightmap_t *db )
{
	INITTYPE( db, db_lightmap_t );

	U_InitList( &db->lightsource_list );

	X_CacheInit( &db->cache, 1024*1024*16, 1024*1024*4, 1024*1024*3 );

	db->lightmap_bin = NULL;

	if( db_internal_init == false )
	{
		db_internal_init = true;

		LA_InsertVarview( "vv_lightcc", VV_LightCC );
		
		LightmapDB_InternalSetupRamps();
	}

}
#endif

void bake_hobj( hobj_t *o );

lightmap_db_cpp::lightmap_db_cpp( const char* lightmap_bin_name, const char* lightmap_class_name, const char* source_class_name ) 
: lm_bin_handle_(lightmap_bin_name),
  lm_bin_mapping_(lm_bin_handle_)

{
    
    U_InitList( &lightsource_list );

    X_CacheInit( &cache, 1024*1024*16, 1024*1024*4, 1024*1024*3 );

    lightmap_bin = NULL;

    std::fill( lightmap_hash, lightmap_hash + LIGHTMAP_SHAPE_ID_HASH_SIZE, (lightmap_t*)0 );
    
    if( db_internal_init == false )
    {
        db_internal_init = true;

        LA_InsertVarview( "vv_lightcc", VV_LightCC );
        
        LightmapDB_InternalSetupRamps();
    }
    
    ////////////////////////////////////////////////////////
    // previous Load code
    int     i;

    hobj_t      *lightmap_root;
    hobj_t      *lightsource_root;
    unsigned char   *lightmap_bin;
    ib_file_t       *h;
    int         filesize;
    hobj_search_iterator_t  iter;
    hobj_t          *lightmap;
    hobj_t          *lightsource;
    tokenstream_t   *ts;

    u_list_iter_t       lsrc_iter;

    lightmap_t      *lmap;
    lightsource_t       *lsrc;

    //
    // load lightmap binary
    //
#if 0
    h = IB_Open( lightmap_bin_name );
    if ( !h )
    {
        __error( "can't open lightmap binary\n" );
    }
    
    filesize = IB_GetSize( h );
    lightmap_bin = (unsigned char *)NEWBYTES( filesize );
    __chkptr( lightmap_bin );

    IB_Read( lightmap_bin, filesize, 1, h );
    IB_Close( h );

    lightmap_bin = lightmap_bin;
#else
    filesize = lm_bin_handle_.get_size();
    
    lightmap_bin = (unsigned char*)const_cast<char *>(lm_bin_mapping_.ptr());
    __chkptr( lightmap_bin );
#endif

    //
    // load lightmap class
    //

    ts = BeginTokenStream( lightmap_class_name );
    if ( !ts )
    {
        __error( "can't open lightmap class\n" );
    }
    
    lightmap_root = ReadClass( ts );
    if ( !lightmap_root )
    {
        __error( "can't load lightmap class\n" );
    }
    EndTokenStream( ts );

    
    bake_hobj( lightmap_root );
    
    InitClassSearchIterator( &iter, lightmap_root, "*" );
    for ( ; ( lightmap = SearchGetNextClass( &iter ) ) ; )
    {
        int     ofs;

        lmap = NEWTYPE( lightmap_t );
        
        EasyFindClsrefAsUnique( &lmap->id_shape, lightmap, "shape_id" );
        EasyFindClsrefAsUnique( &lmap->id_source, lightmap, "source_id" );
        EasyFindInt( &lmap->type, lightmap, "type" );
        EasyFindInt( &ofs, lightmap, "ofs" );
        
        lmap->pixel_ptr = &lightmap_bin[ofs];
        
        insert_hash( lmap );
    }

    //
    // load lightsource class
    //

    ts = BeginTokenStream( source_class_name );
    if ( !ts )
    {
        __error( "can't open lightsource class\n" );
    }

    lightsource_root = ReadClass( ts );
    if ( !lightsource_root )
    {
        __error( "can't load lightsource class\n" );
    }
    EndTokenStream( ts );
    
    InitClassSearchIterator( &iter, lightsource_root, "*" );
    for ( ; ( lightsource = SearchGetNextClass( &iter ) ) ; )
    {
        lsrc = NEWTYPE( lightsource_t );

        lsrc->id_source = StringToUnique( lightsource->name );
        EasyFindClsrefAsUnique( &lsrc->id_any_light, lightsource, "any_light" );
        EasyFindVec3d( lsrc->color, lightsource, "color" );

        U_ListInsertAtHead( &lightsource_list, lsrc );
    }

    //
    // link lightsources and lightmaps
    //

    // for each id_source
    //  link all lightmaps with same id_source
    //  set list head

    U_ListIterInit( &lsrc_iter, &lightsource_list );
    for ( ; ( lsrc = (lightsource_t *)U_ListIterNext( &lsrc_iter ) ) ; ) 
    {
        lsrc->head = NULL;

        for ( i = 0; i < LIGHTMAP_SHAPE_ID_HASH_SIZE; i++ )
        {
            for ( lmap = lightmap_hash[i]; lmap; lmap=lmap->hash_link )
            {
                if ( lmap->id_source != lsrc->id_source )
                {
                    continue;
                }
                
                lmap->source_ref = lsrc;
                
                lmap->source_link = lsrc->head;
                lsrc->head = lmap;
            }
        }
    }

    //
    // destroy classes
    //
    
    DeepDestroyClass( lightmap_root );
    DeepDestroyClass( lightsource_root );    
}


/*
  ==============================
  LightmapDB_CleanUp

  ==============================
*/

static void ObjDestroy( void *obj )
{
	__chkptr( obj );
	FREE( obj );
}
#if 0
void LightmapDB_CleanUp( db_lightmap_t *db )
{     
	int		i;
	
	U_CleanUpList( &db->lightsource_list, ObjDestroy );
	
	//
	// clean up lightmap hash
	//
	for ( i = 0; i < LIGHTMAP_SHAPE_ID_HASH_SIZE; i++ )
	{
		lightmap_t	*lmap, *next;
		
		for ( lmap = db->lightmap_hash[i]; lmap; lmap=next )
		{
			next = lmap->hash_link;
			FREE( lmap );
		}
	}
	
	X_CacheCleanUp( &db->cache );

	if ( db->lightmap_bin )
	{
		FREE( db->lightmap_bin );
	}
}
#endif

lightmap_db_cpp::~lightmap_db_cpp() {
    int     i;
    
    U_CleanUpList( &lightsource_list, ObjDestroy );
    
    //
    // clean up lightmap hash
    //
    for ( i = 0; i < LIGHTMAP_SHAPE_ID_HASH_SIZE; i++ )
    {
        lightmap_t  *lmap, *next;
        
        for ( lmap = lightmap_hash[i]; lmap; lmap=next )
        {
            next = lmap->hash_link;
            FREE( lmap );
        }
    }
    
    X_CacheCleanUp( &cache );

    
//     if ( lightmap_bin )
//     {
//         FREE( lightmap_bin );
//     }
}

/*
  ==============================
  InsertHash

  ==============================
*/
static int CalcHash( unique_t id_shape )
{
	return id_shape % LIGHTMAP_SHAPE_ID_HASH_SIZE;
}

#if 0
static void InsertHash( db_lightmap_t *db, lightmap_t *lmap )
{
	int	entry;
	
	entry = CalcHash( lmap->id_shape );
	
	lmap->hash_link = db->lightmap_hash[entry];
	db->lightmap_hash[entry] = lmap;	
}
#endif

void lightmap_db_cpp::insert_hash( lightmap_t *lmap )
{
    int entry;
    
    entry = CalcHash( lmap->id_shape );
    
    lmap->hash_link = lightmap_hash[entry];
    lightmap_hash[entry] = lmap;    
}
/*
  ==============================
  LightmapDB_Load

  ==============================
*/
#if 0
void LightmapDB_Load( db_lightmap_t *db, char *lightmap_bin_name, char *lightmap_class_name, char *source_class_name )
{
	int		i;

	hobj_t		*lightmap_root;
	hobj_t		*lightsource_root;
	unsigned char	*lightmap_bin;
	ib_file_t		*h;
	int			filesize;
	hobj_search_iterator_t	iter;
	hobj_t			*lightmap;
	hobj_t			*lightsource;
	tokenstream_t	*ts;

	u_list_iter_t		lsrc_iter;

	lightmap_t		*lmap;
	lightsource_t		*lsrc;

	//
	// load lightmap binary
	//

	h = IB_Open( lightmap_bin_name );
	if ( !h )
	{
		__error( "can't open lightmap binary\n" );
	}
	
	filesize = IB_GetSize( h );
	lightmap_bin = (unsigned char *)NEWBYTES( filesize );
	__chkptr( lightmap_bin );

	IB_Read( lightmap_bin, filesize, 1, h );
	IB_Close( h );

	db->lightmap_bin = lightmap_bin;


	//
	// load lightmap class
	//

	ts = BeginTokenStream( lightmap_class_name );
	if ( !ts )
	{
		__error( "can't open lightmap class\n" );
	}
	
	lightmap_root = ReadClass( ts );
	if ( !lightmap_root )
	{
		__error( "can't load lightmap class\n" );
	}
	EndTokenStream( ts );

	InitClassSearchIterator( &iter, lightmap_root, "*" );
	for ( ; ( lightmap = SearchGetNextClass( &iter ) ) ; )
	{
		int		ofs;

		lmap = NEWTYPE( lightmap_t );
		
		EasyFindClsrefAsUnique( &lmap->id_shape, lightmap, "shape_id" );
		EasyFindClsrefAsUnique( &lmap->id_source, lightmap, "source_id" );
		EasyFindInt( &lmap->type, lightmap, "type" );
		EasyFindInt( &ofs, lightmap, "ofs" );
		
		lmap->pixel_ptr = &db->lightmap_bin[ofs];
		
		InsertHash( db, lmap );
	}

	//
	// load lightsource class
	//

	ts = BeginTokenStream( source_class_name );
	if ( !ts )
	{
		__error( "can't open lightsource class\n" );
	}

	lightsource_root = ReadClass( ts );
	if ( !lightsource_root )
	{
		__error( "can't load lightsource class\n" );
	}
	EndTokenStream( ts );
	
	InitClassSearchIterator( &iter, lightsource_root, "*" );
	for ( ; ( lightsource = SearchGetNextClass( &iter ) ) ; )
	{
		lsrc = NEWTYPE( lightsource_t );

		lsrc->id_source = StringToUnique( lightsource->name );
		EasyFindClsrefAsUnique( &lsrc->id_any_light, lightsource, "any_light" );
		EasyFindVec3d( lsrc->color, lightsource, "color" );

		U_ListInsertAtHead( &db->lightsource_list, lsrc );
	}

	//
	// link lightsources and lightmaps
	//

	// for each id_source
	//	link all lightmaps with same id_source
	//	set list head

	U_ListIterInit( &lsrc_iter, &db->lightsource_list );
	for ( ; ( lsrc = (lightsource_t *)U_ListIterNext( &lsrc_iter ) ) ; ) 
	{
		lsrc->head = NULL;

		for ( i = 0; i < LIGHTMAP_SHAPE_ID_HASH_SIZE; i++ )
		{
			for ( lmap = db->lightmap_hash[i]; lmap; lmap=lmap->hash_link )
			{
				if ( lmap->id_source != lsrc->id_source )
				{
					continue;
				}
				
				lmap->source_ref = lsrc;
				
				lmap->source_link = lsrc->head;
				lsrc->head = lmap;
			}
		}
	}

	//
	// destroy classes
	//
	
	DeepDestroyClass( lightmap_root );
	DeepDestroyClass( lightsource_root );
}

void lightmap_db_cpp::load ( const char* lightmap_bin_name, const char* lightmap_class_name, const char* source_class_name )
{
    int     i;

    hobj_t      *lightmap_root;
    hobj_t      *lightsource_root;
    unsigned char   *lightmap_bin;
    ib_file_t       *h;
    int         filesize;
    hobj_search_iterator_t  iter;
    hobj_t          *lightmap;
    hobj_t          *lightsource;
    tokenstream_t   *ts;

    u_list_iter_t       lsrc_iter;

    lightmap_t      *lmap;
    lightsource_t       *lsrc;

    //
    // load lightmap binary
    //

    h = IB_Open( lightmap_bin_name );
    if ( !h )
    {
        __error( "can't open lightmap binary\n" );
    }
    
    filesize = IB_GetSize( h );
    lightmap_bin = (unsigned char *)NEWBYTES( filesize );
    __chkptr( lightmap_bin );

    IB_Read( lightmap_bin, filesize, 1, h );
    IB_Close( h );

    lightmap_bin = lightmap_bin;


    //
    // load lightmap class
    //

    ts = BeginTokenStream( lightmap_class_name );
    if ( !ts )
    {
        __error( "can't open lightmap class\n" );
    }
    
    lightmap_root = ReadClass( ts );
    if ( !lightmap_root )
    {
        __error( "can't load lightmap class\n" );
    }
    EndTokenStream( ts );

    InitClassSearchIterator( &iter, lightmap_root, "*" );
    for ( ; ( lightmap = SearchGetNextClass( &iter ) ) ; )
    {
        int     ofs;

        lmap = NEWTYPE( lightmap_t );
        
        EasyFindClsrefAsUnique( &lmap->id_shape, lightmap, "shape_id" );
        EasyFindClsrefAsUnique( &lmap->id_source, lightmap, "source_id" );
        EasyFindInt( &lmap->type, lightmap, "type" );
        EasyFindInt( &ofs, lightmap, "ofs" );
        
        lmap->pixel_ptr = &lightmap_bin[ofs];
        
        insert_hash( lmap );
    }

    //
    // load lightsource class
    //

    ts = BeginTokenStream( source_class_name );
    if ( !ts )
    {
        __error( "can't open lightsource class\n" );
    }

    lightsource_root = ReadClass( ts );
    if ( !lightsource_root )
    {
        __error( "can't load lightsource class\n" );
    }
    EndTokenStream( ts );
    
    InitClassSearchIterator( &iter, lightsource_root, "*" );
    for ( ; ( lightsource = SearchGetNextClass( &iter ) ) ; )
    {
        lsrc = NEWTYPE( lightsource_t );

        lsrc->id_source = StringToUnique( lightsource->name );
        EasyFindClsrefAsUnique( &lsrc->id_any_light, lightsource, "any_light" );
        EasyFindVec3d( lsrc->color, lightsource, "color" );

        U_ListInsertAtHead( &lightsource_list, lsrc );
    }

    //
    // link lightsources and lightmaps
    //

    // for each id_source
    //  link all lightmaps with same id_source
    //  set list head

    U_ListIterInit( &lsrc_iter, &lightsource_list );
    for ( ; ( lsrc = (lightsource_t *)U_ListIterNext( &lsrc_iter ) ) ; ) 
    {
        lsrc->head = NULL;

        for ( i = 0; i < LIGHTMAP_SHAPE_ID_HASH_SIZE; i++ )
        {
            for ( lmap = lightmap_hash[i]; lmap; lmap=lmap->hash_link )
            {
                if ( lmap->id_source != lsrc->id_source )
                {
                    continue;
                }
                
                lmap->source_ref = lsrc;
                
                lmap->source_link = lsrc->head;
                lsrc->head = lmap;
            }
        }
    }

    //
    // destroy classes
    //
    
    DeepDestroyClass( lightmap_root );
    DeepDestroyClass( lightsource_root );
}
#endif

/*
  ==============================

  ==============================
*/
#if 0
lightsource_t * LightmapDB_SelectLightsourceWhereAnyLightID( db_lightmap_t *db, unique_t id_any_light )
{
	u_list_iter_t	iter;
	lightsource_t	*lsrc;

	U_ListIterInit( &iter, &db->lightsource_list );
	for ( ; ( lsrc = (lightsource_t *)U_ListIterNext( &iter ) ) ; )
	{
		if ( lsrc->id_any_light == id_any_light )
		{
			return lsrc;
		}
	}

	return NULL;
}
#endif

lightsource_t * lightmap_db_cpp::SelectLightsourceWhereAnyLightID( unique_t id_any_light )
{
    u_list_iter_t   iter;
    lightsource_t   *lsrc;

    U_ListIterInit( &iter, &lightsource_list );
    for ( ; ( lsrc = (lightsource_t *)U_ListIterNext( &iter ) ) ; )
    {
        if ( lsrc->id_any_light == id_any_light )
        {
            return lsrc;
        }
    }

    return NULL;
}


/*
  ==============================  

  ==============================
*/
#if 0
void LightmapDB_SelectLightmapsWhereShapeID( db_lightmap_t *db, unique_t id_shape, void (*select_func)(lightmap_t *lmap) )
{
	lightmap_t	*lmap;

	int		entry;

	entry = CalcHash( id_shape );

	for ( lmap = db->lightmap_hash[entry]; lmap ; lmap=lmap->hash_link )
	{
		if ( lmap->id_shape == id_shape )
		{
			select_func( lmap );
		}	
	}
}
#endif

void lightmap_db_cpp::SelectLightmapsWhereShapeID( unique_t id_shape, void (*select_func)(lightmap_t *lmap) )
{
    lightmap_t  *lmap;

    int     entry;

    entry = CalcHash( id_shape );

    for ( lmap = lightmap_hash[entry]; lmap ; lmap=lmap->hash_link )
    {
        if ( lmap->id_shape == id_shape )
        {
            select_func( lmap );
        }   
    }
}


/*
  ==================================================
  combined lightmap cache development

  ==================================================
*/


/*
  ==============================
  LightmapDB_CombineLightmaps

  ==============================
*/



int	x_num_miss = 0;
int	x_num_hit = 0;
int	x_num_flush = 0;

//int	dbg_digest_size;
//unsigned char	dbg_digest[256];

void X_CacheInit( x_cache_t *c, int max_mem, int highwater_mem, int lowwater_mem )
{
	INITTYPE( c, x_cache_t );
	c->data_mem = 0;
	c->max_mem = max_mem;
	c->highwater_mem = highwater_mem;
	c->lowwater_mem = lowwater_mem;
}

void X_CacheCleanUp( x_cache_t *c )
{
	int		i, j;

	for ( i = 0; i < X_CACHE_NUM_ENTRY; i++ )
	{
		for ( j = 0; j < 4; j++ )
		{
			if ( c->entries[i].data[j] )
			{
				FREE( c->entries[i].data[j] );
			}
		}
	}
}

unsigned int X_CacheCalcEntry( x_cache_t *cache, unsigned int real_addr )
{
	return real_addr%X_CACHE_NUM_ENTRY;
}

bool_t X_CacheTestRealAddr( x_cache_t *cache, unsigned int real_addr )
{
	int		i;
	unsigned int	entry;
	x_cache_entry_t	*e;

	entry = X_CacheCalcEntry( cache, real_addr );

	e =  &cache->entries[entry];

	for ( i = 0; i < 4; i++ )
	{
		if ( !e->data[i] )
			continue;

		if ( e->real_addr[i] != real_addr )
			continue;
		
		x_num_hit++;
		return true;
	}

	x_num_miss++;	
	return false;
}

void * X_CacheGetData( x_cache_t *cache, unsigned int real_addr )
{
	int		i;
	unsigned int	entry;
	x_cache_entry_t	*e;

	void	*data;

	entry = X_CacheCalcEntry( cache, real_addr );

	e =  &cache->entries[entry];
	
	data = NULL;

	for ( i = 0; i < 4; i++ )
	{
		if ( !e->data[i] )
			continue;

		if ( e->real_addr[i] != real_addr )
		{
			if ( e->lru[i] < 255 )
				e->lru[i]++;

			continue;
		}
		
		if ( e->real_addr[i] == real_addr )
		{
			data = e->data[i];
			if ( e->lru[i] > 0 )
				e->lru[i]--;
			
		}
	}

	if ( !data )
		__error( "real_addr not in cache\n" );

	return data;
}

void X_CacheFlushLRU( x_cache_t *c )
{
	int		i;
	x_cache_entry_t	*e;

	int		take;
	unsigned char	lru_val;

	c->rollover = ( c->rollover + 1 ) % X_CACHE_NUM_ENTRY;

	e =  &c->entries[c->rollover];

	take = -1;
	lru_val = 0;
	for ( i = 0; i < 4; i++ )
	{
		if ( !e->data[i] )
		{
			continue;
		}

		if ( e->lru[i] >= lru_val )
		{
			lru_val = e->lru[i];
			take = i;
		}	
	}

	if ( take == -1 )
	{
		return;
	}

	FREE( e->data[take] );
	e->data[take] = NULL;
	c->data_mem -= e->size[take];
//	printf( "mem: %.2f KB\n", e->size[take]/1024.0 );
	e->size[take] = 0;
}

void X_CacheCheckMem( x_cache_t *cache, int want_mem )
{
	int		i;

	if ( cache->data_mem >= cache->highwater_mem )
	{
		//
		// highwater mem reached go through cache till 
		// mem sinks bellow lowwater
		//

//		printf( "reached highwater at %.2f KB\n", cache->data_mem/1024.0 );

		for ( i = 0; i < X_CACHE_NUM_ENTRY ; i++ )
		{ 
			X_CacheFlushLRU( cache );

			if ( cache->data_mem <= cache->lowwater_mem )
			{
				break;
			}
		}

		if ( i == X_CACHE_NUM_ENTRY )
		{
			__error( "can't reach cache->lowwater_mem\n" );
		}

//		printf( "reached lowwater at %.2f KB\n", cache->data_mem/1024.0 );
	}

	if ( cache->data_mem+want_mem >= cache->max_mem )
	{
		__error( "cache->data_mem+want_mem >= cache->max_mem\n" );
	}
}

void X_CacheInsertData( x_cache_t *cache, unsigned int real_addr, void *data, int size )
{
	int		i;
	unsigned int	entry;
	x_cache_entry_t	*e;

	int		take;
	unsigned char	lru_val;

	__chkptr( data );

	X_CacheCheckMem( cache, size );

	entry = X_CacheCalcEntry( cache, real_addr );
	e =  &cache->entries[entry];

	take = 0;
	lru_val = 0;

	for ( i = 0; i < 4; i++ )
	{
		if ( !e->data[i] )
		{
			take = i;
			break;
		}
		
		if ( e->lru[i] > lru_val )
		{
			lru_val = e->lru[i];
			take = i;
		}
	}
	
	if ( e->data[take] )
	{
		x_num_flush++;
		FREE( e->data[take] );		
		cache->data_mem -= e->size[take];		
		e->size[take] = 0;
	}
	
	e->real_addr[take] = real_addr;
	e->data[take] = data;
	e->size[take] = size;
	e->lru[take] = 0;

	cache->data_mem += size;
}

char * LightmapDB_CombineLightmaps( lightmap_t *head, int num_pixel )
{
	lightmap_t	*lmap;
	unsigned char	*pixels;
	int		i;

	pixels = (unsigned char *)NEWBYTES( num_pixel*4 );   

	for ( lmap = head; lmap ; lmap=lmap->combine_link )
	{
		unsigned char	*r_ramp;
		unsigned char	*g_ramp;
		unsigned char	*b_ramp;
		
		r_ramp = db_ramps[(int) (lmap->source_ref->color[0] * 255.0)];
		g_ramp = db_ramps[(int) (lmap->source_ref->color[1] * 255.0)];
		b_ramp = db_ramps[(int) (lmap->source_ref->color[2] * 255.0)];
		
		for ( i = 0; i < num_pixel; i++ )
		{
			int	intens;
			unsigned int	r, g, b;

			intens = lmap->pixel_ptr[i];
			r = r_ramp[intens];
			g = g_ramp[intens];
			b = b_ramp[intens];
			
			r += pixels[i*4];
			if ( r > 255 )
				r = 255;

			g += pixels[i*4+1];
			if ( g > 255 )
				g = 255;

			b += pixels[i*4+2];
			if ( b > 255 )
				b = 255;

			pixels[i*4] = r;
			pixels[i*4+1] = g;
			pixels[i*4+2] = b;
			pixels[i*4+3] = 255;
		}
	}

	return (char *)pixels;
}

#if 0
char * LightmapDB_GetPixels( db_lightmap_t *db, lightmap_t *head, int num_pixel )
{	
	lightmap_t	*lmap;

	gen_info_digest_t	gen_digest;
	info_digest_t	digest;

	void	*pixels;

	static int print_next = 0;

	__chkptr( head );


//	pixels = LightmapDB_CombineLightmaps( head, num_pixel );
//	return pixels;	

	//
	// generate digest from current lightmap combine list
	//

	InfoDigest_Begin( &gen_digest );
	for ( lmap = head; lmap ; lmap=lmap->combine_link )
	{
		InfoDigest_Advance( &gen_digest, 4, &lmap );
		InfoDigest_Advance( &gen_digest, 4, &lmap->id_shape );
		InfoDigest_Advance( &gen_digest, 4, &lmap->id_source );
		InfoDigest_Advance( &gen_digest, 12, lmap->source_ref->color );
	}

	digest = InfoDigest_End( &gen_digest );

#if 0	
	// cat x | grep "digest" | sort | uniq | less
	printf( "digest: %u ", digest );
	for ( i = 0; i < gen_digest.num; i++ )
		printf( "%x ", dbg_digest[i] );
	printf( "\n" );
#endif	

	if ( X_CacheTestRealAddr( &db->cache, digest ) )
	{
		// pixels are in cache
		pixels = X_CacheGetData( &db->cache, digest );		
	}
	else
	{
		// pixels not in cache, create them and insert into cache
		pixels = LightmapDB_CombineLightmaps( head, num_pixel );

		X_CacheInsertData( &db->cache, digest, pixels, num_pixel*4 );
	}

	if ( print_next == 0 )
	{		
		fp_t	rate;

		if ( x_num_hit == 0 )
		{
			rate = 0.0;
		}
		else
		{
			rate = 100.0-((x_num_flush*100.0)/(x_num_hit*1.0));
		}

		db_hitrate = rate;
		db_datamem = db->cache.data_mem;

//		printf( "%d cache hits, %d cache miss, %d cache flush, current hit rate %.2f%%\n", x_num_hit, x_num_miss, x_num_flush, rate );
//		printf( " ================================= %.2f KB in cache ==================\n", db->cache.data_mem/1024.0 );
		   
		print_next = 1024;
		x_num_hit = 0;
		x_num_miss = 0;
		x_num_flush = 0;
	}
	else
	{
		print_next--;
	}

//	pixels = LightmapDB_CombineLightmaps( head, num_pixel );
	return (char *)pixels;	
}
#endif

char * lightmap_db_cpp::GetPixels( lightmap_t *head, int num_pixel )
{   
    lightmap_t  *lmap;

    gen_info_digest_t   gen_digest;
    info_digest_t   digest;

    void    *pixels;

    static int print_next = 0;

    __chkptr( head );


//  pixels = LightmapDB_CombineLightmaps( head, num_pixel );
//  return pixels;  

    //
    // generate digest from current lightmap combine list
    //

    InfoDigest_Begin( &gen_digest );
    for ( lmap = head; lmap ; lmap=lmap->combine_link )
    {
        InfoDigest_Advance( &gen_digest, 4, &lmap );
        InfoDigest_Advance( &gen_digest, 4, &lmap->id_shape );
        InfoDigest_Advance( &gen_digest, 4, &lmap->id_source );
        InfoDigest_Advance( &gen_digest, 12, lmap->source_ref->color );
    }

    digest = InfoDigest_End( &gen_digest );

#if 0   
    // cat x | grep "digest" | sort | uniq | less
    printf( "digest: %u ", digest );
    for ( i = 0; i < gen_digest.num; i++ )
        printf( "%x ", dbg_digest[i] );
    printf( "\n" );
#endif  

    if ( X_CacheTestRealAddr( &cache, digest ) )
    {
        // pixels are in cache
        pixels = X_CacheGetData( &cache, digest );      
    }
    else
    {
        // pixels not in cache, create them and insert into cache
        pixels = LightmapDB_CombineLightmaps( head, num_pixel );

        X_CacheInsertData( &cache, digest, pixels, num_pixel*4 );
    }

    if ( print_next == 0 )
    {       
        fp_t    rate;

        if ( x_num_hit == 0 )
        {
            rate = 0.0;
        }
        else
        {
            rate = 100.0-((x_num_flush*100.0)/(x_num_hit*1.0));
        }

        db_hitrate = rate;
        db_datamem = cache.data_mem;

//      printf( "%d cache hits, %d cache miss, %d cache flush, current hit rate %.2f%%\n", x_num_hit, x_num_miss, x_num_flush, rate );
//      printf( " ================================= %.2f KB in cache ==================\n", db->cache.data_mem/1024.0 );
           
        print_next = 1024;
        x_num_hit = 0;
        x_num_miss = 0;
        x_num_flush = 0;
    }
    else
    {
        print_next--;
    }

//  pixels = LightmapDB_CombineLightmaps( head, num_pixel );
    return (char *)pixels;  
}

