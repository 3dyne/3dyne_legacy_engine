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



// sh_menu_new.c
#include "interfaces.h"

#include "version.h"


typedef struct shm_funcmap_s {
	const char	*widgettype;
	void	(*drawfunc)( hobj_t *, int );
	void	(*updatefunc)( hobj_t *, keyevent_t * );
} shm_funcmap_t;

static g_resource_t	*snd_enter;
static g_resource_t	*snd_escape;
static g_resource_t	*snd_scroll;
static vec3d_t		snd_vec;



// ==============================================================================
// widget drawfunctions

// ====================
// shcom_tga
// draws a tga

void WDraw_shcom_tga( hobj_t *class_, int active )
{
	char	pic[256];
	int	ycenter;
	int	x, y, height;

	hpair_t	*pair;

	// the first ntstring in drawdata is the name of the tga to draw

	__chkptr( class_ );
	pair = FindHPair( class_, "pic" );
	__chkptr( pair );
	HPairCastToString( pic, pair );

	pair = FindHPair( class_, "posx" );
	__chkptr( pair );

	HPairCastToInt( &x, pair );

	pair = FindHPair( class_, "posy" );
	__chkptr( pair );

	HPairCastToInt( &y, pair );

	// draw it at curx, cury
	LA_DrawGltexByName( pic, x, y, 0, 0, DRAW2D_DRAWMODE_ARGB );


	// if the widget is active draw big menucursor + some specioal fx
	if( active )
	{
		
		pair = FindHPair( class_, "height" );
		__chkptr( pair );
		
		HPairCastToInt( &height, pair );
		
		ycenter = y + ( height - 32 ) / 2;
		LA_DrawGltexByName( gc_graphcursor, x-32, ycenter, 0, 0, DRAW2D_DRAWMODE_ARGB );
//		LA_DrawGltexByName( "menu.tex.gloom", curx, cury, 0, 0, DRAW2D_DRAWMODE_ADD_RGB );
	}
}

void WDraw_pic( hobj_t *class_, int active )
{
	char	pic[256];
	int	x, y, width, height;

	hpair_t	*pair;

	// the first ntstring in drawdata is the name of the tga to draw

	__chkptr( class_ );
	pair = FindHPair( class_, "pic" );
	__chkptr( pair );
	HPairCastToString( pic, pair );

	pair = FindHPair( class_, "posx" );
	__chkptr( pair );

	HPairCastToInt( &x, pair );

	pair = FindHPair( class_, "posy" );
	__chkptr( pair );

	HPairCastToInt( &y, pair );

	pair = FindHPair( class_, "width" );
	__chkptr( pair );

	HPairCastToInt( &width, pair );

	pair = FindHPair( class_, "height" );
	__chkptr( pair );

	HPairCastToInt( &height, pair );

	// draw it at curx, cury
	LA_DrawGltexByName( pic, x, y, width, height, DRAW2D_DRAWMODE_ARGB );
}


// ====================
// shcom_text
// draws text

void WDraw_shcom_text( hobj_t *class_, int active )
{
	char	text[256];
	hpair_t	*pair;
	int	x, y;

	// get text from drawdata

	
	pair = FindHPair( class_, "text" );
	__chkptr( pair );

	HPairCastToString( text, pair );

	pair = FindHPair( class_, "posx" );
	__chkptr( pair );

	HPairCastToInt( &x, pair );

	pair = FindHPair( class_, "posy" );
	__chkptr( pair );

	HPairCastToInt( &y, pair );

	// display text
	LA_DrawString( text, x, y );


	// draw cursor
	if( active )
		LA_DrawString( ">", x - 16, y );
}

// ====================
// yesno
// draws funny switches
// and lamps

