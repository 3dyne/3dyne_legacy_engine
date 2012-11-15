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



// glx_service.h

#ifndef glx_service_h
#define glx_service_h

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "gl_defs.h"
#include "glx_defs.h"

#ifndef GLAPI
#define GLAPI extern
#endif

void GL_LoadGLXApi( char * );
void GL_RejectGLXApi();



GLAPI XVisualInfo* (* glXChooseVisual ) ( Display *dpy, int screen,
				     int *attribList );

GLAPI GLXContext (* glXCreateContext ) ( Display *dpy, XVisualInfo *vis,
				    GLXContext shareList, Bool direct );

GLAPI void (* glXDestroyContext ) ( Display *dpy, GLXContext ctx );

GLAPI Bool (* glXMakeCurrent ) ( Display *dpy, GLXDrawable drawable,
		     GLXContext ctx);

GLAPI void (* glXCopyContext ) ( Display *dpy, GLXContext src, GLXContext dst,
			    GLuint mask );

GLAPI void (* glXSwapBuffers ) ( Display *dpy, GLXDrawable drawable );

GLAPI GLXPixmap (* glXCreateGLXPixmap ) ( Display *dpy, XVisualInfo *visual,
				     Pixmap pixmap );

GLAPI void (* glXDestroyGLXPixmap ) ( Display *dpy, GLXPixmap pixmap );

GLAPI Bool (* glXQueryExtension ) ( Display *dpy, int *errorb, int *event );

GLAPI Bool (* glXQueryVersion ) ( Display *dpy, int *maj, int *min );

GLAPI Bool (* glXIsDirect ) ( Display *dpy, GLXContext ctx );

GLAPI int (* glXGetConfig ) ( Display *dpy, XVisualInfo *visual,
			 int attrib, int *value );

GLAPI GLXContext (* glXGetCurrentContext ) ( void );

GLAPI GLXDrawable (* glXGetCurrentDrawable ) ( void );

GLAPI void (* glXWaitGL ) ( void );

GLAPI void (* glXWaitX ) ( void );

GLAPI void (* glXUseXFont ) ( Font font, int first, int count, int list );



/* GLX 1.1 and later */
GLAPI const char *(* glXQueryExtensionsString ) ( Display *dpy, int screen );

GLAPI const char *(* glXQueryServerString ) ( Display *dpy, int screen, int name );

GLAPI const char *(* glXGetClientString ) ( Display *dpy, int name );

#endif
