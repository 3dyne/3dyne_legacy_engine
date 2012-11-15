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



// sv_state.h

#ifndef __sv_state
#define __sv_state

#include "server.h"
#include "l_defs.h"

extern l_api_t	l_api;
extern g_api_t g_api;

extern byte_iter_t	*bi_in;
extern byte_iter_t	*bi_out;

extern hobj_t		*sv_at_root;

extern ati_list_t	sv_wakeup;
extern ati_list_t	sv_run;
extern ati_list_t	sv_sleep;
extern ati_list_t	sv_dead;

extern u_map_t		sv_id_to_ati_map;

extern int		sv_frame_count;

extern msec_t		sv_time;		// dto
extern msec_t		sv_time_current_frame;
extern msec_t		sv_time_last_frame;
extern msec_t		sv_time_delta_frame;
extern fp_t		sv_time_delta;
extern fp_t		sv_delta_sec;		// dto

#endif
