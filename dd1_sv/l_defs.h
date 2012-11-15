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



// at_defs.h

#ifndef __at_defs
#define __at_defs

#include "lib_hobjdefs.h"
#include "lib_math.h"
#include "lib_queuedefs.h"
#include "u_defs.h"
#include "events.h"

typedef enum
{
	atiState_none,
	atiState_wakeup,
	atiState_run,
	atiState_sleep,
	atiState_dead,

	atiState_num
} atiState;

typedef enum
{
	atiType_none,
	atiType_player,

	atiType_num
} atiType;

typedef struct ati_s 
{	
	unique_t	id;
	atiType		type;
	atiState	state;
	hobj_t		*ati_obj;	// ati was created from this class

	int		frame_count;	// frames since create

	struct ati_s	*prev;		// in one of these lists:
	struct ati_s	*next;		// wakeup, run, sleep, dead
	
	struct ati_ftbl_s	*ftbl;
} ati_t;

typedef enum
{
	atiRun_think1,
	atiRun_think2,

	atiRun_num
} atiRun;

typedef enum
{
	atiP_none,
	atiP_origin3fv,		// vec3
	atiP_velocity3fv,	// vec3d
	atiP_is_ground,		// bool/int
	atiP_fragadd,		// int
	atiP_num
} atiP;

typedef struct ati_ftbl_s {
	void	(*Destroy)(ati_t *atif);
	const char * (*Com)(ati_t *atif, char const *in);
	void	(*UnpackEvent)(ati_t *atif, unique_t id, eventType type, byte_iter_t *bi );
	void	(*Run)(ati_t *atif, atiRun run );

	// Com was in vain ...
	void	(*Hurt)(ati_t *self, ati_t *other, vec3d_t kill_center, fp_t destruction);
	
	void	(*Seti)(ati_t *self, atiP param, int i );
	int	(*Geti)(ati_t *self, atiP param );
	void	(*Setf)(ati_t *self, atiP param, fp_t f );
	fp_t	(*Getf)(ati_t *self, atiP param );
	void	(*Set3fv)(ati_t *self, atiP param, fp_t fv[3] );
	void	(*Get3fv)(ati_t *self, atiP param, fp_t fv[3] );
} ati_ftbl_t;


typedef struct at_list_s
{
	ati_t		head;
	ati_t		tail;
} ati_list_t;

typedef enum {
	PlaneSlope_none,
	PlaneSlope_floor,
	PlaneSlope_slope,
	PlaneSlope_wall
} planeSlope;

typedef struct move_volume_s
{
	vec3d_t		from;
	vec3d_t		to;
	vec3d_t		rel_min;
	vec3d_t		rel_max;
} move_volume_t;


#endif
