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



// hud_defs.h

#ifndef __hud_defs
#define __hud_defs

#include "g_apidefs.h"
#include "u_defs.h"

typedef struct hud_player_s
{
	msec_t	time_nickname;
	char	nickname[CL_SV_PLAYER_NICKNAME_SIZE];
	msec_t	time_health;
	int	health;
	msec_t	time_numfrag;
	int	numfrag;

	bool_t	night_vision_goggle;
	
	// draw info
	di_rect_t	di_rect;

	di_rect_t	di_health[4];
	di_rect_t	di_face;
	di_rect_t	di_back;
	
	di_rect_t	di_crosshair;

	di_text_t	di_frag;
	di_text_t	di_nickname;
	g_resource_t	*num[11];

} hud_player_t;

#endif
