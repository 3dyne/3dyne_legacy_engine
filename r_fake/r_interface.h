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



// r_interface.h

#ifndef __r_interface
#define __r_interface

#include "defs.h"
#include "wf_info_defs.h"

// set from world framw
void R_SetLocalLightInfos( int info_num, ri_local_light_t *infos[] );
void R_SetSpriteInfos( int info_num, ri_sprite_t *infos[] );
void R_SetPsysInfos( int info_num, ri_psys_t *infos[] );
void R_SetHaloInfos( int info_num, ri_halo_t *infos[] );

void R_SetView( vec3d_t origin, fp_t latitude, fp_t longitude );
void R_RenderView( void );

void R_InitGLResources( void );
void R_FreeGLResources( void );

// external services
void R_SelectShapesInBoundBox( vec3d_t min, vec3d_t max, void (*action_func)(unique_t shape_id ) );


// SIM: added missing declarations. don't know where they actually belong
void R_SetupShapes( g_map_t *map );
void R_InitGLResources();
void R_SetupShapeRefs();
void R_AllShapesLoadLightmaps();
void R_Init();
void R_InitData( g_map_t *map );
void R_CleanUpData( g_map_t *map );
void R_Prepare();
void R_ClearView();
void R_ScreenDump( const char *name );

void R_DlightClearTmpFalloffs( void );
void R_DlightCreateTmpFalloffs_gl( ri_local_light_t *dlight, shape_t *shp );
void R_Talk();
void R_CleanUpAllShapeControlJobs( void );
void R_CleanUpAllShapes();
void R_CleanUpAllShapeRefs();
void R_RenderSkyBox();

#endif
