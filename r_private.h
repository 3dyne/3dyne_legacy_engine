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



// r_private.h


#ifndef r_private_h
#define r_private_h

#include "defs.h"
#include "gl_defs.h"

extern sh_var_t		*r_devicewidth, *r_deviceheight;
extern gl_info_t	*r_glinfo;



#if defined(linux_i386) || defined(irix_mips)

#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#if defined( linux_i386 )
#include <X11/extensions/Xxf86dga.h>
#include <X11/extensions/xf86vmode.h>
#endif

// GL/gl.h is included in shared/sys_interfaces.h !
//#include "gl_service.h"
//#include "glx_service.h"

#include "vgl_interface.h"
#include "vglx_interface.h"


extern Window		g_window;
extern Window		g_rootwin;
extern Display		*g_display;
extern char		*displayname;
extern int		g_scrnum;
extern XVisualInfo	*g_visinfo;
extern GLXContext	g_ctx;

// internal Abstraction

void R_X11StartUp();  // open X11 window
void R_X11ShutDown( Window arg_window, Display *arg_display );

void R_GLXStartUp( Window, Display* ); // init GLX
void R_GLXShutDown( Window arg_window, Display *arg_display );

void I_X11StartUp( Window, Display* ); // init (DGA) keyboard, mouse ... input
void I_X11ShutDown( Window, Display* );

void I_X11Update( Window, Display* );
void I_X11DoKey( XEvent, Display* );

void I_X11GrabMouse( Display *, int );

void I_X11DoDGAMouse( XEvent, Display* );
void I_X11DoMouse( XEvent, Display*, Window );

void I_X11DoMouseButton( XEvent, Display* );

#elif defined( win32_x86 )

#include <windows.h>
//#include <directx.h>
//#include <dinput.h>

#include "vgl_interface.h"
#include "vwgl_interface.h"

extern HINSTANCE	g_wininstance;
extern int		g_ncmdshow;

extern HDC	g_hDC;
extern HGLRC	g_hGLRC;
extern HWND    	g_hWnd;

extern RECT	g_winrect;

//extern LPDIRECTINPUTDEVICE lpdiMouse;
//extern LPDIRECTINPUTDEVICE lpdiKeyboard;

extern unsigned char i_dxscans[];

void R_WindowStartUp();
void R_WindowShutDown();

void R_WGLStartUp();
void R_WGLShutDown();

void I_StartUp();
void I_ShutDown();

void I_GrabMouse( int flag );

//void I_Win32StartUp( HWND );
//void I_Win32ShutDown();

void I_DXInitScanTab( void );

#endif

/*
  =============================================================================
  internal render protoypes
  =============================================================================
*/

/*
  ==================================================
  r_gltex.c

  ==================================================
*/
GLuint R_SetupTexture_TGA_RGB888_mipmap( ca_tga_t *tga );
GLint R_SetupTexture_TGA_RGBA8888_mipmap( ca_tga_t *tga );
GLint R_SetupTexture_TGA_RGB888( ca_tga_t *tga );
GLint R_SetupTexture_TGA_RGBA8888( ca_tga_t *tga );


#endif
