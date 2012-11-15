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



// sst_sdl.c

#if 1
#include <SDL/SDL.h>
#include "interfaces.h"
#include "snd_intern.h"

#define SST_PAINTBUFSIZE ( 4096 )


static sst_spec_t	*myspec;
static int	oldframes = 0;
static int	sdl_paused;
static sh_var_t	*sound;
// static snd_channel_t	*channels;
static int isup;

extern void FillAudio( void *udata, Uint8 *stream, int size )
{
	samplepair_t	paintbuf[SST_PAINTBUFSIZE];
	int	frames;
//	unsigned char	*buf;
	int		pos, i;
	
//	__named_message( "\n" );

	frames = size / 4;
//	printf( "frames: %d\n", frames );

	if( frames >= SST_PAINTBUFSIZE )
	{
		__warning( "too much data\n" );
		return;
	}

#if 1
	for( i = 0; i < frames; i++ )
	{
		paintbuf[i].left = paintbuf[i].right = 0;
	}


	SND_PaintStereo16( paintbuf, myspec, frames, oldframes );
	oldframes = frames;

	pos = 0;

	MUS_PaintStereo16( paintbuf, frames );

	for( i = 0; i < frames; i++ )
	{
		int left, right;

		left  = paintbuf[i].left;
		right = paintbuf[i].right;


		if( left > 0x7fff )
			left = 0x7fff;
		else if( left < (short)0x8000 )
			left = (short)0x8000;
		
		if( right > 0x7fff )
			right = 0x7fff;
		else if( right < (short)0x8000 )
			right = (short)0x8000;

		*((short*)(&stream[pos])) = (short)left;
		*((short*)(&stream[pos+2])) = (short)right;

		pos += 4;
	}

#endif
//	__named_message( "%d\n", frames );
}


int SST_StartUp( sst_spec_t *spec )
{
	SDL_AudioSpec	o, p;
	

	isup = 0;
	__named_message( "\tSDL driver starting up ...\n" );

	if( SDL_Init( SDL_INIT_AUDIO ) < 0 )
	{
		__warning( "Couldn't initialize SDL: %s\n",SDL_GetError());
		return 1;
	}

	if( !spec )
	{
		__warning( "spec == NULL!\n" );
		return 1;
	}
	if( (!spec->ch) || (!spec->chnum) )
	{
		__warning( "self->ch == NULL or self->chnum == 0. failed\n" );
		return 1;
	}



	p.freq = 22050;
	p.format = AUDIO_S16LSB;
	p.channels = 2;
	p.samples = 1024;
	p.callback = FillAudio;
	p.userdata = NULL;

	myspec = spec;

	if( SDL_OpenAudio( &p, &o ) < 0 )
	{
		__warning( "SDL_OpenAudio failed: '%s'\n", SDL_GetError() );
		return 1;
	}

#if 1
	if( ( p.freq != o.freq ) || ( p.format != o.format ) || ( p.samples != o.samples )|| ( p.channels != o.channels ) )
	{

		__warning( "could not get desired audio specs. sound may be strange.\n" );
		printf( "%d %d %d %d\n", o.freq, o.format, o.samples, o.channels );

#if 0
	
		SDL_CloseAudio();
		
		if( SDL_OpenAudio( &p, NULL ) < 0 )
		{
			__warning( "2nd SDL_OpenAudio failed.\n" );
			return 1;
		}
#endif
	}

#endif

	sdl_paused = 1;

	isup = 1;

	sound = SHP_GetVar( "sound" );

	return 0;
}

void SST_ShutDown()
{
	if( !isup )
	{
		__warning( "sst is not up\n" );
		return;
	}

	SDL_CloseAudio();
}


void SST_Update( fp_t ftime )
{
	// nothing, case we are thread based.
	if( sdl_paused )
	{
		SDL_PauseAudio( 0 );
		sdl_paused = 0;
	}
}

void SST_Lock( void )
{
//	__named_message( "\n" );
	SDL_LockAudio();
}

void SST_Unlock( void )
{
//	__named_message( "\n" );
	SDL_UnlockAudio();
}
#else


int SST_StartUp( void *spec )
{
	printf( "dummy sound driver\n" );
}

void SST_ShutDown()
{
}


void SST_Update( float ftime )
{
}

void SST_Lock()
{
}

void SST_Unlock()
{
}
#endif
