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



// s_mem.h

#ifndef __s_mem
#define __s_mem

#include "s_config.h"

#include "stdlib.h"
#include "string.h"

#ifdef NEWTYPE
#undef NEWTYPE
#endif

#ifdef INITTYPE
#undef INITTYPE
#endif

#ifdef NEWBYTES
#undef NEWBYTES
#endif

#ifdef FREE
#undef FREE
#endif

#define NEWTYPE( x )	( (x *)(memset( (S_Malloc(sizeof(x)) ), 0, sizeof(x) ) ) )
#define INITTYPE( _addr, _type )	( (memset( _addr, 0, sizeof( _type ) ) ) )
#define NEWBYTES( x )	( (void *)(memset( (S_Malloc(x) ), 0, x ) ) )
#define FREE( x )	( S_Free( x ), x = NULL ) 

#define NEWTYPEARRAY( _type, _num ) ( (_type *)(memset( (S_Malloc( sizeof(_type)*_num), 0, sizeof(_type)*_num) ) ) )
#define INITTYPEARRAY( _addr, _type, _num ) ( (memset( _addr, 0, sizeof(_type)*_num ) ) )

#endif
