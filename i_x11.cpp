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



// i_x11.c

#include "interfaces.h"
#include "r_private.h"

static sh_var_t	*use_dga;

keyevent_t		keventlist[128];
unsigned int		keventlistptr;

int	md_x, md_y;
//static int		norm_x, norm_y;

static int		type;
static int		isgrabed;
static int		isdga;
static int		isshift = 0;

static char		shiftmap[256];

static int	mouse_accel = 2, mouse_accel2 = 1, mouse_treshold = 2;

void I_Update()
{
	I_X11Update( g_window, g_display );
}


void I_X11StartUp( Window arg_window, Display *arg_display )
{
	int	i;

	TFUNC_ENTER;
	use_dga = SHP_GetVar( "use_xf86dga" );

	XAutoRepeatOff( arg_display );

	XGetPointerControl( arg_display, &mouse_accel, &mouse_accel2, &mouse_treshold );
	__named_message( "\t\tmouse acceleration: %d/%d treshold: %d\n", mouse_accel, mouse_accel2, mouse_treshold );

	XChangePointerControl( arg_display, True, True, 1, 1, 1 );

	isgrabed = 0;

#ifdef USE_DGA
	
	if( use_dga->ivalue )
	{
		int	major, minor;

		XDGAQueryVersion( arg_display, &major, &minor );
		
		if( major < 2 )
		{
			__warning( "DGA version %d %d is not supported\n", major, minor );
			SHP_SetVar( "use_xf86dga", "0", 0 );
		} else
		{
			SHV_Printf( "DGA version %d %d found.\n", major, minor );
		}
	}

#endif

	XSelectInput( arg_display, arg_window, PointerMotionMask | ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask );

	isdga = use_dga->ivalue;

	

	for( i = 0; i < 256; i++ )
	{
		shiftmap[i] = 0;
	}

	shiftmap['1'] = '!';
	shiftmap['2'] = '@';
	shiftmap['3'] = '#';
	shiftmap['4'] = '$';
	shiftmap['5'] = '%';
	shiftmap['6'] = '^';
	shiftmap['7'] = '&';
	shiftmap['8'] = '*';
	shiftmap['9'] = '(';
	shiftmap['0'] = ')';
	shiftmap['-'] = '_';
	shiftmap['='] = '+';
	shiftmap['\\'] = '|';
	shiftmap['['] = '{';
	shiftmap[']'] = '}';
	shiftmap[';'] = ':';
	shiftmap['\''] = '"';
	shiftmap[','] = '<';
	shiftmap['.'] = '>';
	shiftmap['/'] = '?';

	TFUNC_LEAVE;
}

void I_X11ShutDown( Window arg_window, Display *arg_display )
{
	TFUNC_ENTER;

	__named_message( "\n" );


	if( isdga )
	{
#if defined( linux_i386 )
		XF86DGADirectVideo( arg_display, g_scrnum, 0 );
#endif
	}


	XUngrabKeyboard( arg_display, CurrentTime);
	XUngrabPointer( arg_display, CurrentTime);

	XAutoRepeatOn ( arg_display );
//	XAutoRepeatOn ( arg_display );

	XChangePointerControl( arg_display, True, True, mouse_accel, mouse_accel2, mouse_treshold );
	TFUNC_LEAVE;
}


void I_X11Update( Window arg_window, Display *arg_display )
{
	XEvent	event;

	keventlistptr = 1;

	md_x = md_y = 0;

//	SHV_Printf( "bla\n" );

	for( ;; )
	{
		if( !XEventsQueued( arg_display, QueuedAfterFlush ))
		{
			break;
		}
		
		XNextEvent( arg_display, &event );

		switch( event.type )
		{
		case KeyPress:
			type = SYMTYPE_PRESS;
			I_X11DoKey( event, arg_display );
			break;

		case KeyRelease:
			type = SYMTYPE_RELEASE;
//			__named_message( "key\n" );
			I_X11DoKey( event, arg_display );
			break;

		case MotionNotify:
			//__named_message( "motion\n" );
			if( isgrabed )
			{
				if( !isdga ) 
					I_X11DoMouse( event, arg_display, arg_window );
				else
					I_X11DoDGAMouse( event, arg_display );
			}
			break;
		       
		case ButtonPress:
			type = SYMTYPE_PRESS;
			I_X11DoMouseButton( event, arg_display );
			break;
	
		case ButtonRelease:
			type = SYMTYPE_RELEASE;
			I_X11DoMouseButton( event, arg_display );
			break;

		default:
			__named_message( "ignoring event\n" );
			break;
		}
	}

//	if( md_x || md_y )
//		printf( "mdx: %d mdy: %d\n", md_x, md_y );

	keventlist[0].sym = (gs_ksym)keventlistptr; // first element in list is listsize!
}

