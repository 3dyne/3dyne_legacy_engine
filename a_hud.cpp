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



// a_hud.c

#include "interfaces.h"

/*
  ============================================================================
  
  this file contains all the look&feel stuff for TGFKAA I.
  that is initialization of HUD contexts and drawing of the menu + status-bar

  ============================================================================
*/ 
  


//static	hudc_graph_t	*hud_health;
static	res_gltex_cache_t	*num32[10];

void HUD_Num2Arr( int, int arr[3] );

void HUD_StartUp()
{
	int	i;
	g_resource_t	*res;

	char	      name[128];

	TFUNC_ENTER;

	for( i = 0; i < 10; i++ )
	{
		sprintf( name, "gltex.hud.%d", i );		
		res = G_ResourceSearch( g_rs, name );
		num32[i] = (res_gltex_cache_t*)res->res_cache;
	}
	
	


	TFUNC_LEAVE;
}

void HUD_ShutDown()
{
// 	int	i;

	TFUNC_ENTER;

	TFUNC_LEAVE;
}
 

void HUD_Show()
{
// 	char	arr[3];

	TFUNC_ENTER;


#if 0
// draw health
	LA_DrawGltexByName( "gltex.hud.health", 0, 0, 0, 0, DRAW2D_DRAWMODE_ARGB );
//	HUD_Num2Arr( 100, arr );
	
	LA_DrawGltexByGltex( num32[1], 32, 0, 32, 0, DRAW2D_DRAWMODE_ARGB );
	LA_DrawGltexByGltex( num32[0], 60, 0, 32, 0, DRAW2D_DRAWMODE_ARGB );
	LA_DrawGltexByGltex( num32[0], 84, 0, 32, 0, DRAW2D_DRAWMODE_ARGB );

	LA_DrawGltexByName( "gltex.hud.crosshair01", 304, 224, 0, 0, DRAW2D_DRAWMODE_ARGB );
	
#endif
	TFUNC_LEAVE;
}
	
void HUD_Num2Arr( int arg_num, int arr[3] )
{
	int	a;
	int	num;

	TFUNC_ENTER;

	num = arg_num;

	if( num > 999 )
	{
		return;
	}

	arr[0] = 0;
	a = num / 100;
	if( a )
	{
		arr[0] = a;
		num-= a * 100;
	}

	arr[1] = 0;
	a = num / 10;
	if( a )
	{
		arr[1] = a;
		num-= a * 10;
	}

	arr[2] = num;
	TFUNC_LEAVE;
}
