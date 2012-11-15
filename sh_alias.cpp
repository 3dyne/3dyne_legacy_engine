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



#include <stdlib.h>
#include "interfaces.h"


#include "db_lightmap.h"

/*
  ====================
  Normal sh commands
  ====================
*/
void ALIAS_AddSource()
{
	int	stype;
	char	tmp[256];
	char	*prefix;	
	
	stype = IB_ProbeSource( shargs[1] );
	if( stype == -1 )
		SHV_Printf( "cannot probe source type of %s", shargs[1] );
	
	if( ( shargs[1][0] == '.' ) || ( shargs[1][0] == '/' ) || ( shargs[1][1] == ':' ))
	{
		printf( "absolute path\n" );
		IB_AddSource( shargs[1], stype );
		SHV_Printf( "source %s added", shargs[1] );
	} else
	{
		
		prefix = shargs[1];
		memset( tmp, 0, 256 );
		if( shargs[1][0] == '~' )
		{
			strcat( tmp, padir );
			prefix++;
		} else
		{
			strcat( tmp, "." );
		}
		/*
		  else if( SH_GetVar( "game" ))
			strcat( tmp, SH_GetVar( "game" ));
		else
		{
			SH_Printf( "var \"game\" not set\nusing tgfkaa1\n" );
			strcat( tmp, "tgfkaa1" );
		}
		*/
		strcat( tmp, "/" );
		strcat( tmp, prefix );
		IB_AddSource( tmp, stype );
		//	SH_Printf( "source %s added", tmp );
	}

}

void ALIAS_SetVar()
{
//	__named_message( "%s %s\n", shargs[1], shargs[2] );
	SHP_SetVar( shargs[1], shargs[2], 0 );
}

void ALIAS_ShowVar()
{
	sh_var_t	*var;
	var = SHP_GetVar( shargs[1] );
	SHV_Printf( "var \"%s\" is \"%s\"", shargs[1], var->string );
//	raise( 11 );
}

void ALIAS_SetEnv( const char* var, const char* value )
{
	char	text[256];
	
	if( !shargs[1] )
	{
		__warning( "var is a null pointer\n" );
		return;
	}
	if( !shargs[2] )
	{
		__warning( "value is a null pointer\n" );
		return;
	}

//	setenv( shargs[1], shargs[2], 1 );
	sprintf( text, "%s=%s", shargs[1], shargs[2] );

	putenv( text );
}

void ALIAS_RunScript()
{
	ib_file_t	*h;

//	__named_message( "%s\n", shargs[1] );

	h = IB_Open( shargs[1] );
	SHP_ParseFile( h );
	IB_Close( h );
}

void ALIAS_SHToggle()
{
	if( gc_isshell )
		gc_isshell = 0;
	else
		gc_isshell = 1;

}

void ALIAS_Echo()
{
	SHV_Printf( shargs[1] );
}

void ALIAS_R_Grab()
{
	int	flag;

	flag = atoi( shargs[1] );

	if( flag )
		R_Hint( R_HINT_GRAB_MOUSE );
	else
		R_Hint( R_HINT_UNGRAB_MOUSE );
}

void SHM_SetCurPage( const char *name );

void ALIAS_MenuPage()
{
	char	name[256];
	strncpy( name, shargs[1], 255 );
	SHM_SetCurPage( name );
}

void ALIAS_PlaySound()
{
	char	name[256];
	vec3d_t		vec;

	strncpy( name, shargs[1], 255 );
	SND_AddJobByName( name, SND_TYPE_LOCAL, 1, vec );
}

void ALIAS_Toggle()
{
	char	varname[256];

	strncpy( varname, shargs[1], 255 );

	if( (( sh_var_t *)SHP_GetVar( varname ))->ivalue )
		SHP_SetVar( varname, "0", 0 );
	else
		SHP_SetVar( varname, "1", 0 );
}

// ========================================
// modified toggle ( +toggle )
// set var to "1" on keypress and to "0" on release
// ========================================

void IALIAS_Toggle()
{
	char	varname[256];

	strncpy( varname, shargs[1], 255 );

	if( sh_holdhint )
		SHP_SetVar( varname, "0", 0 );
	else
		SHP_SetVar( varname, "1", 0 );
}

void ALIAS_StopSound()
{
	SND_StopAllJobs();
}

void ALIAS_MemInfo()
{
	MM_ShowInfo();
}
    

