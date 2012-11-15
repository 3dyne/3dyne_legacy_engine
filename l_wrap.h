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



// l_wrap.h

#ifndef __l_wrap
#define __l_wrap

g_resource_t * GI_ResourceSearch( const char *name );
g_resource_t * GI_ResourceAttach( char *name, char *user_name,
				  int (*user_callback_func)(g_resource_t *r, int reason) );
void GI_ResourceDetach( char *name, char *user_name );
hobj_t * GI_ReadClass( char *path );

//void GI_RegisterDLightForWF( wf_dlight_info_t *info );

void GI_RegisterWFInfo( wfInfoType type, void *wfinfo );

g_trace_t * GI_TraceBB( vec3d_t min, vec3d_t max, vec3d_t trymin, vec3d_t trymax );

void GI_MapTraceBB( g_trace_t *tr, vec3d_t min, vec3d_t max, vec3d_t trymin, vec3d_t trymax );
void GI_MapTraceLine( g_trace_t *tr, vec3d_t from, vec3d_t to );

void GI_SelectShapesInBoundBox( vec3d_t min, vec3d_t max, void (*action_func)(unique_t shape_id ) );
lightsource_t * GI_SelectLightSourceWhereAnyLightID( unique_t id_any_light );
void GI_LightSourceUpdate( lightsource_t *lsrc );

unique_t GI_GetUniqueID( void );

void GI_MapShapeIDSetFlatColor( unique_t id_shape, vec3d_t flat_color );

void GI_RenderAddSprite( ri_sprite_t *ri );
void GI_RenderAddPsys( ri_psys_t *ri );
void GI_RenderAddModel( ri_model_t *ri );
void GI_RenderAddLocalLight( ri_local_light_t *ri );
void GI_RenderAddHalo( ri_halo_t *ri );

void GI_RenderAddView( ri_view_t *ri );

void GI_DrawRect( di_rect_t *di );
void GI_DrawText( di_text_t *di );

#endif
