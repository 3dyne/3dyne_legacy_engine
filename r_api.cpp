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



#include "interfaces.h"

static int 	rhandle;
r_export_t	re;
r_import_t	ri;
static int	rloaded = 0;

//r_export_t	*(*R_GetAPI_ptr)( r_import_t* );


void ImportRender()
{
	char	*rname;
	r_export_t	*tmp;
	char	dll_name[256];

//	sprintf( rname, "r_%s.so", arg_rname );




	rname = (SH_GetVar( "r_dll" ))->string;
	
	strcpy( dll_name, rname ); // make sure that there's enough space for SYS_DllAddSuffix in the string
	
	SYS_DllAddSuffix( dll_name, SYS_DLLSSYSTEM );
	__named_message( "\tImporting render %s ...\n", dll_name );

	rhandle = SYS_DllBegin( dll_name );
/*
		if( !rhandle )
	{
		__error( "dlopen failed to open '%s' with the following message:\n\"%s\"\n", rname, dlerror() );
	}
*/
	ri.shockhandler =		ShockHandler;

	ri.L_BeginLumpArchive =		L_BeginLumpArchive;
	ri.L_EndLumpArchive =		L_EndLumpArchive;
	ri.L_BeginLumpStream =		L_BeginLumpStream;
	ri.L_EndLumpStream =		L_EndLumpStream;
	ri.L_ReadLump =			L_ReadLump;

	ri.CA_CacheArr =		CA_CacheArr;
	ri.CA_CacheLump =		CA_CacheLump;
	ri.CA_CacheCia = 		CA_CacheCia;
	
	ri.SH_ParseBuf =		SH_ParseBuf;
	ri.SH_SetVar =			SH_SetVar;
	ri.SH_GetVar =			SH_GetVar;

	ri.SH_EventCallback =		SH_EventCallback;
	ri.SH_Printf =			SH_Printf;


	R_GetAPI_ptr = SYS_DllSym( rhandle, "R_GetAPI" );
	__chkptr( R_GetAPI_ptr );
	
	tmp = R_GetAPI_ptr( &ri );
	re = *tmp;
	
	re.R_StartUp();
	rloaded = 1;
}

void RejectRender()
{
	__named_message( "\n" );
	if( rloaded )
	{
		re.R_ShutDown();
		SYS_DllEnd( rhandle );
	} else
		__warning( "no render loaded\n" );

	rloaded = 0;
}