void WDraw_yesno( hobj_t *class_, int active )
{
	char		text[128];
	char		varname[128];
	sh_var_t	*var;
	unsigned int	val;
	int		x, y;
	hpair_t	*pair;

	// drawdata contains the lable text thats drawn left from the cursor

	
	pair = FindHPair( class_, "text" );
	__chkptr( pair );
	
	HPairCastToString( text, pair );

	pair = FindHPair( class_, "varname" );
	__chkptr( pair );
	
	HPairCastToString( varname, pair );

	pair = FindHPair( class_, "posx" );
	__chkptr( pair );

	HPairCastToInt( &x, pair );

	pair = FindHPair( class_, "posy" );
	__chkptr( pair );

	
	HPairCastToInt( &y, pair );
	
	LA_DrawString( text, 0, y );


	var = SHP_GetVar( varname );
	val = var->ivalue;

	// if val is true draw green lamp on and red of. if !val do the opposite
	if( val )
	{
//		sprintf( text, "%cyes %cno", SC_LAMP_GREEN_ON, SC_LAMP_RED_OFF );
//		LA_DrawString( text, curx, cury );
		LA_DrawString( "\4yes \5no", x, y );
	} else
	{
//		sprintf( text, "%cyes %cno", SC_LAMP_GREEN_OFF, SC_LAMP_RED_ON );
//		LA_DrawString( text, curx, cury );
		LA_DrawString( "\6yes \3no", x, y );
	}


	// draw cursor
	if( active )
		LA_DrawString( ">", x - 16, y );

	
}


// ====================
// slider 

void WDraw_slider( hobj_t *class_, int active )
{
	char	varname[128];
	char	text[128];
	fp_t	val;
	sh_var_t	*var;
	int	goffs;
	int	x, y;
	hpair_t	*pair;

	// get shvar to modify from updatedata


	pair = FindHPair( class_, "text" );
	__chkptr( pair );
	
	HPairCastToString( text, pair );

	pair = FindHPair( class_, "varname" );
	__chkptr( pair );
	
	HPairCastToString( varname, pair );

	pair = FindHPair( class_, "posx" );
	__chkptr( pair );

	HPairCastToInt( &x, pair );

	pair = FindHPair( class_, "posy" );
	__chkptr( pair );
	
	HPairCastToInt( &y, pair );

	
	var = SHP_GetVar( varname );
	val = var->fvalue;

	if( val > 1.0 )
		val = 1.0;

	if( val < 0.0 )
		val = 0.0;

	// calculate how far right the grip is in our 224 pixel space
	
	goffs = ( int ) ( 224.0 * val );

	// draw it at curx + 8 + goffs

	LA_DrawGltexByName( "menu.tex.sliderbg", x, y, 0, 0, DRAW2D_DRAWMODE_REPLACE );

	if( active )
	{
		LA_DrawString( ">", x - 16, y + 8 );
		LA_DrawGltexByName( "menu.tex.slidergrip_on", x + 8 + goffs, y, 0, 0, DRAW2D_DRAWMODE_ARGB );
	} else
		LA_DrawGltexByName( "menu.tex.slidergrip_off", x + 8 + goffs, y, 0, 0, DRAW2D_DRAWMODE_ARGB );

	LA_DrawString( text, 0, y + 8 );

}

// ====================
// gets a string from the user

void WDraw_lineedit( hobj_t *class_, int active )
{
	char		text[128];
	char		label[128];
	char		varname[128];
	sh_var_t	*var;
	int		x, y;
	hpair_t		*pair;

	pair = FindHPair( class_, "text" );
	__chkptr( pair );
	
	HPairCastToString( label, pair );

	pair = FindHPair( class_, "varname" );
	__chkptr( pair );
	
	HPairCastToString( varname, pair );

	pair = FindHPair( class_, "posx" );
	__chkptr( pair );

	HPairCastToInt( &x, pair );

	pair = FindHPair( class_, "posy" );
	__chkptr( pair );

	HPairCastToInt( &y, pair );



	var = SHP_GetVar( varname );
	U_EndUnpack();

	// draw text background
	LA_DrawGltexByName( "menu.tex.lineeditbg", x, y, 0, 0, DRAW2D_DRAWMODE_REPLACE );


	// display the last 14 chars of the sh_var
	strncpy( text, strlen( var->string ) >= 14? var->string + ( strlen( var->string ) - 14): var->string , 128 );

	
	LA_DrawString( text, x + 8, y + 8 );

	// if active draw blinking cursor
	if( active )
	{
		LA_DrawString( ">", x - 16, y + 8);
		LA_DrawChar( gc_textcursor, x + strlen( text ) * 16 + 8, y + 8 );
	}

	// draw labeltext from drawdata

	LA_DrawString( label, 0, y + 8 );
}

