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



// tfunc.c
#include "interfaces.h"


#define TF_MAXLEVEL	(32)

static int	tf_level = -1;
static char	*tf_names[TF_MAXLEVEL];
static int	log_h = -1;
static char	spacebuf[] = "                                                              ";

void TF_LogEnter()
{
	if( log_h == -1 )
	{
		log_h = open( "./func.log", O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP );
		if( log_h == -1 )
			__error( "sorry, you need write permissions for function loging\n" );
	}

	write( log_h, spacebuf, tf_level * 2 );
	write( log_h, tf_names[tf_level], strlen( tf_names[tf_level] ) );
	write( log_h, "\n", 1 );
//	fsync( log_h );
}


void TF_Enter( char *name )
{
	tf_level++;

	if( tf_level >= TF_MAXLEVEL )
	{
		tf_level--;
		__warning( "function tracing depth exceeded: %d\n", TF_MAXLEVEL );
		return;
	}

//	__named_message( "%s\n", name );

	// we trust that the function names are static
	tf_names[tf_level] = name;

#if tf_complete
	TF_LogEnter();
#endif

}

void TF_Leave( char *name )
{
	if( tf_level < 0 )
	{
		__warning( "no function entered to leave\n" );
		return;
	}

#if tf_complete

	if( strcmp( name, tf_names[tf_level] ))
	{
		__warning( "leaving wrong function: %s\n", name );
		return;
	}
#endif
	tf_level--;
}

void TF_FuncHistory( char *string, int maxsize )
{
	int	i;
	int	space;

#ifndef trace_functions
	strncpy( string, "unknown", maxsize );
#endif
	space = maxsize;
	
	string[0] = '\0';

	for( i = 0; i <= tf_level; i++ )
	{
		space -= strlen( tf_names[i] );
		if( space < 2 )
			return;
		strcat( string, tf_names[i] );
		strcat( string, "->" );
	}
}

void TF_LastFunction( char *string, int maxsize )
{
	if( !string )
	{
		__warning( "NULL pointer as string.\n" );
		return;
	}
	if( !maxsize )
	{
		__warning( "maxsize == 0\n" );
		return;
	}

#ifndef trace_functions
	strncpy( string, "unknown", maxsize );
	return;
#endif

	if( !tf_names[tf_level] )
	{
		strncpy( string, "unknown", maxsize );
		return;
	}

	strncpy( string, tf_names[tf_level], maxsize );
}

