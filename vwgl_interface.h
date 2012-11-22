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



// vwgl_interface.h

#ifndef __vwgl_interface_h
#define __vwgl_interface_h

#include <windows.h>

#ifdef vgl_notextern
//#warning vgl_notextern
#undef GLAPI
#define GLAPI
#else
#undef GLAPI
#define GLAPI extern
#endif

typedef HGLRC (APIENTRY *vwglCreateContext_t) ( HDC );
typedef BOOL (APIENTRY *vwglMakeCurrent_t) ( HDC, HGLRC );
typedef BOOL (APIENTRY *vwglDeleteContext_t ) ( HGLRC );
typedef PROC (APIENTRY *vwglGetProcAddress_t ) ( LPCSTR );
typedef int (APIENTRY *vwglChoosePixelFormat_t ) ( HDC, CONST PIXELFORMATDESCRIPTOR * );
typedef BOOL (APIENTRY *vwglSetPixelFormat_t ) ( HDC, int, PIXELFORMATDESCRIPTOR * );

typedef BOOL (APIENTRY *vwglSwapBuffers_t ) ( HDC );

GLAPI vwglCreateContext_t vwglCreateContext;
GLAPI vwglMakeCurrent_t vwglMakeCurrent;
GLAPI vwglDeleteContext_t vwglDeleteContext;
GLAPI vwglGetProcAddress_t vwglGetProcAddress;
GLAPI vwglChoosePixelFormat_t vwglChoosePixelFormat;
GLAPI vwglSetPixelFormat_t vwglSetPixelFormat;
GLAPI vwglSwapBuffers_t	vwglSwapBuffers;

#endif
