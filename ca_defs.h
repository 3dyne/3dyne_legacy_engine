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



// ca_defs.h

#include "sys_defs.h"
#ifndef __ca_defs
#define __ca_defs

#define CA_ENTRYNUM	( 2200 )
#define MM_MAXDUMPSIZE	( 1000000 )

#define MM_ZONENUM	( 4 )
//#define ZONE_CACHE	( 0 )
//#define ZONE_LIFE	( 1 )
//#define ZONE_STATIC	( 2 )
//#define ZONE_DYNAMIC	( 3 )

#define CA_LISTNUM	( 4 )

#define CA_WAVE		( 0 )
#define CA_ARR		( 1 )
#define CA_LUMP		( 2 )
#define CA_TGA		( 3 )

// ca_arr

#define CA_ARR_P8	( 1 ) // obsolete
#define CA_ARR_RGB565	( 2 )

#define ARR_HEADERSIZE	( 48 )

typedef struct {
	u_int16_t	width;
	u_int16_t	height;
	u_int16_t	mipmap;
	u_int16_t	steps;
	u_int32_t	bytes;
	u_int32_t	flags;
}	ca_arr_t;

// ca_lump

typedef struct { 
	u_int32_t size;
} ca_lump_t;

// ca_wave

typedef struct {
	u_int32_t	bytes;
}	ca_wave_t;

// ca_tga

typedef struct ca_tga_s
{
	int	width;
	int	height;
	int	bpp;

	// RGB
	// ARGB
	unsigned char	image[16];	// variable sized
} ca_tga_t;

#endif
