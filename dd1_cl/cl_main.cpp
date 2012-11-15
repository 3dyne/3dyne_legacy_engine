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



// cl_main.c

#include "cl_state.h"
#include "cl_head.h"

#include "a_shared.h"

#include "lib_hobj.h"
#include "lib_container.h"
#include "events.h"

#include "shock.h"

hobj_t		*cl_at_root;

u_list_t	cl_ati_list;
u_list_t	cl_ati_wakeup_list;	// create tmp objects, that change to cl_ati_list
u_map_t		cl_id_to_ati_map;


msec_t		cl_time_begin_frame;	// begin of client frame in client_time
msec_t		cl_time;		// dto
msec_t		cl_time_end_frame;	// end of client frame in client_time
//msec_t		cl_time_delta_frame;
fp_t		cl_time_delta;		// duration in sec of current client frame

int		cl_frame_count;

//
// server frame time
//
msec_t		cl_sv_time_begin_frame;	// begin of server frame in server_time
msec_t		cl_sv_time_end_frame;	// end of server frame in server_time

fp_t		cl_sv_time;		// time in sec since current server frame arrived
fp_t		cl_sv_time_delta;		// duration in sec of current server frame


//
// all client player view objects put their hud into the 
// cl_huds table, for frag-hud
// this is done each client during run
//

int		cl_num_hud;
hud_player_t       *cl_huds[CL_MAX_HUD];


/*
  ==============================
  CL_Init

  ==============================
*/
void HUD_DrawFragRanking();

void CL_Init( void )
{
	__named_message( "\n" );

	TFUNC_ENTER;

	U_InitList( &cl_ati_list );
	U_InitList( &cl_ati_wakeup_list );
	U_InitMap( &cl_id_to_ati_map, map_default, Util_AtiCompareKeys, Util_AtiGetKey );

	CL_SwitchableMapReset();

	cl_time = 0;
	cl_time_begin_frame = 0;
	cl_time_end_frame = 0;
	
	cl_sv_time = 0.0;
	cl_sv_time_delta = 0.0;

	cl_frame_count = 0;

	TFUNC_LEAVE;
}

/*
  ==============================
  CL_CleanUp

  ==============================
*/
void CL_CleanUp( void )
{
	__named_message( "\n" );
}

