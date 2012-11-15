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

#include "shock.h"
#include <time.h>
#include <stdio.h>

static void (*new_shock_func)() = NULL;

void SOS_DefaultShockHandler() {
	printf( "%s: ", __PRETTY_FUNCTION__ );
	printf( "Exit program.\n");
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
	printf( "=== State of shock ! =====================\n" );
}

void SOS_Locate( char* file, char* func, int line ) {
	printf("In module %s, function: %s, line %d:\n", file, func, line);
}

void SOS_LocateFunction( char* func ) {
	printf("%s: ", func );
}

void SOS_Error(char* errtext_ptr, ... ) {
	va_list			arg_ptr;
	FILE*			log_handle;
	time_t			time_now;  

	time( &time_now );
	log_handle = fopen( "errors.log", "a" );
	
	//printf("State of shock !\n");
	printf("Error: ");
	
	va_start( arg_ptr, errtext_ptr );
	vprintf( errtext_ptr, arg_ptr );
	fprintf( log_handle, "%s", asctime(localtime( &time_now )));
	vfprintf( log_handle, errtext_ptr, arg_ptr );
	va_end( arg_ptr ); 
	
	fclose( log_handle );	

	SOS_CallShockHandler();

}

void SOS_Warning(char* warntext_ptr, ... ) {

	va_list			arg_ptr;
	
	printf("Warning: ");
	
	va_start( arg_ptr, warntext_ptr );
	vprintf( warntext_ptr, arg_ptr );
	va_end( arg_ptr ); 
}

void SOS_Message(char* text_ptr, ... ) {

	va_list			arg_ptr;

	va_start( arg_ptr, text_ptr );
	vprintf( text_ptr, arg_ptr );
	va_end( arg_ptr ); 
}

void SOS_ChkPtr( char* file, char* func, int line, void* ptr ) {
	if ( ptr==NULL ) {
		SOS_Locate( file, func, line);
		SOS_Error("Null pointer.\n");
	}
}


/*
  int main() {
  WARNING("this is a warning !\n");
  MESSAGE("and this is a message.\n");
  CHKPTR( (void*)0 );
  ERROR("hallo %d %s\n",1234, "hallo");
  }
*/