void I_X11GrabMouse( Display *arg_display, int flag )
{
	switch( flag )
	{
	case 1:
		if( isgrabed )
		{
			__warning( "mouse was grabbed\n" );
			break;
		}

		
		if( isdga )
		{
#if !defined( linux_i386 )
			__named_message( "use_dga is 1 on a system not supporting it\nset to 0\n" );
			isdga = 0;
#endif
		}
		

		if( isdga )
		{
#if defined( linux_i386 )
			XF86DGADirectVideo( arg_display, g_scrnum, XF86DGADirectMouse |XF86DGADirectKeyb );
#else
			__error( "DGA not supported in this system / hardware\n" );
#endif
		} else
		{
//		__error( "no non DGA supported, yet.\n" );
		}
		XGrabKeyboard( arg_display, DefaultRootWindow(arg_display), True, GrabModeAsync,
			       GrabModeAsync,CurrentTime);

		XGrabPointer( arg_display,DefaultRootWindow(arg_display), True,
			      PointerMotionMask | ButtonPressMask |ButtonReleaseMask,
			      GrabModeAsync, GrabModeAsync, None,  None, CurrentTime);
		
		isgrabed = 1;
		
		break;
		
	case 0:
		if( !isgrabed )
		{
			__warning( "pointer was not grabed\n" );
			break;
		}
		
		if( isdga )
		{
#if defined( linux_i386 )
			XF86DGADirectVideo( arg_display, g_scrnum, 0 );
#endif
		}

		XUngrabKeyboard( arg_display, CurrentTime);
		XUngrabPointer( arg_display, CurrentTime);
		isgrabed = 0;

		break;

	}

#ifdef USE_DGA
	if( isdga )
	{
		XSelectInput( g_display, g_window, ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask );
		XDGASelectInput( arg_display, g_scrnum, PointerMotionMask );
	} else
#endif
	{
		XSelectInput( g_display, g_window, PointerMotionMask | ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask );
	}
}
				
void I_X11DoKey( XEvent event, Display *arg_display )
{
	KeySym		keysym;
	KeyCode		keycode;
	unsigned int	gsksym = 0;



	keycode = (( XKeyEvent * ) &event)->keycode;

	keysym = XKeycodeToKeysym( arg_display, keycode, 0 );



//	printf( "keysym = %d\n", keysym );
	if( ( keysym >= 0x020 ) && ( keysym <= 0x07f ))  // west orientation!
	{
		// keysym == ASCII code

		gsksym = keysym;
		goto abs_end;
	}

	// do abstraction from X11 keysym to gsksym

	switch( keysym )
	{
// ascii control codes 0x0 to 0x1F
	case XK_BackSpace:
		gsksym = '\b';
		goto abs_end;

	case XK_Tab:
		gsksym = '\t';
		goto abs_end;	
	
	case XK_Return:
		gsksym = '\n';
		goto abs_end;	

// cursor keys
	case XK_Up:
	case XK_KP_Up:
		gsksym = GSK_CUP;
		goto abs_end;	

	case XK_Down:
	case XK_KP_Down:
		gsksym = GSK_CDOWN;
		goto abs_end;	

	case XK_Right:
	case XK_KP_Right:
		gsksym = GSK_CRIGHT;
		goto abs_end;

	case XK_Left:
	case XK_KP_Left:
		gsksym = GSK_CLEFT;
		goto abs_end;


// modifier keys
	case XK_Shift_L:
	case XK_Shift_R:
		gsksym = GSK_SHIFT;
		if( !isshift )
			isshift = 1;
		else
			isshift = 0;
		SHV_Printf( "isshift" );
		goto abs_end;

	case XK_Control_L:
	case XK_Control_R:
	case XK_Multi_key:
		gsksym = GSK_CTRL;
		goto abs_end;

	case XK_Alt_L:
	case XK_Alt_R:
	case XK_Meta_L:
	case XK_Meta_R:
		gsksym = GSK_ALT;
		goto abs_end;

		
	case XK_Super_L:
	case XK_Super_R:
		gsksym = GSK_FUCK0;
		goto abs_end;

	case XK_Hyper_L:
	case XK_Hyper_R:
		gsksym = GSK_FUCK1;
		goto abs_end;	
		// other cursor conrol keys

	case XK_Insert:
	case XK_KP_Insert:      
		gsksym = GSK_INSERT;
		goto abs_end;

	case XK_Home:
	case XK_KP_Home:
		gsksym = GSK_HOME;
		goto abs_end;	
		
	case XK_End:
	case XK_KP_End:
		gsksym = GSK_END;
		goto abs_end;

	case XK_Page_Up:
	case XK_KP_Page_Up:
		gsksym = GSK_PGUP;
		goto abs_end;

	case XK_Page_Down:
	case XK_KP_Page_Down:
		gsksym = GSK_PGDN;
		goto abs_end;	

	case XK_Escape:
		gsksym = GSK_ESCAPE;
		goto abs_end;

// function keys

	case XK_F1:
		gsksym = GSK_F1;
		goto abs_end;


	case XK_F2:
		gsksym = GSK_F2;
		goto abs_end;

	case XK_F3:
		gsksym = GSK_F3;
		goto abs_end;

	case XK_F4:
		gsksym = GSK_F4;
		goto abs_end;

	case XK_F5:
		gsksym = GSK_F5;
		goto abs_end;

	case XK_F6:
		gsksym = GSK_F6;
		goto abs_end;

	case XK_F7:
		gsksym = GSK_F7;
		goto abs_end;

	case XK_F8:
		gsksym = GSK_F8;
		goto abs_end;

	case XK_F9:
		gsksym = GSK_F9;
		goto abs_end;

	case XK_F10:
		gsksym = GSK_F10;
		goto abs_end;

	case XK_F11:
		gsksym = GSK_F11;
		goto abs_end;

	case XK_F12:
		gsksym = GSK_F12;
		goto abs_end;

	default:
		goto abs_end;

	}

 
abs_end:
	if( shiftmap[gsksym] && isshift )
		gsksym = shiftmap[gsksym];
	
	keventlist[keventlistptr].sym = (gs_ksym)gsksym;
	keventlist[keventlistptr].type = type;
	keventlistptr++;
//	printf( "gsksym: %d\n", gsksym );
}

