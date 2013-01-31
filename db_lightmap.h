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



// db_lightmap.h

#ifndef __db_lightmap
#define __db_lightmap

#include "u_defs.h"		// unique_t
#include "lib_hobjdefs.h"
#include "db_lightmapdefs.h"
#include "ib_service.h"

#include "wf_info_defs.h"
#if 0
void LightmapDB_Init( db_lightmap_t *db );
void LightmapDB_CleanUp( db_lightmap_t *db );

void LightmapDB_Load( db_lightmap_t *db, char *lightmap_bin, char *lightmap_class, char *source_class );

lightsource_t * LightmapDB_SelectLightsourceWhereAnyLightID( db_lightmap_t *db, unique_t id_any_light );
void LightmapDB_SelectLightmapsWhereShapeID( db_lightmap_t *db, unique_t id_shape, void (*select_func)(lightmap_t *lmap) );

char * LightmapDB_GetPixels( db_lightmap_t *db, lightmap_t *head, int num_pixel );
#endif


class lightmap_db_cpp {
public:
    lightmap_db_cpp(const char* lightmap_bin_name, const char* lightmap_class_name, const char* source_class_name ) ;
    ~lightmap_db_cpp();
    
//     void load( const char* lightmap_bin_name, const char* lightmap_class_name, const char* source_class_name );
//     
    lightsource_t * SelectLightsourceWhereAnyLightID( unique_t id_any_light );
    void SelectLightmapsWhereShapeID( unique_t id_shape, void (*select_func)(lightmap_t *lmap) );

    char * GetPixels( lightmap_t *head, int num_pixel );
    
    
private:
    void insert_hash( lightmap_t *lmap );
    
// no special sort order
    u_list_t    lightsource_list;   
//  u_list_t    lightmap_list;  

    unsigned char   *lightmap_bin;      // lightmap pixel binary

    x_cache_t   cache;          // lightmap combine cache

    lightmap_t     *lightmap_hash[LIGHTMAP_SHAPE_ID_HASH_SIZE];
    ibase::file_handle lm_bin_handle_;
    ibase::file_handle::mapping lm_bin_mapping_;
    
    
    
};
#endif