// ====================
// shows the sring of 
// any sh_var

void WDraw_showvar( hobj_t *class_, int active )
{
	char		text[128];
	char		varname[16];
	sh_var_t	*var;
	int		x, y;
	hpair_t		*pair;
	
	pair = FindHPair( class_, "text" );
	__chkptr( pair );
	
	HPairCastToString( text, pair );

	pair = FindHPair( class_, "varname" );
	__chkptr( pair );
	
	HPairCastToString( varname, pair );

	pair = FindHPair( class_, "posx" );
	__chkptr( pair );

	HPairCastToInt( &x, pair );

	pair = FindHPair( class_, "posy" );
	__chkptr( pair );
	
	HPairCastToInt( &y, pair );


	var = SHP_GetVar( varname );

	LA_DrawString( text, 0, y );

	LA_DrawString( var->string, x, y );

	// no menu cursor in this widget, because widgets without updateFunc
	// have no active state

}
		
// ====================
// join

void WDraw_join( hobj_t *class_, int active )
{
#if 0 // first rewrite the joint concept!

    char	command[128];
	char	*symname[2];
	char	text[128];

	U_BeginUnpack( U_PACKED_ASC, w->updatedata, 256 );
	U_UnpackntString( command, 128 );

	U_EndUnpack();

	SHI_GetSymnamesByCommand( command, symname );

	if( !symname[0] )
		symname[0] = "none";

	if( !symname[1] )
		symname[1] = "none";
	

	sprintf( text, "%s or %s", symname[0], symname[1] );

	LA_DrawString( text, curx, cury );
#endif
}


// ============================================================================
// sh_widget update ( aka event handler ) 

// ====================
// shcom_tga
// execute shcommand on
// enter

void WUpdate_shcom_tga( hobj_t *class_, keyevent_t *e )
{
	char	com[256];

	hpair_t *pair;
	



	// ignore everyting but ENTER
	if( e->sym != GSK_ENTER )
		return; // what else

	pair = FindHPair( class_, "command" );

	__chkptr( class_ );

	HPairCastToString( com, pair );

	// doit
	SHP_ParseBuf( com );

	SND_AddJob( snd_enter, SND_TYPE_LOCAL, 1, snd_vec );
	// disble keyevent
	e->sym = (gs_ksym)0;
}


// ====================
// shcom_text
// d.t.o but different
// drawfunc

void WUpdate_shcom_text( hobj_t *class_, keyevent_t *e )
{
	char	com[256];
	hpair_t *pair;

	if( e->sym != GSK_ENTER )
		return; // what else


	
	__chkptr( class_ );
	pair = FindHPair( class_, "command" );

	__chkptr( pair );

	HPairCastToString( com, pair );

	__named_message( "%s\n", com );
	SHP_ParseBuf( com );

	e->sym = (gs_ksym)0;
}

// ====================
// yesno

void WUpdate_yesno( hobj_t *class_, keyevent_t *e )
{
	char		varname[128];
	sh_var_t	*var;
	unsigned int	val;
	hpair_t	*pair;
	

// accepts ENTER + vertical cursor action
	if( !(( e->sym == GSK_ENTER ) || ( e->sym == GSK_CRIGHT ) || ( e->sym == GSK_CLEFT )) )
		return; // it's not me

	SND_AddJob( snd_scroll, SND_TYPE_LOCAL, 1, snd_vec );

	__chkptr( class_ );
	pair = FindHPair( class_, "varname" );
	__chkptr( pair );

	HPairCastToString( varname, pair );


	// get sh_var to modufy

	var = SHP_GetVar( varname );
	val = var->ivalue;

	// modify it
	if( val )
		SHP_SetVar( varname, "0", 0 );
	else
		SHP_SetVar( varname, "1", 0 );

	// disble keyevent
	e->sym = (gs_ksym)0;
}

// ====================
// lineedit

