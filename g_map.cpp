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



// g_map.c
#include "g_ccmap3.h"
#include "r_gen_gl/imgcache.h"
#include "r_gen_gl/shader.h"
#include "r_fake/r_interface.h"
#include "interfaces.h"
#include "db_lightmap.h"
#include "db_shape.h"

#include "s_config.h"
#include "s_mem.h"
#include "g_blockmap.h"
/*
  ==============================
  G_NewState

  ==============================
*/
g_state_t * G_NewState( void )
{
	g_state_t *st;

	st = NEWTYPE( g_state_t );
	return st;
}
 

/*
  ==============================
  G_StateInit

  ==============================
*/
void G_StateInit( g_state_t *st )
{
//	st->map = NULL;
	st->life = NULL;
}



/*
  ==============================
  G_FreeState

  ==============================
*/
void G_FreeState( g_state_t *st )
{
	FREE( st );
}





/*
  ==================================================
  G_MapTryClassLoad

  ==================================================
*/
hmanager_t * G_MapTryClassLoad( const char *path, const char *name )
{
	hmanager_t	*hm;
	tokenstream_t	*ts;
	hobj_t		*root;
	char		tt[256];

//	printf( "path: \"%s\"\n", path );
	sprintf( tt, "%s/%s", path, name );
//	printf( "loading: %s", name );
	GC_GiveBackTime();
	
	ts = BeginTokenStream( tt );
	if ( !ts )
		return NULL;

	root = ReadClass( ts );
	if ( !root )
		return NULL;

	EndTokenStream( ts );

	hm = NewHManagerFromClass( root );

	if ( !hm )
		return NULL;
	return hm;
}


/*
  ==================================================
  G_MapTryCCMap3Load

  ==================================================
*/
ccmap3_t * G_MapTryCCMap3Load( char *path, char *name )
{
	ccmap3_t	*map;
	char		tt[256];

	sprintf( tt, "%s/%s", path, name );
	map = G_LoadCCMap3Binary( tt );
	if ( !map )
		return NULL;
	return map;
}

/*
  ==============================
  G_SpawnMap

  ==============================
*/

void G_SpawnMap( g_map_t *map, char *path )
{
	//
	// load classes
	//
	
	SHV_Printf( " ... planes\n" );
	map->planehm = G_MapTryClassLoad( path, MAP_CLASS_NAME_PLANES );

	SHV_Printf( " ... mapnodes\n" );
	map->mapnodehm = G_MapTryClassLoad( path, MAP_CLASS_NAME_MAPNODES );

	SHV_Printf( " ... texdefs\n" );
	map->texdefhm = G_MapTryClassLoad( path, MAP_CLASS_NAME_TEXDEFS );

	SHV_Printf( " ... textures\n" );
	map->texturehm = G_MapTryClassLoad( path, MAP_CLASS_NAME_TEXTURES );

	SHV_Printf( " ... visleafs\n" );
	map->visleafhm = G_MapTryClassLoad( path, MAP_CLASS_NAME_VISLEAFS );

	SHV_Printf( " ... blockmap brushes\n" );
	map->blockmaphm = G_MapTryClassLoad( path, MAP_CLASS_NAME_BLOCKMAP );

	SHV_Printf( " ... shapes\n" );
	map->shapehm = G_MapTryClassLoad( path, MAP_CLASS_NAME_SHAPES );

	if ( !map->planehm )
		__error( "plane class load failed.\n" );
	if ( !map->mapnodehm )
		__error( "mapnode class load failed.\n" );
	if ( !map->texdefhm )
		__error( "texdef class load failed.\n" );
	if ( !map->texturehm )
		__error( "texture class loaf failed.\n" );
	if ( !map->visleafhm )
		__error( "visleaf class load failed.\n" );
	if ( !map->blockmaphm )
		__error( "blockmap class load failed.\n" );
	if ( !map->shapehm ) 
		__error( "shape class load failed.\n" );

#if 0	
	LOGMSG( " volume map ...\n" );
	map->volume_ccmap3 = G_MapTryCCMap3Load( path, MAP_CCMAP3_NAME_VOLUME );
	LOGMSG( " field map ...\n" );
	map->field_ccmap3 = G_MapTryCCMap3Load( path, MAP_CCMAP3_NAME_FIELD );
	
	if ( !map->volume_ccmap3 )
		__warning( "volume binary load failed.\n" );
	if ( !map->field_ccmap3 )
		__warning( "field binary load failed.\n" );
#endif	

}

/*
  ==============================
  G_RejectMap

  ==============================
*/

void G_RejectMap( g_map_t *map )
{
	printf( "G_RejectMap: freeing classes ...\n" );

	if ( map->planehm )
	{
		HManagerDeepDestroyClass( map->planehm, HManagerGetRootClass( map->planehm ) );
		map->planehm = NULL;
	}

	if ( map->mapnodehm )
	{
		HManagerDeepDestroyClass( map->mapnodehm, HManagerGetRootClass( map->mapnodehm ) );
		map->mapnodehm = NULL;
	}

	if ( map->texdefhm )
	{
		HManagerDeepDestroyClass( map->texdefhm, HManagerGetRootClass( map->texdefhm ) );
		map->texdefhm = NULL;
	}

	if ( map->texturehm )
	{
		HManagerDeepDestroyClass( map->texturehm, HManagerGetRootClass( map->texturehm ) );
		map->texturehm = NULL;
	}

	if ( map->visleafhm )
	{
		HManagerDeepDestroyClass( map->visleafhm, HManagerGetRootClass( map->visleafhm ) );
		map->visleafhm = NULL;
	}

	if ( map->blockmaphm )
	{
		HManagerDeepDestroyClass( map->blockmaphm, HManagerGetRootClass( map->blockmaphm ) );
		map->blockmaphm = NULL;
	}	

	if ( map->shapehm )
	{
		HManagerDeepDestroyClass( map->shapehm, HManagerGetRootClass( map->shapehm ) );
		map->shapehm = NULL;
	}

	if ( map->lightdefhm )
	{
		HManagerDeepDestroyClass( map->lightdefhm, HManagerGetRootClass( map->lightdefhm ) );
		map->lightdefhm = NULL;
	}
}


