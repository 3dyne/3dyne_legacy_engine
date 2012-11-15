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



// g_resources.h

#ifndef __g_resources
#define __g_resources

#include "g_resourcesdefs.h"

g_resources_t * G_NewResources( void );
void G_FreeResources( g_resources_t *res );
void G_ResourcesDump( g_resources_t *res );
void G_ResourceDump( g_resource_t *res );

void G_ResourceTypeRegister( g_resources_t *rp, const char *type_name,
			     g_resource_t * (*register_func)(hobj_t *resobj),
			     void (*unregister_func)(g_resource_t *res),
			     void (*chach_func)(g_resource_t *res),
			     void (*uncache_func)(g_resource_t *res) );

g_resource_t * G_NewResource( char *res_name, char *res_type );

void G_ResourceFromResObj( g_resources_t *res, hobj_t *cls );
void G_ResourceFromClass( g_resources_t *res, const char *name );

bool_t G_ResourceCheck( g_resources_t *res, char *name );
g_resource_t * G_ResourceSearch( g_resources_t *res, const char *name );
g_resource_t * G_ResourceAttach( g_resources_t *res, char *name, char *user_name, int (*user_callback_func)(g_resource_t *r, int reason) );
void G_ResourceDetach( g_resources_t *res, char *name, char *user_name );

void G_CallResourceUser( g_res_user_t *ru, g_resource_t *r, int reason );
void G_CallResourceUsers( g_resource_t *r, int reason );

void G_ResourcesForEach( g_resources_t *res, void (*action_func)(g_resource_t *r) );

#endif
