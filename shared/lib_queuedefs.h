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



// lib_queuedefs.h

#ifndef __lib_queuedefs
#define __lib_queuedefs

#include "u_defs.h"

typedef struct byte_queue_s
{
	int		put_ofs;
	int		ghost_put_ofs;
	int		get_ofs;
	int		num_byte;
	int		size;
	char		*base;
} byte_queue_t;

typedef struct event_queue_s
{
	byte_queue_t	bq;
	int		num_event;
	int		ofs_current;
	int		size_current;
} event_queue_t;

//
// byte iter 
//

typedef struct byte_iter_s
{
	size4_t		size;
	pos4_t		ofs;
	char		*base;
} byte_iter_t;

//
// remote buffer copy
//

#define BUFFER_FRAG_HEADER_SIZE		( 4+4+4 )
#define BUFFER_FRAG_SUM			( 32000 )

typedef struct buffer_send_s
{
	int		id;		// buffer id
	int		size;		// total buffer size
	int		ofs;		// base offset for next frag
	int		sum;		// sum left	
	char		*base;
} buffer_send_t;

typedef struct buffer_recv_s
{
	int		id;
	int		size;		// size of all frags till now
	int		sum;		// sum
	
	int		max_size;
	char		*base;
} buffer_recv_t;

#endif