/*
  ==================================================
  G_InitData
  
  ==================================================
*/
void G_InitData( g_map_t *map )
{
	__named_message( " --- init game data of map ---\n" );
	G_InitBlockmap( map );
}


/*
  ==================================================
  G_CleanUpData

  ==================================================
*/
void G_CleanUpData( g_map_t *map )
{
	__named_message( " --- clean up game data from map ---\n" );

	G_CleanUpBlockmap( map->blockmap );
	FREE( map->blockmap );
}


/*
  ==============================
  G_InitMap

  ==============================
*/
void G_InitMap( g_map_t *map )
{
//	g_map_t		*map;

	char		lightmap_bin[256];
	char		lightmap_class[256];
	char		lightsource_class[256];
	char		tt[256];
	char		path[256];
	tokenstream_t	*ts;

	sh_var_t		*tmp;

	INITTYPE( map, g_map_t );
	map->state = gMapState_try_init;

	SHV_Printf( "\n" );
	SHV_Printf( " --- Init Map ---\n" );

	tmp = SHP_GetVar( "gc_map" );
	SHV_Printf( "gc_map is '%s'\n", tmp->string );

	sprintf( path, "world/%s.d/fake", tmp->string );
	SHV_Printf( "path: '%s'\n", path );

//	SHV_Printf( "create new game map\n" );
//	map = NEWTYPE( g_map_t );

	//
	// load multilayer defs
	//
	SHV_Printf( "load multilayer definition class\n" );
	{
		hobj_t		*root;

		ts = BeginTokenStream( "shape_config/multilayer.hobj" );
		if ( !ts )
			Error( "can't open shape_config/multilayer.hobj" );
		root = ReadClass( ts );
		EndTokenStream( ts );

	        map->multilayerhm = NewFlatHManagerFromClass( root );
	}

	// 
	// bsp classes
	//

	printf( "load map classes\n" );
	G_SpawnMap( map, path );

	printf( "setup map data\n" );
	R_InitData( map );


	//
	// load lightmap database
	//

	printf( "setup lightmap database\n" );
	map->lightmap_db = NEWTYPE( db_lightmap_t );
	LightmapDB_Init( map->lightmap_db );
	sprintf( lightmap_bin, "%s/%s", path, MAP_BIN_NAME_LIGHTMAP );
	sprintf( lightmap_class, "%s/%s", path, MAP_CLASS_NAME_LIGHTMAP );
	sprintf( lightsource_class, "%s/%s", path, "_light_source.hobj" );
	LightmapDB_Load( map->lightmap_db, lightmap_bin, lightmap_class, lightsource_class  );

	//
	// init shape database
	//
	
	printf( "setup shape database\n" );
	map->shape_db = NEWTYPE( db_shape_t );
	ShapeDB_Init( map->shape_db );

	//
	// load glmesh binary
	//
	{
		int		filesize;
		ib_file_t	*h;

		sprintf( tt, "%s/%s", path, MAP_BIN_NAME_GLMESH );
		h = IB_Open( tt );
		if ( !h )
			__error( "can't open glmesh binary\n" );
		
		filesize = IB_GetSize( h );
		map->glmesh = (unsigned char *)NEWBYTES( filesize );
		IB_Read( map->glmesh, filesize, 1, h );
		IB_Close( h );		
	}
	
	//
	// render stuff
	//

	printf( "setup subimage cache\n" );
	map->imgcache = IC_NewMgr( 128, 128, 64+32 );
	printf( "setup shader manager\n" );
	map->shader_mgr = Shader_NewMgr();

	printf( "setup shapes\n" );
	R_SetupShapes( map );

	R_InitGLResources();

	SHV_Printf( "\n" );
	SHV_Printf( "setup tesselation ...\n" );

	// sort the shapes into the bsp tree
	R_SetupShapeRefs();	// render3.c
	SHV_Printf( "\n" );

	G_InitData( map );

	R_AllShapesLoadLightmaps();

	SHV_Printf( "\n" );
	SHV_Printf( " --- Init Map done ---\n" );


	map->state = gMapState_is_init;
//	return map;
}


/*
  ==============================
  G_CleanUpMap

  ==============================
*/
void G_CleanUpMap( g_map_t *map )
{
	SHV_Printf( "\n" );
	SHV_Printf( " --- Clean Up Map ---\n" );	
	G_CleanUpData( map );
	R_CleanUpData( map );

	// clean up imgcache
	IC_MgrCleanUp( map->imgcache );
	IC_FreeMgr( map->imgcache );

	// clean up lightmap db
	SHV_Printf( " clean up lightmap database\n" );
	LightmapDB_CleanUp( map->lightmap_db );
	FREE( map->lightmap_db );

	// clean up shape db
	SHV_Printf( " clean up shape database\n" );
	ShapeDB_CleanUp( map->shape_db );
	FREE( map->shape_db );

	// todo: clean up map classes and binarys
	G_RejectMap( map );

	map->state = gMapState_none;

	SHV_Printf( "\n" );
	SHV_Printf( " --- Clean Up Map done ---\n" );	
}