void WUpdate_lineedit( hobj_t *class_, keyevent_t *e )
{
	char		text[128];
	char		varname[16];
	sh_var_t	*var;
	int		ssize;
	hpair_t		*pair;

	// accept alphanum, backspace and space
	
	if( !( isprint( e->sym ) || ( e->sym == '\b' ) ||( e->sym == ' ' ) ))
		return;

	__chkptr( class_ );
	pair = FindHPair( class_, "varname" );
	__chkptr( pair );

	HPairCastToString( varname, pair );

	var = SHP_GetVar( varname );

	strncpy( text, var->string, 128 );
	ssize = strlen( var->string );


	// manipulate string by keyevent
	switch( (int)(e->sym) )
	{
	case '\b':
		if( !ssize )
			return;
		text[ssize-1] = 0;
		break;

	default:
		if( ssize >= 64 )
			return;

		text[ssize] = e->sym;
		break;
	}
	
	// set var
	SHP_SetVar( varname, text, 0 );

	// disable keysym
	e->sym = (gs_ksym)0;
}
		
void WUpdate_slider( hobj_t *class_, keyevent_t *e )
{
	char	varname[128];
	char	tmp[128];
	sh_var_t	*var;
	fp_t	val;

	fp_t	step;
	hpair_t	*pair;


	if( !(( e->sym == GSK_CRIGHT ) || ( e->sym == GSK_CLEFT )) )
		return; // it's not me

	SND_AddJobByName( "menu.sound.slidermove", SND_TYPE_LOCAL, 1, snd_vec );
		
	__chkptr( class_ );
	pair = FindHPair( class_, "varname" );
	__chkptr( pair );

	HPairCastToString( varname, pair );


	var = SHP_GetVar( varname );
	val = var->fvalue;

	pair = FindHPair( class_, "step" );
	if( !pair )
		step = 0.1;
	else
		HPairCastToFloat( &step, pair );

	if( step < 0.01 )
		step = 0.1;

	if( e->sym == GSK_CLEFT )
	{
		val -= step;
		if( val < 0.0 )
			val = 0.0;

	} else if( e->sym == GSK_CRIGHT )
	{
		val += step;
		if( val > 1.0 )
			val = 1.0;
	}

	sprintf( tmp, "%f", val );

	SHP_SetVar( varname, tmp, 0 );

	e->sym = (gs_ksym)0;
}

// ====================
// join

void WUpdate_join( hobj_t *class_, keyevent_t *e )
{

#if 0 // d.t.o
    char	*symname[2];
	char	command[128];
	char	*ptr;




	if( !powerless )
	{
		if( e->sym != GSK_ENTER )
			return;

		
		SHV_Printf( "now powerless\n", e->sym );
		
		powerless = 1;   // now all events go to this widget
		e->sym = 0;
		return;
	} 

	if( e->sym == GSK_ESCAPE )
	{
		powerless = 0;
		SHV_Printf( "escaping\n" );
		e->sym = 0;

		return;
	}

	SHV_Printf( "joining %d\n", e->sym );

	U_BeginUnpack( U_PACKED_ASC, w->updatedata, 256 );

	U_UnpackntString( command, 128 );

	U_EndUnpack();

	SHI_GetSymnamesByCommand( command, symname );



//	SHI_SymnameByKeysym( ptr, e->sym );
	
	// joint 2 times    
	if( symname[1] )
	{
		SHI_Unjoin( SHI_KeysymByName( symname[0] ));
		SHI_Unjoin( SHI_KeysymByName( symname[1] ));
	}
       
	ptr = SHI_SymnameByKeysym( e->sym );

	SHV_Printf( "ksym: %s", ptr );

	SHI_Join( command, ptr );

	e->sym = 0;

	powerless = 0; // important
#endif
}


// ==============================================================================
// rest of the menu stuff

static hobj_t	*menu;
static char	curpage[256];
static int	powerless;
static shm_funcmap_t	funcmap[] = {
	{ "shcom_pic", WDraw_shcom_tga, WUpdate_shcom_tga },
	{ "pic", WDraw_pic, NULL },
	{ "shcom_text", WDraw_shcom_text, WUpdate_shcom_text },
	{ "yesno", WDraw_yesno, WUpdate_yesno },	
	{ "slider", WDraw_slider, WUpdate_slider },
	{ "lineedit", WDraw_lineedit, WUpdate_lineedit },
	{ "showvar", WDraw_showvar, NULL },
	{ "join", WDraw_join, WUpdate_join },
	{ NULL, NULL, NULL }
};

