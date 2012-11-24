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



// gc_service.c

#ifdef win32_x86
  typedef int socklen_t;
  #include <winsock2.h>  
#endif

#include "interfaces.h"

#include "./r_fake/r_interface.h"
#include "r_private.h"
#include "lib_tag.h"

#include "lib_queue.h"
#include "lib_parse.h"
#include "lib_digest.h"

#include "g_psys.h"

#include "r_count.h"
#include "g_server.h"
#include "g_client.h"
#include "g_library.h"
#include "snd_deamon.h"
#if !defined (win32_x86)
#include <stdio.h>                                                              
#include <sys/types.h>                                                          
#include <sys/socket.h>                                                         
#include <netinet/in.h>                                                         
#include <arpa/inet.h>
#include <time.h>
#else


#endif 

//#include <sys/time.h>                                                           

#include <errno.h>

//#include "shape_defs.h"
//#include "shape_instance.h"
//#include "tess_simple_face.h"

#define MSEC_WF	( 100 ) // msecs of worldframe 
#define GC_ISWORLDFRAME ( 1 )
#define GC_ISRENDERFRAME ( 2 )

static char	graphcursor[4][256];

static sh_var_t *rendershot;

int	gc_fps = 0;
fp_t	gc_fpfps;

int	gc_tps = 0;	// tris/sec
int	gc_tpf = 0;	// tris/frame
static int	gc_inmainloop = 0;

static fp_t	gc_ftimes[8];

unsigned int	ms_win;
unsigned int	ms_rfbegin = 0;
unsigned int	ms_wfbegin = 0;
int	ms_rfdelta = 0;	// time passed since last render frame

int	gc_ismenu = 0;
int	gc_isshell = 0;

//int	gc_ismap = 0;
//int	gc_islife = 0;

//int	gc_isloading = 0;

int	gc_renderframe = 0;
int	gc_localworldframe = 0;

int	gc_textcursor;
char	*gc_graphcursor;

int	gc_wftimebase;

//int	gci_exit; // exit if set

int	gci_domuch;
//int	gci_spawnmap;
//int	gci_dropmap;
//int	gci_restartgl;

//int	gci_startlocalgame;
//int	gci_startremotegame;

int	gc_lastbusy;

playermod_t	pmod; // test only!
int	mx = 0, my = 0;
static 	int wf_mdx, wf_mdy;

int	gc_localseconds;


sh_var_t	*gc_mousescale;
sh_var_t	*gc_invmouse;
static sh_var_t	*gc_showfps;

gc_state_t	*gc_state;

// fixme: sysdep stuff!

#if defined( win32_x86 )
static int gc_wsisup = 0;
static WSADATA winsockdata;
#endif

FILE	*h_sendudp = NULL;
FILE	*h_recvudp = NULL;


void GC_StartRemoteGame( void );
void GC_DropRemoteGame( void );
void GC_TryClientToServerConnect( void );
void GC_TryClientToServerJoin( void );
void GC_ClientInputUpdate();
void GC_RunClientFrame();

void GC_LoadGraphCursor()
{
	TFUNC_ENTER;

	strcpy( graphcursor[0], "menu.tex.bigcurs00" );
	strcpy( graphcursor[1], "menu.tex.bigcurs01" );
	strcpy( graphcursor[2], "menu.tex.bigcurs02" );
	strcpy( graphcursor[3], "menu.tex.bigcurs03" );
	TFUNC_LEAVE;
}

void GC_ClearInitFlags()
{
	TFUNC_ENTER;
//	gci_exit = 0;
	gci_domuch = 0;	
	TFUNC_LEAVE;
}

int GC_CheckWFTimebase()
{
	unsigned int	now;

	now = SYS_GetMsec();
//	cprintf( "ms_frdelta: %d\n", ms_rfdelta );

	if( now - ms_wfbegin > MSEC_WF )
	{
//		cprintf( "left time: %d", 100 - ( now - ms_wfbegin ) );
//		__named_message( "its time\n" );
		ms_wfbegin = SYS_GetMsec();
		gc_wftimebase = 1;
//		printf( "wf: %u\n", now );
		return 1;
	}
	return 0;
}



void GC_DoMuch()
{
	// test givebacktime
	__error( "total crap function called\n" ); // this won't do much on most current compilers...
#if 0
    int	i, i2;
	TFUNC_ENTER;
//	GC_SpawnLife();
	for( i = 0; i < 20; i++ )
	{
		cprintf( "working hard\n" );
		for( i2 = 0; i2 < 10000000; i2++ )
		{
			a = i * i2;
		}
		GC_GiveBackTime();
	}
	TFUNC_LEAVE;
#endif
}

// =============================================================================



/*
  ==============================
  GC_AddPlayer

  ==============================
*/
void GC_AddPlayer( unique_t id )
{
	char		tt[256];

	cprintf( "add player to game\n" );

	if ( gc_state->sv.mode == gServerMode_local )
	{
		sprintf( tt, "SV_AddPlayer #%u", id );
		gc_state->sv.lib.api.Com( tt );
		
//		sprintf( tt, "%u", id );
//		SHP_SetVar( "gc_id_bind_view", tt, 0 );
//		SHP_SetVar( "gc_id_bind_input", tt, 0 );
	}
	else
	{
		__error( "no local server, can't add player\n" );
	}
}

void GC_RemovePlayer( unique_t id )
{
	char	str[256];

	cprintf( "remove player from game\n" );
	
	if ( gc_state->sv.mode == gServerMode_local )
	{
		sprintf( str, "SV_RemovePlayer #%u", id );
		gc_state->sv.lib.api.Com( str );		
	}
	else
	{
		__error( "no local server, can't remove player\n" );
	}
}

void GC_BindControl( unique_t id )
{
	char		tt[256];

	sprintf( tt, "%u", id );
	SHP_SetVar( "gc_id_bind_view", tt, 0 );
	SHP_SetVar( "gc_id_bind_input", tt, 0 );	
}


/*
  ==============================
  GC_InitDemo

  ==============================
*/
void GC_InitDemo( gc_state_t *state )
{
	if ( state->gc_is_demo )
	{
		cprintf( "already in a demo\n" );
		return;
	}
	
	SHP_SetVar( "gc_servermode", "local", 0 );
	SHP_SetVar( "gc_map", "title1", 0 );
	
	state->gc_is_demo = true;

//	GC_InitGame( state );
}

/*
  ==============================
  GC_CleanUpDemo

  ==============================
*/
void GC_CleanUpDemo( gc_state_t *state )
{
	if ( !state->gc_is_demo )
	{
		cprintf( "not in a demo\n" );
		return;
	}

	state->gc_is_demo = false;

//	GC_CleanUpGame( state );
}

/*
  ==============================
  GC_InitSingle

  ==============================
*/
void GC_InitSingle( gc_state_t *state )
{
	if ( state->gc_is_single )
	{
		cprintf( "already in a single player game\n" );
		return;
	}

	
	SHP_SetVar( "gc_servermode", "local", 0 );
//	SHP_SetVar( "gc_map", "test8", 0 );
	
	state->gc_is_single = true;	
}

/*
  ==============================
  GC_CleanUpSingle

  ==============================
*/
void GC_CleanUpSingle( gc_state_t *state )
{
	if ( !state->gc_is_single )
	{
		cprintf( "not in single player game\n" );
		return;
	}
       	
	state->gc_is_single = false;
}

