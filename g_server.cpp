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



// g_server.c

#include "lib_container.h"
#include "lib_queue.h"

#include "g_server.h"
#include "g_library.h"
#include "interfaces.h"

/*
  ==============================
  G_InitServer

  ==============================
*/
void G_InitServer( g_server_t *sv )
{
	sh_var_t		*tmp;

	INITTYPE( sv, g_server_t );

	sv->state = gServerState_try_init;

	SHV_Printf( "\n" );
	SHV_Printf( " --- Init Server ---\n" );

	__chkptr( sv );

	tmp = SHP_GetVar( "gc_servermode" );
	
	__named_message( "gc_servermode is '%s'\n", tmp->string );
	if ( !strcmp( tmp->string, "local" ) )
	{
		//
		// load server dll
		//
		sprintf( sv->lib.name, "sv" );
		if ( G_LibLoad( &sv->lib ) )
		{
			__error( "can't load server dll\n" );			
		}

		sv->lib.api.Init();

		sv->lib.api.Com( "SV_WorldSpring" );

		sv->mode = gServerMode_local;
		sv->state = gServerState_is_init;
	}
	else if ( !strcmp( tmp->string, "remote" ) )
	{
		sv->mode = gServerMode_remote;
		sv->state = gServerState_is_init;
	}
	else
	{
		__error( "shvar 'gc_servermode' is invalide\n" );
	}

	G_ServerResetInput( sv );

	SHV_Printf( "\n" );
	SHV_Printf( " --- Init Server done ---\n" );

}

/*
  ==============================
  G_CleanUpServer

  ==============================
*/
void G_CleanUpServer( g_server_t *sv )
{	
	SHV_Printf( "\n" );
	SHV_Printf( " --- Clean Up Server ---\n" );
	
	__chkptr( sv );

	if ( sv->mode == gServerMode_local )
	{
		sv->lib.api.CleanUp();
		G_LibUnload( &sv->lib );
	}
	else if ( sv->mode == gServerMode_remote )
	{

	}
	else
	{
		__error( "unknown server mode\n" );
	}

	sv->state = gServerState_none;
	sv->mode = gServerMode_none;

	SHV_Printf( "\n" );
	SHV_Printf( " --- Clean Up Server done ---\n" );
}


#if 0
/*
  ==============================
  G_ServerSetInputEventQueue

  ==============================
*/
void G_ServerSetInputByteIter( g_server_t *sv, byte_iter_t *bi )
{
	__chkptr( sv );
	__chkptr( bi );

	sv->bi_in = bi;

	if ( sv->mode == gServerMode_local )
	{
		sv->lib.api.SetInputByteIter( bi );
	}
}

/*
  ==============================
  G_ServerSetOutputEventQueue

  ==============================
*/
void G_ServerSetOutputByteIter( g_server_t *sv, byte_iter_t *bi )
{
	__chkptr( sv );
	__chkptr( bi );

	sv->bi_out = bi;

	if ( sv->mode == gServerMode_local )
	{
		sv->lib.api.SetOutputByteIter( bi );
	}
}
#endif


/*
  ==============================
  G_ServerFillInput

  ==============================
*/
void G_ServerResetInput( g_server_t *sv )
{
	sv->buf_in_size = 0;
}

void G_ServerFillInput( g_server_t *sv, char *buf, int size )
{
	int		i;

	if ( sv->buf_in_size + size >= G_SERVER_INPUT_BUF_SIZE )
	{
		__error( "buffer overflow\n" );
	}

	for ( i = 0; i < size; i++ )
	{
		sv->buf_in[sv->buf_in_size++] = buf[i];
	}
}

/*
  ==============================
  G_ServerGetOutput

  ==============================
*/
void G_ServerResetOutput( g_server_t *sv )
{
	sv->buf_out_size = 0;
}

char * G_ServerGetOutputBase( g_server_t *sv )
{
	return sv->buf_out;
}

int G_ServerGetOutputSize( g_server_t *sv )
{
	return sv->buf_out_size;
}

/*
  ==============================
  G_ServerRunFrame

  ==============================
*/
void G_ServerRunFrame( g_server_t *sv )
{
	__chkptr( sv );

	if ( sv->state != gServerState_is_init )
		__error( "state is not init\n" );

	if ( sv->mode == gServerMode_local )
	{	 
		byte_iter_t	bi_in;
		byte_iter_t	bi_out;

		ByteIter_Init( &bi_in, sv->buf_in, sv->buf_in_size );
		ByteIter_Init( &bi_out, sv->buf_out, G_SERVER_OUTPUT_BUF_SIZE );
		
		sv->lib.api.SetInputByteIter( &bi_in );
		sv->lib.api.SetOutputByteIter( &bi_out );
    
		if ( 1 );
		{
			char	com_call[256];
			const char	*com_result;

			sprintf( com_call, "SV_RunFrame %u", ms_wfbegin );
			com_result = sv->lib.api.Com( com_call );
			
			if ( com_result )
			{
//				printf( "com_result: '%s'\n", com_result );
			}
		}

		sv->buf_out_size = ByteIter_GetOfs( &bi_out );
	}
	else if ( sv->mode == gServerMode_remote )
	{

		//
		// send sv_in as frags through connected_server
		//

//		__named_message( "client sends %d bytes of sv_in\n", ByteIter_GetSize( sv->bi_in ) );		

		
#if 1
		int		i;

		buffer_send_t	send;
		char		*send_buf;
		int		send_num_byte;
		static int	send_id = 1;
		
		int		num_frag;
// 		int		total_udp;

		send_num_byte = sv->buf_in_size;
		send_buf = sv->buf_in;

// 		total_udp = 0;
		BufferSend_Init( &send, send_buf, send_num_byte, send_id++ );
		for ( num_frag = 0; ; num_frag++ )
		{
			byte_iter_t	*bi;
			int		frag_num_byte;
			char		frag_buf[128];

			if ( BufferSend_IsComplete( &send ) )
				break;
			
			bi = GC_NetSendInit( gc_state );
			ByteIter_Packi( bi, netEv_cl2sv_buf_frag );
						
			frag_num_byte = 72;
			BufferSend_GetFrag( &send, frag_buf, &frag_num_byte );

			for ( i = 0; i < frag_num_byte; i++ )
			{
				ByteIter_Packb( bi, frag_buf[i] );
			}
			
			printf( "." );
			GC_NetSendFromLocalPort( gc_state, &gc_state->connected_sv.host );
		}

//		__named_message( "send %d/%d bytes in %d fragments\n",  send_num_byte, total_udp, num_frag );
#endif
		
		//
		// if connected_server got a complete buffer, put it into sv_out
		//


	}
	else
	{
		__error( "unknown server mode\n" );
	}

	//
	// init buffer for next server frame
	//
	
	G_ServerResetInput( sv );
}

