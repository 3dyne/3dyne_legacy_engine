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



// s_private.h

#ifndef s_private_h
#define s_private_h

// get the utils running in different environmets, that are:
// in game 
// in life
// without them

#if defined( S_INGAME ) // game functions

// in game
// ==============================

#include "interfaces.h"

#define S_Malloc	MM_Malloc
#define S_Free		MM_Free
#define S_Printf	SHV_Printf

#define NEWTYPE( x )	( (x *)(memset( (MM_Malloc(sizeof(x)) ), 0, sizeof(x) ) ) )
#define NEWBYTES( x )	( (void *)(memset( (MM_Malloc(x) ), 0, x ) ) )
#define FREE( x )	( MM_Free( x ) ) 

#elif defined( S_INLIFE ) // life functions

// in life
// ==============================

#include "l_locals.h"
#include "shock.h"
#define S_Malloc	li.MM_Malloc
#define S_Free		li.MM_Free
#define S_Printf	li.SH_Printf

#define NEWTYPE( x )	( (x *)(memset( (li.MM_Malloc(sizeof(x)) ), 0, sizeof(x) ) ) )
#define NEWBYTES( x )	( (void *)(memset( (li.MM_Malloc(x) ), 0, x ) ) )
#define FREE( x )	( li.MM_Free( x ) ) 

#elif defined( S_INSST )// sst functions
// ========================================
// in sound subsystem

#include "sst_locals.h"
#include "shock.h"

#define S_Malloc	si.MM_Malloc
#define S_Free		si.MM_Free
#define S_Printf	si.SH_Printf

#define NEWTYPE( x )	( (x *)(memset( (si.MM_Malloc(sizeof(x)) ), 0, sizeof(x) ) ) )
#define NEWBYTES( x )	( (void *)(memset( (si.MM_Malloc(x) ), 0, x ) ) )
#define FREE( x )	( si.MM_Free( x ) ) 


#else

// nothing defined, use libc
// ==============================
#include <stdlib.h>
#warning S_INLIFE or S_INGAME not set. using libc!

#define S_Malloc	malloc
#define S_Free		free
#define S_Printf	printf

#define NEWTYPE( x )	( (x *)(memset( (malloc(sizeof(x)) ), 0, sizeof(x) ) ) )
#define NEWBYTES( x )	( (void *)(memset( (malloc(x) ), 0, x ) ) )
#define FREE( x )	( free( x ) ) 


#endif  


#endif