/*
  ==============================
  CL_RunFrame

  ==============================
*/
const char * CL_RunFrame( const char *in )
{
	int		num_ev;

	int		num_wakeup;
	int		num_run;
	int		num_die;

	u_list_iter_t	iter;
	cl_ati_t	*ati;

	char		tkn[256];
	const char	*tkn_ptr;

//	__named_message( "\n" );

	TFUNC_ENTER;

	tkn_ptr = in;

	//
	// get current frame time
	//
	Parse_GetToken( &tkn_ptr, tkn, 256 );
	if ( !tkn[0] )
	{
		__warning( "missing current time in call\n" );
		TFUNC_LEAVE;
		return "fail";
	}
	else
	{		
		cl_time_begin_frame = cl_time_end_frame;
		cl_time_end_frame = atoi( tkn );

		if ( cl_time_end_frame <= cl_time_begin_frame )
		{
			__warning( "time end < time begin\n" );
			TFUNC_LEAVE;
			return "fail";
		}
		else
		{
			cl_time_delta = (cl_time_end_frame-cl_time_begin_frame)/1000.0;
		}
	}

	if ( cl_sv_time >= cl_sv_time_delta )
	{
//		__warning( "client reached end of current server frame\n" );
	}

#if 0
	__named_message( "times: last %d, current %d, delta %d\n", 
			 cl_time_last_frame,
			 cl_time_current_frame,
			 cl_time_delta_frame );
#endif

	

	//
	// client frame prepare
	//

	cl_time = cl_time_begin_frame;

	// for hud
	cl_num_hud = 0;
      
	// fixme: proxy killboxes stay only for one client frame,
	// should there be a timeout
	CL_ProxyKillBoxMapReset();


	//
	// dispatch all events in the input buffer
	//

	for( num_ev = 0;; num_ev++ )
	{
		int		remain;

// 		int		frm_size;
		byte_iter_t	frm;

		unique_t		id;
		eventType		ev_type;

		remain = ByteIter_GetSize( bi_in ) - ByteIter_GetOfs( bi_in );
		if ( remain == 0 )
			break;

		/*frm_size =*/ UnpackFrame_Begin( bi_in, &frm );
		
		id = ByteIter_Unpacki( &frm );
		ev_type = (eventType)ByteIter_Unpacki( &frm );

		if ( id == UNIQUE_INVALIDE )
		{
			if ( ev_type >= eventType_client_info_first &&
			     ev_type <= eventType_client_info_last )
			{
				if ( ev_type == eventType_client_info_time )
				{
					// server frame time info
					cl_sv_time_begin_frame = ByteIter_Unpacki( &frm );
					cl_sv_time_end_frame = ByteIter_Unpacki( &frm );
					
					cl_sv_time = 0.0;
					cl_sv_time_delta = (cl_sv_time_end_frame-cl_sv_time_begin_frame)/1000.0;
				}
				else
				{
					__error( "unknown info event" );
				}
			}
			else if ( ev_type >= eventType_client_proxy_first &&
				  ev_type <= eventType_client_proxy_last )
			{
				if( ev_type == eventType_client_proxy_killbox )
				{
					CL_ProxyKillBoxMapAdd( &frm );
				}
			}
			else if ( ev_type >= eventType_client_gfx_first &&
				  ev_type <= eventType_client_gfx_last )
			{
				if ( ev_type == eventType_client_gfx_create )
				{
					CL_CreateGfxFromEvent( &frm );
				}
			}
			else
			{
				__error( "unknown anonymous event\n" );
			}
		}
		else
		{
			ati = Util_AtiSearchById( id );
		
			if ( ev_type < eventType_client_first ||
			     ev_type > eventType_client_last )
			{
				__error( "not a 'client' event" );
			}
			
			if ( !ati )
			{
				//
				// if no client object exists for the event
				// only typeinfo events are allowed to create object
				//
				
				if ( ev_type > eventType_client_typeinfo_first &&
				     ev_type < eventType_client_typeinfo_last )
				{
					switch( ev_type )
					{
					case eventType_client_typeinfo_player:
					{
//						__named_message( "'typeinfo_player' event\n" );
						ati = CL_CreatePlayer( id, &frm );
						break;
					}
					case eventType_client_typeinfo_view:
					{
//						__named_message( "'typeinfo_view' event\n" );
						ati = CL_CreateView( id, &frm );
						break;
					}
					case eventType_client_typeinfo_item:
					{
//						__named_message( "'typeinfo_item' event\n" );
						ati = CL_CreateItem( id, &frm );
						break;
					}
					case eventType_client_typeinfo_psys:
					{
//						__named_message( "'typeinfo_psys' event\n" );
						ati = CL_CreatePsys( id, &frm );
						break;
					}
					case eventType_client_typeinfo_rocket:
					{
//						__named_message( "'typeinfo_rocket' event\n" );
						ati = CL_CreateRocket( id, &frm );
						break;
					}
					default:
					{
						__error( "unkown typeinfo\n" );
					}
					}
					
					ati->state = clAtiState_run;
					U_ListInsertAtHead( &cl_ati_list, ati );
					Util_AtiRegisterId( ati );				
				}
				else
				{
					__warning( "ignore event for not existing client object '#%u'\n", id );
				}
			}
			else
			{
				
				if ( ev_type > eventType_client_typeinfo_first &&
				     ev_type < eventType_client_typeinfo_last )
				{
//					__warning( "ignore typeinfo event for existing client object '#%u'\n", id );
				}
				else if ( ev_type > eventType_client_obj_update_first &&
					  ev_type < eventType_client_obj_update_last )
				{
					if ( !ati->ftbl->UnpackEvent )
					{
						__error( "object has no unpack ability\n" );
					}
					else
					{
						ati->ftbl->UnpackEvent( ati, id, ev_type, &frm );
					}
				}
				else if ( ev_type > eventType_client_obj_destroy_first &&
					  ev_type < eventType_client_obj_destroy_last )
				{
					ati->state = clAtiState_dead;
				}
				else
				{
					__error( "unknown event for existing client object '#%u'\n", id );
				}
			}
		}
		UnpackFrame_End( bi_in, &frm );
	}


	if ( num_ev > 0 )
	{
		// a server frame arrived

		
	}

	num_wakeup = 0;
	num_run = 0;
	num_die = 0;

	//
	// move wakeup objects to run list
	//

	for ( ; ( ati = (cl_ati_t*)U_ListRemoveAtHead( &cl_ati_wakeup_list ) ) ; )
	{
		ati->state = clAtiState_run;
		U_ListInsertAtHead( &cl_ati_list, ati );
		num_wakeup++;
	}

	//
	// run client objects
	//

	U_ListIterInit( &iter, &cl_ati_list );
	for ( ; ( ati = (cl_ati_t*)U_ListIterNext( &iter ) ) ; )
	{
		if ( ati->ftbl->Run && ( ati->state == clAtiState_none || ati->state == clAtiState_run ) )
		{
			ati->ftbl->Run( ati );
			num_run++;
		}
	}

	//
	// do hud frag
	//
	HUD_DrawFragRanking();

	//
	// remove dead objects
	// 

	U_ListIterInit( &iter, &cl_ati_list );
	for ( ; ( ati = (cl_ati_t*)U_ListIterNext( &iter ) ) ; )
	{
		if ( ati->state == clAtiState_dead )
		{
			U_ListIterRemoveGoNext( &iter );
			
			if ( ati->id != UNIQUE_INVALIDE )
			{
				Util_AtiUnregisterRegisterId( ati );	
			}
			ati->ftbl->Destroy( ati );
			num_die++;
		}		
	}

	cl_frame_count++;

	cl_sv_time += cl_time_delta;
//	__named_message( "client: %d wakeup, %d run, %d die\n", num_wakeup, num_run, num_die );	
	TFUNC_LEAVE;
	return "ok";
}