void SHM_StartUp()
{
	tokenstream_t	*ts;

	ts = BeginTokenStream( "menu/menu.hobj" );
	__chkptr( ts );

     	menu = ReadClass( ts );
	__chkptr( menu );

	EndTokenStream( ts );

	strcpy( curpage, "none" );
	powerless = 0;

	snd_enter = G_ResourceSearch( g_rs, "menu.sound.enter" );
	__chkptr( snd_enter );

	snd_escape = G_ResourceSearch( g_rs, "menu.sound.escape" );
	__chkptr( snd_escape );

	snd_scroll = G_ResourceSearch( g_rs, "menu.sound.scroll" );
	__chkptr( snd_scroll );
	
}

shm_funcmap_t *SHM_LookupFuncmap( char *name )
{
	int	i;

	for( i = 0; ; i++ )
	{
		if( !funcmap[i].widgettype )
			return NULL;

		if( !strcmp( name, funcmap[i].widgettype ))
			return &funcmap[i];
	}
	return NULL;
}

void SHM_SetCurPage( const char *name )
{
	__named_message( "%s\n", name );
	strncpy( curpage, name, 255 );
}


void SHM_DrawPage( hobj_t *page )
{
	int	i;
	char	entryname[256];
	shm_funcmap_t	*map;
	hpair_t	*pair;

	hobj_t	*entry;
//	hobj_search_iterator_t *iter;


	pair = FindHPair( page, "active" );
	__chkptr( pair );

	i = 255;
	for( ; ; )
	{
		sprintf( entryname, "%d", i );

		entry = FindClass( page, entryname );

		if( !entry ) {
			if( i >= 255 )
			{
				i = 0;
				continue;
			}
			else {
				break;
            }
        }
		map = SHM_LookupFuncmap( entry->type );
		if( !map )
		{
			__warning( "unknown widget type '%s'\n", entry->type );
			continue;
		}
		if( !map->drawfunc )
			__error( "no drawfunc for '%s'\n", entry->type );

		if( !strcmp( pair->value, entry->name ) )		
		{

			map->drawfunc( entry, 1 );
		}
		else
			map->drawfunc( entry, 0 );
		i++;
	}
}
				   
// faky help screen
void SHM_FakeHelp()
{
	int	y;
	LA_DrawGltexByName( "menu.tex.readthis", 0, 448, 0, 0, DRAW2D_DRAWMODE_ARGB );
	
	y = 432;
	
	LA_DrawStringCRT( "Movement:", 0, y );
	y-=16;
	LA_DrawString( "Forward:...................... up arrow", 0, y );
	             // ========================================
	y-=16;
	LA_DrawString( "Back:....................... down arrow", 0, y );
	y-=16;
	LA_DrawString( "Left:....................... left arrow", 0, y );
	y-=16;
	LA_DrawString( "Right:..................... right arrow", 0, y );
	
	y-=32;
	LA_DrawStringCRT( "Actions:", 0, y );
	y-=16;
	LA_DrawString( "Fire:................. right mouse/CTRL", 0, y );
	y-=16;
	LA_DrawString( "Jump:..................... middle mouse", 0, y );
	y-=16;
	LA_DrawString( "Strafe:................. left mouse/ALT", 0, y );
	y-=16;
	LA_DrawString( "Use/Switch:...................... space", 0, y );

	y-=32;
	LA_DrawStringCRT( "Specials:", 0, y );
	y-=16;
	LA_DrawString( "Open/Close Menu:................... ESC", 0, y );
	y-=16;
	LA_DrawString( "Open/Close Console:................ ~/C", 0, y );
	y-=16;
	LA_DrawString( "Add Dummy:........................... A", 0, y );
	y-=16;
	LA_DrawString( "Show Frag Statistics:.............. TAB", 0, y );
	y-=16;
	LA_DrawString( "Show Wireframe:..................... F4", 0, y );
	y-=16;
	LA_DrawString( "Show Only Lightmaps................. F5", 0, y );
	y-=16;
	LA_DrawString( "Show Frames Per Second.............. F9", 0, y );

	y-=48;
	LA_DrawString( "This is neither a pre alpha nor an alpha", 0, y );
	y-=16;
	LA_DrawString( "nor a beta demo nor a final game.", 0, y );
	y-=24;
	LA_DrawString( "So it is not optimized in any way", 0, y );
	y-=16;
	LA_DrawString( "Neither for speed nor for memory usage.", 0, y );
	y-=20;
	if( gc_textcursor == 1) // mega fake!
		LA_DrawStringCRT( "         It is just a tech demo", 0, y );


	y = 0;
	LA_DrawString80( "darkestdays.cjb.net                                          darkestdays@gmx.net", 0, y );


}

