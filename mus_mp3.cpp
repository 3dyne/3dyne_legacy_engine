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



// mus_mp3.c

#include "interfaces.h"

//#include "mpglib/mpg123.h"
//#include "mpglib/mpglib.h"

#define MUS_FIFOSIZE	( 256 * 256 )

static int mus_uptr, mus_size;

static int mus_new = 0;
static res_mp3_cache_t	*mus_cache;

//static struct mpstr	mus_mp;

static FILE	*dh;

static char	mus_fifo[MUS_FIFOSIZE];
static unsigned int	fr, fw;

static int	mp3_hasdata;

void MUS_FifoInit()
{
	fr = fw = 0;
}

int MUS_FifoValidSize()
{
	unsigned int	v;

	if( fr > fw )
	{
		__warning( "fr > fw. flatting fifo ...\n" );
		MUS_FifoInit();
		return 0;
	}

	v = fw - fr;
	return v;
}

void MUS_FifoWrite( char *buf, int size )
{
	int	v, f;
    int	i;
	
	if( fr > fw )
	{
		__warning( "fr > fw. flatting fifo ...\n" );
		MUS_FifoInit();
		return ;
	}

	v = MUS_FifoValidSize();
	
	f = MUS_FIFOSIZE - v;

	printf( "free in fifo: %d\n", f );

	if( size > f )
	{
		__warning( "fifo overrun\n" );
		return;
	}
	
	
	for( i = 0; i < size; i++ )
	{
		mus_fifo[(i + fw)%MUS_FIFOSIZE] = buf[i];
	}

	fw += size;
	printf( "fw: %d\n", fw );
}

void MUS_FifoRead( char *buf, int size )
{
	int	v;
	int	i;

	v = MUS_FifoValidSize();

	if( size > v )
	{
		__warning( "fifo underrun\n" );
		return;

	}
	
	for( i = 0; i < size; i++ )
	{
		buf[i] = mus_fifo[(i+fw)%MUS_FIFOSIZE];
	}

	fr += size;
}


void MUS_StartUp()
{
	mus_cache = NULL;
	
	//	InitMP3( &mus_mp );

}

void MUS_ShutDown()
{
  //	ExitMP3( &mus_mp );
}


	
int MUS_Mp3Unpack( char *buf, int size )
{
	int	rsize;

	if( !mus_cache )
	{
		return 0;
	}

	if(  mus_cache->size - mus_uptr < size )
	{
		rsize = mus_cache->size - mus_uptr;
	} else
		rsize = size;

	fprintf( stderr, "rsize: %d\n", rsize );

	memcpy( buf, &mus_cache->data[mus_uptr], rsize );
	

//	fwrite( buf, rsize, 1, dh );

	fprintf( stderr, "ready.\n" );

	mus_uptr += rsize;
	return rsize;
}
		
void MUS_PlayByName( const char *name )
{
	char	bla[8192];

	g_resource_t	*res;
	

	MUS_FifoInit();

	//	ExitMP3( &mus_mp );
	//InitMP3( &mus_mp );

	res = G_ResourceSearch( g_rs, name );
	mus_cache = (res_mp3_cache_t*)res->res_cache;

	mus_new = 1;
	mus_uptr = 0;
	mus_size = 0;
	memset( bla, 0, 8192 );
	dh = fopen( name, "wb" );

	mp3_hasdata = 0;

}


void MUS_PaintStereo16( samplepair_t *pb, int frames )
{
#if 0
	int	i;
	int ret;
	char	out[8192], obuf[8192], buf[16384];
	
	int	size;
	int	isize;
	
		
	printf( "======\n" );

	if( frames != 2048 )
	{
		__warning( "framenum != 2048\n" );
		return;
	}

	size = frames * 4;

	for( ;; )
	{
		int	v;

		v = MUS_FifoValidSize();

		printf( "valid in fifo: %d\n", v );

		

		if( v >= size )
		{
			printf( "corping fifo\n" );
			MUS_FifoRead( out, 8192 );
			
			printf( "valid in fifo: %d\n", MUS_FifoValidSize() );
			break;
		} else
		{
//			printf( "decoding ...\n" );
			
			if( !mp3_hasdata )
			{
				
				printf( "first write\n" );
				isize = MUS_Mp3Unpack( buf, 16384 );
				printf( "isize: %d\n", isize );
				ret = decodeMP3( &mus_mp, buf, isize, obuf, 8192, &mus_size );
				
				printf( "mus_size: %d\n", mus_size );

				MUS_FifoWrite( obuf, mus_size );
				mp3_hasdata = 1;
			} else 
			{
				ret = decodeMP3( &mus_mp, NULL, 0, obuf, 8192, &mus_size );
				if( ret != MP3_OK )
				{
					fprintf( stderr, "mp3 needs data\n" );
					isize = MUS_Mp3Unpack( buf, 16384 );
					
					fprintf( stderr, "isize: %d ===========================\n", isize );
					ret = decodeMP3( &mus_mp, buf, isize, obuf, 8192, &mus_size );
				}

				MUS_FifoWrite( obuf, mus_size );
			}
		}

	}


	printf( "mus_size: %d\n", size );

fwrite( out, size, 1, dh );
 
#if 1	
	for( i = 0; i < frames; i++ )
	{
		pb[i].right += ( short )(out[i*2]);
//		out = *(out[2]);
		pb[i].left += ( short )(out[i*2 + 2]);
//		out = *out[2];
	}
#endif
#endif
}
	
