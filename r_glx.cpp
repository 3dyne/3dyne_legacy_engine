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



// r_glx.c

#define vgl_notextern
#include <X11/cursorfont.h>
#include "interfaces.h"
#include "r_private.h"
#include "r_interface.h"

#include "vgl_interface.inc"


Window		g_window, g_rootwin;
Display		*g_display;
char		*displayname = NULL;
int		g_scrnum;
XVisualInfo	*g_visinfo;
GLXContext	g_ctx;
sh_var_t	*r_devicewidth, *r_deviceheight;

int		r_flushinterval;
gl_info_t	*r_glinfo = NULL;

static Cursor	emptyCursor = 0;
static Cursor	normalCursor = 0;
static Pixmap	emptyPixmap = 0;

static sh_var_t	*r_gldriver;
static sh_var_t	*r_fullscreen;

static sh_var_t	*vgl_warn;

static	int	vgl_notresolved;


// shutdown locks. they prevent the exit functions from being called. 
static int	mtx_i_x11 = 1;
static int	mtx_r_glx = 1;
static int	mtx_r_x11 = 1;

static XF86VidModeModeInfo	init_mode;
static XF86VidModeModeInfo stdvga_mode; 

static int	glh;

vgl_field_t	vglx_fields[] = {
	{ "glXSwapBuffers", ( void ** )&vglXSwapBuffers },
	{ "glXWaitGL", ( void ** )&vglXWaitGL },
	{ "glXCreateContext", ( void ** )&vglXCreateContext },
	{ "glXMakeCurrent", ( void ** )&vglXMakeCurrent },
	{ "glXDestroyContext", ( void ** )&vglXDestroyContext },
	{ "glXChooseVisual", ( void ** )&vglXChooseVisual },
	{ NULL, NULL }
};
		

void VGL_ResolveField( vgl_field_t *field )
{
	*field->proc = SYS_DllSym( glh, field->name );

	if( !*field->proc )
	{
		vgl_notresolved++;
		if( vgl_warn->ivalue )
			__warning( "cannot resolve '%s'\n", field->name );
	}

}

void VGL_ResolveAPI()
{
	int	i;
	
	vgl_notresolved = 0;
	vgl_warn = SHP_GetVar( "vgl_warn" );

	for( i = 0; ; i++ )
	{
		if( !vgl_fields[i].name )
		{
	
			break;
		}

		VGL_ResolveField( &vgl_fields[i] );
	}

	if( vgl_notresolved )
		__warning( "%d symbols unresolved\n", vgl_notresolved );
	__message( "\t\tgl api loaded\n" );
}


void VGL_FreeAPI()
{
	int	i;
	
	for( i = 0;; i++ )
	{
		if( !vgl_fields[i].name )
			break;

		*vgl_fields[i].proc = NULL;
	}

	__message( "gl api freed.\n" );
}

void VGLX_ResolveAPI()
{
	int	i;
	
	glh = SYS_DllBegin( r_gldriver->string );

	for( i = 0; ; i++ )
	{
		if( !vglx_fields[i].name )
		{
	
			break;
		}

		VGL_ResolveField( &vglx_fields[i] );
	}
	__message( "\t\tglx api loaded\n" );
}
	
	
void VGLX_FreeAPI()
{
	int	i;

	for( i = 0;; i++ )
	{
		if( !vglx_fields[i].name )
			break;

		*vglx_fields[i].proc = NULL;
	}

	__message( "glx api freed.\n" );
}


static void VGL_ResolveExt( void )
{

	if( !r_glinfo )
	{
		__warning( "r_glinfo is NULL\n" );
		return;
	}

	
	if( r_glinfo->arb_multitexture )
	{
		__named_message( "\tloading ARB_MULTITEXTURE\n" );
		vglActiveTextureARB = (vglActiveTextureARB_t)SYS_DllSym( glh, "glActiveTextureARB" );
		vglClientActiveTextureARB = (vglClientActiveTextureARB_t)SYS_DllSym( glh, "glClientActiveTextureARB" );

		vglMultiTexCoord2fARB = (vglMultiTexCoord2fARB_t)SYS_DllSym( glh, "glMultiTexCoord2fARB" );
		vglMultiTexCoord2fvARB = (vglMultiTexCoord2fvARB_t)SYS_DllSym( glh, "glMultiTexCoord2fvARB" );

		__chkptr( vglActiveTextureARB );
		__chkptr( vglClientActiveTextureARB );
				    
		__chkptr( vglMultiTexCoord2fARB );
		__chkptr( vglMultiTexCoord2fvARB );

	
		glActiveTextureARB = vglActiveTextureARB;
		glClientActiveTextureARB = vglClientActiveTextureARB;
		glMultiTexCoord2fARB = vglMultiTexCoord2fARB;
		glMultiTexCoord2fvARB = vglMultiTexCoord2fvARB;
	}

}			


