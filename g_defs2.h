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



// g_defs2.h

#ifndef __g_defs2
#define __g_defs2

#include "lib_containerdefs.h"
#include "lib_queuedefs.h"

#include "wf_info_defs.h"
#include "events.h"
#include "g_api.h"

#if !defined( win32_x86 )
#include <sys/types.h>                                                          
#include <sys/socket.h>                                                         
#include <netinet/in.h>                                                         
#include <sys/time.h>                                                           
#include <arpa/inet.h>

#else
#undef BTYPE
#include <winsock.h>
#endif

#if 1
//
// event
//

typedef struct g_event_s
{
	char		buf[256];
} g_event_t;
#endif

//
// server
//

typedef enum 
{
	gServerMode_none,
	gServerMode_local,
	gServerMode_remote,

	gServerMode_num
} gServerMode;

typedef enum
{
	gServerState_none,

	gServerState_try_init,
	gServerState_is_init,

	gServerState_num
} gServerState;

#define G_SERVER_INPUT_BUF_SIZE	( 1024 )
#define G_SERVER_OUTPUT_BUF_SIZE ( 4096 )

typedef struct g_server_s
{
	gServerMode	mode;
	gServerState	state;

//	event_queue_t	*eq_in;		// is gc_state.eq_server
//	event_queue_t	*eq_out;	// is gc_state.eq_client
//	byte_iter_t	*bi_out;
//	byte_iter_t	*bi_in;

	int		buf_in_size;
	int		buf_out_size;
	char		buf_in[G_SERVER_INPUT_BUF_SIZE];
	char		buf_out[G_SERVER_OUTPUT_BUF_SIZE];

	// remote server
	struct sockaddr_in	remote_addr;

	// local server dll
	l_library_t	lib;
} g_server_t;

//
// client
//

#define G_CLIENT_INPUT_BUF_SIZE	( 1024*4 )
#define G_CLIENT_OUTPUT_BUF_SIZE ( 1024*16 )

#define G_CLIENT_MAX_RI_MODELS	( 1024 )
#define G_CLIENT_MAX_RI_SPRITES	( 256 )
#define G_CLIENT_MAX_RI_PSYSS	( 256 )
#define G_CLIENT_MAX_RI_LOCAL_LIGHTS	( 256 )
#define G_CLIENT_MAX_RI_HALOS	( 256 )

#define G_CLIENT_MAX_DI_RECTS	( 2048 )
#define G_CLIENT_MAX_DI_TEXTS	( 2048 )

typedef enum
{
	gClientMode_none,
	gClientMode_local,
	gClientMode_num
} gClientMode;

typedef enum
{
	gClientState_none,
	gClientState_try_init,
	gClientState_is_init,
	gClientState_num
} gClientState;


typedef struct g_client_s
{
	gClientMode	mode;
	gClientState	state;

//	byte_iter_t	*bi_in;
//	byte_iter_t	*bi_out;

	int		buf_in_size;
	int		buf_out_size;
	char		buf_in[G_CLIENT_INPUT_BUF_SIZE];
	char		buf_out[G_CLIENT_OUTPUT_BUF_SIZE];

	// local client dll
	l_library_t	lib;

	// render infos
	int		ri_model_num;
	ri_model_t	*ri_models[G_CLIENT_MAX_RI_MODELS]; 

	int		ri_sprite_num;
	ri_sprite_t	*ri_sprites[G_CLIENT_MAX_RI_SPRITES];

	int		ri_psys_num;
	ri_psys_t		*ri_psyss[G_CLIENT_MAX_RI_PSYSS];

	int		ri_local_light_num;
	ri_local_light_t		*ri_local_lights[G_CLIENT_MAX_RI_LOCAL_LIGHTS];

	int		ri_halo_num;
	ri_halo_t	*ri_halos[G_CLIENT_MAX_RI_HALOS];

	// draw infos
	int		di_rect_num;
	int		di_text_num;

	di_rect_t	*di_rects[G_CLIENT_MAX_DI_RECTS];
	di_text_t	*di_texts[G_CLIENT_MAX_DI_TEXTS];

	// render views
	ri_view_t	*ri_view_player;
	ri_view_t	*ri_view_spectator;
	ri_view_t	*ri_view_sky;
#if 0
	
	// dlights registered for world frame by GI_RegisterDLightForWF
	int		wf_dlight_num;
	wf_dlight_info_t	*wf_dlights[G_CLIENT_MAX_DLIGHTS];

	// field particle systems registered for world frame by GI_RegisterWFInfo
	int		wf_fpsys_num;
	wf_fpsys_info_t		*wf_fpsyss[G_CLIENT_MAX_FPSYSS];

	// lightsource state changes registered for world frame by GI_RegisterWFInfo
	int		wf_lightsource_num;
	wf_light_info_t		*wf_lights[G_CLIENT_MAX_LIGHTS];
	
	// shape manipulations registered for world frame by GI_RegisterWFInfo
	int		wf_shapecolor_num;
	wf_shape_info_t		*wf_shapes[G_CLIENT_MAX_SHAPES];
#endif	

} g_client_t;

#endif
