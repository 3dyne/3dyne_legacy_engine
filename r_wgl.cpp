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



// r_wgl.c

#define vgl_notextern

#include "interfaces.h"
#include "r_private.h"
#include "version.h"
#include "vgl_interface.inc"

#if 0
vgl_field_t	vwgl_fields[] = {
	{ "wglCreateContext", ( void ** )&vwglCreateContext },
	{ "wglDeleteContext", ( void ** )&vwglDeleteContext },
	{ "wglMakeCurrent", ( void ** )&vwglMakeCurrent },
	{ "wglGetProcAddress", ( void ** )&vwglGetProcAddress },
	{ "wglChoosePixelFormat", ( void ** )&vwglChoosePixelFormat },
	{ "wglSetPixelFormat", ( void ** )&vwglSetPixelFormat },
	{ "wglSwapBuffers", ( void ** )&vwglSwapBuffers },
	{ NULL ,NULL }
};
#endif
static char	*className = "OpenGL";
static char	*windowName = "DarkestDays " DD_BTYPE " - " BVERSION;

static int	init_width = 1024, init_height = 768, init_bpp = 16;

static int r_firsttime = 1;
static DEVMODE init_devmode;
static DEVMODE r_curmode;

static LPCRITICAL_SECTION	csection;
RECT	g_winrect;

HDC	g_hDC;
HGLRC	g_hGLRC;
HWND   	g_hWnd;
sh_var_t	*r_devicewidth, *r_deviceheight;

gl_info_t       *r_glinfo;

static sh_var_t	*r_gldriver, *r_fullscreen;

static WNDCLASS		wndClass;

static int wglh;

volatile unsigned int	testvar = 0;

void VGL_ResolveAPI()
{
#if 0
	int	i;
	for( i = 0; ; i++ )
	{
		if( !vgl_fields[i].name )
		{
	
			break;
		}

		*vgl_fields[i].proc = SYS_DllSym( wglh, vgl_fields[i].name );
		
		//		printf( "%s %p\n", vgl_fields[i].name, vwglGetProcAddress( vgl_fields[i].name ));

		if( !*vgl_fields[i].proc )
		{

	       	  	__warning( "%d: cannot resolve '%s'\n", i, vgl_fields[i].name );
		}
	}
	__message( "\t\tgl api loaded\n" );
#endif
}



#if 0
void VWGL_ResolveAPI()
{
	int	i;
	
	wglh = SYS_DllBegin( r_gldriver->string );

	for( i = 0; ; i++ )
	{
		if( !vwgl_fields[i].name )
		{
	
			break;
		}

		*vwgl_fields[i].proc = SYS_DllSym( wglh, vwgl_fields[i].name );
		
		if( !*vwgl_fields[i].proc )
		{

			__error( "cannot resolve '%s'\n", vwgl_fields[i].name );
		}
	}
	__message( "\t\twgl api loaded\n" );
}
#endif
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

		vglActiveTextureARB = (vglActiveTextureARB_t)wglGetProcAddress( "glActiveTextureARB" );
		vglClientActiveTextureARB = (vglClientActiveTextureARB_t)wglGetProcAddress( "glClientActiveTextureARB" );

		vglMultiTexCoord2fARB = (vglMultiTexCoord2fARB_t) wglGetProcAddress( "glMultiTexCoord2fARB" );
		vglMultiTexCoord2fvARB = (vglMultiTexCoord2fvARB_t) wglGetProcAddress( "glMultiTexCoord2fvARB" );

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

static void VGL_GetInfo( gl_info_t *info ) // fill malloced struct
{
	info->extstring = (char*) glGetString( GL_EXTENSIONS );
	printf( "%s\n", info->extstring );

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
	

	printf( "texunits %d\n", info->texenv_units );
	printf( "have add %d\n", info->texenv_have_add );

	info->flushinterval = 16;
}

void R_StartUp( void )
{
#if 1

#ifdef linux_i386
	asm( "movl %esp, _testvar\n" );
	fprintf( stderr, "esp: %x\n", testvar );
#endif

	TFUNC_ENTER;
	r_gldriver = SHP_GetVar( "r_gldriver_win32" );
	__named_message( "\topenGL driver is: %s\n", r_gldriver->string );

	r_fullscreen = SHP_GetVar( "r_fullscreen" );

//	GL_LoadGLApi( r_gldriver->string );
	
//	GL_LoadWGLApi( r_gldriver->string );
	//VWGL_ResolveAPI();

	//VGL_ResolveAPI();

	

//	#if( !vglBegin || !vglVertex3f )
	//  {
	 //   __error( "gl api is fucked up. This happens on windows sometimes. plaease try running again!\n" );
	//  }
	

//	InitializeCriticalSection( csection );
#endif

	r_devicewidth = SHP_GetVar( "r_devicewidth" );
	r_deviceheight = SHP_GetVar( "r_deviceheight" );

//	asm( "movl %esp, _testvar\n" );
//	printf( "esp: %x\n", testvar );

	R_WindowStartUp();
//	asm( "movl %esp, _testvar\n" );
//	printf( "esp: %x\n", testvar );

	fflush( stdout );

	R_WGLStartUp();
	
	Sleep( 1000 );

#if 1
	glBegin( GL_POLYGON );

	glVertex3f( -0.5, -0.5, 0 );
	glVertex3f( 0.5, -0.5, 0 );
	glVertex3f( 0.5, 0.5, 0 );
	glVertex3f( -0.5, 0.5, 0 );
	glEnd();

	glFlush();
       	SwapBuffers( g_hDC );
      
#endif

	I_StartUp();

	if( r_fullscreen->ivalue ) // grab mouse in fullscreen mode.
		R_Hint( R_HINT_GRAB_MOUSE );
	
	TFUNC_LEAVE;
}