/*
  ==============================
  GC_InitConnectPublicServer

  ==============================
*/
void GC_TryClientToServerConnect( void )
{
	gc_host_addr_t		assumed_server;

	cprintf( "try to connect ...\n" );

	if ( gc_state->connected_sv.state != connectedServerState_none )
		__error( "expected connectedServerState_none\n" );

	//
	// set server address from local settings
	//

	{
		struct sockaddr_in	addr;
		sh_var_t			*tmp;
		
		tmp = SHP_GetVar( "gc_remoteaddr" );
		__named_message( "gc_remoteaddr is '%s'\n", tmp->string );
		addr.sin_addr.s_addr = inet_addr( tmp->string );
		
		tmp = SHP_GetVar( "gc_remoteport" );
		__named_message( "gc_remoteport is '%s'\n", tmp->string );
		addr.sin_port = htons( atoi( tmp->string ) );
		
		addr.sin_family = AF_INET;	
		
		GC_InitHostAddrFromInetAddr( &assumed_server, &addr );
	}

	GC_ConnectedServerInit( gc_state, &gc_state->connected_sv, &assumed_server );
}


void GC_InitConnectPublicServer( gc_state_t *state )
{
	if ( state->gc_has_server_connection )
	{
		cprintf( "already connected\n" );
		return;
	}
	
	if ( GC_NetOpenLocalPort( state, true ) != true )
	{
		__error( "can't open local port\n" );
	}

	GC_TryClientToServerConnect();	

	state->gc_has_server_connection = true;
}

/*
  ==============================
  GC_CleanUpConnectPublicServer

  ==============================
*/
void GC_CleanUpConnectPublicServer( gc_state_t *state )
{
	if ( !state->gc_has_server_connection )
	{
		cprintf( "not connected\n" );
		return;
	}

	GC_ConnectedServerCleanUp( state, &state->connected_sv );

	GC_NetCloseLocalPort( state );

	state->gc_has_server_connection = false;
}

/*
  ==============================
  GC_InitPublicServer

  ==============================
*/
void GC_InitPublicServer( gc_state_t *state )
{
	if ( state->gc_is_public_server )
	{
		cprintf( "already a public server\n" );
		return;
	}

	SHP_SetVar( "gc_servermode", "local", 0 );
	if ( GC_NetOpenLocalPort( state, true ) != true )
	{
		__error( "can't open local port\n" );
	}

	state->u_start_game = true;
	state->gc_is_public_server = true;
}

/*
  ==============================
  GC_CleanUpPublicServer

  ==============================
*/
void  GC_CleanUpPublicServer( gc_state_t *state )
{ 
	int			i;
	gc_connected_client_t	*cc;

	if ( !state->gc_is_public_server )
	{
		cprintf( "not a public server\n" );
		return;
	}	

	for ( i = 0; i < GC_STATE_MAX_CONNECTED_CLIENTS; i++ )
	{
		cc = &state->connected_cl_tbl[i];
		
		if ( cc->state == connectedClientState_none )
		{
			continue;
		}
		
		GC_ConnectedClientCleanUp( state, cc );
	}

	GC_NetCloseLocalPort( state );
	state->gc_is_public_server = false;
}

/*
  ==============================
  GC_InitGame

  ==============================
*/
void GC_InitGame( gc_state_t *state )
{
	if ( state->gc_in_game )
	{
		cprintf( "already in a game\n" );
		return;
	}

		sc_readbytes = 0;

	g_st = G_NewState();
	G_StateInit( g_st );
	
	state->gc_is_loading = true;
	G_InitMap( &gc_state->map );
	G_InitServer( &gc_state->sv );
	G_InitClient( &gc_state->cl );

	state->gc_is_loading = false;
	state->gc_in_game = true;

	if ( state->gc_is_single || state->gc_is_public_server )
	{
		unique_t	id;
		
		id = GC_GetUniqueID( state );

		GC_AddPlayer( id );
		GC_BindControl( id );	
	}

	//
	// tell server that client is ready to handle events
	//
	if ( state->gc_has_server_connection )
	{
		if ( state->connected_sv.state == connectedServerState_is_starting )
		{
			state->connected_sv.state = connectedServerState_is_started;	
		}
	}
	
	gc_localworldframe = 0;
//	gc_islife = 1;
//	gc_ismap = 1;
	
//	gc_isloading = 0;
	
	//
	gc_state->state = gcState_game_ready;
}

/*
  ==============================
  GC_CleanUpGame

  ==============================
*/
void GC_CleanUpGame( gc_state_t *state )
{
	if ( !state->gc_in_game )
	{
		cprintf( "not in a game\n" );
		return;
	}

	G_CleanUpClient( &gc_state->cl );
	G_CleanUpServer( &gc_state->sv );
	G_CleanUpMap( &gc_state->map );
	
	FREE( g_st );	

	state->gc_in_game = false;
}

/*
  ==============================
  GC_CleanUp

  ==============================
*/
void GC_CleanUp( gc_state_t *state )
{
	if ( state->gc_has_server_connection )
	{
		GC_CleanUpConnectPublicServer( state );	
	}
	if ( state->gc_is_public_server )
	{
		GC_CleanUpPublicServer( state );
	}
	if ( state->gc_is_demo )
	{
		GC_CleanUpDemo( state );
	}
	if ( state->gc_is_single )
	{
		GC_CleanUpSingle( state );
	}
	if ( state->gc_is_loading )
	{
		__error( "while loading ...\n" );
	}
	if ( state->gc_in_game )
	{
		GC_CleanUpGame( state );
	}

//	state->u_start_demo = true;
}

/*
  ==================================================
  net_events handled by game_control

  ==================================================
*/

void GC_NetEventHandle_host2host_connect_as_client( gc_state_t *state )
{
	int			entry;
	gc_connected_client_t	*cc;

	cprintf( "recv host==>host: connect_as_client\n" );


	entry = GC_ConnectedClientTableGetEntryByHostAddr( state, &state->host_last_recv );
	cprintf( "remote host is: %s\n", state->host_last_recv.addr_str );

	if ( entry != -1 )
	{
		//
		// the client is already connected 
		//
		
		cprintf( "ignore, host is already a client\n" );
//		GC_NetEventReply_sv2cl_refuse( state, &state->host_last_recv );
		return;
	}

	entry = GC_ConnectedClientTableGetFreeEntry( state );
	if ( entry == -1 )
	{
		//
		// the server can't handle more clients
		//

		cprintf( "ignore, don't accpet more clients\n" );
//		GC_NetEventReply_sv2cl_refuse( state, &state->host_last_recv );
		return;
	}

	cc = &state->connected_cl_tbl[entry];
	GC_ConnectedClientInit( state, cc, &state->host_last_recv );

	cprintf( "server: accept host as client\n" );       
}


// =============================================================================

 



void GC_SendServerOutputToLocalClient( gc_state_t *state )
{
	char		*sv_out_buf;
	int		sv_out_size;	

	sv_out_buf = G_ServerGetOutputBase( &gc_state->sv );
	sv_out_size = G_ServerGetOutputSize( &gc_state->sv );

	//
	// copy server output to local client input
	//
	

	G_ClientFillInput( &state->cl, sv_out_buf, sv_out_size );
}

