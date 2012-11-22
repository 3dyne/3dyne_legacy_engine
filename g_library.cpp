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



// g_library.c
// shared lib handling

#include "interfaces.h"
#include "g_api.h"
#include "l_wrap.h"

// common api exdported to all libs;
static g_api_t	g_api;
static bool_t	g_api_valid = false;

// initialize common api
void G_LibInit()
{
	if ( g_api_valid == true )
		return;
	g_api_valid = true;

	g_api.version = G_API_VERSION;
	g_api.size = G_API_SIZE;
	
	g_api.ShockHandler = ShockHandler;
	g_api.TF_Enter = TF_Enter;
	g_api.TF_Leave = TF_Leave;
	

	g_api.Malloc = _MM_Malloc;
	g_api.Free = _MM_Free;
	
	g_api.Printf = SHV_Printf;
	g_api.SH_SetVar = SHP_SetVar;
	g_api.SH_GetVar = SHP_GetVar;
	
	g_api.Map_TraceBB = GI_MapTraceBB;
	g_api.Map_TraceLine = GI_MapTraceLine;
	g_api.Map_SelectShapeIDsInBoundBox = GI_SelectShapesInBoundBox;
	g_api.Map_SelectLightSourceWhereAnyLightID = GI_SelectLightSourceWhereAnyLightID;
	g_api.Map_LightSourceUpdate = GI_LightSourceUpdate;
	g_api.Map_GetUniqueID = GI_GetUniqueID;
	g_api.Map_ShapeIDSetFlatColor = GI_MapShapeIDSetFlatColor;

	g_api.Render_Sprite = GI_RenderAddSprite;
	g_api.Render_Psys = GI_RenderAddPsys;
	g_api.Render_LocalLight = GI_RenderAddLocalLight;
	g_api.Render_Model = GI_RenderAddModel;
	g_api.Render_Halo = GI_RenderAddHalo;

	g_api.Render_View = GI_RenderAddView;

	g_api.Draw_Rect = GI_DrawRect;
	g_api.Draw_Text = GI_DrawText;

	g_api.GI_ResourceSearch = GI_ResourceSearch;
	g_api.GI_ReadClass = GI_ReadClass;
}

int G_LibLoad( l_library_t *lib )
{
	char	filename[256];
	l_api_t	*api_ptr;
	
	G_LibInit();

	if( lib->isloaded )
	{
		__warning( "library is in use. name: '%s'\n", lib->name );
		return 1;
	}
	
	

#ifdef win32_x86
	strcpy( filename, lib->name );
	SYS_DllAddSuffix( filename, 0 );
#else
	sprintf( filename, "dd1/%s", lib->name );
	SYS_DllAddSuffix( filename, SYS_DLLSSYSTEM );
#endif

	__named_message( "filename: %s\n", filename );


	lib->h = SYS_DllBegin( filename );

	lib->ApiExchange = (api_exchange_t)SYS_DllSym( lib->h, "ApiExchange" );

	if( !lib->ApiExchange )
	{
		__warning( "symbol 'ApiExchange' not found in '%s'\n", filename );
		lib->isloaded = 0;
		SYS_DllEnd( lib->h );
		return 1;
	}

	
	api_ptr = lib->ApiExchange( &g_api );

	if( !api_ptr->valid )
	{
		__warning( "g_api is invalid for the dll, because:\n" );
		
		if( api_ptr->version == API_LESS )
		{
			__message( "the dll is too old\n" );
		} else if( api_ptr->version == API_MORE )
		{
			__message( "the dll is too new\n" );
		} else if( api_ptr->size < 0 )
		{
			__message( "the dll has a different g_api size\n" );
		} else
		{
			__message( "you should never see this warning\n" );
		}
		
		lib->isloaded = 0;
		SYS_DllEnd( lib->h );
		return 1;
	}
		
	if( api_ptr->size <= 0 )
	{
		__warning( "l_api->size <= 0, but not invalid. something went very bad\n" );
		lib->isloaded = 0;
		SYS_DllEnd( lib->h );
		return 1;	   
	}

	if( api_ptr->version < L_API_VERSION )
	{
		__warning( "l_api is too old\n" );
		lib->isloaded = 0;
		SYS_DllEnd( lib->h );
		return 1;	   
	} else if( api_ptr->version > L_API_VERSION )
	{
		__warning( "l_api is too new.\n" );
		lib->isloaded = 0;
		SYS_DllEnd( lib->h );
		return 1;	   
	} else if( api_ptr->size < L_API_SIZE )
	{
		__warning( "l_api is too new.\n" );
		lib->isloaded = 0;
		SYS_DllEnd( lib->h );
		return 1;	   
	} else if( api_ptr->size > L_API_SIZE )
	{	
		__warning( "l_api is too new.\n" );
		lib->isloaded = 0;
		SYS_DllEnd( lib->h );
		return 1;	   
	}


	memcpy( &lib->api, api_ptr, L_API_SIZE );

	__named_message( "%s APIs are valid\n", lib->api.owner );

	lib->isloaded = 1;
	return 0;
}


void G_LibUnload( l_library_t *lib )
{
	if( !lib->isloaded )
	{
		__warning( "library is not loaded\n" );
		return;
	}

	SYS_DllEnd( lib->h );
}
