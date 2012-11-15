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



// shock.c bla

//#include "shock.h"
#include "s_config.h"
#include <time.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "version.h"
#include "shock.h"

static void (*new_shock_func)() = NULL;

void SOS_DefaultShockHandler() {
	cprintf( "%s: ", __PRETTY_FUNCTION__ );
	cprintf( "Exit program.\n");
	exit(-1);	
}

void SOS_CallShockHandler() {

//	void (*shock_func)();
//	shock_func = SOS_DefaultShockHandler;//= new_shock_func;

	if ( new_shock_func == NULL ) {
		SOS_DefaultShockHandler();
	} else {
		new_shock_func();
	}
	
}

void SOS_SetShockHandler( void (*func_shock)() ) {

	new_shock_func = func_shock;

}


void SOS_Seperator( void )
{
	fprintf( stderr, "=== State of shock ! =================                    <snip>\n" );
	fprintf( stderr, "Version: %s\n", BVERSION );
	fprintf( stderr, "Built at %s on %s\n", BDATE, BHOST );
}

void SOS_Locate( const char* file, const char* func, int line ) {
	fprintf( stderr, "In module %s, function: %s, line %d:\n", file, func, line);
}

void SOS_LocateFunction( const char* func ) {
	cprintf("%s: ", func );
}


// Todo: what todo with stderr stuff?
void SOS_Error(const char* errtext_ptr, ... ) {
	va_list			arg_ptr;
	FILE*			log_handle;
	time_t			time_now;  

	time( &time_now );
	log_handle = fopen( "errors.log", "a" );
	
	//printf("State of shock !\n");
	fprintf( stderr, "Error: ");

	va_start( arg_ptr, errtext_ptr );
	vfprintf( stderr, errtext_ptr, arg_ptr );

	if( log_handle )
	{	
		fprintf( log_handle, "%s", asctime(localtime( &time_now )));
		vfprintf( log_handle, errtext_ptr, arg_ptr );
		fclose( log_handle );	
	}

	va_end( arg_ptr ); 
						 
	fprintf( stderr, "\n=== End of error Message =============                    <snap>\n" );

	SOS_CallShockHandler();

}

void SOS_Warning(const char* warntext_ptr, ... ) {

	va_list			arg_ptr;
	char	buf[512];

	cprintf("Warning: ");
	
	va_start( arg_ptr, warntext_ptr );
	vsprintf( buf, warntext_ptr, arg_ptr );
	va_end( arg_ptr );

	cprintf( buf );
}

void SOS_Message(const char* text_ptr, ... ) {

	va_list			arg_ptr;
	char	buf[256];

	va_start( arg_ptr, text_ptr );
	vsprintf( buf, text_ptr, arg_ptr );
	va_end( arg_ptr ); 

	cprintf( buf );
}

void SOS_ChkPtr( const char* file, const char* func, int line, const void* ptr ) {
	if ( ptr==NULL ) {
		SOS_Locate( file, func, line);
		SOS_Error("Null pointer.\n");
	}
}


// this is really prehistoric!
/*
  int main() {
  WARNING("this is a warning !\n");
  MESSAGE("and this is a message.\n");
  CHKPTR( (void*)0 );
  ERROR("hallo %d %s\n",1234, "hallo");
  }
*/
