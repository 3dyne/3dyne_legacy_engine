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



// res_mp3.c

#include "interfaces.h"
#include "g_shared.h"

#include "res_mp3.h"
#include "res_mp3defs.h"

res_mp3_register_t * Res_SetupRegisterMp3( char *path )
{
	res_mp3_register_t	*reg;

	int size;

	size = strlen( path ) + 1;

	reg = (res_mp3_register_t *)NEWBYTES( size );
	strcpy( reg->path, path );
	
	return reg;
}

g_resource_t *Res_RegisterMp3( hobj_t *resobj )
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
	
	r->res_register = Res_SetupRegisterMp3( path->value );
	r->state = G_RESOURCE_STATE_REGISTERED;
	
	return r;
}

void Res_UnregisterMp3( g_resource_t *res )
{
	__named_message( "do nothing\n" );
}

res_mp3_cache_t *Res_CacheInMp3( res_mp3_register_t *reg )
{
	res_mp3_cache_t	*c;
	ib_file_t	*h;

	int size;

	h = IB_Open( reg->path );

	if( !h )
		__error( "cannot load %s\n", reg->path );

	size = IB_GetSize( h );
	
	c = ( res_mp3_cache_t * )MM_Malloc( sizeof( res_mp3_cache_t ));
	__chkptr( c );

	c->size = size;
	
	c->data = (unsigned char *)MM_Malloc( size ); // so!
	
	IB_Read( c->data, size, 1, h );
	

	IB_Close( h );

	return c;
}

void Res_CacheMp3( g_resource_t *res )
{
	res_mp3_register_t	*reg;

	if ( res->state != G_RESOURCE_STATE_REGISTERED )
		return;

	reg = ( res_mp3_register_t * ) res->res_register;

	res->res_cache = Res_CacheInMp3( reg );
	res->state = G_RESOURCE_STATE_CACHED;
}

void Res_UncacheMp3( g_resource_t *res )
{
	__named_message( "doing nothing.\n" );
}
