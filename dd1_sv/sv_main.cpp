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



// sv_main.c

#include "sv_state.h"

#include "a_shared.h"	// for sh_var_t

#include "l_head.h"

#include "u_defs.h"
#include "lib_hobj.h"
#include "lib_container.h"

#include "shock.h"

hobj_t		*sv_at_root;

ati_list_t	sv_wakeup;
ati_list_t	sv_run;
ati_list_t	sv_sleep;
ati_list_t	sv_dead;

u_map_t		sv_id_to_ati_map;

int		sv_frame_count;

msec_t		sv_time;		
msec_t		sv_time_current_frame;	// dto
msec_t		sv_time_last_frame;
msec_t		sv_time_delta_frame;
fp_t		sv_time_delta;
fp_t		sv_delta_sec;

//
// player object nicknames
//

#define	SV_PLAYER_NICKNAME_MAX		( 8 )
static const char		*sv_nicknames[SV_PLAYER_NICKNAME_MAX] =
{ "mick",
  "joe",
  "topper",
  "paul",
  "battersea",
  "bernie",
  "janie",
  "rudie"
};

static bool_t		sv_nn_use[SV_PLAYER_NICKNAME_MAX];

//
// active players
//

#define SV_MAX_PLAYER	( 8 )
int		sv_num_player;

/*
  ==============================
  SV_Init

  ==============================
*/
void SV_Init( void )
{
	int		i;
	TFUNC_ENTER;

	L_InitAtiList( &sv_wakeup );
	L_InitAtiList( &sv_run );
	L_InitAtiList( &sv_sleep );
	L_InitAtiList( &sv_dead );	

	U_InitMap( &sv_id_to_ati_map, map_default, L_AtiCompareKeys, L_AtiGetKey );

	L_SwitchableMapReset();

	sv_frame_count = 0;
	sv_num_player = 0;

	for ( i = 0; i < SV_PLAYER_NICKNAME_MAX; i++ )
		sv_nn_use[i] = false;

	TFUNC_LEAVE;
}


/*
  ==============================
  SV_CleanUp

  ==============================
*/
void SV_CleanUp( void )
{
	__named_message( "\n" );
}