void GC_SendServerOutputToAllConnectedClients( gc_state_t *state )
{
	int		i, j;

	char		*sv_out_buf;
	int		sv_out_size;	

	static int		send_id = 1;
	buffer_send_t		send;

	int		num_frag;

	sv_out_buf = G_ServerGetOutputBase( &gc_state->sv );
	sv_out_size = G_ServerGetOutputSize( &gc_state->sv );
	
	BufferSend_Init( &send, sv_out_buf, sv_out_size, send_id++ );
	for ( num_frag = 0; ; num_frag++ )
	{
		byte_iter_t	*bi;
		int		frag_num_byte;
		char		frag_buf[128];
		
		if ( BufferSend_IsComplete( &send ) )
			break;
		
		bi = GC_NetSendInit( gc_state );
		ByteIter_Packi( bi, netEv_sv2cl_buf_frag );
		
		frag_num_byte = 72;
		BufferSend_GetFrag( &send, frag_buf, &frag_num_byte );
		
		for ( i = 0; i < frag_num_byte; i++ )
		{
			ByteIter_Packb( bi, frag_buf[i] );
		}
		
		for ( j = 0; j < GC_STATE_MAX_CONNECTED_CLIENTS; j++ )
		{
			gc_connected_client_t	*cc;

			cc = &state->connected_cl_tbl[j];
			if ( cc->state == connectedClientState_is_joined )
			{
				GC_NetSendFromLocalPort( gc_state, &cc->host );
			}
		}
	}
}

void GC_InputToBuffer( gc_state_t *state, byte_iter_t *bi )
{
	sh_var_t		*tmp;
	unique_t	id;

	byte_iter_t		frm;

	tmp = SHP_GetVar( "gc_id_bind_input" );
	id = tmp->ivalue;

	if ( id == UNIQUE_INVALIDE )
	{
		return;
	}

//	cprintf( "lon: %.2f, lat: %.2f, speed: %.2f\n", gc_state->i_lon, gc_state->i_lat, gc_state->i_speed );

	PackFrame_Begin( bi, &frm );
	ByteIter_Packi( &frm, id );
	ByteIter_Packi( &frm, eventType_server_update_player );
	ByteIter_Packf( &frm, gc_state->i_speed );
	ByteIter_Packf( &frm, gc_state->i_strafespeed );
	ByteIter_Packf( &frm, gc_state->i_lon );
	ByteIter_Packf( &frm, gc_state->i_lat );
	ByteIter_Packi( &frm, gc_state->i_shoot );
	ByteIter_Packi( &frm, gc_state->i_use );
	ByteIter_Packi( &frm, gc_state->i_jump );
	PackFrame_End( bi, &frm );
}

void GC_SendLocalInputToServer( void )
{
	char		sv_in_buf[1024];
	int		sv_in_size;
	byte_iter_t	bi_in;
	

	if ( gc_state->sv.state != gServerState_is_init )
	{
		return;
	}

	if ( gc_state->gc_is_demo )
	{
		return;
	}

	if ( gc_state->gc_has_server_connection && gc_state->connected_sv.state != connectedServerState_is_joined )
	{
		return;
	}

	ByteIter_Init( &bi_in, sv_in_buf, 1024 );
	GC_InputToBuffer( gc_state, &bi_in );
	sv_in_size = ByteIter_GetOfs( &bi_in );
	
	G_ServerFillInput( &gc_state->sv, sv_in_buf, sv_in_size );
}

void GC_RunServerFrame()
{
	TFUNC_ENTER;

//	__named_message( "\n" );

	if ( gc_state->sv.state == gServerState_is_init )

	{
// 		char		*sv_out_buf;
// 		int		sv_out_size;

		//
		// remote hosts put their pmod during GC_ConnectedClientRun 
		// into server input buffer
		//
		
		//
		// put local pmod into server input buffer
		//

#if 0
		if ( !gc_state->gc_is_demo )
		{
			ByteIter_Init( &bi_in, sv_in_buf, 1024 );
			GC_InputToBuffer( gc_state, &bi_in );
			sv_in_size = ByteIter_GetOfs( &bi_in );
			
			G_ServerFillInput( &gc_state->sv, sv_in_buf, sv_in_size );
		}
#endif
		
		GC_SendLocalInputToServer();

		G_ServerRunFrame( &gc_state->sv );

		/*sv_out_buf = */G_ServerGetOutputBase( &gc_state->sv );
		/*sv_out_size = */G_ServerGetOutputSize( &gc_state->sv );

		//
		// copy server output to local client input
		//
		GC_SendServerOutputToLocalClient( gc_state );

		//
		// send server output via connected_cl_tbl[] to all remote hosts 
		//

		// fixme: is this the right place for that
		GC_SendServerOutputToAllConnectedClients( gc_state );
		

		// handle server output only once
		G_ServerResetOutput( &gc_state->sv );

		if ( gc_state->sv.mode == gServerMode_local )
		{

		}
	}

	TFUNC_LEAVE;
}

// ====================
// GC_MainLoop
// this one should loop 
// forever
// does one loop per 
// worldframe
// ====================


// fake!


// ====

void GC_SecondFrame()
{
	gc_localseconds++;
	SHV_ScrollSOut();
}

int GC_ClassifyFrame()
{
	if( GC_CheckWFTimebase()) // time for worldframe
	{
		ms_wfbegin = SYS_GetMsec();
		return GC_ISWORLDFRAME;
	} else
		return GC_ISRENDERFRAME;

	return 0;
}	

//
// network
//

/*
  ==============================
  GC_NetSendFromLocalPort

  ==============================
*/
byte_iter_t * GC_NetSendInit( gc_state_t *state )
{
	ByteIter_Init( &state->udp_send_bi, state->udp_send_buf, GC_MAX_NET_UDP_SIZE );
	return &state->udp_send_bi;
}

void GC_NetSendFromLocalPort( gc_state_t *state, gc_host_addr_t *host )
{
	pos4_t	ofs;
    // SIM: fixme: most of the called functions work with char * but there are some arithmetics in this functions that might rely on unsigned
	unsigned char	*buf;

	unsigned short	chksum;

	if ( state->udp_state != gcUdpState_is_init )
		__error( "expected gcUdpState_is_init\n" );
	
	ofs = ByteIter_GetOfs( &gc_state->udp_send_bi );
	buf = (unsigned char *)ByteIter_GetBase( &gc_state->udp_send_bi );

	if ( ofs == 0 )
	{
		return;
	}
	
	//
	// calc 16 bit chksum for buffer and append it
	//
	
	chksum = BufferChecksum16( ofs, (char *)buf );
//	printf( "send chksum: %u\n", chksum );
	buf[ofs++] = (chksum&255);	// low
	buf[ofs++] = (chksum>>8)&255;	// high

	sendto( state->local_sock, (char *)buf, ofs, 0, (struct sockaddr *)&host->addr, sizeof(struct sockaddr) );

	if ( h_sendudp )
	{
		pos4_t		i;
		int	frag_id, frag_ofs, frag_sum;
		// assume buffer fragment
		BufferFrag_GetInfo( (char*)&buf[4], &frag_id, &frag_ofs, &frag_sum );
		fprintf( h_sendudp, "time: %d, size: %d, buf_id %d, frag_ofs %d, frag_sum %d ", ms_rfbegin, ofs, frag_id, frag_ofs, frag_sum );
		for ( i = 0; i < ofs; i++ ) 
		{
			if ( (i&15) == 0 )
				fprintf( h_sendudp, "\n %.4x: ", i );
			fprintf( h_sendudp, "%.2x ", (unsigned char) buf[i] );
		}
		fprintf( h_sendudp, "\n\n" );
	}
}

