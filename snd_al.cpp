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



// snd_al.c

#include "interfaces.h"
#include <AL/al.h>
#include <AL/alc.h>

#define SND_MAX_SRCNUM	( 16 )


typedef struct {
	int	inuse;
	ALuint	al_source; // every snd_source_t has its own al_source for ever.
	int	id;
	int	pri;
} snd_source_t;

static snd_source_t	srcs[SND_MAX_SRCNUM];

static void	*alc_ctx;
vec3d_t	snd_origin;
vec3d_t snd_view;

static int	sst_isup = 0;
static ALsizei	al_srcnum;
static unsigned int snd_num;

static ALfloat zeroes[] = { 0.0f, 0.0f,  0.0f };                               
static ALfloat back[]   = { 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f };             
static ALfloat front[]  = { 0.0f, 0.0f,  1.0f, 0.0f, 1.0f, 0.0f };

void SND_StartUp()
{
	ALuint	sources[SND_MAX_SRCNUM];

	int	i;
/*	
	int	attr[] = {
		ALC_FREQUENCY,
		22050,
		0
	};
*/
	if( sst_isup )
	{
		__warning( "sound is already up\n" );
		return;
	}

	if( !((sh_var_t *)SHP_GetVar( "sound" ))->ivalue )
	{
		__named_message( "\tsound disabled\n" );
		sst_isup = 0;
		return;
	}

	Vec3dInit( snd_origin, 0.0, 0.0, 0.0 );
	

	alc_ctx = alcCreateContext( NULL );
	alcMakeContextCurrent( alc_ctx );

// init listener
	alListenerfv( AL_POSITION, zeroes );
	alListenerfv( AL_VELOCITY, zeroes );
	{
		alListenerfv( AL_ORIENTATION, front );
	}


// init sources
	memset( srcs, 0, SND_MAX_SRCNUM * sizeof( snd_source_t ));

	al_srcnum = alGenSources( SND_MAX_SRCNUM, sources );

	if( al_srcnum != SND_MAX_SRCNUM )
	{
		__warning( "got only %d sources\n", al_srcnum );
	}

	for( i = 0; i < al_srcnum; i++ )
	{
		srcs[i].al_source = sources[i];
		printf( "source %d: %d\n", i, srcs[i].al_source );

//		alSource3f( srcs[i].al_source, AL_POSITION, 0.0, 0.0, 0.0 );
		
	}



	alAttenuationScale( 0.7 );

	
	snd_num = 0;

	sst_isup = 1;


//	if( 1 )
	{


		ALuint sources[2];                                                                 
		ALuint boom, l2;

		ALsizei size;                                                           
		ALsizei bits;                                                           
		ALsizei freq;                                                           
		ALsizei format;
		void *wave = NULL;

		ALuint left = 0;
		ALuint right = 0;

		__named_message( "test...\n" );		

		if( alGenBuffers( 1, &boom ) != 1 ) {                                   
			fprintf( stderr, "aldemo: couldn't generate buffers\n" );           
			exit( 1 );                                                          
		}

		alutLoadWAV( "boom.wav", &wave, &format, &size, &bits, &freq);          
		
		alBufferData( boom, format, wave, size, freq );
		alSource3f( boom, AL_POSITION, -5.0, 0.0, 0.0 );                                   
		alSourcefv( boom, AL_VELOCITY, zeroes );                                           
		alSourcefv( boom, AL_ORIENTATION, back );                                          
		alSourcei( boom, AL_BUFFER, boom );                                                
		alSourcei( boom, AL_LOOPING, AL_FALSE ); 

		alSourcePlay( boom );

		alAttenuationScale(0.7);
	}



}

void SND_ShutDown()
{
}

/*
===================
 SND_AddJob( g_resource_t sound resource, type of sound (local, position), priority, origin )
 
 add ( positional ) sound job. lower priority lose if now channels left.
===================
*/

