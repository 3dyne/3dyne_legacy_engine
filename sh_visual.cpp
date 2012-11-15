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



// sh_visual.c

#include "interfaces.h"
#include "version.h"

#define MAXLINESIZE	CON_MAXLINESIZE

static char	inputline[MAXLINESIZE];
static char	validline[CON_MAXLINESIZE];
static int	lineptr;

static hudc_text_t	*hud_input;
static hudc_text_t	*hud_output = NULL;
static hudc_text_t	*hud_soutput = NULL;

static sh_var_t	*ziggy;

static char	bar[4][5];
static vec3d_t	snd_vec;
static char	hist[256];

static int sout_lines = 0;

void SHV_StartUp()
{
	lineptr = 0;
	
	hud_input = LA_HUD_CTextInit( 78, 1, LA_DrawString80CRT );
	hud_output = LA_HUD_CTextInit( 78, 17, LA_DrawString80CRT );
	hud_soutput = LA_HUD_CTextInit( 80, 3, LA_DrawString80 );
	SHV_Printf( "========= Wellcome at GSHell =========" );
	SHV_Printf( " DarkestDays %s %s", DD_BTYPE, BVERSION );
	SHV_Printf( "ready." );


	ziggy = SHP_GetVar( "ziggy" );

	strcpy( bar[0], "=== " );
	strcpy( bar[1], " ===" );
	strcpy( bar[2], "= ==" );
	strcpy( bar[3], "== =" );

	memset( hist, 0, 255 );
	sout_lines = 0;
	validline[0] = '\0';
}

void SHV_Update( keyevent_t *_list )
{
	int	c;
	int	listsize, i; 
	keyevent_t	*list;

	listsize = _list[0].sym - 1;

	list = &_list[1];
	
	c = 0;

	for( i = 0; i < listsize; i++ )
	{
	       

		if( list[i].type == SYMTYPE_RELEASE )
		{
			continue;
		} else if( ( list[i].sym == '`' ) || ( list[i].sym == '^' )  )
		{
			continue;
		} else if( isprint( list[i].sym )  )
		{
			c = list[i].sym;
			list[i].sym = (gs_ksym)0;
			SHV_AppendChar( c );
			continue;
		} else if( ( list[i].sym == '\n' ) || ( list[i].sym == '\b' ) ||( list[i].sym == ' ' ) || ( list[i].sym == '\t' ))
		{
			c = list[i].sym;
			list[i].sym = (gs_ksym)0;
			if( c == '\n' )
			{
				strncpy( hist, inputline, 255 );

			}

			SHV_AppendChar( c );
/*
			if( c == '\n' )
				SHV_Printf( "ready.\n" );
*/


			continue;
		} else if( list[i].sym == '-' ) // i'm sorry but...
		{
			c = '_';
			list[i].sym = (gs_ksym)0;
			SHV_AppendChar( c );
			continue;
		} else if( list[i].sym == GSK_CUP )
		{
			strncpy( inputline, hist, MAXLINESIZE );
			lineptr = strlen( hist );
			continue;
		} else
			continue;
		
	}
	if( !c )
		return;

//	SHV_AppendChar( c );
}


int SHV_FindCommand( char *com )
{
	int	i;
	for( i = 0;; i++)
	{
		if( !shcommands[i].shcommand )
			break;

		if( !memcmp( com, shcommands[i].shcommand, strlen( com ) ))
			return i;
	}

	return -1;
}
		    
void SHV_AppendChar( int c )
{
	int	i;

	SND_AddJobByName( "console.sound.char", SND_TYPE_LOCAL, 1, snd_vec );

	if( c == '\n' )
	{
		inputline[lineptr] = 0;
		LA_HUDPutLine( hud_output, inputline );
		memcpy( validline, inputline, CON_MAXLINESIZE );
		lineptr = 0;
		inputline[0] = 0;
	} else if( c == '\b' )
	{
		if( !lineptr )
			return;

		lineptr--;
		inputline[lineptr] = 0;
	} else if( c == '\t' )
	{
		i = SHV_FindCommand( inputline );
		if( i < 0 )
			return;
		
		strcpy( inputline, shcommands[i].shcommand );
		strcat( inputline, " " );
		lineptr = strlen( inputline );

	} else
	{		
		if( lineptr >= MAXLINESIZE - 1 )
			return;

		inputline[lineptr] = c;
		inputline[lineptr+1] = 0;
		lineptr++;
	}
//	__named_message( "line: %s\n", inputline );
}

int SHV_GetInputline( char *line )
{
	if( !validline[0] )
		return 0;

	memcpy( line, validline, CON_MAXLINESIZE );

	validline[0] = '\0';
	
	return 1;
}

			

void SHV_PutString( char * string )
{

#if 1
	if( hud_output )
		LA_HUDPutString( hud_output, string );

	if( hud_soutput )
	{
		LA_HUDPutString( hud_soutput, string);
	}
//	LA_HUDPutLine( hud_igoutput, buf );
//	ig_lastuse = m_counter;

#endif
}

void SHV_Draw( int x, int y )
{
	char	tmp[78];
	char	curs[2];

	tmp[0] = '>';
	memcpy( &tmp[1], lineptr >= 77?inputline + ( lineptr - 77):inputline, 77 );

//	LA_DrawTgaByName( "misc/conback.tga", x, y, 640, 240, DRAW2D_DRAWMODE_REPLACE );
	LA_DrawGltexByName( "misc.tex.conback1", x, y, 320, 320, DRAW2D_DRAWMODE_REPLACE );
	LA_DrawGltexByName( "misc.tex.conback2", x + 320, y, 320, 320, DRAW2D_DRAWMODE_REPLACE );
	LA_HUDPutLine( hud_input, tmp );
	LA_HUD_CTextFlush( hud_input, x + 16, y + 16 );
	LA_HUD_CTextFlush( hud_output, x + 16, y + 32 );

	curs[0] = gc_textcursor;
	curs[1] = '\0';

	LA_DrawString80CRT( curs, x + 24 + ( lineptr>=76?76:lineptr )*8, y+16 );
}

void SHV_DrawSmall( int x, int y )
{
	LA_HUD_CTextFlush( hud_soutput, x, y );
}

void SHV_ScrollSOut()
{
#if 0
	if( sout_lines <= 0 )
		return;

	LA_HUDPutLine( hud_soutput, "\n" );
	sout_lines--;
#endif
}

static char	mill[4] = { '-', '\\', '|', '/' };

void SHV_DrawLoading( int real, int x, int y ) 
{
	
	if( !real )
		return;

	LA_DrawString( IB_GetLastname(), 0, 0 );





	LA_DrawGltexByName( "menu.tex.lineeditbg", x, y, 0, 0, DRAW2D_DRAWMODE_REPLACE );
	LA_DrawStringCRT( "loading ...", x+8, y+8 );
//	LA_DrawCharCRT( mill[gc_renderframe%4], x+8+((gc_renderframe/4)%29)*8, y+8 );

       
	
	
	if( ziggy->ivalue )
		LA_DrawCharCRT( '*', x+(gc_renderframe%29)*8+8, y+8 );
	else
//		LA_DrawStringCRT( bar[gc_renderframe%4], x + 182, y + 8 );
		LA_DrawCharCRT( mill[gc_renderframe%4], x+182+((gc_renderframe/4)%6)*8, y+8 );

	


}