/*
  ==============================
  GC_NetRecvFromLocalPort

  ==============================
*/
byte_iter_t * GC_NetRecvFromLocalPort( gc_state_t *state )
{
	int		size;
	socklen_t		addr_len;
	int	ret;

	struct timeval	tv;
	fd_set	readfs;


	if ( state->udp_state != gcUdpState_is_init )
		__error( "expected gcUdpState_is_init\n" );

	
		
	tv.tv_sec = 0;
	tv.tv_usec = 0;


	FD_ZERO(  &readfs );
	FD_SET( gc_state->local_sock, &readfs );

	ret = select( FD_SETSIZE, &readfs, NULL, NULL, &tv );

/*
	if( ret < 0 )
		cprintf( "select: %s\n", strerror( errno ));
*/
//	cprintf( "select: %d %d\n", ret, errno );

	if( ret <= 0 )
	{

		ByteIter_Init( &state->udp_recv_bi, state->udp_recv_buf, 0 );
		return NULL;
	}

	addr_len = sizeof( struct sockaddr );
	size = recvfrom( state->local_sock, state->udp_recv_buf, GC_MAX_NET_UDP_SIZE, 0, (struct sockaddr *) &state->addr_last_recv, &addr_len );	
	

	if ( size >= 2 )
	{
		unsigned short	chksum1;
		unsigned short	chksum2;
		unsigned char		*ptr;
		ptr = (unsigned char *)state->udp_recv_buf;
		//
		// test checksum
		// 
		chksum1 = BufferChecksum16( size-2, state->udp_recv_buf );
		chksum2 = ((unsigned short)(ptr[size-2]))|(((unsigned short)(ptr[size-1]))<<8);
//		printf( "recv chksum: %u %u\n", chksum1, chksum2 );		
		if ( chksum1 != chksum2 )
		{
			__warning( "udp chksum failed\n" );
			size = -1;
		}
		else
		{
			size -= 2;
		}
	}

	if ( h_recvudp && size >= 0 )
	{
		int		i;
		int	frag_id, frag_ofs, frag_sum;
		// assume buffer fragment
		BufferFrag_GetInfo( &state->udp_recv_buf[4], &frag_id, &frag_ofs, &frag_sum );
		fprintf( h_recvudp, "time: %d, size: %d, buf_id %d, frag_ofs %d, frag_sum %d ", ms_rfbegin, size, frag_id, frag_ofs, frag_sum );
		for ( i = 0; i < size; i++ ) 
		{
			if ( (i&15) == 0 )
				fprintf( h_recvudp, "\n %.4x: ", i );
			fprintf( h_recvudp, "%.2x ", (unsigned char) state->udp_recv_buf[i] );
		}
		fprintf( h_recvudp, "\n\n" );
	}
	
	if ( size < 0 )
	{
		size = 0;
	}

	ByteIter_Init( &state->udp_recv_bi, state->udp_recv_buf, size );
	
	if ( size == 0 )
		return NULL;
	return &state->udp_recv_bi;
}

/*
  ==============================
  GC_NetOpenLocalPort

  ==============================
*/
#define GC_LOCAL_PORT_TRY	( 26000 )
#define GC_LOCAL_PORT_TRY_NUM	( 64 )

bool_t GC_NetOpenLocalPort( gc_state_t *state, bool_t auto_port )
{
	sh_var_t	*tmp;
	int		res;

	if ( state->udp_state != gcUdpState_none )
		__error( "expected gcUdpState_none\n" );

	state->udp_state = gcUdpState_is_init;

// sydep stuff!
#if defined( win32_x86 )
	if( !gc_wsisup )
	{
		int r;
		gc_wsisup = 1;
		
		r = WSAStartup( MAKEWORD( 1, 1), &winsockdata );
		
		if( r )
		{
			__error( "WSAStartup failed\n" );
		}
	}
#endif

	state->local_sock = socket( PF_INET, SOCK_DGRAM, 0 );

	if ( state->local_sock == -1 ) 
	{
		__error( "socket failed: %s ( %d )\n", strerror( errno ), errno );
	}

	// NONBLOCK replaced by select call
// fcntl( state->local_sock, F_SETFL, O_NONBLOCK ); 
	state->local_port.sin_addr.s_addr = INADDR_ANY;
	state->local_port.sin_family = AF_INET;

	if ( auto_port == false )
	{
		tmp = SHP_GetVar( "gc_localport" );
		__named_message( "gc_localport is '%s'\n", tmp->string );
		state->local_port.sin_port = htons( atoi( tmp->string ) );

		res = bind( state->local_sock, 
			    (struct sockaddr *) &gc_state->local_port, 
			    sizeof( gc_state->local_port ));
		
		if ( res == -1 )
			return false;

		cprintf( "port is '%d'\n", state->local_port.sin_port  );
	}
	else
	{
		int		i;

		for ( i = 0; i < GC_LOCAL_PORT_TRY_NUM; i++ )
		{
			state->local_port.sin_port = htons( GC_LOCAL_PORT_TRY+i );

			res = bind( state->local_sock, 
				    (struct sockaddr *) &gc_state->local_port, 
				    sizeof( gc_state->local_port ));

			if ( res != -1 )
			{
				cprintf( "port is '%d'\n", GC_LOCAL_PORT_TRY+i );
				return true;
			}
		}
	}

	state->udp_state = gcUdpState_none;
	return false;
}

/*
  ==============================
  GC_NetCloseLocalPort

  ==============================
*/
void GC_NetCloseLocalPort( gc_state_t *state )
{
	int	res;

	if ( state->udp_state != gcUdpState_is_init )
		__error( "expected gcUdpState_is_init\n" );

	
#ifndef win32_x86
	res = close( state->local_sock );
#else
	res = closesocket( state->local_sock );
#endif

	if ( res == -1 )
		__error( "close failed\n" );

	state->udp_state = gcUdpState_none;
}

/*
  ==============================
  GC_NetHandleLocalPort

  ==============================
*/
void GC_NetHandleLocalPort( gc_state_t *gc )
{
	byte_iter_t	*bi;
	netEv		type;

	for(;;)
	{
		// that's a hack, cause the connection can vanish
		if ( gc->udp_state != gcUdpState_is_init )
			break;
		
		if ( !( bi = GC_NetRecvFromLocalPort( gc ) ) )
			break;
		
		GC_InitHostAddrFromInetAddr( &gc->host_last_recv, &gc->addr_last_recv );
		
		type = (netEv)ByteIter_Unpacki( bi );	       
		

		if ( type == netEv_host2host_connect_as_client )
		{
			__named_message( "netEv_host2host_connect_as_client\n" );
			GC_NetEventHandle_host2host_connect_as_client( gc );
		}
		else if ( type > netEv_cl2sv_first &&
			  type < netEv_cl2sv_last )
		{
			int	entry;

			entry = GC_ConnectedClientVerify( gc );
			if ( entry == -1 )
			{
				__warning( "net_event from unknown client\n" );
			}
			else
			{
				GC_ConnectedClientNetEv( gc, &gc->connected_cl_tbl[entry], type, bi );
			}
		}
		else if ( type > netEv_sv2cl_first && 
			  type < netEv_sv2cl_last )
		{
			if ( GC_ConnectedServerVerify( gc ) )
			{
				GC_ConnectedServerNetEv( gc, &gc->connected_sv, type, bi );
			}
			else
			{
				__warning( "net_event from unknown server\n" );
			}			
		}
		else
		{
			__error( "unknown net_event type\n" );
		}
	}
}


