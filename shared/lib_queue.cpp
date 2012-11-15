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



// lib_queue.c

#include "lib_queue.h"
#include "s_mem.h"

#include "shock.h"

/*
  ==================================================
  byte queue

  ==================================================
*/

/*
  ==============================
  ByteQueue_Init

  ==============================
*/
void ByteQueue_Init( byte_queue_t *q, char *queue_base, int queue_size )
{
	INITTYPE( q, byte_queue_t );

	q->base = queue_base;
	q->size = queue_size;

	q->put_ofs = 0;
	q->ghost_put_ofs = 0;
	q->get_ofs = 0;
	q->num_byte = 0;
}

/*
  ==============================
  ByteQueue_CanPut

  ==============================
*/
int ByteQueue_CanPut( byte_queue_t *q )
{	
//	if ( q->num_byte < q->size )
	return ((q->size-1)-q->num_byte);
//	return 0;
}

/*
  ==============================
  ByteQueue_CanGet

  ==============================
*/
int ByteQueue_CanGet( byte_queue_t *q )
{
	return q->num_byte;
}

/*
  ==============================
  ByteQueue_Put

  ==============================
*/
void ByteQueue_Put( byte_queue_t *q, char c )
{
	if ( q->num_byte >= q->size )
		__warning( "buffer overflow\n" );

	q->base[q->put_ofs] = c;
	q->num_byte++;
	q->put_ofs++;
	if ( q->put_ofs >= q->size )
		q->put_ofs = 0;
}

/*
  ==============================
  ByteQueue_Get

  ==============================
*/
char ByteQueue_Get( byte_queue_t *q )
{
	char		c;

	if ( q->num_byte < 0 )
		__warning( "buffer underrun\n" );

	c = q->base[q->get_ofs];
	q->num_byte--;
	q->get_ofs++;
	if ( q->get_ofs >= q->size )
		q->get_ofs = 0;

	return c;
}

/*
  ==============================
  ByteQueue_GetPutOfs

  ==============================
*/
int ByteQueue_GetPutOfs( byte_queue_t *q )
{
	return q->put_ofs;
}

/*
  ==============================
  ByteQueue_SetGhostPutOfs

  ==============================
*/
void ByteQueue_SetGhostPutOfs( byte_queue_t *q, int ofs )
{
	q->ghost_put_ofs = ofs;
}

/*
  ==============================
  ByteQueue_GhostPut

  ==============================
*/
void ByteQueue_GhostPut( byte_queue_t *q, char c )
{
	q->base[q->ghost_put_ofs] = c;
	q->ghost_put_ofs++;
	if ( q->ghost_put_ofs >= q->size )
		q->ghost_put_ofs = 0;	
}


/*
  ==================================================
  event queue

  ==================================================
*/

/*
  ==============================
  EventQueue_BeginPack

  ==============================
*/
void EventQueue_Init( event_queue_t *q, char *buf_ptr, int buf_size )
{
	ByteQueue_Init( &q->bq, buf_ptr, buf_size );	

	q->num_event = 0;
}

/*
  ==============================
  EventQueue_CanPut

  ==============================
*/
int EventQueue_CanPut( event_queue_t *q )
{
	return 0;
}

/*
  ==============================
  EventQueue_CanGet

  ==============================
*/
int EventQueue_CanGet( event_queue_t *q )
{
//	return ByteQueue_CanGet( &q->bq );
	return q->num_event;
}

/*
  ==============================
  EventQueue_BeginPack

  ==============================
*/
void EventQueue_BeginPack( event_queue_t *q )
{
	char		b = 0;

	// reserve 'int' for event size
	q->ofs_current = ByteQueue_GetPutOfs( &q->bq );
	q->size_current = 0;
	
	ByteQueue_Put( &q->bq, b );
	ByteQueue_Put( &q->bq, b );
	ByteQueue_Put( &q->bq, b );
	ByteQueue_Put( &q->bq, b );
}

/*
  ==============================
  EventQueue_EndPack

  ==============================
*/
int  EventQueue_EndPack( event_queue_t *q )
{
	// write event size as 'int' before the event into the reserved space
	ByteQueue_SetGhostPutOfs( &q->bq, q->ofs_current );
	{
		char	b[4];

		memcpy( b, &q->size_current, 4 );
		ByteQueue_GhostPut( &q->bq, b[0] );
		ByteQueue_GhostPut( &q->bq, b[1] );	
		ByteQueue_GhostPut( &q->bq, b[2] );
		ByteQueue_GhostPut( &q->bq, b[3] );	
	}
	q->num_event++;

	return 0;
}

