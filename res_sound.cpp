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



// res_sound.c

#include "interfaces.h"
#include "g_shared.h"

#include "res_sound.h"
#include "res_sounddefs.h"

#ifdef DD_USE_OPENAL
#include <AL/al.h>
#include <AL/alut.h>
#endif

res_sound_register_t * Res_SetupRegisterSound( char *path )
{
	res_sound_register_t	*reg;
	int		size;

	size = strlen( path ) + 1;

	reg = (res_sound_register_t *)NEWBYTES( size );
	strcpy( reg->path, path );

	return reg;
}

g_resource_t * Res_RegisterSound( hobj_t *resobj )
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
	
	r->res_register = Res_SetupRegisterSound( path->value );
	r->state = G_RESOURCE_STATE_REGISTERED;

	return r;
}

void Res_UnregisterSound( g_resource_t *res )
{
	__named_message( "do nothing\n" );
}



res_sound_cache_t *Res_CacheInSound( res_sound_register_t *reg )
{
	res_sound_cache_t	*sound;
	ib_file_t	*h;
	int		filesize;
	char		*buf;
	int		speed;
	int		bytes;
	char		id[4];

	h = IB_Open( reg->path );
	if( !h )
		__error( "cannot load %s\n", reg->path );

	filesize = IB_GetSize( h );
	buf = (char *)alloca( filesize );
	if( !buf )
		__error( "alloca for '%s' failed\n" );

	IB_Read( buf, filesize, 1, h );
	IB_Close( h );

	// interpret buffer as f'ing wave

	sound = NULL;

	U_BeginUnpack( U_PACKED_BIN, buf, filesize );
	U_UnpackSkip( 8 );
	U_UnpackString( id, 4 );
	if( memcmp( id, "WAVE", 4 ))
	{
		__warning( "wrong id in '%s'\n", reg->path );
		return NULL;
	}
			   
	
	U_UnpackString( id, 4 );
	if( memcmp( id, "fmt ", 4 ))
	{
		__warning( "'%s' is not fmt / with comment.\n" );
		return NULL;
	}
	
	U_UnpackSkip( 8 ); // skip all. 
	U_Unpacks32( &speed );

	if( speed != 22050 )
	{
		__warning( "sampling speed of '%s' not 22050.\n" );
		return NULL;
	}

	U_UnpackSkip( 12 );
	
	U_Unpacks32( &bytes );

	
	sound = ( res_sound_cache_t * )MM_Malloc( sizeof( res_sound_cache_t ));
	__chkptr( sound );
	sound->sound = (unsigned char *)MM_Malloc( bytes ); // so!
	__chkptr( sound->sound );

	U_UnpackString( (char *)sound->sound, bytes );

	sound->size = bytes;
	U_EndUnpack();

	sound->al_buf = 0;
	

#ifdef DD_USE_OPENAL
#if 1
	if( alGenBuffers( 1, &sound->al_buf ) != 1 )
	{
		__error( "alGenBuffers failed\n" );
	}

	alBufferData( sound->al_buf, AL_FORMAT_MONO16, sound->sound, sound->size, 22050 );

	SHV_Printf( "%s: %d\n", reg->path, sound->al_buf );
	
	MM_Free( sound->sound );
	sound->sound = NULL;
	sound->size = 0;
#else
	{
		ALsizei size;                                                           
		ALsizei bits;                                                           
		ALsizei freq;                                                           
		ALsizei format;
		void *wave = NULL;

		SHV_Printf( "%s: %d\n", reg->path, sound->al_buf );

		if( alGenBuffers( 1, &sound->al_buf ) != 1 )
		{
			__error( "alGenBuffers failed\n" );
		}
		
		alutLoadWAV( "boom.wav", &wave, &format, &size, &bits, &freq);          
		alBufferData( sound->al_buf, format, wave, size, freq );
	}
#endif
#endif
	return sound;
}

void Res_CacheSound( g_resource_t *res )
{
	res_sound_register_t	*reg;

	if ( res->state != G_RESOURCE_STATE_REGISTERED )
		return;

	reg = ( res_sound_register_t * ) res->res_register;

	res->res_cache = Res_CacheInSound( reg );
	res->state = G_RESOURCE_STATE_CACHED;
}

void Res_UncacheSound( g_resource_t *res )
{
	__named_message( "doing nothing.\n" );
}
