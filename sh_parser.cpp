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



// sh_parser.c
#include "interfaces.h"
#include "sh_alias.h"

#include <ctype.h>



sh_command_t shcommands[] = {
	{ "addsource", 1, ALIAS_AddSource, "addsouce <sourcename>" },
	{ "let", 2, ALIAS_SetVar, "let <var> <string>" },
	{ "set", 2, ALIAS_SetVar, "set <var> <string>" },
	{ "showvar", 1, ALIAS_ShowVar, "showvar <var>" },
// 	{ "setenv", 2, ALIAS_SetEnv, "setenv <var> <value>" }, 
	{ "runscript", 1, ALIAS_RunScript, "runscript <script.gsh>" },
	{ "join", 2, SH_ALIAS_Join, "join <key> <command>" },  // implemented in shell.c
	{ "sh_toggle", 0, ALIAS_SHToggle, "sh_toggle" },
	{ "echo", 1, ALIAS_Echo, "echo <string>" },
	{ "r_grab", 1, ALIAS_R_Grab, "r_grab 0/1" },
	{ "dlevel_start", 0, ALIAS_Dlevel_Start, "bla" },
	{ "menupage", 1, ALIAS_MenuPage, "menupage <string>" },
	{ "playsound", 1, ALIAS_PlaySound, "playsound <sound resource>" },
	{ "toggle", 1, ALIAS_Toggle, "toggle <varname>" },
	{ "stopsound", 0, ALIAS_StopSound, "stopsound" },
	{ "meminfo", 0, ALIAS_MemInfo, "meminfo" },
	{ "list", 0, ALIAS_C64List, "list" },
	{ "run", 0, ALIAS_C64Run, "run" },
	{ "who", 0, ALIAS_Who, "who" },

	{ "addplayer", 1, ALIAS_AddPlayer, "addplayer <unique_id>" },
	{ "adddummy", 0, ALIAS_AddDummy, "adddummy" },
	{ "startlocalgame", 0, ALIAS_StartLocalGame, "startlocalgame" },
	{ "startremotegame", 0, ALIAS_StartRemoteGame, "startremotegame" },
	{ "dropgame", 0, ALIAS_DropGame, "dropgame" },
	{ "startdemo", 0, ALIAS_StartDemo, "startgame" },
	{ "startsingle", 0, ALIAS_StartSingle, "startsingle" },
	{ "quit", 0, ALIAS_Quit, "quit" },


	{ "+toggle", 1, IALIAS_Toggle, "+toggle <varname>" },
	{ "+forward", 0, IALIAS_Forward, "toggle forward" },
	{ "+back", 0, IALIAS_Back, "toggle back" },
	{ "+left", 0, IALIAS_Left, "toggle left" },
	{ "+right", 0, IALIAS_Right, "toggle right" },
	{ "+strafeleft", 0, IALIAS_StrafeLeft, "toggle strafe left" },
	{ "+straferight", 0, IALIAS_StrafeRight, "toggle strafe right" },
	{ "+fire0", 0, IALIAS_Fire0, "toggle fire waepon 0" },
	{ "+use", 0, IALIAS_Use, "use" },
	{ "+jump", 0, IALIAS_Jump, "jump" },
	{ "+strafe", 0, IALIAS_Strafe, "strafe" },
	{ "+lookup", 0, IALIAS_LookUp, "toggle look up" },
	{ "+lookdown", 0, IALIAS_LookDown, "toggle look down" },

	{ NULL, 0, NULL, NULL }
};

char	shargs[16][256];

static sh_var_t	vars[SH_VARNUM];
static int	varcount;
static sh_var_t	nullvar;

static char	sh_token[256];
static vec3d_t	snd_vec;

void SHP_StartUp( void )
{
	int	i;
	varcount = 0;
	
	for( i = 0; i < SH_VARNUM; i++ )
		memset( &vars[i], 0, sizeof( sh_var_t ));

	memset( &nullvar, 0, sizeof( sh_var_t ));
	
}


void SHP_ShutDown( void )
{
}

sh_var_t *SHP_SetVar( const char* key_, const char* string_, unsigned int flags )
{
	int	base = 10;
	int	i;

	char	*ptr;

    char key[32];
    strncpy( key, key_, 32 );
    char string[64];
    strncpy( string, string_, 64 );
    
	__chkptr( key );
	__chkptr( string );
	
	if( strlen( key ) > 32 )
		key[31] = 0;
	
	if( strlen( string ) > 64 )
		string[63] = 0;

	if( string[0] == '0' && string[1] == 'x' ) // is hex
		base = 16;
	

	for( i = 0; i < varcount; i++ )
		if( !strcmp( vars[i].key, key ))
		{
			if( !(flags & SH_SV_NOSET))
			{
				strcpy( vars[i].string, string );
				vars[i].ivalue = strtol( string, &ptr, base );
				vars[i].fvalue = atof( string );
			}
			return &vars[i];
		}

	for( i = 0; i < SH_VARNUM; i++ )
		if( !strlen( vars[i].key ))
		{
			strcpy( vars[i].key, key );
			strcpy( vars[i].string, string );
			vars[i].ivalue = strtol( string, &ptr, base );
			vars[i].fvalue = atof( string );
			varcount++;
			return &vars[i];
		}
		
	__error( "out of vars\n" );
	return NULL; // wall ...
}