void SHM_Draw()
{
// 	char	text[256];
	hobj_t	*page;
	
	TFUNC_ENTER;

	if( !strcmp( curpage, "none" ))
	{
		TFUNC_LEAVE;
		return;
	}

	LA_DrawGltexByName( "misc.tex.logo32", 640 - 32, 16, 0, 0, DRAW2D_DRAWMODE_ARGB );
	LA_DrawString( "HE LIKES", 640 - 32 - 16 * 8, 32 );
	LA_DrawString( "IT", 640 - 32 - 16 * 2, 16 );
	LA_DrawString( "v"BVERSION, 640 - 16 * ( sizeof(BVERSION) + 1 ), 0 );

//	printf( "page: %s\n", curpage );

	page = FindClass( menu, curpage );
	__chkptr( page );


	SHM_DrawPage( page );

	if( !strcmp( curpage, "readthis" ))
	{
		SHM_FakeHelp();
	}


	TFUNC_LEAVE;
}

void SHM_UpdatePage( hobj_t *page, keyevent_t *e, int active )
{
        char	text[256];
	hobj_t	*entry;
	shm_funcmap_t	*map;
	

	sprintf( text, "%d", active );
	entry = FindClass( page, text );
	map = SHM_LookupFuncmap( entry->type );
	
	if( !map )
	{
		__warning( "unknown widget type '%s'\n", entry->type );
		return;
	}

	if( !map->updatefunc )
		__error( "no updatefunc for '%s'\n", entry->type );

	map->updatefunc( entry, e );
}
	
void SHM_GetCurpage( char *string )
{
	strcpy( string, curpage );
}	
	
void SHM_Update( keyevent_t *_list)
{
	int	listsize, i;
	char	onescape[256];
	char	text[256];
	int	entrynum;
	int	active;
	
	keyevent_t	*list, *validevent = NULL;
	hobj_t	*page;
	hpair_t	*pair, *pair_active;


	listsize = _list[0].sym;
	list = &_list[1];

	
	if( !strcmp( curpage, "none" ))
	{
		return;
	}
	
	
	for( i = 0; i < listsize; i++ )
	{
		if( list[i].type != SYMTYPE_PRESS )
			continue;
		if( !list[i].sym )
			continue;
		
		validevent = &list[i];
	}


	if( !validevent )
		return;

	if( !powerless )
	{

		page = FindClass( menu, curpage );

		pair = FindHPair( page, "entrynum" );
		__chkptr( pair );
		HPairCastToInt( &entrynum, pair );

		pair_active = FindHPair( page, "active" );
		__chkptr( pair_active );
		HPairCastToInt( &active, pair_active );
		printf( "active: %d\n", active );		

		switch( validevent->sym )
		{
		case GSK_ESCAPE:	
			pair = FindHPair( page, "onescape" );
			HPairCastToString( onescape, pair );
			printf( "leaving page %s\n", onescape );
			SHP_ParseLine( onescape );
			validevent->sym = (gs_ksym)0;
			SND_AddJob( snd_escape, SND_TYPE_LOCAL, 1, snd_vec );
			break;
				
		case GSK_CUP:
			active--;
			if( active < 0 )
				active = entrynum-1;

			validevent->sym = (gs_ksym)0;
			SND_AddJob( snd_scroll, SND_TYPE_LOCAL, 1, snd_vec );
			break;

			

		case GSK_CDOWN:
			active++;
			if( active >= entrynum )
				active = 0;

			validevent->sym = (gs_ksym)0;
			SND_AddJob( snd_scroll, SND_TYPE_LOCAL, 1, snd_vec );
			break;

		default:
			SHM_UpdatePage( page, validevent, active );
			break;
		}
		sprintf( text, "%d", active );
		strcpy( pair_active->value, text );
		
	}
}