/*
  ==============================
  EventQueue_Packp

  ==============================
*/
void EventQueue_Packp( event_queue_t *q, maddr_t ptr )
{

	
	if ( sizeof( maddr_t ) == 4 )
	{
		char	b[4];

		memcpy( b, &ptr, 4 );
		ByteQueue_Put( &q->bq, b[0] );
		ByteQueue_Put( &q->bq, b[1] );	
		ByteQueue_Put( &q->bq, b[2] );
		ByteQueue_Put( &q->bq, b[3] );	
		q->size_current += 4;
	}
	else if ( sizeof( maddr_t ) == 8 )
	{
		char	b[8];

		memcpy( b, &ptr, 8 );
		ByteQueue_Put( &q->bq, b[0] );
		ByteQueue_Put( &q->bq, b[1] );	
		ByteQueue_Put( &q->bq, b[2] );
		ByteQueue_Put( &q->bq, b[3] );	
		ByteQueue_Put( &q->bq, b[4] );
		ByteQueue_Put( &q->bq, b[5] );	
		ByteQueue_Put( &q->bq, b[6] );
		ByteQueue_Put( &q->bq, b[7] );	
		q->size_current += 8;
	}
	else
	{
		__error( "unsupported pointer size\n" );
	}
}

/*
  ==============================
  EventQueue_Packb
  
  ==============================
*/
void EventQueue_Packb( event_queue_t *q, char b )
{
	ByteQueue_Put( &q->bq, b );
	q->size_current++;
}

/*
  ==============================
  EventQueue_Packs

  ==============================
*/
void EventQueue_Packs( event_queue_t *q, short s )
{
	char	b[2];

	memcpy( b, &s, 2 );
	ByteQueue_Put( &q->bq, b[0] );
	ByteQueue_Put( &q->bq, b[1] );
	q->size_current += 2;
}

/*
  ==============================
  EventQueue_Packi

  ==============================
*/
void EventQueue_Packi( event_queue_t *q, int i )
{
	char	b[4];

	memcpy( b, &i, 4 );
	ByteQueue_Put( &q->bq, b[0] );
	ByteQueue_Put( &q->bq, b[1] );	
	ByteQueue_Put( &q->bq, b[2] );
	ByteQueue_Put( &q->bq, b[3] );	
	q->size_current += 4;
}

/*
  ==============================
  EventQueue_Packf

  ==============================
*/
void EventQueue_Packf( event_queue_t *q, fp_t f )
{
	char	b[4];

	memcpy( b, &f, 4 );
	ByteQueue_Put( &q->bq, b[0] );
	ByteQueue_Put( &q->bq, b[1] );	
	ByteQueue_Put( &q->bq, b[2] );
	ByteQueue_Put( &q->bq, b[3] );		
	q->size_current += 4;
}

/*
  ==============================
  EventQueue_Pack2fv

  ==============================
*/
void EventQueue_Pack2fv( event_queue_t *q, fp_t f[2] )
{
	char	b[8];

	memcpy( b, f, 8 );
	ByteQueue_Put( &q->bq, b[0] );
	ByteQueue_Put( &q->bq, b[1] );	
	ByteQueue_Put( &q->bq, b[2] );
	ByteQueue_Put( &q->bq, b[3] );		
	ByteQueue_Put( &q->bq, b[4] );
	ByteQueue_Put( &q->bq, b[5] );	
	ByteQueue_Put( &q->bq, b[6] );
	ByteQueue_Put( &q->bq, b[7] );		
	q->size_current += 8;
}

/*
  ==============================
  EventQueue_Pack3fv
  
  ==============================
*/
void EventQueue_Pack3fv( event_queue_t *q, fp_t f[3] )
{
	char	b[12];

	memcpy( b, f, 12 );
	ByteQueue_Put( &q->bq, b[0] );
	ByteQueue_Put( &q->bq, b[1] );	
	ByteQueue_Put( &q->bq, b[2] );
	ByteQueue_Put( &q->bq, b[3] );		
	ByteQueue_Put( &q->bq, b[4] );
	ByteQueue_Put( &q->bq, b[5] );	
	ByteQueue_Put( &q->bq, b[6] );
	ByteQueue_Put( &q->bq, b[7] );		
	ByteQueue_Put( &q->bq, b[8] );
	ByteQueue_Put( &q->bq, b[9] );	
	ByteQueue_Put( &q->bq, b[10] );
	ByteQueue_Put( &q->bq, b[11] );		
	q->size_current += 12;
}