void R_ShutDown( void )
{
	TFUNC_ENTER;
	I_ShutDown();
	R_WindowShutDown();
	TFUNC_LEAVE;
}
void R_SwapBuffer( void )
{
	TFUNC_ENTER;
	SwapBuffers( g_hDC );
	TFUNC_LEAVE;
}

//============================================================================
// 2d window stuff.
//============================================================================


LRESULT APIENTRY R_WinCallBack(
	HWND hWnd,
	UINT message,
	WPARAM wParam,
	LPARAM lParam )
{
	LONG ret;
	TFUNC_ENTER;
	ret = 0;

       

	switch( message )
	{
	case WM_CREATE:
		__named_message( "WM_CREATE\n" );
		break;

	case WM_DESTROY:
		__named_message( "WM_DESTROY\n" );
		break;

	case WM_MOVE:
		GetWindowRect( g_hWnd, &g_winrect );

		printf( "%d %d\n", g_winrect.left, g_winrect.top );
		break;

	default:
		ret = DefWindowProc(hWnd, message, wParam, lParam);
		break;
	}
	TFUNC_LEAVE;
	return ret;
};


int R_TryFullscreen()
{
	int	i;
	DEVMODE	devmode;
	
	for( i = 0; ; i++ )
	{
		if(!EnumDisplaySettings( NULL, i, &devmode ))
			break;

		if( devmode.dmPelsWidth == r_devicewidth->ivalue && devmode.dmPelsHeight == r_deviceheight->ivalue && devmode.dmBitsPerPel == 16 )
		{
			printf( "found fullscreen mode\n" );
			if( ChangeDisplaySettings( &devmode, 0 ) != DISP_CHANGE_SUCCESSFUL )
			{
				__warning( "ChangeDisplaySetting failed?!\n" );
				return 1;
			}
			return 0;
		}
	}
	printf( "fullscreen mode is not in the valid modelist\n" );
	return 1;
}

void R_GetInitDevmode()
{
       	DEVMODE		devmode;
	int		i;

	init_width = GetSystemMetrics( SM_CXSCREEN );
	init_height = GetSystemMetrics( SM_CYSCREEN );

	printf( "init: %d %d\n", init_width, init_height );


	for( i = 0; ; i++ )
	{
		if(!EnumDisplaySettings( NULL, i, &devmode ))
			break;

		if( devmode.dmPelsWidth == init_width && devmode.dmPelsHeight == init_height && devmode.dmBitsPerPel == init_bpp )
		{
			printf( "current mode: " );
			memcpy( &init_devmode, &devmode, sizeof( DEVMODE ));
		}

		printf( "mode %i: %d-%d-%d\n", i, devmode.dmPelsWidth, devmode.dmPelsHeight, devmode.dmBitsPerPel );
	}
}

void R_WindowStartUp()
{
	MSG		mesg;
	DEVMODE		devmode;
	int		res;

	TFUNC_ENTER;
	


	if( r_firsttime )
	{
		R_GetInitDevmode();
		r_firsttime = 0;
	}
	
	if( r_fullscreen->ivalue )
	{
		if( R_TryFullscreen())
		{
            SHV_Printf( "cannot set fullscreen mode\n" );
			SHP_SetVar( "r_fullscreen", "0", 0 );
		}
	}

	// build window class
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = R_WinCallBack;
	wndClass.cbClsExtra = 0;
	wndClass.cbWndExtra = 0;
	wndClass.hInstance = g_wininstance;
	wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = className;
	RegisterClass(&wndClass);


	// create window
	if( r_fullscreen->ivalue )
	{
		g_hWnd = CreateWindowEx(
			WS_EX_TOPMOST,
			className, windowName,
			WS_POPUP,
			0, 0, r_devicewidth->ivalue, r_deviceheight->ivalue,
			NULL, NULL, g_wininstance, NULL );

		g_winrect.left = 0;
		g_winrect.top = 0;
		
	} else
	{
		g_hWnd = CreateWindow( 
			className, windowName,
			WS_OVERLAPPEDWINDOW,
			0, 0, r_devicewidth->ivalue, r_deviceheight->ivalue,
			NULL, NULL, g_wininstance, NULL );
	}

	g_ncmdshow = 5;
	ShowWindow(g_hWnd, g_ncmdshow);
	UpdateWindow(g_hWnd);

	if( !r_fullscreen->ivalue )
	{
		GetWindowRect( g_hWnd, &g_winrect );
	}

	TFUNC_LEAVE;
}