static void VGL_GetSpecialInfo( gl_info_t *info ) // maintain insane drivers ( 3dfx mesa ... )
{
	char	*ver;

	ver = ( char * )glGetString( GL_RENDERER );


// probe for insane 3dfx mesa
	if( strstr( ver, "Glide" ))
	{
		__named_message( "\tMesa Glide driver flush workaround activated\n" );
	
		info->flushinterval = 0;
	}

	// all the others should work with default values
}
 
static void VGL_GetInfo( gl_info_t *info ) // fill malloced struct
{
	info->extstring = (char*) glGetString( GL_EXTENSIONS );
//	printf( "%s\n", info->extstring );

	if( strstr( info->extstring, "GL_ARB_multitexture" ))
	{
//		printf( "arb mulititexture\n" );
		__warning( " ***** ignore GL_MAX_TEXTURE_UNITS_ARB *****\n" );
		info->texenv_units = 1;
		info->arb_multitexture = 1;
//		glGetIntegerv( GL_MAX_TEXTURE_UNITS_ARB, &info->texenv_units );
	} else
	{
		info->arb_multitexture = 0;
		info->texenv_units = 1;
	}
	if( strstr( info->extstring, "GL_EXT_texture_env_add" ))
	{
		info->texenv_have_add = 1;
	} else
	{
		info->texenv_have_add = 0;
	}		
	

//	printf( "texunits %d\n", info->texenv_units );
//	printf( "have add %d\n", info->texenv_have_add );

	info->flushinterval = 1024;
	VGL_GetSpecialInfo( info );
	
}


int R_X11Error( Display *display, XErrorEvent *ee )
{
	char	msg[256];
	char	funcname[256];

	XGetErrorText( display, ee->error_code, msg, 256 );

	strncpy( funcname, "unknown", 255 );

#ifdef trace_functions
	TF_LastFunction( funcname, 255 );
#endif

	__error( "X11: '%s'\n  Major opcode: %d\n  Minor opcode: %d\n  last function is: '%s'\n", msg, ee->request_code, ee->minor_code, funcname );

    return 0;
}


/*
  ====================
  R_StartUp

  ====================
*/
void R_StartUp()
{
	TFUNC_ENTER;

	XSetErrorHandler( R_X11Error );
	
	r_gldriver = SHP_GetVar( "r_gldriver" );
	__named_message( "\topenGL driver is: %s\n", r_gldriver->string );

//	GL_LoadGLXApi( r_gldriver->string );
//	GL_LoadGLApi( r_gldriver->string );
	

	VGLX_ResolveAPI();
	VGL_ResolveAPI();

	r_devicewidth = SHP_GetVar( "r_devicewidth" );
	r_deviceheight = SHP_GetVar( "r_deviceheight" );
	r_fullscreen = SHP_GetVar( "r_fullscreen" );

	mtx_r_x11 = 1;
	R_X11StartUp();
	mtx_r_x11 = 0;

	mtx_r_glx = 1;
	
	R_GLXStartUp( g_window, g_display );

	r_glinfo = (gl_info_t *)MM_Malloc( sizeof( gl_info_t ));
	__chkptr( r_glinfo );
	VGL_GetInfo( r_glinfo );

	VGL_ResolveExt();

	mtx_r_glx = 0;

       

	mtx_i_x11 = 1;
	I_X11StartUp( g_window, g_display );
	mtx_i_x11 = 0;

	// some X11 postwork
	XMapWindow( g_display, g_window );
	XSync( g_display, True );

	// first visual openGL test
	glBegin( GL_POLYGON );

	glVertex3f( -0.5, -0.5, 0 );
	glVertex3f( 0.5, -0.5, 0 );
	glVertex3f( 0.5, 0.5, 0 );
	glVertex3f( -0.5, 0.5, 0 );
	glEnd();

	glFlush();
	vglXSwapBuffers( g_display, g_window );
	glClear( GL_COLOR_BUFFER_BIT );

// let e'm say sth
	R_Talk();

	if( r_fullscreen->ivalue )
		R_Hint( R_HINT_GRAB_MOUSE );

	TFUNC_LEAVE;
}