/*
  ==============================
  EventQueue_BeginUnpack

  ==============================
*/
int EventQueue_BeginUnpack( event_queue_t *q )		// return size of event start to unpack
{
	if ( q->num_event < 0 )
	{
		__warning( "event queue under run\n" );
	}

	{
		char		b[4];
		
		b[0] = ByteQueue_Get( &q->bq );
		b[1] = ByteQueue_Get( &q->bq );
		b[2] = ByteQueue_Get( &q->bq );
		b[3] = ByteQueue_Get( &q->bq );
		
		memcpy( &q->size_current, b, 4 );
	}

	return q->size_current;
}

/*
  ==============================
  EventQueue_EndUnpack

  ==============================
*/
int EventQueue_EndUnpack( event_queue_t *q )		// return unpacked bytes
{
	if ( q->size_current > 0 )
	{
		__warning( "end unpack before event's logical end\n" );
	}
	
	q->num_event--;

	return 0;
}

/*
  ==============================
  EventQueue_Packp

  ==============================
*/
maddr_t EventQueue_Unpackp( event_queue_t *q )
{
	maddr_t		ptr;

	if ( sizeof( maddr_t ) == 4 )
	{
		char	b[4];


		b[0] = ByteQueue_Get( &q->bq );
		b[1] = ByteQueue_Get( &q->bq );	
		b[2] = ByteQueue_Get( &q->bq );
		b[3] = ByteQueue_Get( &q->bq );	
		q->size_current -= 4;
		memcpy( &ptr, b, 4 );
	}
	else if ( sizeof( maddr_t ) == 8 )
	{
		char	b[8];

		b[0] = ByteQueue_Get( &q->bq );
		b[1] = ByteQueue_Get( &q->bq );	
		b[2] = ByteQueue_Get( &q->bq );
		b[3] = ByteQueue_Get( &q->bq );	
		b[4] = ByteQueue_Get( &q->bq );
		b[5] = ByteQueue_Get( &q->bq );	
		b[6] = ByteQueue_Get( &q->bq );
		b[7] = ByteQueue_Get( &q->bq );	
		q->size_current -= 8;
		memcpy( &ptr, b, 8 );
	}
	else
	{
		__error( "unsupported pointer size\n" );
	}

	return ptr;
}


/*
  ==============================
  EventQueue_Unpackb
  
  ==============================
*/
char  EventQueue_Unpackb( event_queue_t *q )
{
	char		b;

	b = ByteQueue_Get( &q->bq );
	q->size_current--;
	return b;
}

/*
  ==============================
  EventQueue_Unpacks

  ==============================
*/
short EventQueue_Unpacks( event_queue_t *q )
{
	short		s;
	char		b[2];

	b[0] = ByteQueue_Get( &q->bq );
	b[1] = ByteQueue_Get( &q->bq );
	q->size_current -= 2;
	
	memcpy( &s, b, 2 );
	return s;
}

/*
  ==============================
  EventQueue_Unpacki

  ==============================
*/
int   EventQueue_Unpacki( event_queue_t *q )
{
	int		i;
	char		b[4];

	b[0] = ByteQueue_Get( &q->bq );
	b[1] = ByteQueue_Get( &q->bq );
	b[2] = ByteQueue_Get( &q->bq );
	b[3] = ByteQueue_Get( &q->bq );
	q->size_current -= 4;

	memcpy( &i, b, 4 );
	return i;	
}

/*
  ==============================
  EventQueue_Unpackf
  
  ==============================
*/
fp_t  EventQueue_Unpackf( event_queue_t *q )
{
	fp_t		f;
	char		b[4];

	b[0] = ByteQueue_Get( &q->bq );
	b[1] = ByteQueue_Get( &q->bq );
	b[2] = ByteQueue_Get( &q->bq );
	b[3] = ByteQueue_Get( &q->bq );
	q->size_current -= 4;

	memcpy( &f, b, 4 );
	return f;	
}

