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



// gc_util.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gc_defs.h"
#include "shock.h"

/*
  ==============================
  GC_GetUniqueID

  ==============================
*/
unique_t GC_GetUniqueID( gc_state_t *state )
{
	if ( state->id_next >= state->id_last )
		__error( "no more unique id\n" );

	state->id_next++;
	return (state->id_next-1);
}

/*
  ==============================
  GC_InitHostAddrFromInetAddr

  ==============================
*/
void GC_InitHostAddrFromInetAddr( gc_host_addr_t *haddr, struct sockaddr_in *inet_addr )
{
	memcpy( &haddr->addr, inet_addr, sizeof( struct sockaddr_in ) );
	
	sprintf( haddr->addr_str, "%s:%u", 
		 inet_ntoa( inet_addr->sin_addr ), 
		 ntohs( inet_addr->sin_port ) );
}

/*
  ==============================
  GC_HostAddrCompare

  ==============================
*/
bool_t GC_HostAddrCompare( gc_host_addr_t *haddr, gc_host_addr_t *other )
{
	if ( !strcmp( haddr->addr_str, other->addr_str ) )
		return true;
	return false;
}

/*
  ==============================
  GC_HostAddrGetInetAddr

  ==============================
*/
struct sockaddr_in * GC_HostAddrGetInetAddr( gc_host_addr_t *haddr )
{
	return &haddr->addr;
}


/*
  ==============================
  GC_ConnectedClientTableGetEntryByHostAddr

  ==============================
*/
int GC_ConnectedClientTableGetEntryByHostAddr( gc_state_t *state, gc_host_addr_t *host )
{
	int		i;

	for ( i = 0; i < GC_STATE_MAX_CONNECTED_CLIENTS; i++ )
	{
		if ( state->connected_cl_tbl[i].state == connectedClientState_none )
		{
			continue;
		}
		
		if ( GC_HostAddrCompare( &state->connected_cl_tbl[i].host, host ) == true )
		{
			return i;
		}
	}

	return -1;
}

/*
  ==============================
  GC_ConnectedClientTableGetFreeEntry

  ==============================
*/
int GC_ConnectedClientTableGetFreeEntry( gc_state_t *state )
{
	int		i;

	for ( i = 0; i < GC_STATE_MAX_CONNECTED_CLIENTS; i++ )
	{
		if ( state->connected_cl_tbl[i].state == connectedClientState_none )
		{
			return i;
		}
	}

	return -1;
}




/*
  ==============================
  GC_ConnectedServerVerify

  ==============================
*/
bool_t GC_ConnectedServerVerify( gc_state_t *state )
{
	if ( state->connected_sv.state == connectedServerState_none )
	{
		return false;
	}

	if ( GC_HostAddrCompare( &state->host_last_recv, &state->connected_sv.host ) == false )
	{
		return false;
	}
	return true;
}

/*
  ==============================
  GC_ConnectedClientVerify

  ==============================
*/
int GC_ConnectedClientVerify( gc_state_t *state )
{
	int	entry;

	entry = GC_ConnectedClientTableGetEntryByHostAddr( state, &state->host_last_recv );

	return entry;
}

/*
  ==============================
  GC_StateTimeoutKill

  ==============================
*/
void GC_StateTimeoutKill( gc_state_t *state, void (*timeout_func)(gc_state_t *state ) )
{
	if ( timeout_func != NULL )
	{
		if ( timeout_func != state->timeout_func )
		{
			__error( "no such timeout\n" );
		}
	}

	state->timeout_func = NULL;
}

/*
  ==============================
  GC_StateTimeoutStart

  ==============================
*/
void GC_StateTimeoutStart( gc_state_t *state, msec_t timeout_delta, void (*timeout_func)(gc_state_t *state ) )
{
	__chkptr( timeout_func );

	if ( state->timeout_func != NULL )
	{
		__error( "no more timeouts\n" );
	}

	state->timeout_func = timeout_func;
	state->timeout = state->time + timeout_delta;
}