/*
  ====================
  R_ShutDown

  ====================
*/
void R_ShutDown()
{
	TFUNC_ENTER;

	__named_message( "\n" );
	if( mtx_i_x11 )
		__warning( "i_x11 lock set. cannot shutdown i_x11\n" );
	else
	{
		mtx_i_x11 = 1; // exit functions must be protected of recursive errors, too ( is this right? )
		I_X11ShutDown( g_window, g_display );
	}


	if( mtx_r_glx )
		__warning( "r_glx lock set. cannot shutdown r_glx\n" );
	else
	{
		mtx_r_glx = 1;
		R_GLXShutDown( g_window, g_display );
	} 


	if( mtx_r_x11 )
		__warning( "r_x11 lock set. cannot shutdown r_x11\n" );
	else
	{
		mtx_r_x11 = 1;
		R_X11ShutDown( g_window, g_display );
	}
		

//	GL_RejectGLApi();
//	GL_RejectGLXApi();

	VGL_FreeAPI();
	VGLX_FreeAPI();

	XSetErrorHandler( NULL );

	TFUNC_LEAVE;
}

/*
  ====================
  R_SwapBuffer

  ====================
*/
void R_SwapBuffer( void )
{
//	__named_message( "\n" );
	glFlush();
	vglXWaitGL();
	vglXSwapBuffers( g_display, g_window );
	XFlush( g_display );
//	XSync( g_display, True );
}

// =============================================================================


void R_GLXStartUp( Window arg_window, Display *arg_display )
{


	TFUNC_ENTER;

	// startup GLX
	printf( "==============================================================================\nif the program crashes after this line with signal 11, it is probably up to\nthe Mesa 3dfx driver. Try running as root, or to install the 3dfx device.\n==============================================================================\n" );
	
	g_ctx = vglXCreateContext( g_display, g_visinfo, NULL, True );
	vglXMakeCurrent( arg_display, arg_window, g_ctx );


	TFUNC_LEAVE;
}

void R_GLXShutDown( Window arg_window, Display *arg_display )
{
	TFUNC_ENTER;
	__named_message( "\n" );
	vglXMakeCurrent( arg_display, 0, 0 );
	vglXDestroyContext( arg_display, g_ctx );

	if( r_glinfo )
		MM_Free( r_glinfo );
	TFUNC_LEAVE;
}