void ALIAS_C64Run()
{
	int	i;

	for( i = 1; i <= 10; i++ )
		SHV_Printf( "%d\n", i );
}

void ALIAS_C64List()
{
	SHV_Printf( "10 for i = 1 to 10\n" );
	SHV_Printf( "20 print i\n" );
	SHV_Printf( "30 next\n" );
}

void ALIAS_Quit()
{
	// async gc_state update
	gc_state->u_quit = true;
}

void ALIAS_Dlevel_Start()
{
	char	path[256];

	sprintf( path, "world/%s.d", ((sh_var_t *)SHP_GetVar( "direct_level" ))->string );


	SHP_SetVar( "gc_map", path, 0 );
	
	strcat( path, "/fake" );
	SHP_SetVar( "fake_level", path, 0 );
}	

void ALIAS_Who()
{
	int	i;

	for( i = 0; i < GC_STATE_MAX_CONNECTED_CLIENTS; i++ )
	{
		if( gc_state->connected_cl_tbl[i].state != connectedClientState_is_init )
			continue;

		SHV_Printf( "%d: %s\n", i, gc_state->connected_cl_tbl[i].host.addr_str );
	}
}
	       
void ALIAS_AddPlayer()
{	
	GC_AddPlayer( atoi(shargs[1]) );
}


void ALIAS_AddDummy()
{
	GC_AddPlayer( GC_GetUniqueID( gc_state ));
}

void ALIAS_StartLocalGame()
{	
	gc_state->u_start_public_server = true;
}

void ALIAS_StartRemoteGame()
{
	gc_state->u_connect_public_server = true;
}

void ALIAS_StartSingle()
{
	gc_state->u_start_single = true;
}

void ALIAS_StartDemo()
{
	gc_state->u_start_demo = true;
}

void ALIAS_DropGame()
{
	// fixme: u_drop_game cause a sig11
	gc_state->u_start_demo = true;
}


/*
  ====================
  Input-handling commands
  ====================
*/

void IALIAS_Forward()
{
//	SHV_Printf( "hh: %d", sh_holdhint );

//	printf( ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>> forward\n" );
	
//	if( local_hflags&AHF_FORWARD )
	if( sh_holdhint )
		local_hflags &= ~AHF_FORWARD;
	else
		local_hflags |= AHF_FORWARD;
}

void IALIAS_Back()
{
//	if( local_hflags&AHF_BACK )
	if( sh_holdhint )
		local_hflags &= ~AHF_BACK;
	else
		local_hflags |= AHF_BACK;
}

void IALIAS_Left()
{
//	if( local_hflags&AHF_TLEFT )
//	printf( "IALIAS_Left: %d\n", sh_holdhint );
	if( sh_holdhint )
		local_hflags &= ~AHF_TLEFT;
	else
		local_hflags |= AHF_TLEFT;
}

void IALIAS_Right()
{
//	if( local_hflags&AHF_TRIGHT )

	if( sh_holdhint )
		local_hflags &= ~AHF_TRIGHT;
	else
		local_hflags |= AHF_TRIGHT;
}

void IALIAS_StrafeLeft()
{
	if( sh_holdhint )
		local_hflags &= ~AHF_SLEFT;
	else
		local_hflags |= AHF_SLEFT;
}

void IALIAS_StrafeRight()
{


	if( sh_holdhint )
		local_hflags &= ~AHF_SRIGHT;
	else
		local_hflags |= AHF_SRIGHT;
}

void IALIAS_Fire0()
{

	if( sh_holdhint )
		local_hflags &= ~AHF_FIRE0;
	else
		local_hflags |= AHF_FIRE0;

}

void IALIAS_Use()
{

	if( sh_holdhint )
		local_hflags &= ~AHF_USE;
	else
		local_hflags |= AHF_USE;

}

void IALIAS_Jump()
{

	if( sh_holdhint )
		local_hflags &= ~AHF_JUMP;
	else
		local_hflags |= AHF_JUMP;

}

void IALIAS_Strafe()
{

	if( sh_holdhint )
		local_hflags &= ~AHF_STRAFE;
	else
		local_hflags |= AHF_STRAFE;

}

void IALIAS_LookUp()
{
	if( sh_holdhint )
		local_hflags &= ~AHF_LOOKUP;
	else
		local_hflags |= AHF_LOOKUP;
}

void IALIAS_LookDown()
{
	if( sh_holdhint )
		local_hflags &= ~AHF_LOOKDOWN;
	else
		local_hflags |= AHF_LOOKDOWN;
}


