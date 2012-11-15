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



// sh_defs.h

// types of sh_ and la_ modules

#ifndef sh_defs_h
#define sh_defs_h


// sh_deamon.c

#define	SH_VARNUM	( 64 )
#define SH_JOINTNUM	( 256 )  // hmmm, 256g
#define SH_JCOMSIZE	( 64 )
#define SH_KSYMSIZE	( 16 )

#define IMODE_NORMAL	( 0 )
#define IMODE_GETCHAR	( 1 )

#define SH_RDEVNUM	( 8 )  // size


// la_service.c

#define	LA_HUD_GR_NUM	( 16 )


// special chars in the font
enum {
	SC_CURSOR_OFF = 1,
	SC_CURSOR_ON,
	SC_LAMP_RED_ON,
	SC_LAMP_GREEN_ON,	
	SC_LAMP_RED_OFF,
	SC_LAMP_GREEN_OFF	
};


typedef struct {
	char	textbuf[60][81];
	int	cols, rows;
	int	firstline, lastline;
	int	colpointer;
	void	(*drawString)( const char *, int, int );
}	hudc_text_t;

typedef struct {
	void	*picptr[LA_HUD_GR_NUM];
	int	posx[LA_HUD_GR_NUM];
	int	posy[LA_HUD_GR_NUM];
	int	width[LA_HUD_GR_NUM];
	int	height[LA_HUD_GR_NUM];
	int	show[LA_HUD_GR_NUM];
	int	num;
}	hudc_graph_t;

typedef struct {
	const char*	shcommand;
	int		argnum;
	void	(*alias)();
	const char*	usage;
} sh_command_t;

// bad situation: function definition inside a defs file
#include "i_defs.h"


typedef struct sh_widget_s {
	void	(*drawFunc)( struct sh_widget_s *, int );
	void	(*updateFunc)( struct sh_widget_s *, keyevent_t * );
	int	height;
	char	*drawdata;
	char	*updatedata;
} sh_widget_t;

typedef struct {
	int	first;
	int	last;
} sh_menuint_t;

typedef struct {
	char	*menuname;
	char	*headname;
	int	headx;
	int	heady;
	int	maxy;
	int	miny;
	int	xorient;
	int	yspacing;
	int	fixedy;
	char	*onescape;
	int	select;
	int	entrynum;

	sh_widget_t	*widgets;
} sh_menu_t;


#endif
