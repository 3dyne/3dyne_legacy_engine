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



// gc_ccl.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gc_service.h"
#include "g_server.h"

#include "lib_queue.h"
#include "g_console.h"
#include "sh_parser.h"

#include "shock.h"

void GC_ConnectedClientInit( gc_state_t *state, gc_connected_client_t *cc, gc_host_addr_t *host )
{
	__named_message( "\n" );

	cc->state = connectedClientState_is_init;
	cc->timer = 0;
	cc->timeout = state->time;
	cc->id_player = UNIQUE_INVALIDE;

	memcpy( &cc->host, host, sizeof( gc_host_addr_t ) );		

	// invalidate sv_in buffer
	cc->buf_current_id = 0;
	cc->buf_size = 0;

}

void GC_ConnectedClientCleanUp( gc_state_t *state, gc_connected_client_t *cc )
{
	if ( cc->state == connectedClientState_none )
	{
		return;
	}

	if ( cc->id_player != UNIQUE_INVALIDE )
	{
		GC_RemovePlayer( cc->id_player );
	}

	if ( cc->state == connectedClientState_is_disconnected )
	{
		cc->state = connectedClientState_none;
		return;
	}

	//
	// send: sv2cl_disconnect
	//
	{
		byte_iter_t	*bi;
		
		bi = GC_NetSendInit( state );
		ByteIter_Packi( bi, netEv_sv2cl_disconnect );
		
		GC_NetSendFromLocalPort( state, &cc->host );		
	}

	cc->state = connectedClientState_none;
}


void GC_ConnectedClientSetTimer( gc_state_t *state, gc_connected_client_t *cc, msec_t delta_timer )
{
	cc->timer = state->time + delta_timer;
}

void GC_ConnectedClientKillTimer( gc_connected_client_t *cc )
{
	cc->timer = 0;
}

void GC_ConnectedClientUpdateTimeout( gc_state_t *state, gc_connected_client_t *cc )
{
	cc->timeout = state->time;
}

void GC_ConnectedClientRun( gc_state_t *state, gc_connected_client_t *cc )
{
//	__named_message( "\n" );

	if ( state->time > cc->timeout + GC_CONNECTED_CLIENT_TIMEOUT )
	{
		
	}

	if ( state->time < cc->timer )
		return;

	if ( cc->state == connectedClientState_is_init )
	{
		cc->state = connectedClientState_try_connect;
		GC_ConnectedClientSetTimer( state, cc, 1000 );
	}
	else if ( cc->state == connectedClientState_try_connect )
	{
		cc->state = connectedClientState_try_connect_wait;
		GC_ConnectedClientSetTimer( state, cc, 5000 );

//		SHV_Printf( "send server==>client: connect" );
		{
			byte_iter_t	*bi;

			bi = GC_NetSendInit( state );
			ByteIter_Packi( bi, netEv_sv2cl_connect );

			GC_NetSendFromLocalPort( state, &cc->host );		
		}
	}
	else if ( cc->state == connectedClientState_try_connect_wait )
	{
		SHV_Printf( "try client connect timeout\n" );
		cc->state = connectedClientState_is_dead;
	}

	else if ( cc->state == connectedClientState_is_connect )
	{
		SHV_Printf( "client connected\n" );
		cc->state = connectedClientState_try_join;
		GC_ConnectedClientSetTimer( state, cc, 1000 );	       
	}
	else if ( cc->state == connectedClientState_try_join )
	{
		// 
		// get gserver player object id for client
		//
		cc->id_player = GC_GetUniqueID( state );

		
		SHV_Printf( "invite client to join the game ...\n" );
		cc->state = connectedClientState_try_join_wait;
		GC_ConnectedClientSetTimer( state, cc, 30000 );

		{
			byte_iter_t	*bi;

			bi = GC_NetSendInit( gc_state );

			// pack net_event type
			ByteIter_Packi( bi, netEv_sv2cl_game_info );
			
			// pack map_name
			{
				sh_var_t	*tmp;			
				char		map_name[32];

				tmp = SHP_GetVar( "gc_map" );
				memset( map_name, 0, 32 );
				strncpy( map_name, tmp->string, 31 );
				
				ByteIter_Packnbv( bi, map_name, 32 );
			}

			// pack client_id			
			ByteIter_Packi( bi, cc->id_player );


			GC_NetSendFromLocalPort( gc_state, &cc->host );		
		}
	}
	else if ( cc->state == connectedClientState_try_join_wait )
	{
		SHV_Printf( "join game timeout\n" );
		cc->state = connectedClientState_is_dead;
	}
	else if ( cc->state == connectedClientState_try_join_ack )
	{
		cc->state = connectedClientState_is_joined;
		SHV_Printf( "client joined the game\n" );
		GC_AddPlayer( cc->id_player );
	}
	else if ( cc->state == connectedClientState_is_joined )
	{
		if ( !cc->update )
		{
			GC_ConnectedClientCleanUp( state, cc );
			SHV_Printf( "client timeout, remove from game\n" );
			return;
		}

		if ( cc->buf_size > 0 )
		{
		//	__named_message( "valid buffer with id %d and %d bytes\n", cc->buf_current_id, cc->buf_size );
			G_ServerFillInput( &state->sv, cc->buf, cc->buf_size );
			cc->buf_size = 0;
		}

		cc->update = false;
		GC_ConnectedClientSetTimer( state, cc, 5000 );
	}
}

