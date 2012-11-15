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



// sh_input.c

#include "interfaces.h"

// joint flags
#define JF_INUSE	0x1
#define JF_HOLD		0x2
#define JF_CHEAT	0x4

int	sh_holdhint;

unsigned int	watch1;
unsigned int	local_hflags;
unsigned int	watch2;

void SH_ALIAS_Join();
void SHM_SetCurPage( const char *name );

typedef struct {
	unsigned int	flags;
	char	command[SH_JCOMSIZE];
	void		(*fcheat)();
	char	symname[SH_KSYMSIZE];
}	sh_joint_t;

typedef struct {
	char	symname[SH_KSYMSIZE];
	int		index;
}	sh_symname_t;

static sh_symname_t	sntable[] = {
	// special ascii key handling
	{ "ENTER", 10 },
	{ "TAB", '\t' },
	{ "BSPACE", '\b' },


	// gskeysyms >= 128
	{ "CUP", GSK_CUP },
	{ "CDOWN", GSK_CDOWN },
	{ "CLEFT", GSK_CLEFT },
	{ "CRIGHT", GSK_CRIGHT },
	{ "ALT", GSK_ALT },
	{ "CTRL", GSK_CTRL },
	{ "SHIFT", GSK_SHIFT },

	{ "M$0", GSK_FUCK0 },
	{ "M$1", GSK_FUCK1 },
	{ "INSERT", GSK_INSERT },
	{ "HOME", GSK_HOME },
	{ "PGUP", GSK_PGUP },
	{ "DEL", GSK_DEL }, // = 127 !!!
	{ "END", GSK_END },
	{ "PGDN", GSK_PGDN },

	{ "ESCAPE", GSK_ESCAPE },

	{ "F1", GSK_F1 },
	{ "F2", GSK_F2 },
	{ "F3", GSK_F3 },
	{ "F4", GSK_F4 },
	{ "F5", GSK_F5 },
	{ "F6", GSK_F6 },
	{ "F7", GSK_F7 },
	{ "F8", GSK_F8 },
	{ "F9", GSK_F9 },
	{ "F10", GSK_F10 },
	{ "F11", GSK_F11 },
	{ "F12", GSK_F12 },

	{ "MOUSE1", GSK_BUTTON1 },
	{ "MOUSE2", GSK_BUTTON2 },
	{ "MOUSE3", GSK_BUTTON3 },
	{ "MOUSE4", GSK_BUTTON4 },
	{ "MOUSE5", GSK_BUTTON5 },

	{ "", 0 }
};

static sh_joint_t	joints[SH_JOINTNUM];

void SHI_StartUp( void )
{
	int	i;
	for( i = 0; i < SH_JOINTNUM; i++ )
		memset( &joints[i], 0, sizeof( sh_joint_t ));
}

void SHI_FireKsym( keyevent_t event )
{
	int	sym, type;

	sym = event.sym;
	type = event.type;

//	printf( "type: %d\n", type );

	if( !( joints[sym].flags & JF_INUSE ))
		return;

	if( ( type == SYMTYPE_RELEASE ) && !( joints[sym].flags & JF_HOLD ))
	{
//		printf( "holdflag\n" );
		return;
	}

	if( type == SYMTYPE_RELEASE )
		sh_holdhint = 1;
	else
		sh_holdhint = 0;

	if( joints[sym].flags & JF_CHEAT && joints[sym].fcheat )
	{
//		printf( "fcheat ptr: %p found\n", joints[sym].fcheat );
		joints[sym].fcheat();
		return;
	}

//	__named_message( "command: %s\n", joints[sym].command );
	
	if( joints[sym].command )
	{
		SHP_ParseBuf( joints[sym].command );
	}

}

void SHI_Update( keyevent_t *_list )
{
	int	i, listsize;
	keyevent_t	*list;


	listsize = _list[0].sym - 1;

	list = &_list[1];

	for( i = 0; i < listsize; i++ )
	{
//		__named_message( "queued %d: %d\n", i, list[i] );
		if( ! list[i].sym )
			continue;

		SHI_FireKsym( list[i] );
		list[i].sym = (gs_ksym)0; // mark
	}
}

