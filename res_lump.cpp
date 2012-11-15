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



// res_lump.c

#include "interfaces.h"
#include "g_shared.h"

#include "res_lump.h"
#include "res_lumpdefs.h"

res_lump_register_t * Res_SetupRegisterLUMP( char *path )
{
	res_lump_register_t	*reg;
	size_t			size;

	size = strlen( path ) + 1;

	reg = (res_lump_register_t *)NEWBYTES( size );
	strcpy( reg->path, path );

	return reg;
}

res_lump_cache_t * Res_CacheInLUMP( res_lump_register_t *reg )
{
	ib_file_t	*h;
	int		filesize;
	size_t			size;
	res_lump_cache_t	*lump;

	h = IB_Open( reg->path );
	if ( !h )
		__error( "load of '%s' failed\n", reg->path );

	filesize = IB_GetSize( h );

	size = (size_t)&(((res_lump_cache_t *)0)->data[filesize] );
	lump = (res_lump_cache_t *) NEWBYTES( size );

	lump->size = filesize;
	IB_Read( lump->data, filesize, 1, h );
	IB_Close( h );
	
	return lump;
}


g_resource_t * Res_RegisterLUMP( hobj_t *resobj )
{
	g_resource_t	*r;
	hpair_t		*name;
	hpair_t		*path;

	name = FindHPair( resobj, "name" );
	if ( !name )
		return NULL;

	path = FindHPair( resobj, "path" );
	if ( !path )
		return NULL;

	r = G_NewResource( name->value, resobj->name );

	r->res_register = Res_SetupRegisterLUMP( path->value );
	r->state = G_RESOURCE_STATE_REGISTERED;

	return r;
}

void Res_UnregisterLUMP( g_resource_t *res )
{
	__named_message( "shame! not impl\n" );
}

void Res_CacheLUMP( g_resource_t *res )
{
	res_lump_register_t	*res_register;

	if ( res->state != G_RESOURCE_STATE_REGISTERED )
		return;

	res_register = (res_lump_register_t *) res->res_register;

	res->res_cache = Res_CacheInLUMP( res_register );
	res->state = G_RESOURCE_STATE_CACHED;
}

void Res_UncacheLUMP( g_resource_t *res )
{
	__named_message( "shame! not impl\n" );
}