void GC_ConnectedClientNetEv( gc_state_t *state, gc_connected_client_t *cc, netEv type, byte_iter_t *bi )
{
	cc->update = true;
	cc->timeout = state->time;

//	SHV_Printf( "connected_client async event" );

	if ( cc->state == connectedClientState_try_connect_wait )
	{
		if ( type == netEv_cl2sv_connect )
		{
//			SHV_Printf( "async recv client==>server: connect" );
			GC_ConnectedClientKillTimer( cc );
			cc->state = connectedClientState_is_connect;
		}
		else
		{
			__warning( "unexpected event in state 'connectedClientState_try_connect_wait'\n" );
		}
	}

	if ( cc->state == connectedClientState_try_join_wait )
	{
		if ( type == netEv_cl2sv_join_ready )
		{
			GC_ConnectedClientKillTimer( cc );
			cc->state = connectedClientState_try_join_ack;
		}
		else
		{
			__warning ( "unexpected event in state 'connectedClientState_try_join_wait'\n" );
		}
	}

	if ( type == netEv_cl2sv_buf_frag )
	{
		char	*frag_base;
		int		frag_size;
		int		frag_id;
		int		frag_ofs;
		int		frag_sum;

//		__named_message( "get sv_in buf_frag\n" );

		frag_base = ByteIter_GetPtr( bi );
		frag_size = ByteIter_GetSize( bi ) - ByteIter_GetOfs( bi );

		BufferFrag_GetInfo( frag_base, &frag_id, &frag_ofs, &frag_sum );

		if ( frag_id == 0 )
		{
			__error( "frag_id = 0 is not allowed\n" );
		}

		if ( frag_id > cc->buf_current_id )
		{
			cc->buf_size = -1;	// buffer is incomplete
			cc->buf_current_id = frag_id;
			BufferRecv_Init( &cc->buf_recv, cc->buf, GC_BUFFER_SIZE_SERVER, frag_id );
			BufferRecv_PutFrag( &cc->buf_recv, frag_base, frag_size );
		}
		else if ( frag_id == cc->buf_current_id )
		{
			BufferRecv_PutFrag( &cc->buf_recv, frag_base, frag_size );
		}
		else 
		{
			//
			// frags with old id are ignored
			//
		}

		// set size only once
		if ( BufferRecv_IsComplete( &cc->buf_recv ) && cc->buf_size == -1 )
		{
			cc->buf_size = BufferRecv_GetSize( &cc->buf_recv );
			GC_ConnectedClientKillTimer( cc );
		}
		else 
		{		     
			cc->buf_size = -1;
		}		
	}

	if ( type == netEv_cl2sv_disconnect )
	{
		SHV_Printf( "client closed connection\n" );
		cc->state = connectedClientState_is_disconnected;
		GC_ConnectedClientCleanUp( state, cc );
		return;
	}
}