int R_TryFullscreen()
{
// 	int	width, height
	int i, dotclock;
	XF86VidModeModeInfo	**minfo;
	XF86VidModeModeLine	modeline;
	int		modenum;
	int		res = 1;
	int		have_initmode = 0;
	int have_stdvga = 0;

// 	width = r_devicewidth->ivalue;
// 	height = r_deviceheight->ivalue;
	
	XF86VidModeGetAllModeLines( g_display, g_scrnum,
				    &modenum,
				    &minfo );

	res = 1;

	XF86VidModeGetModeLine( g_display, g_scrnum, &dotclock, &modeline );
		
	__message( "\t\tcur mode: %d %d\n", modeline.hdisplay, modeline.vdisplay );
	have_initmode = 0;
	for( i = 0; i < modenum; i++ )
	{
		printf( "%d %d\n", minfo[i]->hdisplay, minfo[i]->vdisplay );
		if( ( minfo[i]->hdisplay == modeline.hdisplay ) && ( minfo[i]->vdisplay == modeline.vdisplay ))
		{
			memcpy( &init_mode, minfo[i], sizeof( XF86VidModeModeInfo ));
			have_initmode = 1;
		}

		if( ( minfo[i]->hdisplay == 640 ) && ( minfo[i]->vdisplay == 480 ))
		{
			memcpy( &stdvga_mode, minfo[i], sizeof( XF86VidModeModeInfo ));
			have_stdvga = 1;
		}
		
	}

	if( !have_stdvga )
	{
		__warning( "no Modeline called \"640x480\" available\n" );
		__message( "try adding \"Modeline \"640x480\"     25.175 640  664  760  800   480  491  493  525\" to the Monitor section of your /etc/XF86Config file\n" );
	}

	if( !have_initmode )
	{
		SHV_Printf( "current mode not found !?. failing!" );
		res = 1;
	} else
	{	
		for( i = 0; i < modenum; i++ )
		{
//			printf( "\t\tmode %d: %d %d\n", i, minfo[i]->hdisplay, minfo[i]->vdisplay );
			
			
			if( ( minfo[i]->hdisplay == r_devicewidth->ivalue ) && ( minfo[i]->vdisplay == r_deviceheight->ivalue ))
			{
				printf( "\t\tfullscreen mode: %dx%d dotclock: %d\n", minfo[i]->hdisplay, minfo[i]->vdisplay, minfo[i]->dotclock  );
				
				XF86VidModeSwitchToMode( g_display, g_scrnum,
							 minfo[i] );
				
				XWarpPointer( g_display, None, None, 0, 0, 0, 0, 0, 0 );
				res = 0;
			}
		}

		if( ( res == 1 ) && have_stdvga )
		{
			__named_message( "requested vidmode (%dx%d) not found. using std vga mode (640x480@60Hz)\n", r_devicewidth->ivalue, r_deviceheight->ivalue );
			
			XF86VidModeSwitchToMode( g_display, g_scrnum, &stdvga_mode );
			
			//XWarpPointer( g_display, None, None, 0, 0, 0, 0, 0, 0 );
			SHP_SetVar( "r_devicewidth", "640", 0 );
			SHP_SetVar( "r_deviceheight", "480", 0 );
			res = 0;
		}

#if 0
		if( res == 1 ) // mode not found. take the standard vga mode
		{
			__named_message( "requested vidmode (%dx%d) not found. using std vga mode (640x480@60Hz)\n", r_devicewidth->ivalue, r_deviceheight->ivalue );
			
			if( ( res2 = XF86VidModeValidateModeLine( g_display, g_scrnum, &stdvga_mode )) != 255 )
			{
				//			__warning( "cannot Validate standard vga mode.\n" );
				__named_message( "Validate: %d\n", res2 ); 
				res = 1;
				goto outside;
			}
					   

			if( XF86VidModeModModeLine( g_display, g_scrnum, &stdvga_mode ))
			{
				__warning( "failed to set standard vga mode (640x480@60). fullscreen mode impossible on this machine\n" );
				XF86VidModeSwitchToMode( g_display, g_scrnum,
							 &init_mode);
				res = 1;

			} else
			{
				XF86VidModeSwitchToMode( g_display, g_scrnum,
							 minfo[i] );
				
				XWarpPointer( g_display, None, None, 0, 0, 0, 0, 0, 0 );
				SHP_SetVar( "r_devicewidth", "640", 0 );
				SHP_SetVar( "r_deviceheight", "480", 0 );

				res = 0;
			}
		
			// I hope this helps!


		}
#endif
	}
// TODO:

// 	outside:
	

#if 0 
	for( i = 0; i < modenum; i++ )
	{
		free( minfo[i] );
	}
#endif

	return res;
}
	

void R_CheckExtensions()
{
	char	**extlist;
	int	extnum;
	int	vidmode = 0, dga = 0, i;

	extlist = XListExtensions( g_display, &extnum );
	
	for( i = 0; i < extnum; i++ )
	{
//		printf( "%s\n", extlist[i] );

		if( !strcmp( extlist[i], "XFree86-VidModeExtension" ))
		{
			vidmode = 1;
			__message( "\t\tXFree86-VidModeExtension\n" );
		}

		if( !strcmp( extlist[i], "XFree86-DGA" ))
		{
			dga = 1;
			__message( "\t\tXFree86-DGA\n" );
		}
	}

	XFreeExtensionList( extlist );

	if( !dga )
		SHP_SetVar( "use_xf86dga", "0", 0 );
	
	if( !vidmode ) {
        SHP_SetVar( "r_fullscreen", "0", 0 );
    }
}


