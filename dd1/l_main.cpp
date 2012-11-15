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



#include <string.h>
#include "l_locals.h"

// create some extern vars declared in l_locals.h
l_export_t	myexport;
l_import_t	li;

arche_t		*archets;

int			at_num_player = 0;
at_player_t		*at_players[AT_PLAYERNUM];

hobj_t		*at_prefs;
int		local_player;
int		world_frame;

//
// lists for archetype states
//

at_list_t	l_wakeup;
at_list_t	l_run;
at_list_t	l_sleep;
at_list_t	l_dead;

hobj_t		*l_lightsource_root;

// 
// event list
//
u_list_t	cl_recv_event_list;	// comes in with Cl_SendEvent
u_list_t	cl_send_event_list;	// goes out with Cl_PollEvent

//
// who got the viewport
//
unique_t	cl_id_bind_view;

/*
  ==============================
  Cl_SendEvent

  ==============================
*/
void Cl_SendEvent( g_event_t *ev )
{
	__chkptr( ev );
	__named_message( "get event %p\n", ev );
	U_ListInsertAtTail( &cl_recv_event_list, ev );
}

/*
  ==============================
  Cl_PollEvent

  ==============================
*/
g_event_t * Cl_PollEvent( void )
{
	g_event_t *ev;

	ev = U_ListRemoveAtHead( &cl_send_event_list );
	return ev;
}

/*
  ==============================
  Cl_BindView

  ==============================
*/
void Cl_BindView( unique_t id_bind_view )
{
	cl_id_bind_view = id_bind_view;
}

#if 0
int mainCRTStartup()
{
	printf( "fuck you 0\n" );
	return 0;
}

int DllEntryPoint( int inst, int reason, void *lala )
{
	printf( "fuck you2: %d/n", reason );
	return 1;
}
#endif

arche_t *L_GetArcheTBase()
{
	return archets;
}

void testfunc( void )
{
	printf( "testfunc\n" );
}

l_export_t *L_GetAPI( l_import_t* import )
{
	li = *import; // copy import to local struct li

	SOS_SetShockHandler( li.ShockHandler );

	myexport.testfunc = testfunc;

	myexport.L_StartUp = L_StartUp;
	myexport.L_ShutDown = L_ShutDown;

	myexport.L_RunFrame = L_RunFrame;
	myexport.WorldSpring = WorldSpring;
	myexport.SpawnPlayer = PlayerSpring;


	myexport.ClientSendEvent = Cl_SendEvent;	// put event into life
	myexport.ClientPollEvent = Cl_PollEvent;	// get event from life

	myexport.ClientBindView = Cl_BindView;		// who sets G_SetRenderHintOriginIpo

	return &myexport;
}

void L_StartUp()
{
	int	i;
	__named_message( "SH_Printf: %p\n", li.SH_Printf );

	li.SH_Printf( "=====================" );
	li.SH_Printf( "  starting new life  " );
	li.SH_Printf( "=====================" );
//	li.SH_Printf( "L_InitATList: %p %p", L_InitATList, L_StartUp );

	U_InitList( &cl_recv_event_list );
	U_InitList( &cl_send_event_list );
	
	L_InitATList( &l_wakeup );
	L_InitATList( &l_run );
	L_InitATList( &l_sleep );
	L_InitATList( &l_dead );

	L_InitSwitchMap();

	L_InitWayNet();

#if 1
	for( i = 0; i < AT_PLAYERNUM; i++ )
	{
		at_players[i] = NULL;
	}
#endif
//	printf( "SH_Printf: %p", li.SH_Printf );
	li.SH_Printf( "reading archetype prefs ..." );

	at_prefs = li.GI_ReadClass( "misc/at_prefs.hobj" );

	world_frame = 0;	
}

void L_ShutDown()
{
	li.SH_Printf( "====================" );
	li.SH_Printf( "    killing life    " );
	li.SH_Printf( "====================" );
}


void L_RunFrame()
{
	int	num_sleep, num_wakeup, num_run, num_dead;
	int	num_think1, num_think2;
	ati_t	*ati, *next;
	g_event_t	*ev;

	//
	// distribute arrived events
	//
	for ( ; ( ev = U_ListRemoveAtHead( &cl_recv_event_list ) ); )
	{
		int		i;
		unique_t	id;

		U_BeginUnpack( U_PACKED_BIN, ev->buf, 256 );
		// get obj id
		U_Unpacks32( &id );
		U_EndUnpack();

		// search player with this name
		for ( i = 0; i < at_num_player; i++ )
		{
			if ( StringToUnique(at_players[i]->ati.name) == id )
			{
				// ok, found player

				// check for old events
				if ( at_players[i]->ev != NULL )
				{
					FREE( at_players[i]->ev );
					__warning( "found unhandled event in player '%s', override it!\n", at_players[i]->ati.name );
				}
				at_players[i]->ev = ev;
				break;
			}
		}

		if ( i == at_num_player )
		{
			// found no player with that name
			__warning( "found no player '#%u' for event, ignore it!", id );
			FREE( ev );
		}
	}

	// 
	// first move all archetypes from the wakeup_list to the run_list
	//

	num_wakeup = 0;

	for ( ; ( ati = L_ATListRemoveAtHead( &l_wakeup ) ) ; )
	{
		L_ChangeToRun( ati );
		num_wakeup++;
	}

	//
	// all archetypes in the run_list Think1 now
	//

	L_BeginCollideMap();

	num_run = 0;
	num_think1 = 0;
	for ( ati = l_run.head.next ; ati != &l_run.tail ; ati = next )
	{
		// archetypes may change to sleep_list or dead_list
		next = ati->next;

		if ( ati->ftbl->Think1 )
		{
			ati->ftbl->Think1( ( void * ) ati );
			num_run++;
			num_think1++;
		}
	}

	L_EndCollideMap();
	
	//
	//  all archetypes in the run_list Think2 now
	//

	num_think2 = 0;
	for ( ati = l_run.head.next ; ati != &l_run.tail ; ati = next )
	{
		// archetypes may change to sleep_list or dead_list
		next = ati->next;

		if ( ati->ftbl->Think2 )
		{
			ati->ftbl->Think2( ( void * ) ati );
			num_think2++;
		}
	}	

	//
	// finally destroy all archetypes in the dead_list 
	//
	
	num_dead = 0;
	
	for ( ; ( ati = L_ATListRemoveAtHead( &l_dead ) ) ; )
	{
		ati->ftbl->Destroy( ( void * ) ati );
		num_dead++;
	}
	

	//
	// count sleeping archetypes
	//

	num_sleep = 0;

	for ( ati = l_sleep.head.next ; ati != &l_sleep.tail ; ati=ati->next )
	{
		num_sleep++;
	}

//	printf( " life: %d sleeps, %d wakeups, %d runs( %d t1, %d t2 ), %d deads\n", num_sleep, num_wakeup, num_run, num_think1, num_think2, num_dead );

	//
	// search archetype that should be used for view
	//
	{
		char		tt[256];
		ati_t		*at;	       

		sprintf( tt, "#%u", cl_id_bind_view );

		at = L_FindArcheT( "", tt );

		if ( !at )
		{
			__warning( "name '#%u' bound to view not found, can't set view\n" );
		}
		else
		{
			vec3d_t		from;
			vec3d_t		delta;

			at->ftbl->GetOrigin( at, from );
			Vec3dInit( delta, 0, 0, 0 );
			li.G_SetRenderHintOriginIpo( 100, from, delta );
		}
	}

	world_frame++;
}