/*
  ==============================
  GC_MainLoop

  ==============================
*/
extern unsigned int	watch1, watch2;
// SIM: FIXME: where are the sh menu prototypes?
void SHM_SetCurPage( const char *);
void SHM_GetCurpage( char * );

void GC_MainLoop()
{
	int	inactive, loops;
	int	tps = 0;
	int	i;
	
	sh_var_t		*var_dumpudp;

	TFUNC_ENTER;
	// this is the third and by now best and most flexible attemp

	watch1 = watch2 = 1234;

	//
	// init gc_state
	// 
	gc_state = NEWTYPE( gc_state_t );
	gc_state->state = gcState_none;
	gc_state->udp_state = gcUdpState_none;

	gc_state->id_first = 0x70000000;
	gc_state->id_last  = 0x7fffffff;
	gc_state->id_next = gc_state->id_first;

	// init event queues
	ByteIter_Init( &gc_state->bi_server, gc_state->buf_server, 0 );
	ByteIter_Init( &gc_state->bi_client, gc_state->buf_client, 0 );
	ByteIter_Init( &gc_state->bi_render, gc_state->buf_render, 0 );
	
	// init gc_state shvars
	SHP_SetVar( "gc_servermode", "local", 0 );
	SHP_SetVar( "gc_id_bind_input", "0", 0 );
	SHP_SetVar( "gc_id_bind_view", "0", 0 );
	SHP_SetVar( "gc_id_player", "0", 0 );

	// init ids
	gc_state->id_bind_input = 123456; //UNIQUE_INVALIDE;
	gc_state->id_bind_view = UNIQUE_INVALIDE;

	rendershot = SHP_GetVar( "rendershot" );

	gc_mousescale = SHP_GetVar( "gc_mousescale" );
	gc_invmouse = SHP_GetVar( "gc_invmouse" );
	
	gc_showfps = SHP_GetVar( "gc_showfps" );

	var_dumpudp = SHP_GetVar( "gc_dumpudp" );

	if ( var_dumpudp->ivalue > 0 )
	{
		char	str[256];
		sprintf( str, "udpsend%d.dump", var_dumpudp->ivalue );
		h_sendudp = fopen( str, "w" );
		sprintf( str, "udprecv%d.dump", var_dumpudp->ivalue );
		h_recvudp = fopen( str, "w" );
	}
	else
	{
		h_sendudp = NULL;
		h_recvudp = NULL;
	}
	
	ms_wfbegin = SYS_GetMsec();
	ms_rfbegin = SYS_GetMsec();
	ms_rfdelta = 0;

	gc_tps = 0;
	gc_inmainloop = 1;

	//
	// start demo
	// 

	gc_state->u_start_demo = true;

	SHM_SetCurPage( "main" );

	for( ;; )
	{
		static int	rfalltime = 0, rfnum = 0;
		
		int	now;

		inactive = 1;
		now = SYS_GetMsec();

		gc_state->time = now;

		//
		// check async state updates
		//

		if ( gc_state->u_quit )
		{
			if ( h_sendudp )
			{
				fclose( h_sendudp );
			}
			if ( h_recvudp )
			{
				fclose( h_recvudp );
			}
			Exit();
		}
		if ( gc_state->u_start_demo )
		{
			gc_state->u_start_demo = false;
			GC_CleanUp( gc_state );
			GC_InitDemo( gc_state );
			GC_InitGame( gc_state );
		}
		if ( gc_state->u_start_single )
		{
			gc_state->u_start_single = false;
			GC_CleanUp( gc_state );
			GC_InitSingle( gc_state );
			GC_InitGame( gc_state );
		}
		if ( gc_state->u_connect_public_server )
		{
			gc_state->u_connect_public_server = false;
			GC_CleanUp( gc_state );
			GC_InitConnectPublicServer( gc_state );
		}
		if ( gc_state->u_connect_failed )
		{
			gc_state->u_connect_failed = false;
//			GC_CleanUpConnectPublicServer( gc_state );
//			GC_CleanUp( gc_state );
			gc_state->u_drop_game = true;
		}
		if ( gc_state->u_start_public_server )
		{
			gc_state->u_start_public_server = false;
			GC_CleanUp( gc_state );
			GC_InitPublicServer( gc_state );
		}
		if ( gc_state->u_start_game )
		{
			gc_state->u_start_game = false;
			GC_InitGame( gc_state );
		}
		if ( gc_state->u_drop_game )
		{
			gc_state->u_drop_game = false;
			GC_CleanUp( gc_state );

			//
			// restart demo
			// 
			gc_state->u_start_demo = true;
		}

#if 0
		if ( gc_state->timeout_func )
		{
			if ( gc_state->time >= gc_state->timeout )
			{
				void (*timeout_func)(gc_state_t *state ) = gc_state->timeout_func;
				gc_state->timeout_func = NULL;
				timeout_func( gc_state );
			}
		}
#endif	

		for ( i = 0; i < GC_STATE_MAX_CONNECTED_CLIENTS; i++ )
		{
			if ( gc_state->connected_cl_tbl[i].state != connectedClientState_none )
			{
				GC_ConnectedClientRun( gc_state, &gc_state->connected_cl_tbl[i] );
			}		       
		}

		if ( gc_state->connected_sv.state != connectedServerState_none )
		{
			GC_ConnectedServerRun( gc_state, &gc_state->connected_sv );
		}
		
		if ( gc_state->udp_state == gcUdpState_is_init )
		{
			GC_NetHandleLocalPort( gc_state );
		}


		GC_ClientInputUpdate();
		

		if( ( now - ms_wfbegin ) >= MSEC_WF )
		{
			inactive = 0;
			SND_Update();
			gc_localworldframe++;

			if( !(gc_localworldframe%10) )
				SHV_ScrollSOut();
			
			ms_wfbegin = SYS_GetMsec();

			if ( g_st )
			{				
				pmod.lon = g_st->view_lon;
				pmod.lat = g_st->view_lat;
			}

			GC_RunServerFrame();

			rfalltime = 0;
			rfnum = 0;
			
			wf_mdx = wf_mdy = 0;
		}

		{
			unsigned int	ms1, ms2;

			inactive = 0;
			ms1 = SYS_GetMsec();
			ms_rfbegin = ms1;
			GC_RunClientFrame();

#if !defined(win32_x86)
			{
				struct timespec ts;
				ts.tv_sec = 0;
				ts.tv_nsec = 16666666; // 60fps according to google (and Wolfram Alpha ;-) ). Man, how times have changed ...
				nanosleep( &ts, NULL );
			}
#else
			Sleep( 16 );
#endif
			gc_tpf = count_be_num_tri;
			tps += gc_tpf;
			ms2 = SYS_GetMsec();
			ms_rfdelta = ms2-ms1;
			rfnum++;
			rfalltime += ms_rfdelta;
		}

		gc_ftimes[gc_renderframe&7] = ms_rfdelta;

		if( (gc_renderframe&7) == 0 )
		{
			fp_t	alltime = 0.0;
			
			for( i = 0; i < 8; i++ )
			{
				alltime += gc_ftimes[i];
			

			}
			gc_fpfps = 1000.0 / (alltime / 8.0);
		}

		if( !inactive )
		{
			loops = 0;
		}
		
	
		loops++;

	}
	TFUNC_LEAVE;
}

