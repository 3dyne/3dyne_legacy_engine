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



// g_api.h
// common game api for shared libraries
// common export api from shared libs.

#ifndef _g_api_h
#define _g_api_h

#include "g_apidefs.h"
#include "g_shareddefs.h"
#include "g_resourcesdefs.h"
#include "a_shared.h"
#include "lib_queuedefs.h"
#include "g_tracedefs.h"
#include "lib_hobjdefs.h"

#define G_API_VERSION ( 1 )
#define L_API_VERSION ( 1 )

#define G_API_SIZE ( sizeof( g_api_t ))
#define L_API_SIZE ( sizeof( l_api_t ))

#define API_LESS	( -1 )
#define API_MORE	( -2 )

// api exported from main game to shared lib
typedef struct {
	int	version;
	size_t	size;
// very basic funcions	
	void (*ShockHandler)();

	void (*TF_Enter)( char * ); // function trace enter
	void (*TF_Leave)( char * ); // func trace leave

	void *(*Malloc)( int );
	void (*Free)( void * );
	
// basic functions
	void (*Printf)( const char*, ... );
	sh_var_t *(*SH_SetVar)( const char*, const char*, unsigned int );
	sh_var_t *(*SH_GetVar)( const char* );
	
// map functions
	void (*Map_TraceBB)( g_trace_t *tr, vec3d_t min_from, vec3d_t max_from, vec3d_t min_to, vec3d_t max_to );

	void (*Map_TraceLine)( g_trace_t *tr, vec3d_t from, vec3d_t to );

// map database
	void (*Map_SelectShapeIDsInBoundBox)( vec3d_t min, vec3d_t max, void (*select_func)( unique_t id_shape ) );

	lightsource_t * (*Map_SelectLightSourceWhereAnyLightID)( unique_t id_any_light );
	void (*Map_LightSourceUpdate)( lightsource_t *lsrc );

	void (*Map_ShapeIDSetFlatColor)( unique_t id_shape, vec3d_t flat_color );

// think: map pvs
//	void (*Map_GetPVSInBoundBox)( g_pvs_t *pvs, vec3d_t min, vec3d_t max );
//
//	
//
//	typedef g_render_s
//	{
//		pvs info
//		view info		
//	} g_render_t;      
//	Render( g_render_t * );

// render infos
	void (*Render_Sprite)( ri_sprite_t *ri );
	void (*Render_Psys)( ri_psys_t *ri );
	void (*Render_LocalLight)( ri_local_light_t *ri );
	void (*Render_Model)( ri_model_t *ri );
	void (*Render_Halo)( ri_halo_t *ri );

	void (*Render_View)( ri_view_t *ri );

// draw infos
	void (*Draw_Rect)( di_rect_t *di );
	void (*Draw_Text)( di_text_t *di );

	unique_t (*Map_GetUniqueID)( void );
// io
	g_resource_t * (*GI_ResourceSearch)( const char *name );
	hobj_t * (*GI_ReadClass)( char *path );
} g_api_t;


// api exported from shared lib to main game.
// the version and size fields are used to report the reason of a failed api exchange, if valid id 0 on return.
// API_LESS means that the local api version is less than the impored, API_MORE means the opposite.
// if valid is != 0, version and size do really contain version and size of the returned api.
typedef struct l_api_s{
	int	version;
	size_t	size;
	int	valid;
	char	owner[64];
	
	int		(*Init)( void );
	void		(*CleanUp)( void );

	const char *	(*Com)( const char *in );

	void		(*SetInputByteIter)( byte_iter_t *bi );
	void		(*SetOutputByteIter)( byte_iter_t *bi );	
} l_api_t;

// this must be called first to make the apis known to each other. This is the only function to be resolved from the shared lib.
typedef l_api_t *(*api_exchange_t)( g_api_t *api);


// l_library_t is the game side representation of a shared lib
typedef struct {
	char	name[64];
	int	h;
	int	isloaded;
	l_api_t	api;
	api_exchange_t	ApiExchange;
} l_library_t;


#endif
