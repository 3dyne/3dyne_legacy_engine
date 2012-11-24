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



// client.c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "client.h"
#include "cl_main.h"

#include "shock.h"


#ifdef win32_x86
#define DD_DLL_EXPORT __declspec(dllexport)
#else
#define DD_DLL_EXPORT
#endif

extern "C" {


DD_DLL_EXPORT l_api_t *ApiExchange( g_api_t *api );
}

l_api_t	l_api;
g_api_t g_api;

char	com_result[256];

byte_iter_t	*bi_in = NULL;
byte_iter_t	*bi_out = NULL;

/*
  ==================================================
  api stuff

  ==================================================
*/


static int Init( void )
{
	g_api.Printf( "client: init\n" );
	
	CL_Init();
	
	return 1;
}

static void CleanUp( void )
{
	g_api.Printf( "client: cleanup\n" );
	CL_CleanUp();
}

static const char * CL_Com( const char *in )
{
	char		tkn[256];
	const char		*ptr;
	const char		*result;

//	__named_message( "get '%s'\n", in );

	result = "ok";

	ptr = in;
	Parse_GetToken( &ptr, tkn, 256 );

//	printf( "tkn: '%s'\n", tkn );

	if ( !tkn[0] )
	{

	}
	else if ( !strcmp( tkn, "CL_RunFrame" ) )
	{
		result = CL_RunFrame( ptr );
	}
	else if ( !strcmp( tkn, "CL_WorldSpring" ) )
	{
		result = CL_WorldSpring( ptr );
	}
	else
	{
		__warning( "can't handle input\n" );
		result = "fail";
	}
	
	return result;
}


static void CL_SetInputByteIter( byte_iter_t *bi )
{
//	__named_message( "byte_iter is %p\n", bi );

	bi_in = bi;
}

static void CL_SetOutputByteIter( byte_iter_t *bi )
{
//	__named_message( "byte_iter is %p\n", bi );

	bi_out = bi;
}

DD_DLL_EXPORT l_api_t *ApiExchange( g_api_t *api )
{
	memset( &l_api, 0, L_API_SIZE );

	if( api->version < G_API_VERSION )
	{
		l_api.version = API_LESS;
		return &l_api;
	} else if( api->version > G_API_VERSION )
	{
		l_api.version = API_MORE;
		return &l_api;
	}
	l_api.version = L_API_VERSION;

	if( api->size < G_API_SIZE )
	{
		l_api.size = API_LESS;
		return &l_api;
	} else if( api->size > G_API_SIZE )
	{
            l_api.size = API_MORE;
		return &l_api;
	}
  	l_api.size = L_API_SIZE;

	memcpy( &g_api, api, G_API_SIZE );

	SOS_SetShockHandler( g_api.ShockHandler );

	l_api.Init = Init;
	l_api.CleanUp = CleanUp;
	l_api.Com = CL_Com;
	l_api.SetInputByteIter = CL_SetInputByteIter;
	l_api.SetOutputByteIter = CL_SetOutputByteIter;

	strcpy( l_api.owner, "client dll" );
	
	l_api.valid = 1;
	return &l_api;
}