/*
  ==============================
  SV_RunFrame

  ==============================
*/
const char * SV_RunFrame( const char *in )
{
	int	num_sleep, num_wakeup, num_dead;
	int	num_think1, num_think2;       
	ati_t	*ati, *next;

	char		tkn[256];
	const char	*tkn_ptr;

	TFUNC_ENTER;

	//
	// get current frame time
	//
	tkn_ptr = in;

	Parse_GetToken( &tkn_ptr, tkn, 256 );
	if ( !tkn[0] )
	{
		__warning( "missing current time in call\n" );
		TFUNC_LEAVE;
		return "fail";
	}
	else
	{		
		sv_time_last_frame = sv_time_current_frame;
		sv_time_current_frame = atoi( tkn );

		if ( sv_time_current_frame <= sv_time_last_frame )
		{
			__warning( "current time < last time\n" );
			TFUNC_LEAVE;
			return "fail";
		}
		else
		{
			sv_time_delta_frame = sv_time_current_frame-sv_time_last_frame;
			sv_delta_sec = sv_time_delta_frame/1000.0;
		}
	}

#if 0
	__named_message( "times: last %d, current %d, delta %d\n", 
			 sv_time_last_frame,
			 sv_time_current_frame,
			 sv_time_delta_frame );
#endif

	if ( sv_delta_sec > 0.5 )
	{
		__warning( "clip server frame time delta !!!\n" );
		sv_delta_sec = 0.5;
	}
	
	sv_time = sv_time_current_frame;
	sv_time_delta = sv_delta_sec;

	// 
	// send server frame time info to client
	//
	{
		byte_iter_t	frm;

		PackFrame_Begin( bi_out, &frm );
		ByteIter_Packi( &frm, UNIQUE_INVALIDE );
		ByteIter_Packi( &frm, eventType_client_info_time );
		ByteIter_Packi( &frm, sv_time_last_frame );
		ByteIter_Packi( &frm, sv_time_current_frame );
		PackFrame_End( bi_out, &frm );		
	}

	//
	// dispatch all events in the input buffer
	//
	{
		int		remain;
		
		for(;;)
		{
			byte_iter_t	frm;
			
			unique_t	id;
			eventType	ev_type;

			remain = ByteIter_GetSize( bi_in ) - ByteIter_GetOfs( bi_in );
			if ( remain == 0 )
				break;
			
			/*frm_size = */UnpackFrame_Begin( bi_in, &frm );
			
			id = ByteIter_Unpacki( &frm );
			ev_type = (eventType)ByteIter_Unpacki( &frm );

			if ( ev_type < eventType_server_first ||
			     ev_type > eventType_server_last )
			{
				__error( "not a 'server' event\n" );
			}

			ati = L_AtiSearchById( id );
			
			if ( !ati )
			{
				__warning( "no server object for id '#%u' found\n", id );
			}
			else
			{
				if ( !ati->ftbl->UnpackEvent )
				{
					__error( "server object '#%u' has no ability to unpack events\n" );
				}
				else
				{
					ati->ftbl->UnpackEvent( ati, id, ev_type, &frm );
				}
			}	
			
			UnpackFrame_End( bi_in, &frm );
		}
	}

	// 
	// first move all archetypes from the wakeup_list to the run_list
	//

	num_wakeup = 0;

	for ( ; ( ati = L_AtiListRemoveAtHead( &sv_wakeup ) ) ; )
	{
		L_AtiChangeToRun( ati );
		num_wakeup++;
	}

	//
	// all archetypes in the run_list Think1 now
	//

//	L_BeginCollideMap();

	num_think1 = 0;
	num_think2 = 0;

	L_CollideMapReset();

	for ( ati = sv_run.head.next ; ati != &sv_run.tail ; ati = next )
	{
		// archetypes may change to sleep_list or dead_list
		next = ati->next;

		if ( ati->ftbl->Run )
		{
			ati->ftbl->Run( ati, atiRun_think1 );
			num_think1++;
		}
	}

	for ( ati = sv_run.head.next ; ati != &sv_run.tail ; ati = next )
	{
		// archetypes may change to sleep_list or dead_list
		next = ati->next;

		if ( ati->ftbl->Run )
		{
			ati->ftbl->Run( ati, atiRun_think2 );
			num_think2++;
		}
	}

	//
	// finally destroy all archetypes in the dead_list 
	//
	
	num_dead = 0;
	
	for ( ; ( ati = L_AtiListRemoveAtHead( &sv_dead ) ) ; )
	{
		ati->ftbl->Destroy( (ati_t*)ati );
		num_dead++;
	}
	

	//
	// count sleeping archetypes
	//

	num_sleep = 0;

	for ( ati = sv_sleep.head.next ; ati != &sv_sleep.tail ; ati=ati->next )
	{
		num_sleep++;
	}

//	printf( " life: %d sleeps, %d wakeups, %d t1, %d t2, %d deads\n", num_sleep, num_wakeup, num_think1, num_think2, num_dead );

	sv_frame_count++;
	TFUNC_LEAVE;

	return "ok";
}