void R_WindowShutDown()
{
	DEVMODE		devmode;
	int	res;
	TFUNC_ENTER;
#if 1
	if( g_hGLRC )
	{
		wglMakeCurrent( NULL, NULL );
		wglDeleteContext( g_hGLRC );
	}
#endif
	ReleaseDC( g_hWnd, g_hDC );

	if( ( !r_firsttime ) && r_fullscreen->ivalue )
	{
		res = ChangeDisplaySettings( &init_devmode, 0 );
		if( res != DISP_CHANGE_SUCCESSFUL )
		{
			__warning( "cannot switch back resolution: %d\n", res );
		}
	}
	__named_message( "\n" );	
	TFUNC_LEAVE;
}

//============================================================================
// WGL startup stuff
//============================================================================

void R_WGLSetupPalette()
{
	
	int pixelFormat = GetPixelFormat(g_hDC);
	PIXELFORMATDESCRIPTOR pfd;
	TFUNC_ENTER;
//	__named_message( "\n" );


//	DescribePixelFormat( g_hDC, pixelFormat, sizeof(PIXELFORMATDESCRIPTOR), &pfd);
	
//	if (pfd.dwFlags & PFD_NEED_PALETTE)
		//__error( "we need at least a 16bit visual\n" );
	TFUNC_LEAVE;
}

void R_WGLSetupPixelFormat()
{
	char	errbuf[512];

	int	pixelFormat;
	PIXELFORMATDESCRIPTOR *ppfd;
	PIXELFORMATDESCRIPTOR pfd = {
		sizeof(PIXELFORMATDESCRIPTOR),  /* size */
		1,                              /* version */
		PFD_SUPPORT_OPENGL |
		PFD_DRAW_TO_WINDOW |
		PFD_DOUBLEBUFFER,               /* support double-buffering */
		PFD_TYPE_RGBA,                  /* color type */
		16,                             /* prefered color depth */
		0, 0, 0, 0, 0, 0,               /* color bits (ignored) */
		0,                              /* no alpha buffer */
		0,                              /* alpha bits (ignored) */
		0,                              /* no accumulation buffer */
		0, 0, 0, 0,                     /* accum bits (ignored) */
		16,                             /* depth buffer */
		0,                              /* no stencil buffer */
		0,                              /* no auxiliary buffers */
		PFD_MAIN_PLANE,                 /* main layer */
		0,                              /* reserved */
		0, 0, 0,                        /* no layer, visible, damage masks */
	};

	

	TFUNC_ENTER;

	ppfd = &pfd;

	

	pixelFormat = ChoosePixelFormat( g_hDC, ppfd);
	
	if( !pixelFormat )
	{
		
		FormatMessage((FORMAT_MESSAGE_FROM_SYSTEM), NULL, GetLastError(),
			      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			      errbuf, 512, NULL);
		//__error( "ChoosePixelFormat failed: '%s'\n", errbuf );
	}

	printf( "pixelformat: %d\n", pixelFormat );

	if (SetPixelFormat( g_hDC, pixelFormat, &pfd) == FALSE )
	{
		FormatMessage((FORMAT_MESSAGE_FROM_SYSTEM), NULL, GetLastError(),
			      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			      errbuf, 512, NULL);
		__error( "SetPixelFormat failed: '%s'\n", errbuf );
	}

	TFUNC_LEAVE;
}

void R_WGLStartUp()
{
	BOOL	res;

	TFUNC_ENTER;
	__named_message( "\n" );
	g_hDC = GetDC( g_hWnd );
	__chkptr( g_hDC );
	__named_message( "have dc\n" );
#if 1
	//	R_WGLSetupPalette();
	R_WGLSetupPixelFormat();

	__chkptr( wglCreateContext );
	__chkptr( wglMakeCurrent );
	

      	g_hGLRC = wglCreateContext( g_hDC );
       	
	if( !g_hGLRC )
	{
		char	errbuf[512];
		FormatMessage((FORMAT_MESSAGE_FROM_SYSTEM), NULL, GetLastError(),
			      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
			      errbuf, 512, NULL);
		__error( "wglCreateContext failed: '%s'\n", errbuf );
	}	

	res =  wglMakeCurrent( g_hDC, g_hGLRC );
             //              _wglMakeCurrent
	printf( "wglMakeCurrent: %d\n", res );
#endif

	r_glinfo = NEWTYPE( gl_info_t );
	__chkptr( r_glinfo );
	//	GL_GetInfo( r_glinfo );

	r_glinfo->flushinterval = 16;

	__named_message( "leaving\n" );
	TFUNC_LEAVE;
}

void R_Hint( int hint )
{
	switch( hint )
	{
	case R_HINT_GRAB_MOUSE:
		I_GrabMouse( 1 );
		SetCursor( LoadCursor( (HINSTANCE)g_hWnd, IDC_NO ));
		break;
	case R_HINT_UNGRAB_MOUSE:
		I_GrabMouse( 0 );
		SetCursor( LoadCursor( (HINSTANCE)g_hWnd, IDC_ARROW ));
		break;
	default:
		__warning( "unsuported render hint\n" );
		break;
	}
}
