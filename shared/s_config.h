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



// s_config.h

#ifndef __s_config
#define __s_config

// get the utils running in different environmets, that are:
// in game 
// in life
// without them

#if defined( S_INGAME )

// in game
// ==============================

//#include "interfaces.h"
#include "ca_service.h"
#include "g_console.h"

#define cprintf	SHV_Printf

#define S_Malloc	MM_Malloc
#define S_Free		MM_Free
#define S_Printf	SHV_Printf

#ifndef trace_functions
#define TFUNC_ENTER
#define TFUNC_LEAVE
#else
#define TFUNC_ENTER	TF_Enter( __PRETTY_FUNCTION__ )
#define TFUNC_LEAVE	TF_Leave( __PRETTY_FUNCTION__ )
#endif

#elif defined( S_INLIFE )

// in life
// ==============================

#include "l_locals.h"
#include "shock.h"

#define S_Malloc	li.MM_Malloc
#define S_Free		li.MM_Free
#define S_Printf	li.SH_Printf

#elif defined( S_INSERVER )

// in server
// ==============================

#include "server.h"
//#include "shock.h"
//#include "lib_math.h"

#define cprintf	g_api.Printf

#define S_Malloc	g_api.Malloc
#define S_Free		g_api.Free
#define S_Printf	g_api.Printf

#ifndef trace_functions
#define TFUNC_ENTER
#define TFUNC_LEAVE
#else
#define TFUNC_ENTER	g_api.TF_Enter( "sv:"__PRETTY_FUNCTION__ )
#define TFUNC_LEAVE	g_api.TF_Leave( "sv:"__PRETTY_FUNCTION__ )
#endif

#elif defined( S_INCLIENT )

// in client
// ==============================

#include "client.h"
//#include "shock.h"
//#include "lib_math.h"

#define cprintf	g_api.Printf

#define S_Malloc	g_api.Malloc
#define S_Free		g_api.Free
#define S_Printf	g_api.Printf

#ifndef trace_functions
#define TFUNC_ENTER
#define TFUNC_LEAVE
#else
#define TFUNC_ENTER	g_api.TF_Enter( "cl:"__PRETTY_FUNCTION__ )
#define TFUNC_LEAVE	g_api.TF_Leave( "cl:"__PRETTY_FUNCTION__ )
#endif

#elif defined( S_INSST )
// ========================================
// in sound subsystem

#include "sst_locals.h"
#include "shock.h"

#define S_Malloc	si.MM_Malloc
#define S_Free		si.MM_Free
#define S_Printf	si.SH_Printf

#else

// nothing defined, use libc
// ==============================
#include <stdlib.h>
#warning S_INLIFE or S_INGAME not set. using libc!

#define S_Malloc	malloc
#define S_Free		free
#define S_Printf	printf
#define TFUNC_ENTER
#define TFUNC_LEAVE

#endif  

#endif