/*
  ==============================
  SV_WorldSpring

  ==============================
*/
const char * SV_WorldSpring( const char *in )
{
	sh_var_t	*tmp;

	char		path[256];
	
	hobj_search_iterator_t	iter;
	hobj_t		*obj;

	static struct { const char *key; ati_t *(*func)(hobj_t *); } resolves[] = {
		{ "item", ATI_CreateItem },
		{ "psys", ATI_CreatePsys },
		{ "switch", ATI_CreateSwitch },
		{ "light", ATI_CreateLight },
		{ "spectator", ATI_CreateSpectator },
		{ "catapult", ATI_CreateCatapult },
		{ "teleport", ATI_CreateTeleport },
		{ NULL, NULL }
	};

	__named_message( "\n" );

	tmp = g_api.SH_GetVar( "gc_map" );
	g_api.Printf( "gc_map is '%s'\n", tmp->string );

	sprintf( path, "world/%s.d/ats.hobj", tmp->string );
	g_api.Printf( "archetypes come from: '%s'\n", path );

	sv_at_root = g_api.GI_ReadClass( path );
	if ( !sv_at_root )
		__error( "can't load class\n" );

	InitClassSearchIterator( &iter, sv_at_root, "*" );
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
			ati_t	*ati;
			
//			printf( " func found !\n" );
			
			ati = resolves[i].func( obj );
			L_AtiChangeToWakeUp( ati );
		}
	}
	
	return "ok";
}

/*
  ==============================
  SV_AddPlayer

  ==============================
*/

void SV_PlayerObjGetNickname( hobj_t *player_obj )
{
	int		i, j;

	j = rand()%SV_PLAYER_NICKNAME_MAX;
	for ( i = 0; i < SV_PLAYER_NICKNAME_MAX; i++ )
	{
		if ( sv_nn_use[j] == false )
		{
			sv_nn_use[j] = true;
			EasyNewString( player_obj, "nickname", sv_nicknames[j] );
			return;
		}

		j = (j+1)%SV_PLAYER_NICKNAME_MAX;
	}

	EasyNewString( player_obj, "nickname", "(no name)" );	
}

void SV_PlayerObjReleaseNickname( hobj_t *player_obj )
{
	hpair_t		*pair;
	int		i;

	pair = FindHPair( player_obj, "nickname" );
	if ( !pair )
		return;

	for ( i = 0; i < SV_PLAYER_NICKNAME_MAX; i++ )
	{
		if ( !strcmp( pair->value, sv_nicknames[i] ) )
		{
			sv_nn_use[i] = false;
			return;
		}
	}
}

const char * SV_AddPlayer( const char *in )
{
	hobj_t		*player_obj;
	
	ati_t		*player;

	unique_t		id;

	const char		*ptr;
	char		tkn[256];

	__named_message( "\n" );

	if ( sv_num_player >= SV_MAX_PLAYER )
	{
		__warning( "reached SV_MAX_PLAYER\n" );
		return "fail";
	}

	ptr = in;
	Parse_GetToken( &ptr, tkn, 256 );
	if ( !tkn[0] )
	{
		__warning( "missing player id, ignore\n" );
		return "fail";
	}

	id = StringToUnique( tkn );
	if ( id == UNIQUE_INVALIDE )
	{
		__warning( "player got an invalid id, ignore\n" );
		return "fail";
	}
	
	player = L_AtiSearchById( id );
	if ( player )
	{
		__warning( "there is already a player with this id\n" );
		return "fail";
	}

	player_obj = NewClass( "player", tkn );


	SV_PlayerObjGetNickname( player_obj );
	
	player = ATI_CreatePlayer( player_obj );	
	L_AtiChangeToWakeUp( player );

	sv_num_player++;

	return "ok";
}

/*
  ==============================
  SV_RemovePlayer

  ==============================
*/
const char * SV_RemovePlayer( const char *in )
{
	ati_t		*player;

	unique_t		id;
	
	const char		*ptr;
	char		tkn[256];

	__named_message( "\n" );

	ptr = in;
	Parse_GetToken( &ptr, tkn, 256 );
	if ( !tkn[0] )
	{
		__warning( "missing player id, ignore\n" );
		return "fail";
	}

	id = StringToUnique( tkn );
	if ( id == UNIQUE_INVALIDE )
	{
		__warning( "player got an invalid id, ignore\n" );
		return "fail";
	}
	
	player = L_AtiSearchById( id );

	if ( !player )
	{
		__warning( "no server object '#%u' found\n", id );
		return "fail";
	}

	SV_PlayerObjReleaseNickname( player->ati_obj );
	
	L_AtiChangeToDead( player );
	sv_num_player--;

	return "ok";
}
