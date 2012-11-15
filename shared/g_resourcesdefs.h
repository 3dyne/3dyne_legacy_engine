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



// g_resourcesdefs.h

#ifndef __g_resourcesdefs
#define __g_resourcesdefs

//#include "g_interfaces.h"
#include "lib_containerdefs.h"
#include "lib_hobjdefs.h"

typedef short	g_res_state_t;

typedef struct g_resources_s
{
	u_map_t		*res_map;

	u_map_t		*type_map;
} g_resources_t;

#define G_RESOURCE_STATE_NONE		( 0 )
#define G_RESOURCE_STATE_REGISTERED	( 1 )	// registered, but not cached
#define G_RESOURCE_STATE_CACHED		( 2 )
#define G_RESOURCE_STATE_UNREGISTERED	( 3 )	// ready for remove

// user callback resons
#define G_RESOURCE_REASON_NONE		( 0 )
#define G_RESOURCE_REASON_ATTACH_OK	( 1 )
#define G_RESOURCE_REASON_GET_UNAVAILABLE	( 2 )
#define G_RESOURCE_REASON_GET_AVAILABLE		( 3 )
#define G_RESOURCE_REASON_DETACH_OK	( 4 )

typedef struct g_resource_s
{
	g_res_state_t		state;

	void			*res_register;
	void			*res_cache;

	u_list_t		*user_list;

	unsigned char	type_ofs;
	unsigned char	name_ofs;
	char		text[32];	// variable sized for type and name
} g_resource_t;

typedef struct g_res_user_s
{
	int (*user_callback_func)(g_resource_t *r, int reason );
	char	user_name[32];		// variable sized
} g_res_user_t;


typedef struct g_res_type_s
{
	char		name[64];

	g_resource_t * (*register_func)(hobj_t *resobj);
	void (*unregister_func)(g_resource_t *res);
	void (*cache_func)(g_resource_t *res);
	void (*uncache_func)(g_resource_t *res);	
} g_res_type_t;


#endif