void R_X11StartUp()
{
	int attrib[] = { GLX_RGBA,
			 GLX_RED_SIZE, 1,
			 GLX_GREEN_SIZE, 1,
			 GLX_BLUE_SIZE, 1,
			 GLX_DOUBLEBUFFER,
			 GLX_DEPTH_SIZE, 2,
			 None };

	XSetWindowAttributes	xattr;

	unsigned long		mask;
//	XF86VidModeModeLine		modeline;
//	int		dotclock;

	// open winow, set size, color depth ....


	TFUNC_ENTER;

	 __named_message( "\n" );
	if( (g_display = XOpenDisplay(displayname )) == NULL )
	{
		__error( "XOpenDisplay failed\n" );
	}


	g_scrnum = DefaultScreen( g_display );

	g_rootwin = RootWindow( g_display, g_scrnum );

	g_visinfo = vglXChooseVisual( g_display, g_scrnum, attrib );

	if( !g_visinfo )
		__error( "glXChooseVisual failed.\n" );

	R_CheckExtensions();

	
	if( r_fullscreen->ivalue )
	{
		if( R_TryFullscreen() ) // failed to find requested mode
		{
          //  __error( "fullscreen failed\n" );

			SHV_Printf( "cannot set fullscreen mode" );
			SHP_SetVar( "r_fullscreen", "0", 0 );
		}
	}
		

	xattr.background_pixel = 0;
	xattr.border_pixel = 0;

	xattr.colormap = XCreateColormap( g_display, g_rootwin, g_visinfo->visual, AllocNone );
	xattr.event_mask = StructureNotifyMask | ExposureMask;


	mask = CWBackPixel | CWBorderPixel | CWColormap | CWEventMask;

	if( r_fullscreen->ivalue )
	{
		xattr.override_redirect = 1;
		mask |= CWOverrideRedirect;
	}

	g_window = XCreateWindow( g_display, g_rootwin, 0, 0, 
				  r_devicewidth->ivalue, r_deviceheight->ivalue,
				  0, g_visinfo->depth, InputOutput,
				  g_visinfo->visual, mask, &xattr );


	

	XSync( g_display, True );


	XWarpPointer( g_display, None, g_window, 0, 0, 0, 0, 0, 0 );

#if 0
	g_window = XCreateSimpleWindow( g_display, RootWindow( g_display, screen_num ), 
				      0, 0,200, 200, 1, BlackPixel( g_display, screen_num ),
				      WhitePixel( g_display, screen_num ));


#endif

        {
          	XColor	Black;
		memset (&Black, 0, sizeof (Black));
                emptyPixmap = XCreatePixmap( g_display, g_window, 1, 1, 1 );

                emptyCursor = XCreatePixmapCursor( g_display, emptyPixmap, emptyPixmap, &Black, &Black, 0, 0 );
		
		normalCursor = XCreateFontCursor( g_display, XC_iron_cross );

		XDefineCursor( g_display, g_window, normalCursor );
	}
//	__named_message( "ready\n" );
	TFUNC_LEAVE;
}


void R_X11ShutDown( Window arg_window, Display *arg_display )
{
	TFUNC_ENTER;

	__named_message( "\n" );
	if( r_fullscreen->ivalue )
	{
		XF86VidModeSwitchToMode( arg_display, g_scrnum,
					 &init_mode );
	}
	
	if( emptyCursor )
	{
		XFreeCursor( g_display, emptyCursor );
		emptyCursor = 0;
	}

	if( normalCursor )
	{
		XFreeCursor( g_display, normalCursor );
		normalCursor = 0;
	}

	if( emptyPixmap )
	{
		XFreePixmap( g_display, emptyPixmap );
		emptyPixmap = 0;
	}

	XDestroyWindow( arg_display, arg_window );
	XFlush( arg_display );
	XCloseDisplay( arg_display );
	TFUNC_LEAVE;
}

void R_Hint( int hint )
{
	switch( hint )
	{
	case R_HINT_GRAB_MOUSE:
		XDefineCursor( g_display, g_window, emptyCursor );
		I_X11GrabMouse( g_display, 1 );
		break;

	case R_HINT_UNGRAB_MOUSE:
		if( r_fullscreen->ivalue )
		{
			SHV_Printf( "cannot ungrab mouse in fullscreen mode\n" );
			break;
		}
		XDefineCursor( g_display, g_window, normalCursor );
		I_X11GrabMouse( g_display, 0 );
		break;

	default:
		__warning( "unknown render hint: %d\n", hint );
	}
}