/*
  ==============================
  EventQueue_Unpack2fv

  ==============================
*/
void  EventQueue_Unpack2fv( event_queue_t *q, fp_t f[2] )
{
	char		b[8];

	b[0] = ByteQueue_Get( &q->bq );
	b[1] = ByteQueue_Get( &q->bq );
	b[2] = ByteQueue_Get( &q->bq );
	b[3] = ByteQueue_Get( &q->bq );
	b[4] = ByteQueue_Get( &q->bq );
	b[5] = ByteQueue_Get( &q->bq );
	b[6] = ByteQueue_Get( &q->bq );
	b[7] = ByteQueue_Get( &q->bq );
	q->size_current -= 8;

	memcpy( f, b, 8 );
}

/*
  ==============================
  EventQueue_Unpack3fv

  ==============================
*/
void  EventQueue_Unpack3fv( event_queue_t *q, fp_t f[3] )
{
	char		b[12];

	b[0] = ByteQueue_Get( &q->bq );
	b[1] = ByteQueue_Get( &q->bq );
	b[2] = ByteQueue_Get( &q->bq );
	b[3] = ByteQueue_Get( &q->bq );
	b[4] = ByteQueue_Get( &q->bq );
	b[5] = ByteQueue_Get( &q->bq );
	b[6] = ByteQueue_Get( &q->bq );
	b[7] = ByteQueue_Get( &q->bq );
	b[8] = ByteQueue_Get( &q->bq );
	b[9] = ByteQueue_Get( &q->bq );
	b[10] = ByteQueue_Get( &q->bq );
	b[11] = ByteQueue_Get( &q->bq );
	q->size_current -= 12;

	memcpy( f, b, 12 );

}

/*
  ==================================================
  byte iter

  ==================================================
*/
/*
  ==============================
  ByteIter_Init

  ==============================
*/
void ByteIter_Init( byte_iter_t *iter, char *base, size4_t size )
{
	iter->size = size;
	iter->ofs = 0;
	iter->base = base;
}

void ByteIter_SetOfs( byte_iter_t *iter, pos4_t ofs )
{
	if ( ofs >= iter->size )
		__error( "offset too large for buffer\n" );
	iter->ofs = ofs;
}

pos4_t ByteIter_GetOfs( byte_iter_t *iter )
{
	return iter->ofs;
}

char * ByteIter_GetPtr( byte_iter_t *iter )
{
	return &iter->base[iter->ofs];
}

char * ByteIter_GetBase( byte_iter_t *iter )
{
	return iter->base;
}

size4_t ByteIter_GetSize( byte_iter_t *iter )
{
	return iter->size;
}

void ByteIter_Skip( byte_iter_t *iter, int num_byte )
{
	iter->ofs += num_byte;
}


/*
  ==============================
  ByteIter_Put

  ==============================
*/

void ByteIter_Put( byte_iter_t *iter, char c )
{
	if ( iter->ofs >= iter->size )
		__error( "reached buffer end ( %d bytes )\n", iter->size );

	iter->base[iter->ofs] = c;
	iter->ofs++;
}

char ByteIter_Get( byte_iter_t *iter )
{
	char		c;

	if ( iter->ofs >= iter->size )
		__error( "reached buffer end ( %d bytes )\n", iter->size );

	c = iter->base[iter->ofs];
	iter->ofs++;

	return c;
}

/*
  ==================================================
  byte iter util

  ==================================================
*/
void ByteIter_Packp( byte_iter_t *q, maddr_t ptr )
{
	if ( sizeof( maddr_t ) == 4 )
	{
		char	b[4];

		memcpy( b, &ptr, 4 );
		ByteIter_Put( q, b[0] );
		ByteIter_Put( q, b[1] );	
		ByteIter_Put( q, b[2] );
		ByteIter_Put( q, b[3] );	
	}
	else if ( sizeof( maddr_t ) == 8 )
	{
		char	b[8];

		memcpy( b, &ptr, 8 );
		ByteIter_Put( q, b[0] );
		ByteIter_Put( q, b[1] );	
		ByteIter_Put( q, b[2] );
		ByteIter_Put( q, b[3] );	
		ByteIter_Put( q, b[4] );
		ByteIter_Put( q, b[5] );	
		ByteIter_Put( q, b[6] );
		ByteIter_Put( q, b[7] );	
	}
	else
	{
		__error( "unsupported pointer size\n" );
	}
}
void ByteIter_Packb( byte_iter_t *q, char b )
{
	ByteIter_Put( q, b );
}