void CenterPointer( Display *arg_display, Window arg_window )
{
	XWarpPointer( arg_display, None, arg_window, 0, 0, 0, 0,
		      r_devicewidth->ivalue/2, r_deviceheight->ivalue/2 );
}

void I_X11DoMouse( XEvent arg_event, Display *arg_display, Window arg_window )
{
//	SHV_Printf( "x: %d y: %d\n", ((XMotionEvent*)&arg_event)->x, ((XMotionEvent*)&arg_event)->y );
	if( (((XMotionEvent*)&arg_event)->x == (int)r_devicewidth->ivalue/2) && (((XMotionEvent*)&arg_event)->y == (int)r_deviceheight->ivalue/2) )
	{
//		SHV_Printf( "repeated event\n" );
		return;  // this event was caused by XWarpPointer
	}

	md_x += ((XMotionEvent*)&arg_event)->x - r_devicewidth->ivalue/2;
	md_y += ((XMotionEvent*)&arg_event)->y - r_deviceheight->ivalue/2;

//	SHV_Printf( "%d %d\n", md_x, md_y );

	CenterPointer( arg_display, arg_window );

	
}
	


void I_X11DoDGAMouse( XEvent arg_event, Display *arg_display )
{

	md_x += ((XMotionEvent*)&arg_event)->x_root;
	md_y += ((XMotionEvent*)&arg_event)->y_root;
//    SHV_Printf( "%d %d\n", md_x, md_y );
}


void I_X11DoMouseButton( XEvent arg_event, Display *arg_display )
{
	unsigned int	gsksym = 0;
	

	
	switch(((XButtonEvent*)&arg_event)->button )
	{
	case Button1:
		gsksym = GSK_BUTTON1;
		break;

	case Button2:
		gsksym = GSK_BUTTON2;
		break;
		
	case Button3:
		gsksym = GSK_BUTTON3;
		break;
	
	case Button4:
		gsksym = GSK_BUTTON4;
		break;

	
	case Button5:
		gsksym = GSK_BUTTON5;
		break;		

	default:
		__warning( "noknown mousebutton pressed\n" );
		break;
	}

	keventlist[keventlistptr].sym = (gs_ksym)gsksym;
	keventlist[keventlistptr].type = type;
	keventlistptr++;
}


void I_SetInputMode( int _mode )
{
	__named_message( "\n" );
}


unsigned char I_GetChar()
{
	return	'q';
}
