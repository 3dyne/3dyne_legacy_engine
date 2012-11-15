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



// u_defs.h

#ifndef __u_defs
#define __u_defs
#include <stddef.h>

#define	U_PACKED_ASC		( 1 )
#define U_PACKED_BIN		( 2 )
#define U_PACKED_BIG		( 4 )
#define U_PACKED_LITTLE		( 8 )


#define UNIQUE_INVALIDE		( 0x0 )
#define UNIQUE_ALL		( 0xfffffffe )
#define UNIQUE_OVERFLOW		( 0xffffffff )

typedef unsigned int	unique_t;

typedef unsigned int	msec_t;

typedef unsigned int	size4_t;
typedef unsigned int	pos4_t;
typedef unsigned int	flag_t;

#define C_R		( 0 )
#define C_G		( 1 )
#define C_B		( 2 )
#define C_A		( 3 )
typedef unsigned char	color_rgba_t[4];	// for glColor4ubv

/*******************************
 !!! WARNING !!! 				

 set maddr_t to the right 
 machine address with
 ******************************/

//typedef unsigned int	maddr_t;
typedef size_t    maddr_t;




#endif