int SND_AddJob( g_resource_t	*res, int type, int pri, vec3d_t origin )
{
	res_sound_cache_t	*s;
	int	i;

	ALuint	src;

       
	if( !sst_isup )
	{
		return 0;
	}


	s = res->res_cache;
	if( !s )
	{
		__warning( "snd->res_cache == NULL\n" );
		return 0;
	}

	if( !pri )
	{
		__warning( "no priority. failed\n" );
		return 0;
	}
	

	// search for free source
	for( i = 0; i < al_srcnum; i++ )
	{
		if( !srcs[i].inuse )
			break;
	}

	if( i >= al_srcnum ) // none free => drop less priority sound
	{
		for( i = 0; i < al_srcnum; i++ )
		{
			if( srcs[i].pri < pri )
				break;
		}	
	}

	if( i >= al_srcnum ) // even none => drop equal priority
	{
		for( i = 0; i < al_srcnum; i++ )
		{
			if( srcs[i].pri == pri )
				break;
		}
	}

	if( i >= al_srcnum ) // even none => drop equal priority
	{
		__warning( "no free source\n" );
		return 0;
	}

	
	snd_num++; // must be > 0

	srcs[i].inuse = 1;
	srcs[i].id = snd_num;
	srcs[i].pri = pri;

	
	src = srcs[i].al_source;

//	alSourcefv( src, AL_POSITION, origin );
	alSource3f( src, AL_POSITION, -5.0, 0.0, 0.0 );
	alSourcefv( src, AL_VELOCITY, zeroes );
	alSourcefv( src, AL_ORIENTATION, back );
//	alSourcef( src, AL_GAIN, 1.0 );

#if 0 
	if( s->al_buf == 0 )
	{
		__error( "al_buf == 0, %s\n", (char * )res->res_register );
	}
#endif

//	SHV_Printf( "playing %d\n", s->al_buf );

	alSourcei( src, AL_BUFFER, s->al_buf );
	alSourcei( src, AL_LOOPING, AL_FALSE );

	alSourcePlay( src );

	return snd_num;
}
	

int SND_AddJobByName( char *name, int type, int pri, vec3d_t origin )
{
	int rc;
	g_resource_t	*res;

	if( !sst_isup )
		return 0;

	if( !g_rs )
		return 0;

	res = G_ResourceSearch( g_rs, name );
	rc = SND_AddJob( res, type, pri, origin );

	return rc;
}	

int SND_JobIsActive( int id )
{
	int	i;

	if( !sst_isup )
		return 0;
	
	for( i = 0; i < al_srcnum; i++ )
	{
		if( !srcs[i].inuse )
			continue;
		
		if( srcs[i].id == id )
			return 1;
	}
	
	return 0;
}

void SND_JobChangeSound( int id, g_resource_t *res )
{
	int	i;

	if( !sst_isup )
		return;
	
	for( i = 0; i < al_srcnum; i++ )
	{
		if( !srcs[i].inuse )
			continue;
		
		if( srcs[i].id == id )
		{
#if 0 // TODO: implement for openal
			if( res )
			{
				res_sound_cache_t	*s;
				s = res->res_cache;
				channels[i].data = ( short * )s->sound;
				channels[i].size = s->size / 2;
			}
			channels[i].lvol = snd_sfxvol->fvalue;
			channels[i].rvol = snd_sfxvol->fvalue;
			channels[i].pointer = 0;
			channels[i].firsttime = 1;
#endif
		}
	}
}


void SND_StopAllJobs()
{
	int	i;
	
	if( !sst_isup )
		return;
	
	for( i = 0; i < al_srcnum; i++ )
	{
		if( srcs[i].inuse )
		{
			alSourceStop( srcs[i].al_source );
		}
		
		srcs[i].inuse = 0;
	}
}

void SND_Update()
{
	int	i;

// check for finished sounds 
	for( i = 0; i < al_srcnum; i++ )
	{
		if( srcs[i].inuse )
		{
			SHV_Printf( "inuse: %d\n", i );
			if( alSourceIsPlaying( srcs[i].al_source ) == AL_FALSE ) // sound ended, so free the source;
			{
				srcs[i].inuse = 0;
			}
		}
	}
}
			

