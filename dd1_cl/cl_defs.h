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



// cl_defs.h

#ifndef __cl_defs
#define __cl_defs

#include "u_defs.h"
#include "events.h"

#define CL_MAX_HUD	( 32 )
#define CL_SV_PLAYER_NICKNAME_SIZE	( 32 )

typedef enum
{
	clAtiState_none,
	clAtiState_run,
	clAtiState_dead,
	clAtiState_num
} clAtiState;

typedef struct cl_ati_s
{
	unique_t	id;	
	clAtiState	state;
	hobj_t		*ati_obj;	// only objects created by CL_WorldSpring use this
	int		frame_count;

	struct cl_ati_ftbl_s	*ftbl;
} cl_ati_t;

typedef struct cl_ati_ftbl_s
{
	void	(*Destroy)(cl_ati_t *atif);
	const char * (*Com)(cl_ati_t *atif, char const *in); // only some objects created by CL_WorldSpring use this
	void	(*UnpackEvent)(cl_ati_t *atif, unique_t id, eventType type, byte_iter_t *bi);
	void	(*Run)(cl_ati_t *atif);
} cl_ati_ftbl_t;

//
// tmp object stuff
//

typedef enum
{
	explosionStyle_hot1,
	explosionStyle_smoke1,
	explosionStyle_blood1,
	explosionStyle_teleport_send,
	explosionStyle_teleport_recv,
	explosionStyle_stardust,

	explosionStyle_num
} explosionStyle;

typedef enum
{
	debrisStyle_ham1,
	debrisStyle_tribalsmiliedead1,
	debrisStyle_tribalsmiliedead2,
	debrisStyle_tribalsmiliedead3,
	debrisStyle_tribalsmiliedead4,

	debrisStyle_num
} debrisStyle;

#endif 