#if 0
void GC_FillTime()
{
	int	count = 0;
	int	ms_rflast;

	TFUNC_ENTER;

	for( ;; ) // do renderframes
	{
		ms_rfbegin = SYS_GetMsec();



		if( ((int)ms_rfdelta) > ((int)( MSEC_WF - ( ms_rfbegin - ms_wfbegin ))))
		{
//			cprintf( "time left: %d %d\n", 100 - ( ms_rfbegin - ms_wfbegin ), ms_rfdelta );
			if( ms_rfdelta >= 100 )
				ms_rfdelta = 20;


			break;

		} else
			GC_DoRenderFrame();

		ms_rfdelta = SYS_GetMsec() - ms_rfbegin;

	}


//	cprintf( "%d fps", count * 10 );
	TFUNC_LEAVE;
}

#endif

void GC_UpdatePmod()
{
	int	inv;


	pmod.speedscale = 0.0;
	pmod.strafespeedscale = 0.0;
	pmod.lon = pmod.lat = 0.0;	


	
//	__named_message( "hflags: %d\n", local_hflags ); 

#if 0
	if( local_hflags & AHF_STRAFE )
	{

		pmod.strafe = 1;
	}
	else
		pmod.strafe = 0;
#endif	

	if( local_hflags & AHF_FORWARD )
	{
//		__named_message( "forward\n" );
		pmod.speedscale += 32.0;
	}

	if( local_hflags & AHF_BACK )
		pmod.speedscale += -32.0;

	
	if( local_hflags & AHF_STRAFE )
	{
		if ( local_hflags & AHF_TRIGHT || local_hflags & AHF_SRIGHT )
			pmod.strafespeedscale += 32.0;

		if ( local_hflags & AHF_TLEFT || local_hflags & AHF_SLEFT )
			pmod.strafespeedscale -= 32.0;
	}
	else
	{
		if ( local_hflags & AHF_SRIGHT )
			pmod.strafespeedscale += 32.0;

		if ( local_hflags & AHF_SLEFT )
			pmod.strafespeedscale -= 32.0;

		if( local_hflags & AHF_TRIGHT )
			pmod.lon += 20.0;
		
			
		if( local_hflags & AHF_TLEFT )
			pmod.lon += -20.0;
		
		pmod.lon += wf_mdx * gc_mousescale->fvalue;
	}
		
	if( local_hflags & AHF_LOOKUP )
		pmod.lat += 5.0;

	
	if( local_hflags & AHF_LOOKDOWN )
		pmod.lat += -5.0;

	if( gc_invmouse->ivalue )
		inv = -1;
	else
		inv = 1;

	pmod.lat += wf_mdy * gc_mousescale->fvalue * inv;

	if( local_hflags & AHF_FIRE0 )
	{
		pmod.shoot = 1;
//		printf( "shoot 1" );
	}
	else 
		pmod.shoot = 0;


	if( local_hflags & AHF_USE )
	{
		cprintf( "realuse\n" );
		pmod.use = 1;
	}
	else
		pmod.use = 0;

	if( local_hflags & AHF_JUMP )
	{

		pmod.jump = 1;
	}
	else
		pmod.jump = 0;

	


//	pmod.right = local_hflags && AHF_TRIGHT;
//	pmod.strafe = local_hflags && AHF_STRAFE;
}

void GC_AccumulateViewAngles( void )
{
	fp_t	delta_sec;

	delta_sec = ((fp_t)(ms_rfdelta)) / 1000.0;

	if ( md_x > 50 )
		md_x = 50;
	if ( md_y > 50 )
		md_y = 50;

//	cprintf( "delta_sec: %.3f\n", delta_sec );

//	printf( "flags: %d %d %d\n", local_hflags, watch1, watch2 );
	gc_state->i_strafespeed = 0.0;
	if ( local_hflags & AHF_STRAFE )
	{
		//
		// strafe right/left
		//

		if ( local_hflags & AHF_TRIGHT || local_hflags & AHF_SRIGHT  )
			gc_state->i_strafespeed += 256.0;
		
		if ( local_hflags & AHF_TLEFT || local_hflags & AHF_SLEFT )
			gc_state->i_strafespeed -= 256.0;

		
		gc_state->i_strafespeed += md_x * gc_mousescale->fvalue * 128.0;

		if ( gc_state->i_strafespeed > 256.0 )
			gc_state->i_strafespeed = 256.0;
		else if ( gc_state->i_strafespeed < -256.0 )
			gc_state->i_strafespeed = -256.0;
	}
	else
	{
		//
		// turn right/left
		//
		if ( local_hflags & AHF_SRIGHT  )
			gc_state->i_strafespeed += 256.0;
		
		if ( local_hflags & AHF_SLEFT )
			gc_state->i_strafespeed -= 256.0;


		if ( local_hflags & AHF_TRIGHT )
		{
			gc_state->i_lon += (100.0*delta_sec);
		}
		
		if ( local_hflags & AHF_TLEFT )
		{
			gc_state->i_lon -= (100.0*delta_sec);
		}

//		gc_state->i_lon += (md_x*delta_sec)*10.0 * gc_mousescale->fvalue;		
		gc_state->i_lon += md_x * gc_mousescale->fvalue;
		
	}

//	cprintf( "delta_sec: %.3f mouse %d\n", delta_sec, md_x );

	//
	// look up/down
	//
		
	if( gc_invmouse->ivalue )
//		gc_state->i_lat += (md_y*delta_sec)*10.0 * gc_mousescale->fvalue * -1;
		gc_state->i_lat += md_y * gc_mousescale->fvalue * -1;
	else
//		gc_state->i_lat += (md_y*delta_sec)*10.0 * gc_mousescale->fvalue;
		gc_state->i_lat += md_y * gc_mousescale->fvalue;

	
	if ( gc_state->i_lat > 90.0 )
		gc_state->i_lat = 90.0;

	if ( gc_state->i_lat < -90.0 )
		gc_state->i_lat = -90.0;

	//
	// walk forward/backward
	//
	gc_state->i_speed = 0.0;
	if( local_hflags & AHF_FORWARD )
	{
		gc_state->i_speed += 256.0;
	}
	if( local_hflags & AHF_BACK )
	{
		gc_state->i_speed -= 256.0;
	}

	
	//
	// fire weapon
	//
	if( local_hflags & AHF_FIRE0 )
	{
		gc_state->i_shoot = 1;
	}
	else 
	{
		gc_state->i_shoot = 0;
	}

	//
	// use something
	//
	if( local_hflags & AHF_USE )
	{
		gc_state->i_use = 1;
	}
	else
	{
		gc_state->i_use = 0;
	}
	
	//
	// jump
	//
	if( local_hflags & AHF_JUMP )
	{

		gc_state->i_jump = 1;
	}
	else
	{
		gc_state->i_jump = 0;
	}
	

//	cprintf( "ms_rfdelta: %u\n", ms_rfdelta );
}