sh_var_t* SHP_GetVar( const char* key )
{
	int	i;

	__chkptr( key );
	for( i = 0; i < varcount; i++ )
		if( !strcmp( vars[i].key, key ))
		{
			return &vars[i];
		}
	
       	return SHP_SetVar( key, "", 0 );
}	    

void SHP_ParseFile( ibfile_t* handle )
{
	char*	buf;
	int	size;
	__chkptr( handle );

	printf( "call!\n" );
	size = IB_GetSize( handle );
	printf( "call!\n" );


	buf = ( char* )alloca( size + 1 );
	__chkptr( buf );
	IB_Read( buf, size, 1, handle );
	buf[size] = 0;


	SHP_ParseBuf( buf );


	return;
}	

void SHP_ParseBuf( const char* buf )
{
	char	linebuf[256];
	int	i;

	memset( linebuf, 0, 255 );

	// vorsicht!: verdamt schlechter Kot. don't fix me.
	for(;;)
	{
		for( i = 0; i < 256; i++ ) // don't want to know what happens to lines longer than 255 chars ...
		{
			linebuf[i] = *buf;
		       
			if( (*buf == 10) || (*buf == '\\' ) )
			{
				linebuf[i] = 0;
				buf++;
				break;
			}
			
			if( *buf == 0 )
			{
				break;
			}
			buf++;
			
		}

		SHP_ParseLine( linebuf );
		if( *buf == 0 )
		{
			break;
		}
	} 
}

void SHP_ParseLine( char* line )
{
	int	i;
	int	lsize;
	int	argnum;


	//
	// done: wicked strtoken code replaced by general GS ParseToken


	lsize = strlen( line );

	if( lsize < 1 )
		return;

	argnum = 0;
	for( i = 0; i < 16; i++ )
	{
		memset( sh_token, 0, 255 );
		SHP_ParseToken( &line );
		if( sh_token[0] == 0 )
			break;
		strcpy( shargs[i], sh_token ); 
		argnum = i;
	}
	if( i <= 0 )
		return;

	for( i = 0;; i++ )
	{
		if( shcommands[i].shcommand == NULL )
		{
			cprintf( "!command '%s' not found\n", shargs[0] );
			SND_AddJobByName( "console.sound.fail", SND_TYPE_LOCAL, 1, snd_vec );
			break;
		}
		if( !strcmp( shcommands[i].shcommand, shargs[0] ))
		{
			if( argnum < shcommands[i].argnum )
			{
				cprintf( "!to few arguments for command %s\n", shargs[0] );
				cprintf( "usage: %s\n", shcommands[i].usage );
				SND_AddJobByName( "console.sound.fail", SND_TYPE_LOCAL, 1,snd_vec );
				break;
			}
			shcommands[i].alias();
			break;
		}
	}
} 


void SHP_ParseToken( char** buf_ptr )
{
	int	c;
	int	lquota = 0;
	int	pos = 0;
	char	*buf;
	

	//u_token = NULL;
	buf = *buf_ptr;

	if( !buf )
	{
		sh_token[0] = 0;
		return;
	}

	for( ;; )
	{
		c = *buf;

		buf++;

		if( c == '\"' )
		{
			lquota = 1;
			c = *(buf++);
			break;
		}
		
		if( !isspace( c ) )
		{
			break;
		}

		if( c == '#' )
		{
			sh_token[0] = 0;
			return;
		}
	}
	
	for( ;; )
	{
		if( c == '\"' && lquota )
			break;

		if( !c )
			break; 


		if( isspace( c )  && !lquota )
		{
			break;
		}

		if( pos >= 255 )
			__error( "token too long\n" );
		
		if( c == '#' )
		{
			sh_token[0] = 0;
			return;
		}
		
		sh_token[pos] = ( char )c;
		
		pos++;
		c = *(buf++);	
	}
	if( pos > 255 )  // double save
		__error( "token too long\n" );	
	
	sh_token[pos] = 0;
	*buf_ptr = buf;
	return;
}

/*
  ==============================
  SHP_ParseToken2

  ==============================
*/
void SHP_ParseToken2( unsigned char **buf_ptr, unsigned char *tkn, int max_tkn_size )
{
	int	c;
	int	lquota = 0;
	int	pos = 0;
	unsigned char	*buf;
	

	//u_token = NULL;
	buf = *buf_ptr;

	if( !buf )
	{
		tkn[0] = 0;
		return;
	}

	for( ;; )
	{
		c = *buf;
		buf++;

		if( c == '\"' )
		{
			lquota = 1;
			c = *(buf++);
			break;
		}
		
		if( !isspace( c ) )
		{
			break;
		}

		if( c == '#' )
		{
			tkn[0] = 0;
			return;
		}
	}
	
	for( ;; )
	{
		if( c == '\"' && lquota )
			break;

		if( !c )
			break; 


		if( isspace( c )  && !lquota )
		{
			break;
		}

		if( pos >= max_tkn_size )
			__error( "token too long\n" );
		
		if( c == '#' )
		{
			tkn[0] = 0;
			return;
		}
		
		tkn[pos] = (unsigned char)c;
		
		pos++;
		c = *(buf++);	
	}
	if( pos > max_tkn_size )  // double save
		__error( "token too long\n" );	
	
	tkn[pos] = 0;
	*buf_ptr = buf;
	return;
}

