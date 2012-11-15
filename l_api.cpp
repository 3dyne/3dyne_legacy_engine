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
#include "./r_fake/r_interface.h"
//
// linked arche_t list

#include "l_wrap.h"

//
// dll stuff

static int	lhandle;
l_export_t	le;
l_import_t	li;
static int	lloaded = 0;

l_export_t	*(*L_GetAPI_ptr)( l_import_t* );

void ImportLife( char *arg_dll )
{
	l_export_t	*tmp;
	char		dll_name[256];

	sprintf( dll_name, "%s/life", arg_dll );
//	strcpy( dll_name, arg_dll ); // make sure that there's enough space for SYS_DllAddSuffix in the string

	SYS_DllAddSuffix( dll_name, SYS_DLLSSYSTEM );

	__named_message( "\tImporting life %s ...\n", dll_name );
	
	lhandle = SYS_DllBegin( dll_name );

/*
	if( !lhandle )
	{
		__error( "SYS_DllOpen failed to open '%s' with the following message:\n\"%s\"\n", dll_name, dlerror() );
	}
*/

	//
	// environment
	//
	li.ShockHandler =		ShockHandler; 
	li.SH_Printf =			SHV_Printf;
	li.SH_SetVar =			SHP_SetVar;
	li.SH_GetVar =			SHP_GetVar;
	li.SH_ParseBuf =		SHP_ParseBuf;

	//
	// memory
	//
	li.MM_Malloc =			_MM_Malloc;
	li.MM_Free =			_MM_Free;
	
	//
	// game
	//
	li.TraceBB = GI_TraceBB;
	li.G_SetRenderHintAngleIpo = G_SetRenderHintAngleIpo;
	li.G_SetRenderHintOriginIpo = G_SetRenderHintOriginIpo;
//	li.GI_RegisterDLightForWF = GI_RegisterDLightForWF;
	li.GI_RegisterWFInfo = GI_RegisterWFInfo;

	//
	// database
	//
	li.GI_SelectShapesInBoundBox = GI_SelectShapesInBoundBox;

	//
	// io
	//
	li.GI_ResourceSearch = GI_ResourceSearch;
	li.GI_ResourceAttach = GI_ResourceAttach;
	li.GI_ResourceDetach = GI_ResourceDetach;
	li.GI_ReadClass = GI_ReadClass;
	
	//
	// sound
	//
	li.SND_AddJob = SND_AddJob;
	li.SND_JobIsActive = SND_JobIsActive;
	li.SND_JobChangeSound = SND_JobChangeSound;
	
	L_GetAPI_ptr = (l_export_t *(*)( l_import_t* ) ) SYS_DllSym( lhandle, "L_GetAPI" );
	__chkptr( L_GetAPI_ptr );

	tmp = L_GetAPI_ptr( &li );
	__chkptr( tmp );

	printf( "ptr: %p\n", tmp );
	
	le = *tmp;

	lloaded = 1;


	__named_message( "\tready.\n" );
	
};

void RejectLife()
{
	__named_message( "\n" );
	if( lloaded )
	{
		le.L_ShutDown();
		SYS_DllEnd( lhandle );
		SHV_Printf( "life killed.\n" );
	} else
		__warning( "no dll loaded\n" );

	lloaded = 0;
}