void SHI_SensibleUpdate( keyevent_t *_list )
{
	int	listsize, i;
	keyevent_t *list;

	listsize = _list[0].sym - 1;
	list = &_list[1];

	for( i = 0; i < listsize; i++ )
	{
		if( list[i].sym && ( list[i].type == SYMTYPE_PRESS ))
		{
			list[i].sym = (gs_ksym)0;
			SHM_SetCurPage( "main" );
			break;
			
		}
	}
}

void SH_ALIAS_Join()
{
	char	*keysym, *command;
// 	int	i;

	keysym = shargs[1];
	command = shargs[2];

	SHI_Join( command, keysym );

}

int SHI_KeysymByName( char *symname )
{
	int	i;

	if( !symname[1] )// very nice: keysym == char
		return (int) symname[0];



	for( i = 0; ; i++ )
	{
		if( !sntable[i].index )
			break;

		if( !strcmp( sntable[i].symname, symname ))
		{
			//printf( "sym %s is index %d\n", keysym, sntable[i].index );

			return	sntable[i].index;
		}
	}
	__warning( "unrsolved keysym name %s\n", symname );
	return 0;
}

void *SHI_SymnameByKeysym( int sym )
{
	int	i;

     

	for( i = 0; ;i++ )
	{
		if( !sntable[i].index )
			break;

		if( sntable[i].index == sym )
			return sntable[i].symname;
	}
	return NULL;
}					 

void SHI_Join( char* command, char* symname )
 {
	int	i;
	int	sym;

	sym = SHI_KeysymByName( symname );


	if( ( sym < 0 ) || ( sym >= 256 ))
	{
		__warning( "trying to join with bad symbol: %d\n", sym );
		return;
	}

	if( joints[sym].flags & JF_INUSE )
	{
		__warning( "cannot rejoin keysym: %d\n", sym );
		return;
	}

	joints[sym].flags = 0;

	if( strlen( command ) >= SH_JCOMSIZE )
		command[SH_JCOMSIZE-1] = 0;

	strcpy( joints[sym].command, command );

	if( strlen( symname ) >= SH_KSYMSIZE )
	{
		symname[SH_KSYMSIZE-1] = 0; 
	}
	strcpy( joints[sym].symname, symname );

	joints[sym].flags |= JF_INUSE;

	if( command[0] == '+' )
	{
		//	printf( "holdflag set\n" );
		joints[sym].flags |= JF_HOLD;
		//printf( "trying function-cheat\n" );
		for( i = 0; ; i++ )
		{
			if( !shcommands[i].shcommand )
				break;
			if( !strcmp( shcommands[i].shcommand, command ))
			{
//				printf( "sh comand found. ptr: %p\n", shcommands[i].alias );
				joints[sym].flags |= JF_CHEAT;
				joints[sym].fcheat = shcommands[i].alias;
				break;
			}
		}
		if( !joints[sym].fcheat )
		{
			__warning( "function-cheat failed.\n" );
			joints[sym].flags &= ~JF_CHEAT;
		}
	}
}

void SHI_Unjoin( int sym )
{
	SHV_Printf( "unjoining %d\n", sym );
	joints[sym].flags = 0;
}

void SHI_GetSymnamesByCommand( char *command, char *ptr[] )
{
	int	i, i2;
	
	i2 = 0;

	ptr[0] = ptr[1] = NULL;
	
	for( i = 0; i < SH_JOINTNUM; i++ )
	{
		if( !strcmp( joints[i].command, command ))
		{
			ptr[i2] = joints[i].symname;
			i2++;
		}
		if( i2 >= 2 )
			break;
	}
	
	return;
}
		
		    


void SHI_WriteJoints( FILE* h )
{
	int	i;

	if( !h )
	{
		__warning( "filehandle == NULL!\n" );
		return;
	}
	
	fprintf( h, "\n\n" );
	fprintf( h, "# join is used to bind a sh command to a key\n" );
	fprintf( h, "# usage: join <key> \"command\"\n" );
	//fprintf( h, "# ===========================================================================\n" );
		 

	for( i = 0; i < SH_JOINTNUM; i++ )
	{
		if( joints[i].flags & JF_INUSE )
		{
			fprintf( h, "join \"%s\" \"%s\"\n", joints[i].symname, joints[i].command );
		}
	}
	fprintf( h, "\n" );
}
