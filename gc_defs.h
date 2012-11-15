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



// gc_defs.h

#ifndef gc_defs_h
#define gc_defs_h

#include "u_defs.h"
#include "g_defs.h"		// g_map_t
#include "g_defs2.h"		// g_server_t, g_client_t 

#include "lib_queuedefs.h"
#if !defined( win32_x86 )
#include <sys/types.h>                                                          
#include <sys/socket.h>                                                         
#include <netinet/in.h>                                                         
#include <sys/time.h>                                                           
#include <arpa/inet.h>
#else
#include <winsock.h>
#endif 


#define GC_BUFFER_SIZE_SERVER	( 1024 *1 )	// server input
#define GC_BUFFER_SIZE_CLIENT	( 1024 *4 )	// server output, client input
#define GC_BUFFER_SIZE_RENDER	( 1024 *64 )	// client output, render input

#if 1
//
// remote game shell client
//

typedef enum
{
	netEv_none,

	//
	// host connect events
	//

	netEv_host2host_connect_as_client,
	netEv_host2host_connect_refuse,

	//
	// client to server events
	//

	netEv_cl2sv_first,

	// cl2sv connect stuff
	netEv_cl2sv_connect,
	netEv_cl2sv_connect_ready,
	netEv_cl2sv_disconnect,

	// cl2sv join game stuff
	netEv_cl2sv_join_ready,
	netEv_cl2sv_buf_frag,

	netEv_cl2sv_last,

	//
	// server to client events
	//

	netEv_sv2cl_first,

	// sv2cl connect stuff
	netEv_sv2cl_connect,
	netEv_sv2cl_disconnect,
	netEv_sv2cl_refuse,

	// sv2cl join game stuff
	netEv_sv2cl_game_info,
	netEv_sv2cl_buf_frag,

	netEv_sv2cl_last,

	netEv_num
} netEv;

#define GC_MAX_NET_UDP_SIZE	( 1024 )
#define GC_MAX_NET_EVENT_SIZE	( GC_MAX_NET_UDP_SIZE-sizeof(netEv) )	// max size of a udp package

#define GC_HOST_ADDR_STR_SIZE	( 32 ) // addr string "xxx.xxx.xxx.xxx:yyyyy\0"
typedef struct gc_host_addr_s
{
	struct sockaddr_in	addr;
	char			addr_str[GC_HOST_ADDR_STR_SIZE];
} gc_host_addr_t;

typedef enum
{
	connectedClientState_none,
	connectedClientState_is_init,
	connectedClientState_is_timeout,

	connectedClientState_try_connect,
	connectedClientState_try_connect_wait,
	connectedClientState_is_connect,
	connectedClientState_is_disconnected,

	connectedClientState_try_join,		// send client a game info 
	connectedClientState_try_join_wait,	// then, wait for netEv_cl2sv_join_ready
	connectedClientState_try_join_ack,	// got it, add player object to gserver
	connectedClientState_is_joined,		// then, enable gserver to gclient events

	connectedClientState_is_dead,
	connectedClientState_num
} connectedClientState;

#define GC_CONNECTED_CLIENT_TIMEOUT	( 3000 )	// 3 sec
#define GC_CONNECTED_SERVER_TIMEOUT	( 3000 )	// 3 sec

struct gc_state_s;

typedef struct gc_connected_client_s
{
	connectedClientState	state;
	gc_host_addr_t		host;		// inet addr of a connected client
	msec_t			timeout;	// time of the last recv
	bool_t			update;		// an event arrived since last run
	msec_t			timer;		// next real run of GC_ConnectedClientRun

	unique_t		id_player;	// gserver object of client

	// sv_in buffer filled with buf_frags from remote client
	int			buf_current_id;
	buffer_recv_t		buf_recv;
	int			buf_size;	
	// if > 0, the current buffer is complete
	// if = 0, the current buffer is complete and handled by a run
	// if = -1, the current buffer is incomplete
	
	char			buf[GC_BUFFER_SIZE_SERVER];
} gc_connected_client_t;