void GC_ClientInputUpdate()
{
	char	menupage[256];
	TFUNC_ENTER;

	I_Update();
	
	wf_mdx += md_x;
	wf_mdy += md_y;

        
	if( gc_isshell )
		SHV_Update( keventlist );
	SHM_Update( keventlist );

	SHM_GetCurpage( menupage );

	if( (!strcmp( menupage, "none" )) && (!gc_state->gc_is_single) && (!gc_state->gc_has_server_connection) && (!gc_state->gc_is_public_server) )
		SHI_SensibleUpdate( keventlist ); // in this state show menu on every keypress
	SHI_Update( keventlist );

	
	//
	// no accumulation during demos ( fix sky )
	//
	if ( !gc_state->gc_is_demo )
	{
		GC_AccumulateViewAngles();
	}

	{ 
		char	inputline[CON_MAXLINESIZE];
	//	if( inputline[0] )
		{
			CON_GetInput( inputline );
			SHP_ParseLine( inputline );
		}
	}

	TFUNC_LEAVE;
}

void GC_RunClientFrame()
{
	TFUNC_ENTER;

//	GC_ClientInputUpdate();

	if ( gc_state->cl.state == gClientState_is_init )
	{
		char		*cl_out_buf;
		int		cl_out_size;

		//
		// fill client input buffer is done by GC_AIWorldFrame
		//

		G_ClientRunFrame( &gc_state->cl );	       
		
		//
		// setup renderer from client output buffer
		//

		cl_out_buf = G_ClientGetOutputBase( &gc_state->cl );
		cl_out_size = G_ClientGetOutputSize( &gc_state->cl );

		{
			byte_iter_t	render_in;
			int		num_ev;

// 			sh_var_t		*tmp;

// 			tmp = SHP_GetVar( "gc_id_bind_view" );

			ByteIter_Init( &render_in, cl_out_buf, cl_out_size );

			for ( num_ev = 0;; num_ev++ )
			{	
// 				unique_t	id;
				eventType	ev_type;
				
				int		remain;
				byte_iter_t	frm;
// 				int		size;

				remain = ByteIter_GetSize( &render_in ) - ByteIter_GetOfs( &render_in );			       

				if ( remain == 0 )
					break;

				/*size = */UnpackFrame_Begin( &render_in, &frm );			
				
				/*id = */ByteIter_Unpacki( &frm );
				ev_type = (eventType)ByteIter_Unpacki( &frm );
				
				if ( ev_type != eventType_render_info_ptr )
					__error( "unknown render event\n" );
				
				UnpackFrame_End( &render_in, &frm );
				
			}
//			__named_message( " %d input events for the renderer\n", num_ev );				
		}
	}
	
	GC_DoRFDraw();
	R_SwapBuffer();
	TFUNC_LEAVE;
}





static int bla = 0;