void ByteIter_Packnbv( byte_iter_t *q, char b[], int num )
{
	int		i;

	for ( i = 0; i < num; i++ )
	{
		ByteIter_Put( q, b[i] );
	}	
}

void ByteIter_Packs( byte_iter_t *q, short s )
{
	char	b[2];

	memcpy( b, &s, 2 );
	ByteIter_Put( q, b[0] );
	ByteIter_Put( q, b[1] );	
}
void ByteIter_Packi( byte_iter_t *q, int i )
{
	char	b[4];

	memcpy( b, &i, 4 );
	ByteIter_Put( q, b[0] );
	ByteIter_Put( q, b[1] );	
	ByteIter_Put( q, b[2] );
	ByteIter_Put( q, b[3] );	
}
void ByteIter_Packf( byte_iter_t *q, fp_t f )
{
	char	b[4];

	memcpy( b, &f, 4 );
	ByteIter_Put( q, b[0] );
	ByteIter_Put( q, b[1] );	
	ByteIter_Put( q, b[2] );
	ByteIter_Put( q, b[3] );		
}
void ByteIter_Pack2fv( byte_iter_t *q, fp_t f[2] )
{
	char	b[8];

	memcpy( b, f, 8 );
	ByteIter_Put( q, b[0] );
	ByteIter_Put( q, b[1] );	
	ByteIter_Put( q, b[2] );
	ByteIter_Put( q, b[3] );		
	ByteIter_Put( q, b[4] );
	ByteIter_Put( q, b[5] );	
	ByteIter_Put( q, b[6] );
	ByteIter_Put( q, b[7] );	
}
void ByteIter_Pack3fv( byte_iter_t *q, fp_t f[3] )
{
	char	b[12];

	memcpy( b, f, 12 );
	ByteIter_Put( q, b[0] );
	ByteIter_Put( q, b[1] );	
	ByteIter_Put( q, b[2] );
	ByteIter_Put( q, b[3] );		
	ByteIter_Put( q, b[4] );
	ByteIter_Put( q, b[5] );	
	ByteIter_Put( q, b[6] );
	ByteIter_Put( q, b[7] );		
	ByteIter_Put( q, b[8] );
	ByteIter_Put( q, b[9] );	
	ByteIter_Put( q, b[10] );
	ByteIter_Put( q, b[11] );	
}

maddr_t ByteIter_Unpackp( byte_iter_t *q )
{
	maddr_t		ptr;

	if ( sizeof( maddr_t ) == 4 )
	{
		char	b[4];


		b[0] = ByteIter_Get( q );
		b[1] = ByteIter_Get( q );	
		b[2] = ByteIter_Get( q );
		b[3] = ByteIter_Get( q );	
		memcpy( &ptr, b, 4 );
	}
	else if ( sizeof( maddr_t ) == 8 )
	{
		char	b[8];

		b[0] = ByteIter_Get( q );
		b[1] = ByteIter_Get( q );	
		b[2] = ByteIter_Get( q );
		b[3] = ByteIter_Get( q );	
		b[4] = ByteIter_Get( q );
		b[5] = ByteIter_Get( q );	
		b[6] = ByteIter_Get( q );
		b[7] = ByteIter_Get( q );	
		memcpy( &ptr, b, 8 );
	}
	else
	{
		__error( "unsupported pointer size\n" );
	}

	return ptr;
}

char  ByteIter_Unpackb( byte_iter_t *q )
{
	char		b;

	b = ByteIter_Get( q );
	return b;	
}

void ByteIter_Unpacknbv( byte_iter_t *q, char b[], int num )
{
	int		i;

	for ( i = 0; i < num; i++ )
	{
		b[i] = ByteIter_Get( q );
	}	
}

