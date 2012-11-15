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



// cl_state.h

#ifndef __cl_state
#define __cl_state

#include "client.h"
#include "cl_defs.h"
#include "hud_defs.h"

extern l_api_t	l_api;
extern g_api_t	g_api;

extern byte_iter_t	*bi_in;
extern byte_iter_t	*bi_out;

extern hobj_t		*cl_at_root;

extern u_list_t		cl_ati_list;
extern u_list_t		cl_ati_wakeup_list;
extern u_map_t		cl_id_to_ati_map;


extern msec_t		cl_time_begin_frame;	// begin of client frame in client_time
extern msec_t		cl_time_end_frame;	// end of client frame in client_time
extern fp_t		cl_time_delta;		// duration in sec of current client frame

extern msec_t		cl_sv_time_begin_frame;	// begin of server frame in server_time
extern msec_t		cl_time;		// dto
extern msec_t		cl_sv_time_end_frame;	// end of server frame in server_time
extern fp_t		cl_sv_time;		// time in sec since current server frame arrived
extern fp_t		cl_sv_time_delta;		// duration in sec of current server frame

extern int		cl_num_hud;		// player_hud num at end of client frame
extern hud_player_t	       *cl_huds[CL_MAX_HUD];	// for frag-hud stuff


#endif
