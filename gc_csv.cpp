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



// gc_csv.c
#include <string.h>
#include "gc_service.h"
#include "g_client.h"
#include "lib_queue.h"
#include "g_console.h"
#include "sh_parser.h"
#include "shock.h"

void GC_ConnectedServerInit( gc_state_t *state, gc_connected_server_t *cs, gc_host_addr_t *host )
{
	__named_message( "\n" );

	cs->state = connectedServerState_is_init;
	cs->timer = 0;
	cs->timeout = state->time;

	memcpy( &cs->host, host, sizeof( gc_host_addr_t ) );

	// invalidate sv_out buffer
	cs->buf_current_id = 0;
	cs->buf_size = 0;
}

void GC_ConnectedServerCleanUp( gc_state_t *state, gc_connected_server_t *cs )
{
	if ( cs->state == connectedServerState_none )
	{
		return;
	}

	if ( cs->state == connectedServerState_is_disconnected )
	{
		cs->state = connectedServerState_none;
		return;
	}
	//
	// send: cl2sv_disconnect
	// 
	{
		byte_iter_t	*bi;
		
		bi = GC_NetSendInit( state );
		ByteIter_Packi( bi, netEv_cl2sv_disconnect );
		
		GC_NetSendFromLocalPort( state, &cs->host );		
	}
	
	cs->state = connectedServerState_none;

}

void GC_ConnectedServerSetTimer( gc_state_t *state, gc_connected_server_t *cs, msec_t delta_timer )
{
	cs->timer = state->time + delta_timer;
}

void GC_ConnectedServerKillTimer( gc_connected_server_t *cs )
{
	cs->timer = 0;
}

void GC_ConnectedServerRun( gc_state_t *state, gc_connected_server_t *cs )
{
//	__named_message( "\n" );

	if ( state->time > cs->timeout + GC_CONNECTED_SERVER_TIMEOUT )
	{
		
	}

	if ( state->time < cs->timer )
		return;

	if ( cs->buf_size > 0 )
	{
//		__named_message( "valid buffer with id %d and %d bytes\n", cs->buf_current_id, cs->buf_size );
		G_ClientFillInput( &state->cl, cs->buf, cs->buf_size );
		cs->buf_size = 0;
	}

	if ( cs->state == connectedServerState_is_init )
	{
		cs->state = connectedServerState_try_connect;
		GC_ConnectedServerSetTimer( state, cs, 1000 );
	}
	else if ( cs->state == connectedServerState_try_connect )
	{
		SHV_Printf( "try server connect ...\n" );
//		SHV_Printf( "send host==>host: connect_as_client" );
		{
			byte_iter_t	*bi;
			
			bi = GC_NetSendInit( gc_state );
			ByteIter_Packi( bi, netEv_host2host_connect_as_client );
			
			GC_NetSendFromLocalPort( gc_state, &cs->host );		
		}
	      
		cs->state = connectedServerState_try_connect_wait;
		GC_ConnectedServerSetTimer( state, cs, 5000 );
	}
	else if ( cs->state == connectedServerState_try_connect_wait )
	{
		// if this state is reached we got a timeout
		SHV_Printf( "try server connect timeout\n" );
		cs->state = connectedServerState_is_dead;

		state->u_connect_failed = true;
	}
	else if ( cs->state == connectedServerState_is_connect )
	{
		SHV_Printf( "server connected\n" );
//		SHV_Printf( "send client==>server: connect" );
		{
			byte_iter_t	*bi;
			
			bi = GC_NetSendInit( gc_state );
			ByteIter_Packi( bi, netEv_cl2sv_connect );
			
			GC_NetSendFromLocalPort( state, &cs->host );		
		}
		
		cs->state = connectedServerState_await_game_info;
		GC_ConnectedServerSetTimer( state, cs, 5000 );
	}
	else if ( cs->state == connectedServerState_await_game_info )
	{
		SHV_Printf( "await game info timeout\n" );
		cs->state = connectedServerState_is_dead;

		state->u_connect_failed = true;
	}
	else  if( cs->state == connectedServerState_is_started )
	{
		SHV_Printf( "startup finished, join game\n" );
		cs->state = connectedServerState_is_joined;

		{
			byte_iter_t	*bi;
			
			bi = GC_NetSendInit( gc_state );
			ByteIter_Packi( bi, netEv_cl2sv_join_ready );
			
			GC_NetSendFromLocalPort( gc_state, &cs->host );		
		}
	}
}

