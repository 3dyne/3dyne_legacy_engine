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



// lib_queue.h

#ifndef __lib_queue
#define __lib_queue

#include "u_defs.h"
#include "lib_queuedefs.h"
#include "lib_math.h"

//
// byte queue stuff
//

// normal use
void ByteQueue_Init( byte_queue_t *q, char *buf_ptr, int buf_size );

int ByteQueue_CanPut( byte_queue_t *q );
int ByteQueue_CanGet( byte_queue_t *q );

void ByteQueue_Put( byte_queue_t *q, char c );
char ByteQueue_Get( byte_queue_t *q );

// manipulate queue
int ByteQueue_GetPutOfs( byte_queue_t *q );
void ByteQueue_SetGhostPutOfs( byte_queue_t *q, int ofs );
void ByteQueue_GhostPut( byte_queue_t *q, char c );

//
// packed event queue
//

void EventQueue_Init( event_queue_t *q, char *buf_ptr, int buf_size );

int EventQueue_CanPut( event_queue_t *q );
int EventQueue_CanGet( event_queue_t *q );

// put event
void EventQueue_BeginPack( event_queue_t *q );
int  EventQueue_EndPack( event_queue_t *q );

void EventQueue_Packp( event_queue_t *q, maddr_t ptr );
void EventQueue_Packb( event_queue_t *q, char b );
void EventQueue_Packs( event_queue_t *q, short s );
void EventQueue_Packi( event_queue_t *q, int i );
void EventQueue_Packf( event_queue_t *q, fp_t f );
void EventQueue_Pack2fv( event_queue_t *q, fp_t f[2] );
void EventQueue_Pack3fv( event_queue_t *q, fp_t f[3] );

// get event
int EventQueue_BeginUnpack( event_queue_t *q );		// return size of event start to unpack
int EventQueue_EndUnpack( event_queue_t *q );	     

maddr_t EventQueue_Unpackp( event_queue_t *q );
char  EventQueue_Unpackb( event_queue_t *q );
short EventQueue_Unpacks( event_queue_t *q );
int   EventQueue_Unpacki( event_queue_t *q );
fp_t  EventQueue_Unpackf( event_queue_t *q );
void  EventQueue_Unpack2fv( event_queue_t *q, fp_t f[2] );
void  EventQueue_Unpack3fv( event_queue_t *q, fp_t f[3] );

//
// byte iterator
//

void ByteIter_Init( byte_iter_t *iter, char *base, size4_t size );

void ByteIter_SetOfs( byte_iter_t *iter, pos4_t ofs );

pos4_t ByteIter_GetOfs( byte_iter_t *iter );
char * ByteIter_GetPtr( byte_iter_t *iter );
char * ByteIter_GetBase( byte_iter_t *iter );

size4_t ByteIter_GetSize( byte_iter_t *iter );

void ByteIter_Skip( byte_iter_t *iter, int num_byte );

void ByteIter_Put( byte_iter_t *iter, char c );
char ByteIter_Get( byte_iter_t *iter );

// byte iter util
void PackFrame_Begin( byte_iter_t *iter, byte_iter_t *frm_iter );
size4_t PackFrame_End( byte_iter_t *iter, byte_iter_t *frm_iter );
size4_t UnpackFrame_Begin( byte_iter_t *iter, byte_iter_t *frm_iter );
void UnpackFrame_End( byte_iter_t *iter, byte_iter_t *frm_iter );

void ByteIter_Packp( byte_iter_t *q, maddr_t ptr );
void ByteIter_Packb( byte_iter_t *q, char b );
void ByteIter_Packnbv( byte_iter_t *q, char b[], int num );
void ByteIter_Packs( byte_iter_t *q, short s );
void ByteIter_Packi( byte_iter_t *q, int i );
void ByteIter_Packf( byte_iter_t *q, fp_t f );
void ByteIter_Pack2fv( byte_iter_t *q, fp_t f[2] );
void ByteIter_Pack3fv( byte_iter_t *q, fp_t f[3] );

maddr_t ByteIter_Unpackp( byte_iter_t *q );
char  ByteIter_Unpackb( byte_iter_t *q );
void ByteIter_Unpacknbv( byte_iter_t *q, char b[], int num );
short ByteIter_Unpacks( byte_iter_t *q );
int   ByteIter_Unpacki( byte_iter_t *q );
fp_t  ByteIter_Unpackf( byte_iter_t *q );
void  ByteIter_Unpack2fv( byte_iter_t *q, fp_t f[2] );
void  ByteIter_Unpack3fv( byte_iter_t *q, fp_t f[3] );


//
// buffer remote copy stuff
//

void BufferSend_Init( buffer_send_t *send, char *buf_base, int buf_size, int buf_id );
bool_t BufferSend_IsComplete( buffer_send_t *send );
bool_t BufferSend_GetFrag( buffer_send_t *send, char *frag_buf, int *frag_size );

void BufferRecv_Init( buffer_recv_t *recv, char *buf_base, int buf_max_size, int buf_id );
bool_t BufferRecv_IsComplete( buffer_recv_t *recv );
void BufferRecv_PutFrag( buffer_recv_t *recv, char *frag_ptr, int frag_size );
int BufferRecv_GetSize( buffer_recv_t *recv );

void BufferFrag_GetInfo( char *frag_ptr, int *id, int *ofs, int *sum );

#endif