short ByteIter_Unpacks( byte_iter_t *q )
{
	short		s;
	char		b[2];

	b[0] = ByteIter_Get( q );
	b[1] = ByteIter_Get( q );
	
	memcpy( &s, b, 2 );
	return s;	
}
int   ByteIter_Unpacki( byte_iter_t *q )
{
	int		i;
	char		b[4];

	b[0] = ByteIter_Get( q );
	b[1] = ByteIter_Get( q );
	b[2] = ByteIter_Get( q );
	b[3] = ByteIter_Get( q );

	memcpy( &i, b, 4 );
	return i;	
}
fp_t  ByteIter_Unpackf( byte_iter_t *q )
{
	fp_t		f;
	char		b[4];

	b[0] = ByteIter_Get( q );
	b[1] = ByteIter_Get( q );
	b[2] = ByteIter_Get( q );
	b[3] = ByteIter_Get( q );

	memcpy( &f, b, 4 );
	return f;		
}
void  ByteIter_Unpack2fv( byte_iter_t *q, fp_t f[2] )
{
	char		b[8];

	b[0] = ByteIter_Get( q );
	b[1] = ByteIter_Get( q );
	b[2] = ByteIter_Get( q );
	b[3] = ByteIter_Get( q );
	b[4] = ByteIter_Get( q );
	b[5] = ByteIter_Get( q );
	b[6] = ByteIter_Get( q );
	b[7] = ByteIter_Get( q );

	memcpy( f, b, 8 );	
}
void  ByteIter_Unpack3fv( byte_iter_t *q, fp_t f[3] )
{
	char		b[12];

	b[0] = ByteIter_Get( q );
	b[1] = ByteIter_Get( q );
	b[2] = ByteIter_Get( q );
	b[3] = ByteIter_Get( q );
	b[4] = ByteIter_Get( q );
	b[5] = ByteIter_Get( q );
	b[6] = ByteIter_Get( q );
	b[7] = ByteIter_Get( q );
	b[8] = ByteIter_Get( q );
	b[9] = ByteIter_Get( q );
	b[10] = ByteIter_Get( q );
	b[11] = ByteIter_Get( q );

	memcpy( f, b, 12 );
}

/*
  ==============================
  PackFrame_Begin

  ==============================
*/
void PackFrame_Begin( byte_iter_t *iter, byte_iter_t *frm_iter )
{
	pos4_t		ofs;
	size4_t		size;
	char		*ptr;

	int		remain;

	ofs = ByteIter_GetOfs( iter );
	size = ByteIter_GetSize( iter );
	ptr = ByteIter_GetPtr( iter );

	remain = size-ofs;

	if ( remain < 4 )
		__error( "not enough space left in buffer\n" );

	ByteIter_Init( frm_iter, ptr+4, remain-4 );
}

/*
  ==============================
  PackFrame_End

  ==============================
*/
size4_t PackFrame_End( byte_iter_t *iter, byte_iter_t *frm_iter )
{
	int	frm_ofs;

	frm_ofs = ByteIter_GetOfs( frm_iter );

	ByteIter_Packi( iter, frm_ofs );
	ByteIter_Skip( iter, frm_ofs );

	return frm_ofs;
}

/*
  ==============================
  UnpackFrame_Begin

  ==============================
*/
size4_t UnpackFrame_Begin( byte_iter_t *iter, byte_iter_t *frm_iter )
{
	size4_t		frm_size;
	char		*frm_base;

	frm_size = ByteIter_Unpacki( iter );
	frm_base = ByteIter_GetPtr( iter );
	
	if ( frm_size + ByteIter_GetOfs( iter ) > ByteIter_GetSize( iter ) )
	{
		__error( "frame is too large ( %d bytes at %d, %d bytes total )\n", frm_size, ByteIter_GetOfs( iter ), ByteIter_GetSize( iter ) );
	}

	ByteIter_Init( frm_iter, frm_base, frm_size );

	return frm_size;
}

/*
  ==============================
  UnpackFrame_End

  ==============================
*/
void UnpackFrame_End( byte_iter_t *iter, byte_iter_t *frm_iter )
{
	int	frm_size;
	int	frm_ofs;
	int	frm_remain;
	
	frm_size = ByteIter_GetSize( frm_iter );
	frm_ofs = ByteIter_GetOfs( frm_iter );

	frm_remain = frm_size - frm_ofs;

	if ( frm_remain > 0 )
	{
		__warning( "unpack not complete, skip %d bytes\n", frm_remain );
	}
	
	ByteIter_Skip( iter, frm_size );
}