void GC_ConnectedServerNetEv( gc_state_t *state, gc_connected_server_t *cs, netEv type, byte_iter_t *bi )
{
//	SHV_Printf( "connected_server async event" );

	if ( cs->state == connectedServerState_try_connect_wait )
	{
		if ( type == netEv_sv2cl_connect )
		{
//			SHV_Printf( "async recv server==>client: connect" );
			cs->state = connectedServerState_is_connect;	
			GC_ConnectedServerKillTimer( cs );
		}
		else
		{
			__warning( "unexpected event in state 'connectedServer_try_connect_wait'\n" );
		}
	}

	if ( cs->state == connectedServerState_await_game_info )
	{
		if ( type == netEv_sv2cl_game_info )
		{
			char		map_name[32];
			unique_t	client_id;
			char		str[256];
			
			ByteIter_Unpacknbv( bi, map_name, 32 );
			client_id = ByteIter_Unpacki( bi );
			
			sprintf( str, "%u", client_id );
			
			SHV_Printf( "game_info: map is '%s'\n", map_name );
			SHV_Printf( "game_info: client_id is '#%u'\n", client_id );
			
			// setup environment
			SHP_SetVar( "gc_servermode", "remote", 0 );
			SHP_SetVar( "gc_map", map_name, 0 );

			GC_BindControl( client_id );
						
			GC_ConnectedServerKillTimer( cs );
			cs->state = connectedServerState_is_starting;
			state->u_start_game = true;
		}
		else
		{
			__warning( "unexpected event in state 'connectedServerState_await_game_info'\n" );
		}
	}

	if ( type == netEv_sv2cl_buf_frag )
	{
		char	*frag_base;
		int	frag_size;
		int	frag_id;
		int	frag_ofs;
		int	frag_sum;

//		__named_message( "get sv_out buf_frag\n" );

		frag_base = ByteIter_GetPtr( bi );
		frag_size = ByteIter_GetSize( bi ) - ByteIter_GetOfs( bi );

		BufferFrag_GetInfo( frag_base, &frag_id, &frag_ofs, &frag_sum );

		if ( frag_id == 0 )
		{
			__error( "frag_id = 0 is not allowed\n" );
		}

		if ( frag_id > cs->buf_current_id )
		{
			cs->buf_size = -1;	// buffer is incomplete
			cs->buf_current_id = frag_id;
			BufferRecv_Init( &cs->buf_recv, cs->buf, GC_BUFFER_SIZE_CLIENT, frag_id );
			BufferRecv_PutFrag( &cs->buf_recv, frag_base, frag_size );
		}
		else if ( frag_id == cs->buf_current_id )
		{
			BufferRecv_PutFrag( &cs->buf_recv, frag_base, frag_size );
		}
		else
		{
			//
			// frags with old id are ignored
			//
		}

		// set size only once
		if ( BufferRecv_IsComplete( &cs->buf_recv ) && cs->buf_size == -1 )
		{
			cs->buf_size = BufferRecv_GetSize( &cs->buf_recv );
		}
		else
		{
			cs->buf_size = -1;
		}		
	}

	if ( type == netEv_sv2cl_disconnect )
	{
		SHV_Printf( "server closed connection\n" );
		cs->state = connectedServerState_is_disconnected;
		state->u_drop_game = true;
	}
}
