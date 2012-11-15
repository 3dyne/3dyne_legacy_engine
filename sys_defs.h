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



// sys_defs.h

// primitive types ( bittypes )

#ifndef sys_defs_h
#define sys_defs_h

#include <stdio.h> // FILE* not for interfaces!

#if !defined __GNUC__
#define __inline__
#endif

#if defined(linux_i386) || defined (irix_mips)
#include <sys/types.h> // for bittypes
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#define APIENTRY
#define FOPEN_READ	"r"
#define FOPEN_WRITE	"w"

#elif defined(win32_x86)
#include <fcntl.h>


typedef	int	int32_t;
typedef	unsigned int	u_int32_t;
typedef short	int16_t;
typedef	unsigned short	u_int16_t;
#if !defined (win32_x86)
typedef char	int8_t;
typedef	unsigned char	u_int8_t;
#endif

#define M_PI            3.14159265358979323846  /* pi */


#define FOPEN_READ	"rb"
#define FOPEN_WRITE	"wb"

#endif

#define SYS_DLLSSYSTEM	( 1 ) 

/* time */

typedef struct timeval timeval_t;
typedef long usec_t;
typedef unsigned long sec_t;


typedef int 	gsbool;

#endif
