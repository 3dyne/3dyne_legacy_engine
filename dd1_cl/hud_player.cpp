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



// hud_player.c

#include "cl_state.h"
#include "cl_head.h"

#include "s_mem.h"
#include "shock.h"

#include "hud_player.h"


#define HUD_SCALE_X ( 1.0/640.0 )
#define HUD_SCALE_Y ( 1.0/480.0 )

#define HUD_NUM_X ( 32.0 )
#define HUD_NUM_Y ( 32.0 )

// hacked in for td 0.2
static sh_var_t	*cl_showfrags = NULL;

static void SetDiText( di_text_t *di, const char *string, int x, int y )
{
	di->x = x * HUD_SCALE_X;
	di->y = y * HUD_SCALE_Y;

	strncpy( di->string, string, DI_TEXT_STRINGSIZE );
}

static void SetDiRect( di_rect_t *di_arg, const char *resname, int x, int y, int w, int h )
{
	di_arg->gltex = g_api.GI_ResourceSearch( resname );
	di_arg->x = x * HUD_SCALE_X;			
	di_arg->y = y * HUD_SCALE_Y;

	di_arg->w = w * HUD_SCALE_X;	
	di_arg->h = h * HUD_SCALE_Y;
	
	di_arg->tx = 0.0;
	di_arg->ty = 0.0;
	di_arg->tw = 1.0;
	di_arg->th = 1.0;

	di_arg->flat_color[0] = 0xff;
	di_arg->flat_color[1] = 0xff;
	di_arg->flat_color[2] = 0xff;
	di_arg->flat_color[3] = 0xff;
}


/*
  ==================================================
  players main hud

  ==================================================
*/

void HUD_PlayerInit( hud_player_t *hud )
{
	//
	// init test rect
	//

	int	i;
	char	name[128];

	for( i = 0; i < 11; i++ )
	{
		
		if( i != 10 )
			sprintf( name, "gltex.hud.%d", i );
		else
			sprintf( name, "gltex.hud.minus" );
		
		hud->num[i] = g_api.GI_ResourceSearch( name );
	}

	SetDiRect( &hud->di_back, "gltex.hud.back", 254, 0, 132, 34 );
	SetDiRect( &hud->di_face, "gltex.smilie.nice", 256, 0, HUD_NUM_X, HUD_NUM_Y );
	SetDiRect( &hud->di_health[0], "gltex.hud.0", 288, 0, HUD_NUM_X, HUD_NUM_Y );
	SetDiRect( &hud->di_health[1], "gltex.hud.1", 320, 0, HUD_NUM_X, HUD_NUM_Y );
	SetDiRect( &hud->di_health[2], "gltex.hud.2", 352, 0, HUD_NUM_X, HUD_NUM_Y );
	SetDiRect( &hud->di_crosshair, "gltex.hud.crosshair01", 304, 224, HUD_NUM_X, HUD_NUM_Y );
	

// test text
	SetDiText( &hud->di_frag, "frags: 0", 0, 0 );
			

	hud->health = 600;
	
	cl_showfrags = g_api.SH_GetVar( "cl_showfrags" );


}

void HUD_PlayerCleanUp( hud_player_t *hud )
{

}


static void HUD_DrawNumHealth( hud_player_t *hud, int value )
{
	char	num[16];
	int	i;
	

	if( value > 999 )
		value = 999;

	if( value < -99 )
		value = -99;


	sprintf( num, "%i", value );

	for( i = 0; i < 3; i++ )
	{
		if( num[i] == 0 )
			break;

		if( num[i] == '-' )
			hud->di_health[i].gltex = hud->num[10];
		else
			hud->di_health[i].gltex = hud->num[num[i] - '0']; // pew!
		
		g_api.Draw_Rect( &hud->di_health[i] );
	}
	g_api.Draw_Rect( &hud->di_face );
}

void HUD_PlayerDraw( hud_player_t *hud )
{
	// draw test rect
	
//	g_api.Draw_Rect( &hud->di_rect );
	g_api.Draw_Rect( &hud->di_back );
	HUD_DrawNumHealth( hud, hud->health );
	g_api.Draw_Rect( &hud->di_crosshair );

	sprintf( hud->di_frag.string, "frags: %d", hud->numfrag );

	g_api.Draw_Text( &hud->di_frag );
	g_api.Draw_Text( &hud->di_nickname );


	{
		char name[128];
		sprintf( name, "I am: %s", hud->nickname );

		SetDiText( &hud->di_nickname, name, 384, 0 );
	}
}

/*
  ==================================================
  frag ranking hud

  ==================================================
*/

/*
  ==============================
  HUD_DrawFragRanking

  ==============================
*/

void HUD_DrawFragRanking( void )
{
	int		i, j;

	static di_text_t	frags[CL_MAX_HUD];

	if( cl_showfrags )
	{
		if( !cl_showfrags->ivalue )
			return;
	} else
		return;

	//
	// sort cl_huds by fragnum
	//
	for ( i = cl_num_hud-1; i >= 0; i-- )
	{
		for ( j = 1; j <= i; j++ )
		{
			if ( cl_huds[j-1]->numfrag > cl_huds[j]->numfrag )
			{
				hud_player_t	*tmp;
				
				tmp = cl_huds[j-1];
				cl_huds[j-1] = cl_huds[j];
				cl_huds[j] = tmp;
			}
		}
	}

	for ( i = 0; i < cl_num_hud; i++ )
	{
		char	str[256];
		sprintf( str, "rank %d: '%s' got %d frags", cl_num_hud-i, cl_huds[i]->nickname, cl_huds[i]->numfrag );
		SetDiText( &frags[i], str, 0, 256+i*20 );
		g_api.Draw_Text( &frags[i] );
	}
}