typedef enum
{
	connectedServerState_none,

	connectedServerState_is_init,
	connectedServerState_try_connect,
	connectedServerState_try_connect_wait,
	connectedServerState_is_connect,
	connectedServerState_is_disconnected,

	connectedServerState_await_game_info,	// wait for game_info from server
	connectedServerState_is_starting,	// after game_info recv, load map
	connectedServerState_is_started,	// set from local gc if map is loaded, send server a netEv_cl2sv_join_ready

	connectedServerState_is_joined,		// set after netEv_cl2sv_join_ready is sent
	
	connectedServerState_is_dead,
	connectedServerState_num
} connectedServerState;

typedef struct gc_connected_server_s
{
	connectedServerState	state;
	gc_host_addr_t		host;	// inet addr of a connected server
	
	msec_t			timeout;	// time of last recieved
	msec_t			timer;	// next real run of GC_ConnectedServerRun

	// sv_out buffer filled with buf_frags from remote server
	int			buf_current_id;
	buffer_recv_t		buf_recv;
	int			buf_size;
	// buf_size like client

	char			buf[GC_BUFFER_SIZE_CLIENT];
} gc_connected_server_t;


//
// main game control state
//
#define GC_STATE_MAX_CONNECTED_CLIENTS	( 16 )


typedef enum
{
	gcState_none,
	gcState_is_init,
	gcState_start_game,
	gcState_game_ready,
//	gcState_try_server_connect,
//	gcState_is_server_connected,

//	gcState_try_server_join,
	gcState_num
} gcState;

typedef enum
{
	gcUdpState_none,
	gcUdpState_is_init,
	gcUdpState_num
} gcUdpState;

typedef struct gc_state_s
{
	//
	// machine state
	//
	gcState			state;
	msec_t			time;
	msec_t			timeout;
	void (*timeout_func)(struct gc_state_s *);

	unique_t		id_first;
	unique_t		id_last;
	unique_t		id_next;
	
	// async state changes
	bool_t		u_start_demo;	// init game for demo
	bool_t		u_start_single;	// init game for singel player
	bool_t		u_connect_public_server;	// init game as network client
	bool_t		u_connect_failed;
	bool_t		u_start_public_server;		// init game as network server

	bool_t		u_start_game;	// start game
	bool_t		u_drop_game;	// stop game
	bool_t		u_quit;

	// current state
	bool_t		gc_is_demo;
	bool_t		gc_is_single;
	bool_t		gc_has_server_connection;
	bool_t		gc_is_public_server;
	bool_t		gc_is_loading;
	bool_t		gc_in_game;

	// local port
	gcUdpState		udp_state;
	byte_iter_t		udp_send_bi;
	byte_iter_t		udp_recv_bi;
	char			udp_send_buf[GC_MAX_NET_UDP_SIZE];
	char			udp_recv_buf[GC_MAX_NET_UDP_SIZE];

	int			local_sock;
	struct sockaddr_in	local_port;

	struct sockaddr_in	addr_last_recv;
	gc_host_addr_t		host_last_recv;

	// connected hosts

	gc_connected_client_t	connected_cl_tbl[GC_STATE_MAX_CONNECTED_CLIENTS];
	
	gc_connected_server_t	connected_sv;

	// the game map
	g_map_t			map;

	// the game/physic server
	g_server_t		sv;

	// the local game/visualization client
	g_client_t		cl;

	// 
	unique_t		id_bind_input;
	unique_t		id_bind_view;

	// event queues and their buffers
	byte_iter_t		bi_server;		// queue for input => server
	byte_iter_t		bi_client;
	byte_iter_t		bi_render;		// queue for client=> render

	char			buf_server[GC_BUFFER_SIZE_SERVER];
	char			buf_client[GC_BUFFER_SIZE_CLIENT];
	char			buf_render[GC_BUFFER_SIZE_RENDER];

	// from input
	fp_t		i_lat, i_lon;
	bool_t		i_use;
	bool_t		i_jump;
	bool_t		i_shoot;
	fp_t		i_speed;
	fp_t		i_strafespeed;

} gc_state_t;

#endif

#endif