/*
  ==============================
  CL_WorldSpring

  ==============================
*/
const char * CL_WorldSpring( const char *in )
{
	sh_var_t	*tmp;
	
	char		path[256];
	
	hobj_search_iterator_t	iter;
	hobj_t		*obj;

	static struct { const char *key; cl_ati_t *(*func)(hobj_t *); } resolves[] = {
		{ "switch", CL_CreateSwitch },
		{ "light", CL_CreateLight },
		{ "spotlight", CL_CreateLight },
		{ "facelight", CL_CreateLight },
		{ NULL, NULL }
	};

	__named_message( "\n" );

	tmp = g_api.SH_GetVar( "gc_map" );
	g_api.Printf( "gc_map is '%s'\n", tmp->string );

	//
	// load archetype class
	//

	sprintf( path, "world/%s.d/ats.hobj", tmp->string );
	g_api.Printf( "archetypes come from: '%s'\n", path );

	cl_at_root = g_api.GI_ReadClass( path );
	if ( !cl_at_root )
	{
		__error( "can't load class\n" );
	}

	InitClassSearchIterator( &iter, cl_at_root, "*" );
	for ( ; ( obj = SearchGetNextClass( &iter ) ) ; )
	{
		int		i;

//		g_api.Printf( "[%s,%s]\n", obj->type, obj->name );

		for ( i = 0; resolves[i].key ; i++ )
			if ( !strcmp( resolves[i].key, obj->type ) ) break;
		
		if ( !resolves[i].key )
		{
//			printf( " no func found !\n" );
		}
		else
		{
			cl_ati_t	*ati;

//			printf( " func found !\n" );

			ati = resolves[i].func( obj );
			
			U_ListInsertAtHead( &cl_ati_list, ati );
			Util_AtiRegisterId( ati );
		}
	}
	
	return "ok";	
}
