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



// a_shared.h
#ifndef __a_shared_h
#define __a_shared_h

#include "lib_math.h"
//
// hold flags
#define	AHF_FORWARD	1
#define AHF_BACK	2
#define AHF_TLEFT	4
#define AHF_TRIGHT	8
#define AHF_FIRE0	16
#define AHF_STRAFE	32
#define AHF_LOOKUP	64
#define AHF_LOOKDOWN	128
#define AHF_USE		256
#define AHF_JUMP		512
#define AHF_SLEFT	1024
#define AHF_SRIGHT	2048

//
// action flags
#define AAF_JUMP	1

//
// SH_SetVar flags
#define SH_SV_NONE	( 0 )
#define SH_SV_NOSET	( 1 ) // only set if not found
#define SH_SV_RDONLY	( 2 ) // make var readonly ( no avaliable )


typedef struct {
	unsigned int	hflags;  // hold flags
} a_pinput_t;

typedef struct {
	int		id;
	
//	vec3d_t		origin;
//	vec3d_t		motion;

	vec3d_t		dir;
	float		speed;

	float		roll, pitch, yaw;
	
	a_pinput_t	input;
} a_player_t;

typedef struct {
	char	key[32];
	char	string[64];
	unsigned int	ivalue;
	float		fvalue;
}	sh_var_t;	

#endif