void GC_DoRFDraw()
{
	int		i, j;
	TFUNC_ENTER;

	//
	// reset gl backend counters
	//
	COUNT_RESET( count_be_num_transform );
	COUNT_RESET( count_be_num_begin );
	COUNT_RESET( count_be_num_flush );
	COUNT_RESET( count_be_num_tri );

	gc_textcursor = ( (gc_localworldframe/4)&1 )+ 1;  // 1 or 2
	
	gc_graphcursor = graphcursor[gc_localworldframe%4];

	// Fake render draw call
	// set render_origin from ipo
	if ( gc_state->gc_in_game )
	{
		ri_view_t	*ri_vplayer;
		ri_view_t	*ri_vspectator;
		ri_view_t	*ri_vsky;

		matrix3_t		render_matrix;

		static sh_var_t	*var_drawtris = NULL;

		ri_vplayer = gc_state->cl.ri_view_player;
		ri_vspectator = gc_state->cl.ri_view_spectator;
		ri_vsky = gc_state->cl.ri_view_sky;

		//
		// if we only draw lines, the color buffer have to be cleared
		//

		if ( !var_drawtris )
			var_drawtris = SHP_GetVar( "r_drawtris" );
		
		if ( var_drawtris->ivalue == 1 )
		{
			glClearColor( 0.3, 0.3, 0.3, 1.0 );
			glClear(GL_COLOR_BUFFER_BIT);
		}

		//
		// setup render models
		//
		for ( i = 0; i < gc_state->cl.ri_model_num; i++ )
		{
			G_SetupModel( gc_state->cl.ri_models[i]->md->cs_root, gc_state->cl.ri_models[i]->ref_origin, gc_state->cl.ri_models[i]->ref_axis );
		}
		
		//
		// setup render psys
		//
		for ( i = 0; i < gc_state->cl.ri_psys_num; i++ )
		{
			G_FieldParticleSystemRun( &gc_state->cl.ri_psyss[i]->fpsys, ms_rfdelta );	
		}
		
		for ( j = 0; j < 3; j++ )
		{
			if ( ri_vsky && j == 0 )
			{
			  //		__named_message( "view_sky\n" );
				R_SetView( ri_vsky->origin, gc_state->i_lat, gc_state->i_lon ); 
				Matrix3SetupRotate( render_matrix, 0*D2R, gc_state->i_lat*D2R, gc_state->i_lon*D2R );
				R_BE_SetVertexMatrix( &render_matrix );
				R_BE_SetVertexOrigin( ri_vsky->origin );

				goto render;
			}
			if ( ri_vplayer && j == 1 )
			{
			  //	__named_message( "view_player\n" );
				R_SetView( ri_vplayer->origin, gc_state->i_lat, gc_state->i_lon ); 
				Matrix3SetupRotate( render_matrix, 0*D2R, gc_state->i_lat*D2R, gc_state->i_lon*D2R );
				R_BE_SetVertexMatrix( &render_matrix );
				R_BE_SetVertexOrigin( ri_vplayer->origin );
				
				goto render;
			}
			if ( ri_vspectator && j == 2 )
			{
//				__named_message( "view_spectator\n" );
				R_SetView( ri_vspectator->origin, ri_vspectator->lat, ri_vspectator->lon ); 
				Matrix3SetupRotate( render_matrix, 0*D2R, ri_vspectator->lat*D2R, ri_vspectator->lon*D2R );
				R_BE_SetVertexMatrix( &render_matrix );
				R_BE_SetVertexOrigin( ri_vspectator->origin );
				goto render;
			}		
			
			continue;

		render:

			glClearDepth( 0.0 );
			glClear(GL_DEPTH_BUFFER_BIT);

			{
				// render the view
				R_Prepare();

				if ( j != 0 )
				{
					
					R_SetLocalLightInfos( gc_state->cl.ri_local_light_num, gc_state->cl.ri_local_lights );
					
					R_SetSpriteInfos( gc_state->cl.ri_sprite_num, gc_state->cl.ri_sprites );
					R_SetPsysInfos( gc_state->cl.ri_psys_num, gc_state->cl.ri_psyss );
					R_SetHaloInfos( gc_state->cl.ri_halo_num, gc_state->cl.ri_halos );
				}
				
				//
				// draw render models
				//
				if ( j != 0 )
				{
					for ( i = 0; i < gc_state->cl.ri_model_num; i++ )
					{
						G_DrawModel( gc_state->cl.ri_models[i]->md->cs_root );
					}
				}
				
				R_RenderView();		

				{ // do sound gerecht origin and view
					fp_t	theta;
					Vec3dCopy( snd_origin, g_st->render_origin );
					theta = g_st->view_lon * D2R;
					snd_view[0] = cos( theta );
					snd_view[1] = 0.0;
					snd_view[2] = sin( theta );
				}	
			}
		}

		//
		// do draw infos
		//

		glMatrixMode (GL_PROJECTION);                                                
		glLoadIdentity ();                                                           
		glOrtho (0.0, 1.0, 0.0, 1.0, -1.0, 1.0 );
		glMatrixMode(GL_MODELVIEW);  
		glLoadIdentity ();         
		glDisable( GL_DEPTH_TEST );
		glEnable( GL_TEXTURE_2D );
		glEnable( GL_BLEND );

		for ( i = 0; i < gc_state->cl.di_rect_num; i++ )
		{
			di_rect_t	*rect;
			res_gltex_cache_t	*cache;
			int		texobj;
			
			rect = gc_state->cl.di_rects[i];
			__chkptr( rect );
			
			cache = ( res_gltex_cache_t * ) rect->gltex->res_cache;
			__chkptr( cache );
			texobj = cache->texobj;
			glBindTexture( GL_TEXTURE_2D, texobj );

			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

			if ( cache->comp == resGltexComponents_rgb )
			{
				glBlendFunc( GL_ONE, GL_ZERO );	
			}
			else
			{
				glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
//				glBlendFunc( GL_ONE, GL_ONE );	
			}
			
			glColor4ubv( rect->flat_color );
			
			glBegin( GL_TRIANGLE_FAN );
			glTexCoord2f( rect->tx, rect->ty );
			glVertex2f( rect->x, rect->y );
			
			glTexCoord2f( rect->tx+rect->tw, rect->ty );
			glVertex2f( rect->x+rect->w, rect->y );
			
			glTexCoord2f( rect->tx+rect->tw, rect->ty+rect->th );
			glVertex2f( rect->x+rect->w, rect->y+rect->h );
			
			glTexCoord2f( rect->tx, rect->ty+rect->th );
			glVertex2f( rect->x, rect->y+rect->h );
			glEnd();
		}
	}
	else
	{
		// for clear buffers
		R_ClearView();
	}

	if( rendershot->ivalue )
	{
		unsigned int	time;
		char	filename[256];

		time = SYS_GetSec();
		
		sprintf( filename, "shot%d.tga", time );

		SHP_SetVar( "rendershot", "0", 0 );
		cprintf( "dumping %s\n", filename );
		R_ScreenDump( filename );
	}


	LA_BeginDraw();
// ========================================
// draw di_text

	for ( i = 0; i < gc_state->cl.di_text_num; i++ )
	{
// we have to un normalize our coords again. ( this is crap )
		
		LA_DrawString( gc_state->cl.di_texts[i]->string, gc_state->cl.di_texts[i]->x * 640.0, gc_state->cl.di_texts[i]->y * 480.0 );
	}

	HUD_Show();
	if( ( SHP_GetVar( "nightvision" )->ivalue ))
		LA_DrawGltexByName( "gltex.fx.vision", 192, 112, 0, 0, DRAW2D_DRAWMODE_ADD_RGB );

	if( !gc_state->gc_in_game )
	{
		LA_DrawGltexByName( "misc.tex.title", 0, 0, 640, 480, DRAW2D_DRAWMODE_REPLACE );

#if 0
		if( (m_counter/4)&1 )
		{

			LA_DrawGltexByName( "misc.tex.extra_vote" , 32, 480 - 160, 128, 128, DRAW2D_DRAWMODE_ARGB );
		}
#else
		switch( (m_counter/8)&7 )
		{
		case 0:
			LA_DrawGltexByName( "misc.tex.extra_vote" , 32, 480 - 160, 128, 128, DRAW2D_DRAWMODE_ARGB );
			break;

		case 2:
			LA_DrawGltexByName( "misc.tex.extra_font" , 32, 480 - 160, 128, 128, DRAW2D_DRAWMODE_ARGB );
			break;
		case 4:
			LA_DrawGltexByName( "misc.tex.extra_color" , 32, 480 - 160, 128, 128, DRAW2D_DRAWMODE_ARGB );
			break;
			
		default:
			break;
		}
#endif
		
	}

	if( (( sh_var_t *)SHP_GetVar( "yesnotest" ))->ivalue )
	{	
		bla+=(int)( SHP_GetVar( "slidertest" )->fvalue * 10 );
		LA_DrawGltexByName( "menu.tex.main_title", bla%640, bla%480, 0, 0, DRAW2D_DRAWMODE_ARGB );
	} 
#if 0
else 
	{
		LA_DrawGltexByName( "menu.tex.main_title", 640 - 256, 0, 0, 0, DRAW2D_DRAWMODE_ARGB );
	}

#endif

	LA_ShowVarviews();

	if( !gc_isshell )
		SHV_DrawSmall( 0, 480 - 16 * 3 );

//	if( gc_ismenu )
	SHM_Draw();

	if( gc_isshell )
		SHV_Draw( 0, 480 - 320 );

	if( gc_renderframe - ib_lastaction < 32 )
		LA_DrawGltexByName( "misc.tex.load", 608, -(gc_renderframe - ib_lastaction), 0, 0, DRAW2D_DRAWMODE_ARGB );

	if( gc_renderframe - gc_lastbusy < 32 )
		LA_DrawGltexByName( "misc.tex.busy", 576, -(gc_renderframe - gc_lastbusy), 0, 0, DRAW2D_DRAWMODE_ARGB );





	mx += md_x;
	my -= md_y;

	if( mx > 640 )
		mx = 640;
	if( mx < 0 )
		mx = 0;

	if( my > 480 )
		my = 480;
	
	if( my < 0 )
		my = 0;

	if( gc_ismenu )
	{
		LA_DrawGltexByName( "misc.tex.busy", mx, my, 0, 0, DRAW2D_DRAWMODE_ARGB ); 
	}
//	if( gc_isloading )
	{
		SHV_DrawLoading( gc_state->gc_is_loading, 192, 224 );
	}


	LA_EndDraw();


	gc_renderframe++;

	TFUNC_LEAVE;
}
	


// ====================
// this one gives back
// time to the user interface
// does one renderframe
// ====================

static int gc_lastgiveback = 0;

void GC_GiveBackTime()
{
	unsigned int	now;
	char	name[256];
		
	TFUNC_ENTER;
	
	if( (!gc_inmainloop) || (!gc_state->gc_is_loading) )
	{
		TFUNC_LEAVE;
		return;
	}

	now = SYS_GetMsec();

//	__named_message( "%d %d\n", now, gc_lastgiveback );
	if( now - gc_lastgiveback < 100 )
	{
		TFUNC_LEAVE;
	}

	R_ClearView();

	LA_BeginDraw();

	sprintf( name, "misc.tex.levels.%s", ((sh_var_t *)SHP_GetVar( "gc_map"))->string );

	if( !G_ResourceCheck( g_rs, name ))
		sprintf( name, "misc.tex.levels.default" );
       

	LA_DrawGltexByName( name, 0, 0, 640, 480, DRAW2D_DRAWMODE_REPLACE );
	
	if( gc_renderframe - ib_lastaction < 32 )
		LA_DrawGltexByName( "misc.tex.load", 608, -(gc_renderframe - ib_lastaction), 0, 0, DRAW2D_DRAWMODE_ARGB );

	if( gc_renderframe - gc_lastbusy < 32 )
		LA_DrawGltexByName( "misc.tex.busy", 576, -(gc_renderframe - gc_lastbusy), 0, 0, DRAW2D_DRAWMODE_ARGB );


	LA_ShowVarviews();

	{
		SHV_DrawLoading( gc_state->gc_is_loading, 192, 224 );
	}

	LA_EndDraw();

	R_SwapBuffer();

	gc_renderframe++;

	TFUNC_LEAVE;
}