/*
  ==================================================
  remote buffer copy stuff

  ==================================================
*/
/*
  ==============================
  BufferSend_Init

  ==============================
*/
void BufferSend_Init( buffer_send_t *send, char *buf_base, int buf_size, int buf_id )
{
	send->id = buf_id;
	send->size = buf_size;
	send->base = buf_base;
	send->ofs = 0;

	send->sum = BUFFER_FRAG_SUM;
}

/*
  ==============================
  BufferSend_IsComplete

  ==============================
*/
bool_t BufferSend_IsComplete( buffer_send_t *send )
{
	if ( send->size == send->ofs )
		return true;
	
	return false;
}

/*
  ==============================
  BufferSend_GetFrag

  ==============================
*/
bool_t BufferSend_GetFrag( buffer_send_t *send, char *frag_buf, int *frag_size )
{
	int		i;
	byte_iter_t	iter;
	int		num_remain;
	int		send_byte;
	int		send_sum;

	if ( send->size == send->ofs )
		return false;
	
	num_remain = send->size-send->ofs;

	if ( num_remain <= *frag_size-BUFFER_FRAG_HEADER_SIZE )
	{
		// the last frag of buffer
		send_byte = num_remain;
		send_sum = send->sum;
	}
	else
	{
		send_byte = *frag_size-BUFFER_FRAG_HEADER_SIZE;
		send_sum = 1;
	}
	
	*frag_size = send_byte + BUFFER_FRAG_HEADER_SIZE;
	
	ByteIter_Init( &iter, frag_buf, *frag_size );
	ByteIter_Packi( &iter, send->ofs );
	ByteIter_Packi( &iter, send_sum );
	ByteIter_Packi( &iter, send->id );

//	printf( "get_frag: ofs %d, sum %d, id %d\n", send->ofs, send_sum, send->id );

	for ( i = 0; i < send_byte; i++ )
	{
		ByteIter_Packb( &iter, send->base[send->ofs++] );
	}
	
	send->sum -= send_sum;

	return true;
}

void BufferRecv_Init( buffer_recv_t *recv, char *buf_base, int buf_max_size, int buf_id )
{
	recv->id = buf_id;
	recv->size = 0;
	recv->sum = 0;
	recv->base = buf_base;
	recv->max_size = buf_max_size;
}
bool_t BufferRecv_IsComplete( buffer_recv_t *recv )
{
	if ( recv->sum == BUFFER_FRAG_SUM )
		return true;

	return false;
}
void BufferRecv_PutFrag( buffer_recv_t *recv, char *frag_ptr, int frag_size )
{
	byte_iter_t	iter;
	byte_iter_t		iter2;

	int		i;
	int		recv_ofs;
	int		recv_sum;
	int		recv_id;
	
	int			num_byte;

	ByteIter_Init( &iter, frag_ptr, frag_size );
	
	recv_ofs = ByteIter_Unpacki( &iter );
	recv_sum = ByteIter_Unpacki( &iter );
	recv_id = ByteIter_Unpacki( &iter );

	num_byte = frag_size - BUFFER_FRAG_HEADER_SIZE;

	if ( recv_id != recv->id )
		__error( "fragment has got a wrong id ( %d should be %d )\n", recv_id, recv->id );

	recv->sum += recv_sum;
	recv->size += num_byte;
	
	if ( recv->sum > BUFFER_FRAG_SUM )
		__error( "fragment sum inconsitancy\n" );

	ByteIter_Init( &iter2, recv->base, recv->max_size );
	ByteIter_SetOfs( &iter2, recv_ofs );
	
	for ( i = 0; i < num_byte; i++ )
	{
		char	c;

		c = ByteIter_Unpackb( &iter );
		ByteIter_Packb( &iter2, c );
	}
}

int BufferRecv_GetSize( buffer_recv_t *recv )
{
	return recv->size;
}

/*
  ==============================
  BufferFrag_GetInfo

  ==============================
*/
void BufferFrag_GetInfo( char *frag_ptr, int *id, int *ofs, int *sum )
{
	byte_iter_t	iter;

	ByteIter_Init( &iter, frag_ptr, BUFFER_FRAG_HEADER_SIZE );
	
	*ofs = ByteIter_Unpacki( &iter );
	*sum = ByteIter_Unpacki( &iter );
	*id = ByteIter_Unpacki( &iter );	
}
