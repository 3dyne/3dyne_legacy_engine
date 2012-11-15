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



#include "gl_service.h"
#include "glx_service.h"
#include "sys_dll.h"
#include "shock.h"


void GL_SymNotFound( char *symname )
{
    __warning( "could not resolve symbol %s\n", symname );
}
void oldc_ghetto_load_glx( int glx_h ) {
    glXChooseVisual = SYS_DllSym( glx_h, "glXChooseVisual" );
    if( !glXChooseVisual )
        __warning( "could not resolve symbol glXChooseVisual from GLX lib\n" );
    __named_message( "glXChooseVisual: %p %p\n", glXChooseVisual,  SYS_DllSym( glx_h, "glXChooseVisual" ) );
    
    
    glXCreateContext = SYS_DllSym( glx_h, "glXCreateContext" );
    if( !glXCreateContext )
        __warning( "could not resolve symbol glXCreateContext from GLX lib\n" );

    glXMakeCurrent = SYS_DllSym( glx_h, "glXMakeCurrent" );
    if( !glXMakeCurrent )
        __warning( "could not resolve symbol glXMakeCurrent from GLX lib\n" );


    glXSwapBuffers = SYS_DllSym( glx_h, "glXSwapBuffers" );
    if( !glXSwapBuffers )
        __warning( "could not resolve symbol glXSwapBuffers from GLX lib\n" );

    glXDestroyContext = SYS_DllSym( glx_h, "glXDestroyContext" );
    if( !glXDestroyContext )
        __warning( "could not resolve symbol glXDestroyContext from GLX lib\n" );

    glXWaitGL = SYS_DllSym( glx_h, "glXWaitGL" );
    if( !glXWaitGL )
        __warning( "could not resolve symbol glXWaitGL from GLX lib\n" );   
}

void oldc_ghetto_load_gl( int gl_h ) {
    char *symname;
    
    symname = "glClearIndex";
    glClearIndex = SYS_DllSym( gl_h, symname );
    if( ! glClearIndex )
        GL_SymNotFound( symname );
 
    symname = "glClearColor";
    glClearColor = SYS_DllSym( gl_h, symname );
    if( ! glClearColor )
        GL_SymNotFound( symname );
 
    symname = "glClear";
    glClear = SYS_DllSym( gl_h, symname );
    if( ! glClear )
        GL_SymNotFound( symname );
 
    symname = "glIndexMask";
    glIndexMask = SYS_DllSym( gl_h, symname );
    if( ! glIndexMask )
        GL_SymNotFound( symname );
 
    symname = "glColorMask";
    glColorMask = SYS_DllSym( gl_h, symname );
    if( ! glColorMask )
        GL_SymNotFound( symname );
 
    symname = "glAlphaFunc";
    glAlphaFunc = SYS_DllSym( gl_h, symname );
    if( ! glAlphaFunc )
        GL_SymNotFound( symname );
 
    symname = "glBlendFunc";
    glBlendFunc = SYS_DllSym( gl_h, symname );
    if( ! glBlendFunc )
        GL_SymNotFound( symname );
 
    symname = "glLogicOp";
    glLogicOp = SYS_DllSym( gl_h, symname );
    if( ! glLogicOp )
        GL_SymNotFound( symname );
 
    symname = "glCullFace";
    glCullFace = SYS_DllSym( gl_h, symname );
    if( ! glCullFace )
        GL_SymNotFound( symname );
 
    symname = "glFrontFace";
    glFrontFace = SYS_DllSym( gl_h, symname );
    if( ! glFrontFace )
        GL_SymNotFound( symname );
 
    symname = "glPointSize";
    glPointSize = SYS_DllSym( gl_h, symname );
    if( ! glPointSize )
        GL_SymNotFound( symname );
 
    symname = "glLineWidth";
    glLineWidth = SYS_DllSym( gl_h, symname );
    if( ! glLineWidth )
        GL_SymNotFound( symname );
 
    symname = "glLineStipple";
    glLineStipple = SYS_DllSym( gl_h, symname );
    if( ! glLineStipple )
        GL_SymNotFound( symname );
 
    symname = "glPolygonMode";
    glPolygonMode = SYS_DllSym( gl_h, symname );
    if( ! glPolygonMode )
        GL_SymNotFound( symname );
 
    symname = "glPolygonOffset";
    glPolygonOffset = SYS_DllSym( gl_h, symname );
    if( ! glPolygonOffset )
        GL_SymNotFound( symname );
 
    symname = "glPolygonStipple";
    glPolygonStipple = SYS_DllSym( gl_h, symname );
    if( ! glPolygonStipple )
        GL_SymNotFound( symname );
 
    symname = "glGetPolygonStipple";
    glGetPolygonStipple = SYS_DllSym( gl_h, symname );
    if( ! glGetPolygonStipple )
        GL_SymNotFound( symname );
 
    symname = "glEdgeFlag";
    glEdgeFlag = SYS_DllSym( gl_h, symname );
    if( ! glEdgeFlag )
        GL_SymNotFound( symname );
 
    symname = "glEdgeFlagv";
    glEdgeFlagv = SYS_DllSym( gl_h, symname );
    if( ! glEdgeFlagv )
        GL_SymNotFound( symname );
 
    symname = "glScissor";
    glScissor = SYS_DllSym( gl_h, symname );
    if( ! glScissor )
        GL_SymNotFound( symname );
 
    symname = "glClipPlane";
    glClipPlane = SYS_DllSym( gl_h, symname );
    if( ! glClipPlane )
        GL_SymNotFound( symname );
 
    symname = "glGetClipPlane";
    glGetClipPlane = SYS_DllSym( gl_h, symname );
    if( ! glGetClipPlane )
        GL_SymNotFound( symname );
 
    symname = "glDrawBuffer";
    glDrawBuffer = SYS_DllSym( gl_h, symname );
    if( ! glDrawBuffer )
        GL_SymNotFound( symname );
 
    symname = "glReadBuffer";
    glReadBuffer = SYS_DllSym( gl_h, symname );
    if( ! glReadBuffer )
        GL_SymNotFound( symname );
 
    symname = "glEnable";
    glEnable = SYS_DllSym( gl_h, symname );
    if( ! glEnable )
        GL_SymNotFound( symname );
 
    symname = "glDisable";
    glDisable = SYS_DllSym( gl_h, symname );
    if( ! glDisable )
        GL_SymNotFound( symname );
 
    symname = "glIsEnabled";
    glIsEnabled = SYS_DllSym( gl_h, symname );
    if( ! glIsEnabled )
        GL_SymNotFound( symname );
 
    symname = "glEnableClientState";
    glEnableClientState = SYS_DllSym( gl_h, symname );
    if( ! glEnableClientState )
        GL_SymNotFound( symname );
 
    symname = "glDisableClientState";
    glDisableClientState = SYS_DllSym( gl_h, symname );
    if( ! glDisableClientState )
        GL_SymNotFound( symname );
 
    symname = "glGetBooleanv";
    glGetBooleanv = SYS_DllSym( gl_h, symname );
    if( ! glGetBooleanv )
        GL_SymNotFound( symname );
 
    symname = "glGetDoublev";
    glGetDoublev = SYS_DllSym( gl_h, symname );
    if( ! glGetDoublev )
        GL_SymNotFound( symname );
 
    symname = "glGetFloatv";
    glGetFloatv = SYS_DllSym( gl_h, symname );
    if( ! glGetFloatv )
        GL_SymNotFound( symname );
 
    symname = "glGetIntegerv";
    glGetIntegerv = SYS_DllSym( gl_h, symname );
    if( ! glGetIntegerv )
        GL_SymNotFound( symname );
 
    symname = "glPushAttrib";
    glPushAttrib = SYS_DllSym( gl_h, symname );
    if( ! glPushAttrib )
        GL_SymNotFound( symname );
 
    symname = "glPopAttrib";
    glPopAttrib = SYS_DllSym( gl_h, symname );
    if( ! glPopAttrib )
        GL_SymNotFound( symname );
 
    symname = "glPushClientAttrib";
    glPushClientAttrib = SYS_DllSym( gl_h, symname );
    if( ! glPushClientAttrib )
        GL_SymNotFound( symname );
 
    symname = "glPopClientAttrib";
    glPopClientAttrib = SYS_DllSym( gl_h, symname );
    if( ! glPopClientAttrib )
        GL_SymNotFound( symname );
 
    symname = "glRenderMode";
    glRenderMode = SYS_DllSym( gl_h, symname );
    if( ! glRenderMode )
        GL_SymNotFound( symname );
 
    symname = "glGetError";
    glGetError = SYS_DllSym( gl_h, symname );
    if( ! glGetError )
        GL_SymNotFound( symname );
 
    symname = "glGetString";
    glGetString = SYS_DllSym( gl_h, symname );
    if( ! glGetString )
        GL_SymNotFound( symname );
 
    symname = "glFinish";
    glFinish = SYS_DllSym( gl_h, symname );
    if( ! glFinish )
        GL_SymNotFound( symname );
 
    symname = "glFlush";
    glFlush = SYS_DllSym( gl_h, symname );
    if( ! glFlush )
        GL_SymNotFound( symname );
 
    symname = "glHint";
    glHint = SYS_DllSym( gl_h, symname );
    if( ! glHint )
        GL_SymNotFound( symname );
 
    symname = "glClearDepth";
    glClearDepth = SYS_DllSym( gl_h, symname );
    if( ! glClearDepth )
        GL_SymNotFound( symname );
 
    symname = "glDepthFunc";
    glDepthFunc = SYS_DllSym( gl_h, symname );
    if( ! glDepthFunc )
        GL_SymNotFound( symname );
 
    symname = "glDepthMask";
    glDepthMask = SYS_DllSym( gl_h, symname );
    if( ! glDepthMask )
        GL_SymNotFound( symname );
 
    symname = "glDepthRange";
    glDepthRange = SYS_DllSym( gl_h, symname );
    if( ! glDepthRange )
        GL_SymNotFound( symname );
 
    symname = "glClearAccum";
    glClearAccum = SYS_DllSym( gl_h, symname );
    if( ! glClearAccum )
        GL_SymNotFound( symname );
 
    symname = "glAccum";
    glAccum = SYS_DllSym( gl_h, symname );
    if( ! glAccum )
        GL_SymNotFound( symname );
 
    symname = "glMatrixMode";
    glMatrixMode = SYS_DllSym( gl_h, symname );
    if( ! glMatrixMode )
        GL_SymNotFound( symname );
 
    symname = "glOrtho";
    glOrtho = SYS_DllSym( gl_h, symname );
    if( ! glOrtho )
        GL_SymNotFound( symname );
 
    symname = "glFrustum";
    glFrustum = SYS_DllSym( gl_h, symname );
    if( ! glFrustum )
        GL_SymNotFound( symname );
 
    symname = "glViewport";
    glViewport = SYS_DllSym( gl_h, symname );
    if( ! glViewport )
        GL_SymNotFound( symname );
 
    symname = "glPushMatrix";
    glPushMatrix = SYS_DllSym( gl_h, symname );
    if( ! glPushMatrix )
        GL_SymNotFound( symname );
 
    symname = "glPopMatrix";
    glPopMatrix = SYS_DllSym( gl_h, symname );
    if( ! glPopMatrix )
        GL_SymNotFound( symname );
 
    symname = "glLoadIdentity";
    glLoadIdentity = SYS_DllSym( gl_h, symname );
    if( ! glLoadIdentity )
        GL_SymNotFound( symname );
 
    symname = "glLoadMatrixd";
    glLoadMatrixd = SYS_DllSym( gl_h, symname );
    if( ! glLoadMatrixd )
        GL_SymNotFound( symname );
 
    symname = "glLoadMatrixf";
    glLoadMatrixf = SYS_DllSym( gl_h, symname );
    if( ! glLoadMatrixf )
        GL_SymNotFound( symname );
 
    symname = "glMultMatrixd";
    glMultMatrixd = SYS_DllSym( gl_h, symname );
    if( ! glMultMatrixd )
        GL_SymNotFound( symname );
 
    symname = "glMultMatrixf";
    glMultMatrixf = SYS_DllSym( gl_h, symname );
    if( ! glMultMatrixf )
        GL_SymNotFound( symname );
 
    symname = "glRotated";
    glRotated = SYS_DllSym( gl_h, symname );
    if( ! glRotated )
        GL_SymNotFound( symname );
 
    symname = "glRotatef";
    glRotatef = SYS_DllSym( gl_h, symname );
    if( ! glRotatef )
        GL_SymNotFound( symname );
 
    symname = "glScaled";
    glScaled = SYS_DllSym( gl_h, symname );
    if( ! glScaled )
        GL_SymNotFound( symname );
 
    symname = "glScalef";
    glScalef = SYS_DllSym( gl_h, symname );
    if( ! glScalef )
        GL_SymNotFound( symname );
 
    symname = "glTranslated";
    glTranslated = SYS_DllSym( gl_h, symname );
    if( ! glTranslated )
        GL_SymNotFound( symname );
 
    symname = "glTranslatef";
    glTranslatef = SYS_DllSym( gl_h, symname );
    if( ! glTranslatef )
        GL_SymNotFound( symname );
 
    symname = "glIsList";
    glIsList = SYS_DllSym( gl_h, symname );
    if( ! glIsList )
        GL_SymNotFound( symname );
 
    symname = "glDeleteLists";
    glDeleteLists = SYS_DllSym( gl_h, symname );
    if( ! glDeleteLists )
        GL_SymNotFound( symname );
 
    symname = "glGenLists";
    glGenLists = SYS_DllSym( gl_h, symname );
    if( ! glGenLists )
        GL_SymNotFound( symname );
 
    symname = "glNewList";
    glNewList = SYS_DllSym( gl_h, symname );
    if( ! glNewList )
        GL_SymNotFound( symname );
 
    symname = "glEndList";
    glEndList = SYS_DllSym( gl_h, symname );
    if( ! glEndList )
        GL_SymNotFound( symname );
 
    symname = "glCallList";
    glCallList = SYS_DllSym( gl_h, symname );
    if( ! glCallList )
        GL_SymNotFound( symname );
 
    symname = "glCallLists";
    glCallLists = SYS_DllSym( gl_h, symname );
    if( ! glCallLists )
        GL_SymNotFound( symname );
 
    symname = "glListBase";
    glListBase = SYS_DllSym( gl_h, symname );
    if( ! glListBase )
        GL_SymNotFound( symname );
 
    symname = "glBegin";
    glBegin = SYS_DllSym( gl_h, symname );
    if( ! glBegin )
        GL_SymNotFound( symname );
 
    symname = "glEnd";
    glEnd = SYS_DllSym( gl_h, symname );
    if( ! glEnd )
        GL_SymNotFound( symname );
 
    symname = "glVertex2d";
    glVertex2d = SYS_DllSym( gl_h, symname );
    if( ! glVertex2d )
        GL_SymNotFound( symname );
 
    symname = "glVertex2f";
    glVertex2f = SYS_DllSym( gl_h, symname );
    if( ! glVertex2f )
        GL_SymNotFound( symname );
 
    symname = "glVertex2i";
    glVertex2i = SYS_DllSym( gl_h, symname );
    if( ! glVertex2i )
        GL_SymNotFound( symname );
 
    symname = "glVertex2s";
    glVertex2s = SYS_DllSym( gl_h, symname );
    if( ! glVertex2s )
        GL_SymNotFound( symname );
 
    symname = "glVertex3d";
    glVertex3d = SYS_DllSym( gl_h, symname );
    if( ! glVertex3d )
        GL_SymNotFound( symname );
 
    symname = "glVertex3f";
    glVertex3f = SYS_DllSym( gl_h, symname );
    if( ! glVertex3f )
        GL_SymNotFound( symname );
 
    symname = "glVertex3i";
    glVertex3i = SYS_DllSym( gl_h, symname );
    if( ! glVertex3i )
        GL_SymNotFound( symname );
 
    symname = "glVertex3s";
    glVertex3s = SYS_DllSym( gl_h, symname );
    if( ! glVertex3s )
        GL_SymNotFound( symname );
 
    symname = "glVertex4d";
    glVertex4d = SYS_DllSym( gl_h, symname );
    if( ! glVertex4d )
        GL_SymNotFound( symname );
 
    symname = "glVertex4f";
    glVertex4f = SYS_DllSym( gl_h, symname );
    if( ! glVertex4f )
        GL_SymNotFound( symname );
 
    symname = "glVertex4i";
    glVertex4i = SYS_DllSym( gl_h, symname );
    if( ! glVertex4i )
        GL_SymNotFound( symname );
 
    symname = "glVertex4s";
    glVertex4s = SYS_DllSym( gl_h, symname );
    if( ! glVertex4s )
        GL_SymNotFound( symname );
 
    symname = "glVertex2dv";
    glVertex2dv = SYS_DllSym( gl_h, symname );
    if( ! glVertex2dv )
        GL_SymNotFound( symname );
 
    symname = "glVertex2fv";
    glVertex2fv = SYS_DllSym( gl_h, symname );
    if( ! glVertex2fv )
        GL_SymNotFound( symname );
 
    symname = "glVertex2iv";
    glVertex2iv = SYS_DllSym( gl_h, symname );
    if( ! glVertex2iv )
        GL_SymNotFound( symname );
 
    symname = "glVertex2sv";
    glVertex2sv = SYS_DllSym( gl_h, symname );
    if( ! glVertex2sv )
        GL_SymNotFound( symname );
 
    symname = "glVertex3dv";
    glVertex3dv = SYS_DllSym( gl_h, symname );
    if( ! glVertex3dv )
        GL_SymNotFound( symname );
 
    symname = "glVertex3fv";
    glVertex3fv = SYS_DllSym( gl_h, symname );
    if( ! glVertex3fv )
        GL_SymNotFound( symname );
 
    symname = "glVertex3iv";
    glVertex3iv = SYS_DllSym( gl_h, symname );
    if( ! glVertex3iv )
        GL_SymNotFound( symname );
 
    symname = "glVertex3sv";
    glVertex3sv = SYS_DllSym( gl_h, symname );
    if( ! glVertex3sv )
        GL_SymNotFound( symname );
 
    symname = "glVertex4dv";
    glVertex4dv = SYS_DllSym( gl_h, symname );
    if( ! glVertex4dv )
        GL_SymNotFound( symname );
 
    symname = "glVertex4fv";
    glVertex4fv = SYS_DllSym( gl_h, symname );
    if( ! glVertex4fv )
        GL_SymNotFound( symname );
 
    symname = "glVertex4iv";
    glVertex4iv = SYS_DllSym( gl_h, symname );
    if( ! glVertex4iv )
        GL_SymNotFound( symname );
 
    symname = "glVertex4sv";
    glVertex4sv = SYS_DllSym( gl_h, symname );
    if( ! glVertex4sv )
        GL_SymNotFound( symname );
 
    symname = "glNormal3b";
    glNormal3b = SYS_DllSym( gl_h, symname );
    if( ! glNormal3b )
        GL_SymNotFound( symname );
 
    symname = "glNormal3d";
    glNormal3d = SYS_DllSym( gl_h, symname );
    if( ! glNormal3d )
        GL_SymNotFound( symname );
 
    symname = "glNormal3f";
    glNormal3f = SYS_DllSym( gl_h, symname );
    if( ! glNormal3f )
        GL_SymNotFound( symname );
 
    symname = "glNormal3i";
    glNormal3i = SYS_DllSym( gl_h, symname );
    if( ! glNormal3i )
        GL_SymNotFound( symname );
 
    symname = "glNormal3s";
    glNormal3s = SYS_DllSym( gl_h, symname );
    if( ! glNormal3s )
        GL_SymNotFound( symname );
 
    symname = "glNormal3bv";
    glNormal3bv = SYS_DllSym( gl_h, symname );
    if( ! glNormal3bv )
        GL_SymNotFound( symname );
 
    symname = "glNormal3dv";
    glNormal3dv = SYS_DllSym( gl_h, symname );
    if( ! glNormal3dv )
        GL_SymNotFound( symname );
 
    symname = "glNormal3fv";
    glNormal3fv = SYS_DllSym( gl_h, symname );
    if( ! glNormal3fv )
        GL_SymNotFound( symname );
 
    symname = "glNormal3iv";
    glNormal3iv = SYS_DllSym( gl_h, symname );
    if( ! glNormal3iv )
        GL_SymNotFound( symname );
 
    symname = "glNormal3sv";
    glNormal3sv = SYS_DllSym( gl_h, symname );
    if( ! glNormal3sv )
        GL_SymNotFound( symname );
 
    symname = "glIndexd";
    glIndexd = SYS_DllSym( gl_h, symname );
    if( ! glIndexd )
        GL_SymNotFound( symname );
 
    symname = "glIndexf";
    glIndexf = SYS_DllSym( gl_h, symname );
    if( ! glIndexf )
        GL_SymNotFound( symname );
 
    symname = "glIndexi";
    glIndexi = SYS_DllSym( gl_h, symname );
    if( ! glIndexi )
        GL_SymNotFound( symname );
 
    symname = "glIndexs";
    glIndexs = SYS_DllSym( gl_h, symname );
    if( ! glIndexs )
        GL_SymNotFound( symname );
 
    symname = "glIndexub";
    glIndexub = SYS_DllSym( gl_h, symname );
    if( ! glIndexub )
        GL_SymNotFound( symname );
 
    symname = "glIndexdv";
    glIndexdv = SYS_DllSym( gl_h, symname );
    if( ! glIndexdv )
        GL_SymNotFound( symname );
 
    symname = "glIndexfv";
    glIndexfv = SYS_DllSym( gl_h, symname );
    if( ! glIndexfv )
        GL_SymNotFound( symname );
 
    symname = "glIndexiv";
    glIndexiv = SYS_DllSym( gl_h, symname );
    if( ! glIndexiv )
        GL_SymNotFound( symname );
 
    symname = "glIndexsv";
    glIndexsv = SYS_DllSym( gl_h, symname );
    if( ! glIndexsv )
        GL_SymNotFound( symname );
 
    symname = "glIndexubv";
    glIndexubv = SYS_DllSym( gl_h, symname );
    if( ! glIndexubv )
        GL_SymNotFound( symname );
 
    symname = "glColor3b";
    glColor3b = SYS_DllSym( gl_h, symname );
    if( ! glColor3b )
        GL_SymNotFound( symname );
 
    symname = "glColor3d";
    glColor3d = SYS_DllSym( gl_h, symname );
    if( ! glColor3d )
        GL_SymNotFound( symname );
 
    symname = "glColor3f";
    glColor3f = SYS_DllSym( gl_h, symname );
    if( ! glColor3f )
        GL_SymNotFound( symname );
 
    symname = "glColor3i";
    glColor3i = SYS_DllSym( gl_h, symname );
    if( ! glColor3i )
        GL_SymNotFound( symname );
 
    symname = "glColor3s";
    glColor3s = SYS_DllSym( gl_h, symname );
    if( ! glColor3s )
        GL_SymNotFound( symname );
 
    symname = "glColor3ub";
    glColor3ub = SYS_DllSym( gl_h, symname );
    if( ! glColor3ub )
        GL_SymNotFound( symname );
 
    symname = "glColor3ui";
    glColor3ui = SYS_DllSym( gl_h, symname );
    if( ! glColor3ui )
        GL_SymNotFound( symname );
 
    symname = "glColor3us";
    glColor3us = SYS_DllSym( gl_h, symname );
    if( ! glColor3us )
        GL_SymNotFound( symname );
 
    symname = "glColor4b";
    glColor4b = SYS_DllSym( gl_h, symname );
    if( ! glColor4b )
        GL_SymNotFound( symname );
 
    symname = "glColor4d";
    glColor4d = SYS_DllSym( gl_h, symname );
    if( ! glColor4d )
        GL_SymNotFound( symname );
 
    symname = "glColor4f";
    glColor4f = SYS_DllSym( gl_h, symname );
    if( ! glColor4f )
        GL_SymNotFound( symname );
 
    symname = "glColor4i";
    glColor4i = SYS_DllSym( gl_h, symname );
    if( ! glColor4i )
        GL_SymNotFound( symname );
 
    symname = "glColor4s";
    glColor4s = SYS_DllSym( gl_h, symname );
    if( ! glColor4s )
        GL_SymNotFound( symname );
 
    symname = "glColor4ub";
    glColor4ub = SYS_DllSym( gl_h, symname );
    if( ! glColor4ub )
        GL_SymNotFound( symname );
 
    symname = "glColor4ui";
    glColor4ui = SYS_DllSym( gl_h, symname );
    if( ! glColor4ui )
        GL_SymNotFound( symname );
 
    symname = "glColor4us";
    glColor4us = SYS_DllSym( gl_h, symname );
    if( ! glColor4us )
        GL_SymNotFound( symname );
 
    symname = "glColor3bv";
    glColor3bv = SYS_DllSym( gl_h, symname );
    if( ! glColor3bv )
        GL_SymNotFound( symname );
 
    symname = "glColor3dv";
    glColor3dv = SYS_DllSym( gl_h, symname );
    if( ! glColor3dv )
        GL_SymNotFound( symname );
 
    symname = "glColor3fv";
    glColor3fv = SYS_DllSym( gl_h, symname );
    if( ! glColor3fv )
        GL_SymNotFound( symname );
 
    symname = "glColor3iv";
    glColor3iv = SYS_DllSym( gl_h, symname );
    if( ! glColor3iv )
        GL_SymNotFound( symname );
 
    symname = "glColor3sv";
    glColor3sv = SYS_DllSym( gl_h, symname );
    if( ! glColor3sv )
        GL_SymNotFound( symname );
 
    symname = "glColor3ubv";
    glColor3ubv = SYS_DllSym( gl_h, symname );
    if( ! glColor3ubv )
        GL_SymNotFound( symname );
 
    symname = "glColor3uiv";
    glColor3uiv = SYS_DllSym( gl_h, symname );
    if( ! glColor3uiv )
        GL_SymNotFound( symname );
 
    symname = "glColor3usv";
    glColor3usv = SYS_DllSym( gl_h, symname );
    if( ! glColor3usv )
        GL_SymNotFound( symname );
 
    symname = "glColor4bv";
    glColor4bv = SYS_DllSym( gl_h, symname );
    if( ! glColor4bv )
        GL_SymNotFound( symname );
 
    symname = "glColor4dv";
    glColor4dv = SYS_DllSym( gl_h, symname );
    if( ! glColor4dv )
        GL_SymNotFound( symname );
 
    symname = "glColor4fv";
    glColor4fv = SYS_DllSym( gl_h, symname );
    if( ! glColor4fv )
        GL_SymNotFound( symname );
 
    symname = "glColor4iv";
    glColor4iv = SYS_DllSym( gl_h, symname );
    if( ! glColor4iv )
        GL_SymNotFound( symname );
 
    symname = "glColor4sv";
    glColor4sv = SYS_DllSym( gl_h, symname );
    if( ! glColor4sv )
        GL_SymNotFound( symname );
 
    symname = "glColor4ubv";
    glColor4ubv = SYS_DllSym( gl_h, symname );
    if( ! glColor4ubv )
        GL_SymNotFound( symname );
 
    symname = "glColor4uiv";
    glColor4uiv = SYS_DllSym( gl_h, symname );
    if( ! glColor4uiv )
        GL_SymNotFound( symname );
 
    symname = "glColor4usv";
    glColor4usv = SYS_DllSym( gl_h, symname );
    if( ! glColor4usv )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord1d";
    glTexCoord1d = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord1d )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord1f";
    glTexCoord1f = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord1f )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord1i";
    glTexCoord1i = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord1i )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord1s";
    glTexCoord1s = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord1s )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord2d";
    glTexCoord2d = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord2d )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord2f";
    glTexCoord2f = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord2f )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord2i";
    glTexCoord2i = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord2i )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord2s";
    glTexCoord2s = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord2s )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord3d";
    glTexCoord3d = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord3d )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord3f";
    glTexCoord3f = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord3f )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord3i";
    glTexCoord3i = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord3i )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord3s";
    glTexCoord3s = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord3s )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord4d";
    glTexCoord4d = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord4d )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord4f";
    glTexCoord4f = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord4f )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord4i";
    glTexCoord4i = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord4i )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord4s";
    glTexCoord4s = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord4s )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord1dv";
    glTexCoord1dv = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord1dv )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord1fv";
    glTexCoord1fv = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord1fv )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord1iv";
    glTexCoord1iv = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord1iv )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord1sv";
    glTexCoord1sv = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord1sv )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord2dv";
    glTexCoord2dv = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord2dv )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord2fv";
    glTexCoord2fv = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord2fv )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord2iv";
    glTexCoord2iv = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord2iv )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord2sv";
    glTexCoord2sv = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord2sv )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord3dv";
    glTexCoord3dv = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord3dv )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord3fv";
    glTexCoord3fv = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord3fv )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord3iv";
    glTexCoord3iv = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord3iv )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord3sv";
    glTexCoord3sv = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord3sv )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord4dv";
    glTexCoord4dv = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord4dv )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord4fv";
    glTexCoord4fv = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord4fv )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord4iv";
    glTexCoord4iv = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord4iv )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord4sv";
    glTexCoord4sv = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord4sv )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos2d";
    glRasterPos2d = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos2d )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos2f";
    glRasterPos2f = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos2f )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos2i";
    glRasterPos2i = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos2i )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos2s";
    glRasterPos2s = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos2s )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos3d";
    glRasterPos3d = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos3d )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos3f";
    glRasterPos3f = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos3f )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos3i";
    glRasterPos3i = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos3i )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos3s";
    glRasterPos3s = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos3s )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos4d";
    glRasterPos4d = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos4d )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos4f";
    glRasterPos4f = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos4f )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos4i";
    glRasterPos4i = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos4i )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos4s";
    glRasterPos4s = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos4s )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos2dv";
    glRasterPos2dv = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos2dv )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos2fv";
    glRasterPos2fv = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos2fv )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos2iv";
    glRasterPos2iv = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos2iv )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos2sv";
    glRasterPos2sv = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos2sv )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos3dv";
    glRasterPos3dv = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos3dv )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos3fv";
    glRasterPos3fv = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos3fv )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos3iv";
    glRasterPos3iv = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos3iv )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos3sv";
    glRasterPos3sv = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos3sv )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos4dv";
    glRasterPos4dv = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos4dv )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos4fv";
    glRasterPos4fv = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos4fv )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos4iv";
    glRasterPos4iv = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos4iv )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos4sv";
    glRasterPos4sv = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos4sv )
        GL_SymNotFound( symname );
 
    symname = "glRectd";
    glRectd = SYS_DllSym( gl_h, symname );
    if( ! glRectd )
        GL_SymNotFound( symname );
 
    symname = "glRectf";
    glRectf = SYS_DllSym( gl_h, symname );
    if( ! glRectf )
        GL_SymNotFound( symname );
 
    symname = "glRecti";
    glRecti = SYS_DllSym( gl_h, symname );
    if( ! glRecti )
        GL_SymNotFound( symname );
 
    symname = "glRects";
    glRects = SYS_DllSym( gl_h, symname );
    if( ! glRects )
        GL_SymNotFound( symname );
 
    symname = "glRectdv";
    glRectdv = SYS_DllSym( gl_h, symname );
    if( ! glRectdv )
        GL_SymNotFound( symname );
 
    symname = "glRectfv";
    glRectfv = SYS_DllSym( gl_h, symname );
    if( ! glRectfv )
        GL_SymNotFound( symname );
 
    symname = "glRectiv";
    glRectiv = SYS_DllSym( gl_h, symname );
    if( ! glRectiv )
        GL_SymNotFound( symname );
 
    symname = "glRectsv";
    glRectsv = SYS_DllSym( gl_h, symname );
    if( ! glRectsv )
        GL_SymNotFound( symname );
 
    symname = "glVertexPointer";
    glVertexPointer = SYS_DllSym( gl_h, symname );
    if( ! glVertexPointer )
        GL_SymNotFound( symname );
 
    symname = "glNormalPointer";
    glNormalPointer = SYS_DllSym( gl_h, symname );
    if( ! glNormalPointer )
        GL_SymNotFound( symname );
 
    symname = "glColorPointer";
    glColorPointer = SYS_DllSym( gl_h, symname );
    if( ! glColorPointer )
        GL_SymNotFound( symname );
 
    symname = "glIndexPointer";
    glIndexPointer = SYS_DllSym( gl_h, symname );
    if( ! glIndexPointer )
        GL_SymNotFound( symname );
 
    symname = "glTexCoordPointer";
    glTexCoordPointer = SYS_DllSym( gl_h, symname );
    if( ! glTexCoordPointer )
        GL_SymNotFound( symname );
 
    symname = "glEdgeFlagPointer";
    glEdgeFlagPointer = SYS_DllSym( gl_h, symname );
    if( ! glEdgeFlagPointer )
        GL_SymNotFound( symname );
 
    symname = "glGetPointerv";
    glGetPointerv = SYS_DllSym( gl_h, symname );
    if( ! glGetPointerv )
        GL_SymNotFound( symname );
 
    symname = "glArrayElement";
    glArrayElement = SYS_DllSym( gl_h, symname );
    if( ! glArrayElement )
        GL_SymNotFound( symname );
 
    symname = "glDrawArrays";
    glDrawArrays = SYS_DllSym( gl_h, symname );
    if( ! glDrawArrays )
        GL_SymNotFound( symname );
 
    symname = "glDrawElements";
    glDrawElements = SYS_DllSym( gl_h, symname );
    if( ! glDrawElements )
        GL_SymNotFound( symname );
 
    symname = "glInterleavedArrays";
    glInterleavedArrays = SYS_DllSym( gl_h, symname );
    if( ! glInterleavedArrays )
        GL_SymNotFound( symname );
 
    symname = "glShadeModel";
    glShadeModel = SYS_DllSym( gl_h, symname );
    if( ! glShadeModel )
        GL_SymNotFound( symname );
 
    symname = "glLightf";
    glLightf = SYS_DllSym( gl_h, symname );
    if( ! glLightf )
        GL_SymNotFound( symname );
 
    symname = "glLighti";
    glLighti = SYS_DllSym( gl_h, symname );
    if( ! glLighti )
        GL_SymNotFound( symname );
 
    symname = "glLightfv";
    glLightfv = SYS_DllSym( gl_h, symname );
    if( ! glLightfv )
        GL_SymNotFound( symname );
 
    symname = "glLightiv";
    glLightiv = SYS_DllSym( gl_h, symname );
    if( ! glLightiv )
        GL_SymNotFound( symname );
 
    symname = "glGetLightfv";
    glGetLightfv = SYS_DllSym( gl_h, symname );
    if( ! glGetLightfv )
        GL_SymNotFound( symname );
 
    symname = "glGetLightiv";
    glGetLightiv = SYS_DllSym( gl_h, symname );
    if( ! glGetLightiv )
        GL_SymNotFound( symname );
 
    symname = "glLightModelf";
    glLightModelf = SYS_DllSym( gl_h, symname );
    if( ! glLightModelf )
        GL_SymNotFound( symname );
 
    symname = "glLightModeli";
    glLightModeli = SYS_DllSym( gl_h, symname );
    if( ! glLightModeli )
        GL_SymNotFound( symname );
 
    symname = "glLightModelfv";
    glLightModelfv = SYS_DllSym( gl_h, symname );
    if( ! glLightModelfv )
        GL_SymNotFound( symname );
 
    symname = "glLightModeliv";
    glLightModeliv = SYS_DllSym( gl_h, symname );
    if( ! glLightModeliv )
        GL_SymNotFound( symname );
 
    symname = "glMaterialf";
    glMaterialf = SYS_DllSym( gl_h, symname );
    if( ! glMaterialf )
        GL_SymNotFound( symname );
 
    symname = "glMateriali";
    glMateriali = SYS_DllSym( gl_h, symname );
    if( ! glMateriali )
        GL_SymNotFound( symname );
 
    symname = "glMaterialfv";
    glMaterialfv = SYS_DllSym( gl_h, symname );
    if( ! glMaterialfv )
        GL_SymNotFound( symname );
 
    symname = "glMaterialiv";
    glMaterialiv = SYS_DllSym( gl_h, symname );
    if( ! glMaterialiv )
        GL_SymNotFound( symname );
 
    symname = "glGetMaterialfv";
    glGetMaterialfv = SYS_DllSym( gl_h, symname );
    if( ! glGetMaterialfv )
        GL_SymNotFound( symname );
 
    symname = "glGetMaterialiv";
    glGetMaterialiv = SYS_DllSym( gl_h, symname );
    if( ! glGetMaterialiv )
        GL_SymNotFound( symname );
 
    symname = "glColorMaterial";
    glColorMaterial = SYS_DllSym( gl_h, symname );
    if( ! glColorMaterial )
        GL_SymNotFound( symname );
 
    symname = "glPixelZoom";
    glPixelZoom = SYS_DllSym( gl_h, symname );
    if( ! glPixelZoom )
        GL_SymNotFound( symname );
 
    symname = "glPixelStoref";
    glPixelStoref = SYS_DllSym( gl_h, symname );
    if( ! glPixelStoref )
        GL_SymNotFound( symname );
 
    symname = "glPixelStorei";
    glPixelStorei = SYS_DllSym( gl_h, symname );
    if( ! glPixelStorei )
        GL_SymNotFound( symname );
 
    symname = "glPixelTransferf";
    glPixelTransferf = SYS_DllSym( gl_h, symname );
    if( ! glPixelTransferf )
        GL_SymNotFound( symname );
 
    symname = "glPixelTransferi";
    glPixelTransferi = SYS_DllSym( gl_h, symname );
    if( ! glPixelTransferi )
        GL_SymNotFound( symname );
 
    symname = "glPixelMapfv";
    glPixelMapfv = SYS_DllSym( gl_h, symname );
    if( ! glPixelMapfv )
        GL_SymNotFound( symname );
 
    symname = "glPixelMapuiv";
    glPixelMapuiv = SYS_DllSym( gl_h, symname );
    if( ! glPixelMapuiv )
        GL_SymNotFound( symname );
 
    symname = "glPixelMapusv";
    glPixelMapusv = SYS_DllSym( gl_h, symname );
    if( ! glPixelMapusv )
        GL_SymNotFound( symname );
 
    symname = "glGetPixelMapfv";
    glGetPixelMapfv = SYS_DllSym( gl_h, symname );
    if( ! glGetPixelMapfv )
        GL_SymNotFound( symname );
 
    symname = "glGetPixelMapuiv";
    glGetPixelMapuiv = SYS_DllSym( gl_h, symname );
    if( ! glGetPixelMapuiv )
        GL_SymNotFound( symname );
 
    symname = "glGetPixelMapusv";
    glGetPixelMapusv = SYS_DllSym( gl_h, symname );
    if( ! glGetPixelMapusv )
        GL_SymNotFound( symname );
 
    symname = "glBitmap";
    glBitmap = SYS_DllSym( gl_h, symname );
    if( ! glBitmap )
        GL_SymNotFound( symname );
 
    symname = "glReadPixels";
    glReadPixels = SYS_DllSym( gl_h, symname );
    if( ! glReadPixels )
        GL_SymNotFound( symname );
 
    symname = "glDrawPixels";
    glDrawPixels = SYS_DllSym( gl_h, symname );
    if( ! glDrawPixels )
        GL_SymNotFound( symname );
 
    symname = "glCopyPixels";
    glCopyPixels = SYS_DllSym( gl_h, symname );
    if( ! glCopyPixels )
        GL_SymNotFound( symname );
 
    symname = "glStencilFunc";
    glStencilFunc = SYS_DllSym( gl_h, symname );
    if( ! glStencilFunc )
        GL_SymNotFound( symname );
 
    symname = "glStencilMask";
    glStencilMask = SYS_DllSym( gl_h, symname );
    if( ! glStencilMask )
        GL_SymNotFound( symname );
 
    symname = "glStencilOp";
    glStencilOp = SYS_DllSym( gl_h, symname );
    if( ! glStencilOp )
        GL_SymNotFound( symname );
 
    symname = "glClearStencil";
    glClearStencil = SYS_DllSym( gl_h, symname );
    if( ! glClearStencil )
        GL_SymNotFound( symname );
 
    symname = "glTexGend";
    glTexGend = SYS_DllSym( gl_h, symname );
    if( ! glTexGend )
        GL_SymNotFound( symname );
 
    symname = "glTexGenf";
    glTexGenf = SYS_DllSym( gl_h, symname );
    if( ! glTexGenf )
        GL_SymNotFound( symname );
 
    symname = "glTexGeni";
    glTexGeni = SYS_DllSym( gl_h, symname );
    if( ! glTexGeni )
        GL_SymNotFound( symname );
 
    symname = "glTexGendv";
    glTexGendv = SYS_DllSym( gl_h, symname );
    if( ! glTexGendv )
        GL_SymNotFound( symname );
 
    symname = "glTexGenfv";
    glTexGenfv = SYS_DllSym( gl_h, symname );
    if( ! glTexGenfv )
        GL_SymNotFound( symname );
 
    symname = "glTexGeniv";
    glTexGeniv = SYS_DllSym( gl_h, symname );
    if( ! glTexGeniv )
        GL_SymNotFound( symname );
 
    symname = "glGetTexGendv";
    glGetTexGendv = SYS_DllSym( gl_h, symname );
    if( ! glGetTexGendv )
        GL_SymNotFound( symname );
 
    symname = "glGetTexGenfv";
    glGetTexGenfv = SYS_DllSym( gl_h, symname );
    if( ! glGetTexGenfv )
        GL_SymNotFound( symname );
 
    symname = "glGetTexGeniv";
    glGetTexGeniv = SYS_DllSym( gl_h, symname );
    if( ! glGetTexGeniv )
        GL_SymNotFound( symname );
 
    symname = "glTexEnvf";
    glTexEnvf = SYS_DllSym( gl_h, symname );
    if( ! glTexEnvf )
        GL_SymNotFound( symname );
 
    symname = "glTexEnvi";
    glTexEnvi = SYS_DllSym( gl_h, symname );
    if( ! glTexEnvi )
        GL_SymNotFound( symname );
 
    symname = "glTexEnvfv";
    glTexEnvfv = SYS_DllSym( gl_h, symname );
    if( ! glTexEnvfv )
        GL_SymNotFound( symname );
 
    symname = "glTexEnviv";
    glTexEnviv = SYS_DllSym( gl_h, symname );
    if( ! glTexEnviv )
        GL_SymNotFound( symname );
 
    symname = "glGetTexEnvfv";
    glGetTexEnvfv = SYS_DllSym( gl_h, symname );
    if( ! glGetTexEnvfv )
        GL_SymNotFound( symname );
 
    symname = "glGetTexEnviv";
    glGetTexEnviv = SYS_DllSym( gl_h, symname );
    if( ! glGetTexEnviv )
        GL_SymNotFound( symname );
 
    symname = "glTexParameterf";
    glTexParameterf = SYS_DllSym( gl_h, symname );
    if( ! glTexParameterf )
        GL_SymNotFound( symname );
 
    symname = "glTexParameteri";
    glTexParameteri = SYS_DllSym( gl_h, symname );
    if( ! glTexParameteri )
        GL_SymNotFound( symname );
 
    symname = "glTexParameterfv";
    glTexParameterfv = SYS_DllSym( gl_h, symname );
    if( ! glTexParameterfv )
        GL_SymNotFound( symname );
 
    symname = "glTexParameteriv";
    glTexParameteriv = SYS_DllSym( gl_h, symname );
    if( ! glTexParameteriv )
        GL_SymNotFound( symname );
 
    symname = "glGetTexParameterfv";
    glGetTexParameterfv = SYS_DllSym( gl_h, symname );
    if( ! glGetTexParameterfv )
        GL_SymNotFound( symname );
 
    symname = "glGetTexParameteriv";
    glGetTexParameteriv = SYS_DllSym( gl_h, symname );
    if( ! glGetTexParameteriv )
        GL_SymNotFound( symname );
 
    symname = "glGetTexLevelParameterfv";
    glGetTexLevelParameterfv = SYS_DllSym( gl_h, symname );
    if( ! glGetTexLevelParameterfv )
        GL_SymNotFound( symname );
 
    symname = "glGetTexLevelParameteriv";
    glGetTexLevelParameteriv = SYS_DllSym( gl_h, symname );
    if( ! glGetTexLevelParameteriv )
        GL_SymNotFound( symname );
 
    symname = "glTexImage1D";
    glTexImage1D = SYS_DllSym( gl_h, symname );
    if( ! glTexImage1D )
        GL_SymNotFound( symname );
 
    symname = "glTexImage2D";
    glTexImage2D = SYS_DllSym( gl_h, symname );
    if( ! glTexImage2D )
        GL_SymNotFound( symname );
 
    symname = "glGetTexImage";
    glGetTexImage = SYS_DllSym( gl_h, symname );
    if( ! glGetTexImage )
        GL_SymNotFound( symname );
 
    symname = "glGenTextures";
    glGenTextures = SYS_DllSym( gl_h, symname );
    if( ! glGenTextures )
        GL_SymNotFound( symname );
 
    symname = "glDeleteTextures";
    glDeleteTextures = SYS_DllSym( gl_h, symname );
    if( ! glDeleteTextures )
        GL_SymNotFound( symname );
 
    symname = "glBindTexture";
    glBindTexture = SYS_DllSym( gl_h, symname );
    if( ! glBindTexture )
        GL_SymNotFound( symname );
 
    symname = "glPrioritizeTextures";
    glPrioritizeTextures = SYS_DllSym( gl_h, symname );
    if( ! glPrioritizeTextures )
        GL_SymNotFound( symname );
 
    symname = "glAreTexturesResident";
    glAreTexturesResident = SYS_DllSym( gl_h, symname );
    if( ! glAreTexturesResident )
        GL_SymNotFound( symname );
 
    symname = "glIsTexture";
    glIsTexture = SYS_DllSym( gl_h, symname );
    if( ! glIsTexture )
        GL_SymNotFound( symname );
 
    symname = "glTexSubImage1D";
    glTexSubImage1D = SYS_DllSym( gl_h, symname );
    if( ! glTexSubImage1D )
        GL_SymNotFound( symname );
 
    symname = "glTexSubImage2D";
    glTexSubImage2D = SYS_DllSym( gl_h, symname );
    if( ! glTexSubImage2D )
        GL_SymNotFound( symname );
 
    symname = "glCopyTexImage1D";
    glCopyTexImage1D = SYS_DllSym( gl_h, symname );
    if( ! glCopyTexImage1D )
        GL_SymNotFound( symname );
 
    symname = "glCopyTexImage2D";
    glCopyTexImage2D = SYS_DllSym( gl_h, symname );
    if( ! glCopyTexImage2D )
        GL_SymNotFound( symname );
 
    symname = "glCopyTexSubImage1D";
    glCopyTexSubImage1D = SYS_DllSym( gl_h, symname );
    if( ! glCopyTexSubImage1D )
        GL_SymNotFound( symname );
    symname = "glClearIndex";
    glClearIndex = SYS_DllSym( gl_h, symname );
    if( ! glClearIndex )
        GL_SymNotFound( symname );
 
    symname = "glClearColor";
    glClearColor = SYS_DllSym( gl_h, symname );
    if( ! glClearColor )
        GL_SymNotFound( symname );
 
    symname = "glClear";
    glClear = SYS_DllSym( gl_h, symname );
    if( ! glClear )
        GL_SymNotFound( symname );
 
    symname = "glIndexMask";
    glIndexMask = SYS_DllSym( gl_h, symname );
    if( ! glIndexMask )
        GL_SymNotFound( symname );
 
    symname = "glColorMask";
    glColorMask = SYS_DllSym( gl_h, symname );
    if( ! glColorMask )
        GL_SymNotFound( symname );
 
    symname = "glAlphaFunc";
    glAlphaFunc = SYS_DllSym( gl_h, symname );
    if( ! glAlphaFunc )
        GL_SymNotFound( symname );
 
    symname = "glBlendFunc";
    glBlendFunc = SYS_DllSym( gl_h, symname );
    if( ! glBlendFunc )
        GL_SymNotFound( symname );
 
    symname = "glLogicOp";
    glLogicOp = SYS_DllSym( gl_h, symname );
    if( ! glLogicOp )
        GL_SymNotFound( symname );
 
    symname = "glCullFace";
    glCullFace = SYS_DllSym( gl_h, symname );
    if( ! glCullFace )
        GL_SymNotFound( symname );
 
    symname = "glFrontFace";
    glFrontFace = SYS_DllSym( gl_h, symname );
    if( ! glFrontFace )
        GL_SymNotFound( symname );
 
    symname = "glPointSize";
    glPointSize = SYS_DllSym( gl_h, symname );
    if( ! glPointSize )
        GL_SymNotFound( symname );
 
    symname = "glLineWidth";
    glLineWidth = SYS_DllSym( gl_h, symname );
    if( ! glLineWidth )
        GL_SymNotFound( symname );
 
    symname = "glLineStipple";
    glLineStipple = SYS_DllSym( gl_h, symname );
    if( ! glLineStipple )
        GL_SymNotFound( symname );
 
    symname = "glPolygonMode";
    glPolygonMode = SYS_DllSym( gl_h, symname );
    if( ! glPolygonMode )
        GL_SymNotFound( symname );
 
    symname = "glPolygonOffset";
    glPolygonOffset = SYS_DllSym( gl_h, symname );
    if( ! glPolygonOffset )
        GL_SymNotFound( symname );
 
    symname = "glPolygonStipple";
    glPolygonStipple = SYS_DllSym( gl_h, symname );
    if( ! glPolygonStipple )
        GL_SymNotFound( symname );
 
    symname = "glGetPolygonStipple";
    glGetPolygonStipple = SYS_DllSym( gl_h, symname );
    if( ! glGetPolygonStipple )
        GL_SymNotFound( symname );
 
    symname = "glEdgeFlag";
    glEdgeFlag = SYS_DllSym( gl_h, symname );
    if( ! glEdgeFlag )
        GL_SymNotFound( symname );
 
    symname = "glEdgeFlagv";
    glEdgeFlagv = SYS_DllSym( gl_h, symname );
    if( ! glEdgeFlagv )
        GL_SymNotFound( symname );
 
    symname = "glScissor";
    glScissor = SYS_DllSym( gl_h, symname );
    if( ! glScissor )
        GL_SymNotFound( symname );
 
    symname = "glClipPlane";
    glClipPlane = SYS_DllSym( gl_h, symname );
    if( ! glClipPlane )
        GL_SymNotFound( symname );
 
    symname = "glGetClipPlane";
    glGetClipPlane = SYS_DllSym( gl_h, symname );
    if( ! glGetClipPlane )
        GL_SymNotFound( symname );
 
    symname = "glDrawBuffer";
    glDrawBuffer = SYS_DllSym( gl_h, symname );
    if( ! glDrawBuffer )
        GL_SymNotFound( symname );
 
    symname = "glReadBuffer";
    glReadBuffer = SYS_DllSym( gl_h, symname );
    if( ! glReadBuffer )
        GL_SymNotFound( symname );
 
    symname = "glEnable";
    glEnable = SYS_DllSym( gl_h, symname );
    if( ! glEnable )
        GL_SymNotFound( symname );
 
    symname = "glDisable";
    glDisable = SYS_DllSym( gl_h, symname );
    if( ! glDisable )
        GL_SymNotFound( symname );
 
    symname = "glIsEnabled";
    glIsEnabled = SYS_DllSym( gl_h, symname );
    if( ! glIsEnabled )
        GL_SymNotFound( symname );
 
    symname = "glEnableClientState";
    glEnableClientState = SYS_DllSym( gl_h, symname );
    if( ! glEnableClientState )
        GL_SymNotFound( symname );
 
    symname = "glDisableClientState";
    glDisableClientState = SYS_DllSym( gl_h, symname );
    if( ! glDisableClientState )
        GL_SymNotFound( symname );
 
    symname = "glGetBooleanv";
    glGetBooleanv = SYS_DllSym( gl_h, symname );
    if( ! glGetBooleanv )
        GL_SymNotFound( symname );
 
    symname = "glGetDoublev";
    glGetDoublev = SYS_DllSym( gl_h, symname );
    if( ! glGetDoublev )
        GL_SymNotFound( symname );
 
    symname = "glGetFloatv";
    glGetFloatv = SYS_DllSym( gl_h, symname );
    if( ! glGetFloatv )
        GL_SymNotFound( symname );
 
    symname = "glGetIntegerv";
    glGetIntegerv = SYS_DllSym( gl_h, symname );
    if( ! glGetIntegerv )
        GL_SymNotFound( symname );
 
    symname = "glPushAttrib";
    glPushAttrib = SYS_DllSym( gl_h, symname );
    if( ! glPushAttrib )
        GL_SymNotFound( symname );
 
    symname = "glPopAttrib";
    glPopAttrib = SYS_DllSym( gl_h, symname );
    if( ! glPopAttrib )
        GL_SymNotFound( symname );
 
    symname = "glPushClientAttrib";
    glPushClientAttrib = SYS_DllSym( gl_h, symname );
    if( ! glPushClientAttrib )
        GL_SymNotFound( symname );
 
    symname = "glPopClientAttrib";
    glPopClientAttrib = SYS_DllSym( gl_h, symname );
    if( ! glPopClientAttrib )
        GL_SymNotFound( symname );
 
    symname = "glRenderMode";
    glRenderMode = SYS_DllSym( gl_h, symname );
    if( ! glRenderMode )
        GL_SymNotFound( symname );
 
    symname = "glGetError";
    glGetError = SYS_DllSym( gl_h, symname );
    if( ! glGetError )
        GL_SymNotFound( symname );
 
    symname = "glGetString";
    glGetString = SYS_DllSym( gl_h, symname );
    if( ! glGetString )
        GL_SymNotFound( symname );
 
    symname = "glFinish";
    glFinish = SYS_DllSym( gl_h, symname );
    if( ! glFinish )
        GL_SymNotFound( symname );
 
    symname = "glFlush";
    glFlush = SYS_DllSym( gl_h, symname );
    if( ! glFlush )
        GL_SymNotFound( symname );
 
    symname = "glHint";
    glHint = SYS_DllSym( gl_h, symname );
    if( ! glHint )
        GL_SymNotFound( symname );
 
    symname = "glClearDepth";
    glClearDepth = SYS_DllSym( gl_h, symname );
    if( ! glClearDepth )
        GL_SymNotFound( symname );
 
    symname = "glDepthFunc";
    glDepthFunc = SYS_DllSym( gl_h, symname );
    if( ! glDepthFunc )
        GL_SymNotFound( symname );
 
    symname = "glDepthMask";
    glDepthMask = SYS_DllSym( gl_h, symname );
    if( ! glDepthMask )
        GL_SymNotFound( symname );
 
    symname = "glDepthRange";
    glDepthRange = SYS_DllSym( gl_h, symname );
    if( ! glDepthRange )
        GL_SymNotFound( symname );
 
    symname = "glClearAccum";
    glClearAccum = SYS_DllSym( gl_h, symname );
    if( ! glClearAccum )
        GL_SymNotFound( symname );
 
    symname = "glAccum";
    glAccum = SYS_DllSym( gl_h, symname );
    if( ! glAccum )
        GL_SymNotFound( symname );
 
    symname = "glMatrixMode";
    glMatrixMode = SYS_DllSym( gl_h, symname );
    if( ! glMatrixMode )
        GL_SymNotFound( symname );
 
    symname = "glOrtho";
    glOrtho = SYS_DllSym( gl_h, symname );
    if( ! glOrtho )
        GL_SymNotFound( symname );
 
    symname = "glFrustum";
    glFrustum = SYS_DllSym( gl_h, symname );
    if( ! glFrustum )
        GL_SymNotFound( symname );
 
    symname = "glViewport";
    glViewport = SYS_DllSym( gl_h, symname );
    if( ! glViewport )
        GL_SymNotFound( symname );
 
    symname = "glPushMatrix";
    glPushMatrix = SYS_DllSym( gl_h, symname );
    if( ! glPushMatrix )
        GL_SymNotFound( symname );
 
    symname = "glPopMatrix";
    glPopMatrix = SYS_DllSym( gl_h, symname );
    if( ! glPopMatrix )
        GL_SymNotFound( symname );
 
    symname = "glLoadIdentity";
    glLoadIdentity = SYS_DllSym( gl_h, symname );
    if( ! glLoadIdentity )
        GL_SymNotFound( symname );
 
    symname = "glLoadMatrixd";
    glLoadMatrixd = SYS_DllSym( gl_h, symname );
    if( ! glLoadMatrixd )
        GL_SymNotFound( symname );
 
    symname = "glLoadMatrixf";
    glLoadMatrixf = SYS_DllSym( gl_h, symname );
    if( ! glLoadMatrixf )
        GL_SymNotFound( symname );
 
    symname = "glMultMatrixd";
    glMultMatrixd = SYS_DllSym( gl_h, symname );
    if( ! glMultMatrixd )
        GL_SymNotFound( symname );
 
    symname = "glMultMatrixf";
    glMultMatrixf = SYS_DllSym( gl_h, symname );
    if( ! glMultMatrixf )
        GL_SymNotFound( symname );
 
    symname = "glRotated";
    glRotated = SYS_DllSym( gl_h, symname );
    if( ! glRotated )
        GL_SymNotFound( symname );
 
    symname = "glRotatef";
    glRotatef = SYS_DllSym( gl_h, symname );
    if( ! glRotatef )
        GL_SymNotFound( symname );
 
    symname = "glScaled";
    glScaled = SYS_DllSym( gl_h, symname );
    if( ! glScaled )
        GL_SymNotFound( symname );
 
    symname = "glScalef";
    glScalef = SYS_DllSym( gl_h, symname );
    if( ! glScalef )
        GL_SymNotFound( symname );
 
    symname = "glTranslated";
    glTranslated = SYS_DllSym( gl_h, symname );
    if( ! glTranslated )
        GL_SymNotFound( symname );
 
    symname = "glTranslatef";
    glTranslatef = SYS_DllSym( gl_h, symname );
    if( ! glTranslatef )
        GL_SymNotFound( symname );
 
    symname = "glIsList";
    glIsList = SYS_DllSym( gl_h, symname );
    if( ! glIsList )
        GL_SymNotFound( symname );
 
    symname = "glDeleteLists";
    glDeleteLists = SYS_DllSym( gl_h, symname );
    if( ! glDeleteLists )
        GL_SymNotFound( symname );
 
    symname = "glGenLists";
    glGenLists = SYS_DllSym( gl_h, symname );
    if( ! glGenLists )
        GL_SymNotFound( symname );
 
    symname = "glNewList";
    glNewList = SYS_DllSym( gl_h, symname );
    if( ! glNewList )
        GL_SymNotFound( symname );
 
    symname = "glEndList";
    glEndList = SYS_DllSym( gl_h, symname );
    if( ! glEndList )
        GL_SymNotFound( symname );
 
    symname = "glCallList";
    glCallList = SYS_DllSym( gl_h, symname );
    if( ! glCallList )
        GL_SymNotFound( symname );
 
    symname = "glCallLists";
    glCallLists = SYS_DllSym( gl_h, symname );
    if( ! glCallLists )
        GL_SymNotFound( symname );
 
    symname = "glListBase";
    glListBase = SYS_DllSym( gl_h, symname );
    if( ! glListBase )
        GL_SymNotFound( symname );
 
    symname = "glBegin";
    glBegin = SYS_DllSym( gl_h, symname );
    if( ! glBegin )
        GL_SymNotFound( symname );
 
    symname = "glEnd";
    glEnd = SYS_DllSym( gl_h, symname );
    if( ! glEnd )
        GL_SymNotFound( symname );
 
    symname = "glVertex2d";
    glVertex2d = SYS_DllSym( gl_h, symname );
    if( ! glVertex2d )
        GL_SymNotFound( symname );
 
    symname = "glVertex2f";
    glVertex2f = SYS_DllSym( gl_h, symname );
    if( ! glVertex2f )
        GL_SymNotFound( symname );
 
    symname = "glVertex2i";
    glVertex2i = SYS_DllSym( gl_h, symname );
    if( ! glVertex2i )
        GL_SymNotFound( symname );
 
    symname = "glVertex2s";
    glVertex2s = SYS_DllSym( gl_h, symname );
    if( ! glVertex2s )
        GL_SymNotFound( symname );
 
    symname = "glVertex3d";
    glVertex3d = SYS_DllSym( gl_h, symname );
    if( ! glVertex3d )
        GL_SymNotFound( symname );
 
    symname = "glVertex3f";
    glVertex3f = SYS_DllSym( gl_h, symname );
    if( ! glVertex3f )
        GL_SymNotFound( symname );
 
    symname = "glVertex3i";
    glVertex3i = SYS_DllSym( gl_h, symname );
    if( ! glVertex3i )
        GL_SymNotFound( symname );
 
    symname = "glVertex3s";
    glVertex3s = SYS_DllSym( gl_h, symname );
    if( ! glVertex3s )
        GL_SymNotFound( symname );
 
    symname = "glVertex4d";
    glVertex4d = SYS_DllSym( gl_h, symname );
    if( ! glVertex4d )
        GL_SymNotFound( symname );
 
    symname = "glVertex4f";
    glVertex4f = SYS_DllSym( gl_h, symname );
    if( ! glVertex4f )
        GL_SymNotFound( symname );
 
    symname = "glVertex4i";
    glVertex4i = SYS_DllSym( gl_h, symname );
    if( ! glVertex4i )
        GL_SymNotFound( symname );
 
    symname = "glVertex4s";
    glVertex4s = SYS_DllSym( gl_h, symname );
    if( ! glVertex4s )
        GL_SymNotFound( symname );
 
    symname = "glVertex2dv";
    glVertex2dv = SYS_DllSym( gl_h, symname );
    if( ! glVertex2dv )
        GL_SymNotFound( symname );
 
    symname = "glVertex2fv";
    glVertex2fv = SYS_DllSym( gl_h, symname );
    if( ! glVertex2fv )
        GL_SymNotFound( symname );
 
    symname = "glVertex2iv";
    glVertex2iv = SYS_DllSym( gl_h, symname );
    if( ! glVertex2iv )
        GL_SymNotFound( symname );
 
    symname = "glVertex2sv";
    glVertex2sv = SYS_DllSym( gl_h, symname );
    if( ! glVertex2sv )
        GL_SymNotFound( symname );
 
    symname = "glVertex3dv";
    glVertex3dv = SYS_DllSym( gl_h, symname );
    if( ! glVertex3dv )
        GL_SymNotFound( symname );
 
    symname = "glVertex3fv";
    glVertex3fv = SYS_DllSym( gl_h, symname );
    if( ! glVertex3fv )
        GL_SymNotFound( symname );
 
    symname = "glVertex3iv";
    glVertex3iv = SYS_DllSym( gl_h, symname );
    if( ! glVertex3iv )
        GL_SymNotFound( symname );
 
    symname = "glVertex3sv";
    glVertex3sv = SYS_DllSym( gl_h, symname );
    if( ! glVertex3sv )
        GL_SymNotFound( symname );
 
    symname = "glVertex4dv";
    glVertex4dv = SYS_DllSym( gl_h, symname );
    if( ! glVertex4dv )
        GL_SymNotFound( symname );
 
    symname = "glVertex4fv";
    glVertex4fv = SYS_DllSym( gl_h, symname );
    if( ! glVertex4fv )
        GL_SymNotFound( symname );
 
    symname = "glVertex4iv";
    glVertex4iv = SYS_DllSym( gl_h, symname );
    if( ! glVertex4iv )
        GL_SymNotFound( symname );
 
    symname = "glVertex4sv";
    glVertex4sv = SYS_DllSym( gl_h, symname );
    if( ! glVertex4sv )
        GL_SymNotFound( symname );
 
    symname = "glNormal3b";
    glNormal3b = SYS_DllSym( gl_h, symname );
    if( ! glNormal3b )
        GL_SymNotFound( symname );
 
    symname = "glNormal3d";
    glNormal3d = SYS_DllSym( gl_h, symname );
    if( ! glNormal3d )
        GL_SymNotFound( symname );
 
    symname = "glNormal3f";
    glNormal3f = SYS_DllSym( gl_h, symname );
    if( ! glNormal3f )
        GL_SymNotFound( symname );
 
    symname = "glNormal3i";
    glNormal3i = SYS_DllSym( gl_h, symname );
    if( ! glNormal3i )
        GL_SymNotFound( symname );
 
    symname = "glNormal3s";
    glNormal3s = SYS_DllSym( gl_h, symname );
    if( ! glNormal3s )
        GL_SymNotFound( symname );
 
    symname = "glNormal3bv";
    glNormal3bv = SYS_DllSym( gl_h, symname );
    if( ! glNormal3bv )
        GL_SymNotFound( symname );
 
    symname = "glNormal3dv";
    glNormal3dv = SYS_DllSym( gl_h, symname );
    if( ! glNormal3dv )
        GL_SymNotFound( symname );
 
    symname = "glNormal3fv";
    glNormal3fv = SYS_DllSym( gl_h, symname );
    if( ! glNormal3fv )
        GL_SymNotFound( symname );
 
    symname = "glNormal3iv";
    glNormal3iv = SYS_DllSym( gl_h, symname );
    if( ! glNormal3iv )
        GL_SymNotFound( symname );
 
    symname = "glNormal3sv";
    glNormal3sv = SYS_DllSym( gl_h, symname );
    if( ! glNormal3sv )
        GL_SymNotFound( symname );
 
    symname = "glIndexd";
    glIndexd = SYS_DllSym( gl_h, symname );
    if( ! glIndexd )
        GL_SymNotFound( symname );
 
    symname = "glIndexf";
    glIndexf = SYS_DllSym( gl_h, symname );
    if( ! glIndexf )
        GL_SymNotFound( symname );
 
    symname = "glIndexi";
    glIndexi = SYS_DllSym( gl_h, symname );
    if( ! glIndexi )
        GL_SymNotFound( symname );
 
    symname = "glIndexs";
    glIndexs = SYS_DllSym( gl_h, symname );
    if( ! glIndexs )
        GL_SymNotFound( symname );
 
    symname = "glIndexub";
    glIndexub = SYS_DllSym( gl_h, symname );
    if( ! glIndexub )
        GL_SymNotFound( symname );
 
    symname = "glIndexdv";
    glIndexdv = SYS_DllSym( gl_h, symname );
    if( ! glIndexdv )
        GL_SymNotFound( symname );
 
    symname = "glIndexfv";
    glIndexfv = SYS_DllSym( gl_h, symname );
    if( ! glIndexfv )
        GL_SymNotFound( symname );
 
    symname = "glIndexiv";
    glIndexiv = SYS_DllSym( gl_h, symname );
    if( ! glIndexiv )
        GL_SymNotFound( symname );
 
    symname = "glIndexsv";
    glIndexsv = SYS_DllSym( gl_h, symname );
    if( ! glIndexsv )
        GL_SymNotFound( symname );
 
    symname = "glIndexubv";
    glIndexubv = SYS_DllSym( gl_h, symname );
    if( ! glIndexubv )
        GL_SymNotFound( symname );
 
    symname = "glColor3b";
    glColor3b = SYS_DllSym( gl_h, symname );
    if( ! glColor3b )
        GL_SymNotFound( symname );
 
    symname = "glColor3d";
    glColor3d = SYS_DllSym( gl_h, symname );
    if( ! glColor3d )
        GL_SymNotFound( symname );
 
    symname = "glColor3f";
    glColor3f = SYS_DllSym( gl_h, symname );
    if( ! glColor3f )
        GL_SymNotFound( symname );
 
    symname = "glColor3i";
    glColor3i = SYS_DllSym( gl_h, symname );
    if( ! glColor3i )
        GL_SymNotFound( symname );
 
    symname = "glColor3s";
    glColor3s = SYS_DllSym( gl_h, symname );
    if( ! glColor3s )
        GL_SymNotFound( symname );
 
    symname = "glColor3ub";
    glColor3ub = SYS_DllSym( gl_h, symname );
    if( ! glColor3ub )
        GL_SymNotFound( symname );
 
    symname = "glColor3ui";
    glColor3ui = SYS_DllSym( gl_h, symname );
    if( ! glColor3ui )
        GL_SymNotFound( symname );
 
    symname = "glColor3us";
    glColor3us = SYS_DllSym( gl_h, symname );
    if( ! glColor3us )
        GL_SymNotFound( symname );
 
    symname = "glColor4b";
    glColor4b = SYS_DllSym( gl_h, symname );
    if( ! glColor4b )
        GL_SymNotFound( symname );
 
    symname = "glColor4d";
    glColor4d = SYS_DllSym( gl_h, symname );
    if( ! glColor4d )
        GL_SymNotFound( symname );
 
    symname = "glColor4f";
    glColor4f = SYS_DllSym( gl_h, symname );
    if( ! glColor4f )
        GL_SymNotFound( symname );
 
    symname = "glColor4i";
    glColor4i = SYS_DllSym( gl_h, symname );
    if( ! glColor4i )
        GL_SymNotFound( symname );
 
    symname = "glColor4s";
    glColor4s = SYS_DllSym( gl_h, symname );
    if( ! glColor4s )
        GL_SymNotFound( symname );
 
    symname = "glColor4ub";
    glColor4ub = SYS_DllSym( gl_h, symname );
    if( ! glColor4ub )
        GL_SymNotFound( symname );
 
    symname = "glColor4ui";
    glColor4ui = SYS_DllSym( gl_h, symname );
    if( ! glColor4ui )
        GL_SymNotFound( symname );
 
    symname = "glColor4us";
    glColor4us = SYS_DllSym( gl_h, symname );
    if( ! glColor4us )
        GL_SymNotFound( symname );
 
    symname = "glColor3bv";
    glColor3bv = SYS_DllSym( gl_h, symname );
    if( ! glColor3bv )
        GL_SymNotFound( symname );
 
    symname = "glColor3dv";
    glColor3dv = SYS_DllSym( gl_h, symname );
    if( ! glColor3dv )
        GL_SymNotFound( symname );
 
    symname = "glColor3fv";
    glColor3fv = SYS_DllSym( gl_h, symname );
    if( ! glColor3fv )
        GL_SymNotFound( symname );
 
    symname = "glColor3iv";
    glColor3iv = SYS_DllSym( gl_h, symname );
    if( ! glColor3iv )
        GL_SymNotFound( symname );
 
    symname = "glColor3sv";
    glColor3sv = SYS_DllSym( gl_h, symname );
    if( ! glColor3sv )
        GL_SymNotFound( symname );
 
    symname = "glColor3ubv";
    glColor3ubv = SYS_DllSym( gl_h, symname );
    if( ! glColor3ubv )
        GL_SymNotFound( symname );
 
    symname = "glColor3uiv";
    glColor3uiv = SYS_DllSym( gl_h, symname );
    if( ! glColor3uiv )
        GL_SymNotFound( symname );
 
    symname = "glColor3usv";
    glColor3usv = SYS_DllSym( gl_h, symname );
    if( ! glColor3usv )
        GL_SymNotFound( symname );
 
    symname = "glColor4bv";
    glColor4bv = SYS_DllSym( gl_h, symname );
    if( ! glColor4bv )
        GL_SymNotFound( symname );
 
    symname = "glColor4dv";
    glColor4dv = SYS_DllSym( gl_h, symname );
    if( ! glColor4dv )
        GL_SymNotFound( symname );
 
    symname = "glColor4fv";
    glColor4fv = SYS_DllSym( gl_h, symname );
    if( ! glColor4fv )
        GL_SymNotFound( symname );
 
    symname = "glColor4iv";
    glColor4iv = SYS_DllSym( gl_h, symname );
    if( ! glColor4iv )
        GL_SymNotFound( symname );
 
    symname = "glColor4sv";
    glColor4sv = SYS_DllSym( gl_h, symname );
    if( ! glColor4sv )
        GL_SymNotFound( symname );
 
    symname = "glColor4ubv";
    glColor4ubv = SYS_DllSym( gl_h, symname );
    if( ! glColor4ubv )
        GL_SymNotFound( symname );
 
    symname = "glColor4uiv";
    glColor4uiv = SYS_DllSym( gl_h, symname );
    if( ! glColor4uiv )
        GL_SymNotFound( symname );
 
    symname = "glColor4usv";
    glColor4usv = SYS_DllSym( gl_h, symname );
    if( ! glColor4usv )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord1d";
    glTexCoord1d = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord1d )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord1f";
    glTexCoord1f = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord1f )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord1i";
    glTexCoord1i = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord1i )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord1s";
    glTexCoord1s = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord1s )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord2d";
    glTexCoord2d = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord2d )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord2f";
    glTexCoord2f = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord2f )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord2i";
    glTexCoord2i = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord2i )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord2s";
    glTexCoord2s = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord2s )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord3d";
    glTexCoord3d = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord3d )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord3f";
    glTexCoord3f = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord3f )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord3i";
    glTexCoord3i = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord3i )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord3s";
    glTexCoord3s = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord3s )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord4d";
    glTexCoord4d = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord4d )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord4f";
    glTexCoord4f = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord4f )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord4i";
    glTexCoord4i = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord4i )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord4s";
    glTexCoord4s = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord4s )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord1dv";
    glTexCoord1dv = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord1dv )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord1fv";
    glTexCoord1fv = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord1fv )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord1iv";
    glTexCoord1iv = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord1iv )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord1sv";
    glTexCoord1sv = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord1sv )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord2dv";
    glTexCoord2dv = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord2dv )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord2fv";
    glTexCoord2fv = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord2fv )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord2iv";
    glTexCoord2iv = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord2iv )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord2sv";
    glTexCoord2sv = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord2sv )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord3dv";
    glTexCoord3dv = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord3dv )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord3fv";
    glTexCoord3fv = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord3fv )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord3iv";
    glTexCoord3iv = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord3iv )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord3sv";
    glTexCoord3sv = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord3sv )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord4dv";
    glTexCoord4dv = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord4dv )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord4fv";
    glTexCoord4fv = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord4fv )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord4iv";
    glTexCoord4iv = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord4iv )
        GL_SymNotFound( symname );
 
    symname = "glTexCoord4sv";
    glTexCoord4sv = SYS_DllSym( gl_h, symname );
    if( ! glTexCoord4sv )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos2d";
    glRasterPos2d = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos2d )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos2f";
    glRasterPos2f = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos2f )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos2i";
    glRasterPos2i = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos2i )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos2s";
    glRasterPos2s = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos2s )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos3d";
    glRasterPos3d = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos3d )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos3f";
    glRasterPos3f = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos3f )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos3i";
    glRasterPos3i = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos3i )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos3s";
    glRasterPos3s = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos3s )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos4d";
    glRasterPos4d = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos4d )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos4f";
    glRasterPos4f = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos4f )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos4i";
    glRasterPos4i = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos4i )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos4s";
    glRasterPos4s = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos4s )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos2dv";
    glRasterPos2dv = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos2dv )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos2fv";
    glRasterPos2fv = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos2fv )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos2iv";
    glRasterPos2iv = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos2iv )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos2sv";
    glRasterPos2sv = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos2sv )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos3dv";
    glRasterPos3dv = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos3dv )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos3fv";
    glRasterPos3fv = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos3fv )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos3iv";
    glRasterPos3iv = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos3iv )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos3sv";
    glRasterPos3sv = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos3sv )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos4dv";
    glRasterPos4dv = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos4dv )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos4fv";
    glRasterPos4fv = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos4fv )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos4iv";
    glRasterPos4iv = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos4iv )
        GL_SymNotFound( symname );
 
    symname = "glRasterPos4sv";
    glRasterPos4sv = SYS_DllSym( gl_h, symname );
    if( ! glRasterPos4sv )
        GL_SymNotFound( symname );
 
    symname = "glRectd";
    glRectd = SYS_DllSym( gl_h, symname );
    if( ! glRectd )
        GL_SymNotFound( symname );
 
    symname = "glRectf";
    glRectf = SYS_DllSym( gl_h, symname );
    if( ! glRectf )
        GL_SymNotFound( symname );
 
    symname = "glRecti";
    glRecti = SYS_DllSym( gl_h, symname );
    if( ! glRecti )
        GL_SymNotFound( symname );
 
    symname = "glRects";
    glRects = SYS_DllSym( gl_h, symname );
    if( ! glRects )
        GL_SymNotFound( symname );
 
    symname = "glRectdv";
    glRectdv = SYS_DllSym( gl_h, symname );
    if( ! glRectdv )
        GL_SymNotFound( symname );
 
    symname = "glRectfv";
    glRectfv = SYS_DllSym( gl_h, symname );
    if( ! glRectfv )
        GL_SymNotFound( symname );
 
    symname = "glRectiv";
    glRectiv = SYS_DllSym( gl_h, symname );
    if( ! glRectiv )
        GL_SymNotFound( symname );
 
    symname = "glRectsv";
    glRectsv = SYS_DllSym( gl_h, symname );
    if( ! glRectsv )
        GL_SymNotFound( symname );
 
    symname = "glVertexPointer";
    glVertexPointer = SYS_DllSym( gl_h, symname );
    if( ! glVertexPointer )
        GL_SymNotFound( symname );
 
    symname = "glNormalPointer";
    glNormalPointer = SYS_DllSym( gl_h, symname );
    if( ! glNormalPointer )
        GL_SymNotFound( symname );
 
    symname = "glColorPointer";
    glColorPointer = SYS_DllSym( gl_h, symname );
    if( ! glColorPointer )
        GL_SymNotFound( symname );
 
    symname = "glIndexPointer";
    glIndexPointer = SYS_DllSym( gl_h, symname );
    if( ! glIndexPointer )
        GL_SymNotFound( symname );
 
    symname = "glTexCoordPointer";
    glTexCoordPointer = SYS_DllSym( gl_h, symname );
    if( ! glTexCoordPointer )
        GL_SymNotFound( symname );
 
    symname = "glEdgeFlagPointer";
    glEdgeFlagPointer = SYS_DllSym( gl_h, symname );
    if( ! glEdgeFlagPointer )
        GL_SymNotFound( symname );
 
    symname = "glGetPointerv";
    glGetPointerv = SYS_DllSym( gl_h, symname );
    if( ! glGetPointerv )
        GL_SymNotFound( symname );
 
    symname = "glArrayElement";
    glArrayElement = SYS_DllSym( gl_h, symname );
    if( ! glArrayElement )
        GL_SymNotFound( symname );
 
    symname = "glDrawArrays";
    glDrawArrays = SYS_DllSym( gl_h, symname );
    if( ! glDrawArrays )
        GL_SymNotFound( symname );
 
    symname = "glDrawElements";
    glDrawElements = SYS_DllSym( gl_h, symname );
    if( ! glDrawElements )
        GL_SymNotFound( symname );
 
    symname = "glInterleavedArrays";
    glInterleavedArrays = SYS_DllSym( gl_h, symname );
    if( ! glInterleavedArrays )
        GL_SymNotFound( symname );
 
    symname = "glShadeModel";
    glShadeModel = SYS_DllSym( gl_h, symname );
    if( ! glShadeModel )
        GL_SymNotFound( symname );
 
    symname = "glLightf";
    glLightf = SYS_DllSym( gl_h, symname );
    if( ! glLightf )
        GL_SymNotFound( symname );
 
    symname = "glLighti";
    glLighti = SYS_DllSym( gl_h, symname );
    if( ! glLighti )
        GL_SymNotFound( symname );
 
    symname = "glLightfv";
    glLightfv = SYS_DllSym( gl_h, symname );
    if( ! glLightfv )
        GL_SymNotFound( symname );
 
    symname = "glLightiv";
    glLightiv = SYS_DllSym( gl_h, symname );
    if( ! glLightiv )
        GL_SymNotFound( symname );
 
    symname = "glGetLightfv";
    glGetLightfv = SYS_DllSym( gl_h, symname );
    if( ! glGetLightfv )
        GL_SymNotFound( symname );
 
    symname = "glGetLightiv";
    glGetLightiv = SYS_DllSym( gl_h, symname );
    if( ! glGetLightiv )
        GL_SymNotFound( symname );
 
    symname = "glLightModelf";
    glLightModelf = SYS_DllSym( gl_h, symname );
    if( ! glLightModelf )
        GL_SymNotFound( symname );
 
    symname = "glLightModeli";
    glLightModeli = SYS_DllSym( gl_h, symname );
    if( ! glLightModeli )
        GL_SymNotFound( symname );
 
    symname = "glLightModelfv";
    glLightModelfv = SYS_DllSym( gl_h, symname );
    if( ! glLightModelfv )
        GL_SymNotFound( symname );
 
    symname = "glLightModeliv";
    glLightModeliv = SYS_DllSym( gl_h, symname );
    if( ! glLightModeliv )
        GL_SymNotFound( symname );
 
    symname = "glMaterialf";
    glMaterialf = SYS_DllSym( gl_h, symname );
    if( ! glMaterialf )
        GL_SymNotFound( symname );
 
    symname = "glMateriali";
    glMateriali = SYS_DllSym( gl_h, symname );
    if( ! glMateriali )
        GL_SymNotFound( symname );
 
    symname = "glMaterialfv";
    glMaterialfv = SYS_DllSym( gl_h, symname );
    if( ! glMaterialfv )
        GL_SymNotFound( symname );
 
    symname = "glMaterialiv";
    glMaterialiv = SYS_DllSym( gl_h, symname );
    if( ! glMaterialiv )
        GL_SymNotFound( symname );
 
    symname = "glGetMaterialfv";
    glGetMaterialfv = SYS_DllSym( gl_h, symname );
    if( ! glGetMaterialfv )
        GL_SymNotFound( symname );
 
    symname = "glGetMaterialiv";
    glGetMaterialiv = SYS_DllSym( gl_h, symname );
    if( ! glGetMaterialiv )
        GL_SymNotFound( symname );
 
    symname = "glColorMaterial";
    glColorMaterial = SYS_DllSym( gl_h, symname );
    if( ! glColorMaterial )
        GL_SymNotFound( symname );
 
    symname = "glPixelZoom";
    glPixelZoom = SYS_DllSym( gl_h, symname );
    if( ! glPixelZoom )
        GL_SymNotFound( symname );
 
    symname = "glPixelStoref";
    glPixelStoref = SYS_DllSym( gl_h, symname );
    if( ! glPixelStoref )
        GL_SymNotFound( symname );
 
    symname = "glPixelStorei";
    glPixelStorei = SYS_DllSym( gl_h, symname );
    if( ! glPixelStorei )
        GL_SymNotFound( symname );
 
    symname = "glPixelTransferf";
    glPixelTransferf = SYS_DllSym( gl_h, symname );
    if( ! glPixelTransferf )
        GL_SymNotFound( symname );
 
    symname = "glPixelTransferi";
    glPixelTransferi = SYS_DllSym( gl_h, symname );
    if( ! glPixelTransferi )
        GL_SymNotFound( symname );
 
    symname = "glPixelMapfv";
    glPixelMapfv = SYS_DllSym( gl_h, symname );
    if( ! glPixelMapfv )
        GL_SymNotFound( symname );
 
    symname = "glPixelMapuiv";
    glPixelMapuiv = SYS_DllSym( gl_h, symname );
    if( ! glPixelMapuiv )
        GL_SymNotFound( symname );
 
    symname = "glPixelMapusv";
    glPixelMapusv = SYS_DllSym( gl_h, symname );
    if( ! glPixelMapusv )
        GL_SymNotFound( symname );
 
    symname = "glGetPixelMapfv";
    glGetPixelMapfv = SYS_DllSym( gl_h, symname );
    if( ! glGetPixelMapfv )
        GL_SymNotFound( symname );
 
    symname = "glGetPixelMapuiv";
    glGetPixelMapuiv = SYS_DllSym( gl_h, symname );
    if( ! glGetPixelMapuiv )
        GL_SymNotFound( symname );
 
    symname = "glGetPixelMapusv";
    glGetPixelMapusv = SYS_DllSym( gl_h, symname );
    if( ! glGetPixelMapusv )
        GL_SymNotFound( symname );
 
    symname = "glBitmap";
    glBitmap = SYS_DllSym( gl_h, symname );
    if( ! glBitmap )
        GL_SymNotFound( symname );
 
    symname = "glReadPixels";
    glReadPixels = SYS_DllSym( gl_h, symname );
    if( ! glReadPixels )
        GL_SymNotFound( symname );
 
    symname = "glDrawPixels";
    glDrawPixels = SYS_DllSym( gl_h, symname );
    if( ! glDrawPixels )
        GL_SymNotFound( symname );
 
    symname = "glCopyPixels";
    glCopyPixels = SYS_DllSym( gl_h, symname );
    if( ! glCopyPixels )
        GL_SymNotFound( symname );
 
    symname = "glStencilFunc";
    glStencilFunc = SYS_DllSym( gl_h, symname );
    if( ! glStencilFunc )
        GL_SymNotFound( symname );
 
    symname = "glStencilMask";
    glStencilMask = SYS_DllSym( gl_h, symname );
    if( ! glStencilMask )
        GL_SymNotFound( symname );
 
    symname = "glStencilOp";
    glStencilOp = SYS_DllSym( gl_h, symname );
    if( ! glStencilOp )
        GL_SymNotFound( symname );
 
    symname = "glClearStencil";
    glClearStencil = SYS_DllSym( gl_h, symname );
    if( ! glClearStencil )
        GL_SymNotFound( symname );
 
    symname = "glTexGend";
    glTexGend = SYS_DllSym( gl_h, symname );
    if( ! glTexGend )
        GL_SymNotFound( symname );
 
    symname = "glTexGenf";
    glTexGenf = SYS_DllSym( gl_h, symname );
    if( ! glTexGenf )
        GL_SymNotFound( symname );
 
    symname = "glTexGeni";
    glTexGeni = SYS_DllSym( gl_h, symname );
    if( ! glTexGeni )
        GL_SymNotFound( symname );
 
    symname = "glTexGendv";
    glTexGendv = SYS_DllSym( gl_h, symname );
    if( ! glTexGendv )
        GL_SymNotFound( symname );
 
    symname = "glTexGenfv";
    glTexGenfv = SYS_DllSym( gl_h, symname );
    if( ! glTexGenfv )
        GL_SymNotFound( symname );
 
    symname = "glTexGeniv";
    glTexGeniv = SYS_DllSym( gl_h, symname );
    if( ! glTexGeniv )
        GL_SymNotFound( symname );
 
    symname = "glGetTexGendv";
    glGetTexGendv = SYS_DllSym( gl_h, symname );
    if( ! glGetTexGendv )
        GL_SymNotFound( symname );
 
    symname = "glGetTexGenfv";
    glGetTexGenfv = SYS_DllSym( gl_h, symname );
    if( ! glGetTexGenfv )
        GL_SymNotFound( symname );
 
    symname = "glGetTexGeniv";
    glGetTexGeniv = SYS_DllSym( gl_h, symname );
    if( ! glGetTexGeniv )
        GL_SymNotFound( symname );
 
    symname = "glTexEnvf";
    glTexEnvf = SYS_DllSym( gl_h, symname );
    if( ! glTexEnvf )
        GL_SymNotFound( symname );
 
    symname = "glTexEnvi";
    glTexEnvi = SYS_DllSym( gl_h, symname );
    if( ! glTexEnvi )
        GL_SymNotFound( symname );
 
    symname = "glTexEnvfv";
    glTexEnvfv = SYS_DllSym( gl_h, symname );
    if( ! glTexEnvfv )
        GL_SymNotFound( symname );
 
    symname = "glTexEnviv";
    glTexEnviv = SYS_DllSym( gl_h, symname );
    if( ! glTexEnviv )
        GL_SymNotFound( symname );
 
    symname = "glGetTexEnvfv";
    glGetTexEnvfv = SYS_DllSym( gl_h, symname );
    if( ! glGetTexEnvfv )
        GL_SymNotFound( symname );
 
    symname = "glGetTexEnviv";
    glGetTexEnviv = SYS_DllSym( gl_h, symname );
    if( ! glGetTexEnviv )
        GL_SymNotFound( symname );
 
    symname = "glTexParameterf";
    glTexParameterf = SYS_DllSym( gl_h, symname );
    if( ! glTexParameterf )
        GL_SymNotFound( symname );
 
    symname = "glTexParameteri";
    glTexParameteri = SYS_DllSym( gl_h, symname );
    if( ! glTexParameteri )
        GL_SymNotFound( symname );
 
    symname = "glTexParameterfv";
    glTexParameterfv = SYS_DllSym( gl_h, symname );
    if( ! glTexParameterfv )
        GL_SymNotFound( symname );
 
    symname = "glTexParameteriv";
    glTexParameteriv = SYS_DllSym( gl_h, symname );
    if( ! glTexParameteriv )
        GL_SymNotFound( symname );
 
    symname = "glGetTexParameterfv";
    glGetTexParameterfv = SYS_DllSym( gl_h, symname );
    if( ! glGetTexParameterfv )
        GL_SymNotFound( symname );
 
    symname = "glGetTexParameteriv";
    glGetTexParameteriv = SYS_DllSym( gl_h, symname );
    if( ! glGetTexParameteriv )
        GL_SymNotFound( symname );
 
    symname = "glGetTexLevelParameterfv";
    glGetTexLevelParameterfv = SYS_DllSym( gl_h, symname );
    if( ! glGetTexLevelParameterfv )
        GL_SymNotFound( symname );
 
    symname = "glGetTexLevelParameteriv";
    glGetTexLevelParameteriv = SYS_DllSym( gl_h, symname );
    if( ! glGetTexLevelParameteriv )
        GL_SymNotFound( symname );
 
    symname = "glTexImage1D";
    glTexImage1D = SYS_DllSym( gl_h, symname );
    if( ! glTexImage1D )
        GL_SymNotFound( symname );
 
    symname = "glTexImage2D";
    glTexImage2D = SYS_DllSym( gl_h, symname );
    if( ! glTexImage2D )
        GL_SymNotFound( symname );
 
    symname = "glGetTexImage";
    glGetTexImage = SYS_DllSym( gl_h, symname );
    if( ! glGetTexImage )
        GL_SymNotFound( symname );
 
    symname = "glGenTextures";
    glGenTextures = SYS_DllSym( gl_h, symname );
    if( ! glGenTextures )
        GL_SymNotFound( symname );
 
    symname = "glDeleteTextures";
    glDeleteTextures = SYS_DllSym( gl_h, symname );
    if( ! glDeleteTextures )
        GL_SymNotFound( symname );
 
    symname = "glBindTexture";
    glBindTexture = SYS_DllSym( gl_h, symname );
    if( ! glBindTexture )
        GL_SymNotFound( symname );
 
    symname = "glPrioritizeTextures";
    glPrioritizeTextures = SYS_DllSym( gl_h, symname );
    if( ! glPrioritizeTextures )
        GL_SymNotFound( symname );
 
    symname = "glAreTexturesResident";
    glAreTexturesResident = SYS_DllSym( gl_h, symname );
    if( ! glAreTexturesResident )
        GL_SymNotFound( symname );
 
    symname = "glIsTexture";
    glIsTexture = SYS_DllSym( gl_h, symname );
    if( ! glIsTexture )
        GL_SymNotFound( symname );
 
    symname = "glTexSubImage1D";
    glTexSubImage1D = SYS_DllSym( gl_h, symname );
    if( ! glTexSubImage1D )
        GL_SymNotFound( symname );
 
    symname = "glTexSubImage2D";
    glTexSubImage2D = SYS_DllSym( gl_h, symname );
    if( ! glTexSubImage2D )
        GL_SymNotFound( symname );
 
    symname = "glCopyTexImage1D";
    glCopyTexImage1D = SYS_DllSym( gl_h, symname );
    if( ! glCopyTexImage1D )
        GL_SymNotFound( symname );
 
    symname = "glCopyTexImage2D";
    glCopyTexImage2D = SYS_DllSym( gl_h, symname );
    if( ! glCopyTexImage2D )
        GL_SymNotFound( symname );
 
    symname = "glCopyTexSubImage1D";
    glCopyTexSubImage1D = SYS_DllSym( gl_h, symname );
    if( ! glCopyTexSubImage1D )
        GL_SymNotFound( symname );
 
    symname = "glCopyTexSubImage2D";
    glCopyTexSubImage2D = SYS_DllSym( gl_h, symname );
    if( ! glCopyTexSubImage2D )
        GL_SymNotFound( symname );
 
    symname = "glMap1d";
    glMap1d = SYS_DllSym( gl_h, symname );
    if( ! glMap1d )
        GL_SymNotFound( symname );
 
    symname = "glMap1f";
    glMap1f = SYS_DllSym( gl_h, symname );
    if( ! glMap1f )
        GL_SymNotFound( symname );
 
    symname = "glMap2d";
    glMap2d = SYS_DllSym( gl_h, symname );
    if( ! glMap2d )
        GL_SymNotFound( symname );
 
    symname = "glMap2f";
    glMap2f = SYS_DllSym( gl_h, symname );
    if( ! glMap2f )
        GL_SymNotFound( symname );
 
    symname = "glGetMapdv";
    glGetMapdv = SYS_DllSym( gl_h, symname );
    if( ! glGetMapdv )
        GL_SymNotFound( symname );
 
    symname = "glGetMapfv";
    glGetMapfv = SYS_DllSym( gl_h, symname );
    if( ! glGetMapfv )
        GL_SymNotFound( symname );
 
    symname = "glGetMapiv";
    glGetMapiv = SYS_DllSym( gl_h, symname );
    if( ! glGetMapiv )
        GL_SymNotFound( symname );
 
    symname = "glEvalCoord1d";
    glEvalCoord1d = SYS_DllSym( gl_h, symname );
    if( ! glEvalCoord1d )
        GL_SymNotFound( symname );
 
    symname = "glEvalCoord1f";
    glEvalCoord1f = SYS_DllSym( gl_h, symname );
    if( ! glEvalCoord1f )
        GL_SymNotFound( symname );
 
    symname = "glEvalCoord1dv";
    glEvalCoord1dv = SYS_DllSym( gl_h, symname );
    if( ! glEvalCoord1dv )
        GL_SymNotFound( symname );
 
    symname = "glEvalCoord1fv";
    glEvalCoord1fv = SYS_DllSym( gl_h, symname );
    if( ! glEvalCoord1fv )
        GL_SymNotFound( symname );
 
    symname = "glEvalCoord2d";
    glEvalCoord2d = SYS_DllSym( gl_h, symname );
    if( ! glEvalCoord2d )
        GL_SymNotFound( symname );
 
    symname = "glEvalCoord2f";
    glEvalCoord2f = SYS_DllSym( gl_h, symname );
    if( ! glEvalCoord2f )
        GL_SymNotFound( symname );
 
    symname = "glEvalCoord2dv";
    glEvalCoord2dv = SYS_DllSym( gl_h, symname );
    if( ! glEvalCoord2dv )
        GL_SymNotFound( symname );
 
    symname = "glEvalCoord2fv";
    glEvalCoord2fv = SYS_DllSym( gl_h, symname );
    if( ! glEvalCoord2fv )
        GL_SymNotFound( symname );
 
    symname = "glMapGrid1d";
    glMapGrid1d = SYS_DllSym( gl_h, symname );
    if( ! glMapGrid1d )
        GL_SymNotFound( symname );
 
    symname = "glMapGrid1f";
    glMapGrid1f = SYS_DllSym( gl_h, symname );
    if( ! glMapGrid1f )
        GL_SymNotFound( symname );
 
    symname = "glMapGrid2d";
    glMapGrid2d = SYS_DllSym( gl_h, symname );
    if( ! glMapGrid2d )
        GL_SymNotFound( symname );
 
    symname = "glMapGrid2f";
    glMapGrid2f = SYS_DllSym( gl_h, symname );
    if( ! glMapGrid2f )
        GL_SymNotFound( symname );
 
    symname = "glEvalPoint1";
    glEvalPoint1 = SYS_DllSym( gl_h, symname );
    if( ! glEvalPoint1 )
        GL_SymNotFound( symname );
 
    symname = "glEvalPoint2";
    glEvalPoint2 = SYS_DllSym( gl_h, symname );
    if( ! glEvalPoint2 )
        GL_SymNotFound( symname );
 
    symname = "glEvalMesh1";
    glEvalMesh1 = SYS_DllSym( gl_h, symname );
    if( ! glEvalMesh1 )
        GL_SymNotFound( symname );
 
    symname = "glEvalMesh2";
    glEvalMesh2 = SYS_DllSym( gl_h, symname );
    if( ! glEvalMesh2 )
        GL_SymNotFound( symname );
 
    symname = "glFogf";
    glFogf = SYS_DllSym( gl_h, symname );
    if( ! glFogf )
        GL_SymNotFound( symname );
 
    symname = "glFogi";
    glFogi = SYS_DllSym( gl_h, symname );
    if( ! glFogi )
        GL_SymNotFound( symname );
 
    symname = "glFogfv";
    glFogfv = SYS_DllSym( gl_h, symname );
    if( ! glFogfv )
        GL_SymNotFound( symname );
 
    symname = "glFogiv";
    glFogiv = SYS_DllSym( gl_h, symname );
    if( ! glFogiv )
        GL_SymNotFound( symname );
 
    symname = "glFeedbackBuffer";
    glFeedbackBuffer = SYS_DllSym( gl_h, symname );
    if( ! glFeedbackBuffer )
        GL_SymNotFound( symname );
 
    symname = "glPassThrough";
    glPassThrough = SYS_DllSym( gl_h, symname );
    if( ! glPassThrough )
        GL_SymNotFound( symname );
 
    symname = "glSelectBuffer";
    glSelectBuffer = SYS_DllSym( gl_h, symname );
    if( ! glSelectBuffer )
        GL_SymNotFound( symname );
 
    symname = "glInitNames";
    glInitNames = SYS_DllSym( gl_h, symname );
    if( ! glInitNames )
        GL_SymNotFound( symname );
 
    symname = "glLoadName";
    glLoadName = SYS_DllSym( gl_h, symname );
    if( ! glLoadName )
        GL_SymNotFound( symname );
 
    symname = "glPushName";
    glPushName = SYS_DllSym( gl_h, symname );
    if( ! glPushName )
        GL_SymNotFound( symname );
 
    symname = "glPopName";
    glPopName = SYS_DllSym( gl_h, symname );
    if( ! glPopName )
        GL_SymNotFound( symname );
 
    symname = "glActiveTextureARB";
    glActiveTextureARB = SYS_DllSym( gl_h, symname );
    if( ! glActiveTextureARB )
        GL_SymNotFound( symname );
 
    symname = "glClientActiveTextureARB";
    glClientActiveTextureARB = SYS_DllSym( gl_h, symname );
    if( ! glClientActiveTextureARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord1dARB";
    glMultiTexCoord1dARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord1dARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord1dvARB";
    glMultiTexCoord1dvARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord1dvARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord1fARB";
    glMultiTexCoord1fARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord1fARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord1fvARB";
    glMultiTexCoord1fvARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord1fvARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord1iARB";
    glMultiTexCoord1iARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord1iARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord1ivARB";
    glMultiTexCoord1ivARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord1ivARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord1sARB";
    glMultiTexCoord1sARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord1sARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord1svARB";
    glMultiTexCoord1svARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord1svARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord2dARB";
    glMultiTexCoord2dARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord2dARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord2dvARB";
    glMultiTexCoord2dvARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord2dvARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord2fARB";
    glMultiTexCoord2fARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord2fARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord2fvARB";
    glMultiTexCoord2fvARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord2fvARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord2iARB";
    glMultiTexCoord2iARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord2iARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord2ivARB";
    glMultiTexCoord2ivARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord2ivARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord2sARB";
    glMultiTexCoord2sARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord2sARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord2svARB";
    glMultiTexCoord2svARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord2svARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord3dARB";
    glMultiTexCoord3dARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord3dARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord3dvARB";
    glMultiTexCoord3dvARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord3dvARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord3fARB";
    glMultiTexCoord3fARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord3fARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord3fvARB";
    glMultiTexCoord3fvARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord3fvARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord3iARB";
    glMultiTexCoord3iARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord3iARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord3ivARB";
    glMultiTexCoord3ivARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord3ivARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord3sARB";
    glMultiTexCoord3sARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord3sARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord3svARB";
    glMultiTexCoord3svARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord3svARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord4dARB";
    glMultiTexCoord4dARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord4dARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord4dvARB";
    glMultiTexCoord4dvARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord4dvARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord4fARB";
    glMultiTexCoord4fARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord4fARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord4fvARB";
    glMultiTexCoord4fvARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord4fvARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord4iARB";
    glMultiTexCoord4iARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord4iARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord4ivARB";
    glMultiTexCoord4ivARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord4ivARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord4sARB";
    glMultiTexCoord4sARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord4sARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord4svARB";
    glMultiTexCoord4svARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord4svARB )
        GL_SymNotFound( symname );
 
    symname = "glPointParameterfEXT";
    glPointParameterfEXT = SYS_DllSym( gl_h, symname );
    if( ! glPointParameterfEXT )
        GL_SymNotFound( symname );
 
    symname = "glPointParameterfvEXT";
    glPointParameterfvEXT = SYS_DllSym( gl_h, symname );
    if( ! glPointParameterfvEXT )
        GL_SymNotFound( symname );
 
    symname = "glBlendFuncSeparateINGR";
    glBlendFuncSeparateINGR = SYS_DllSym( gl_h, symname );
    if( ! glBlendFuncSeparateINGR )
        GL_SymNotFound( symname );
 
    symname = "glDrawRangeElements";
    glDrawRangeElements = SYS_DllSym( gl_h, symname );
    if( ! glDrawRangeElements )
        GL_SymNotFound( symname );
 
    symname = "glTexImage3D";
    glTexImage3D = SYS_DllSym( gl_h, symname );
    if( ! glTexImage3D )
        GL_SymNotFound( symname );
 
    symname = "glTexSubImage3D";
    glTexSubImage3D = SYS_DllSym( gl_h, symname );
    if( ! glTexSubImage3D )
        GL_SymNotFound( symname );
 
    symname = "glCopyTexSubImage3D";
    glCopyTexSubImage3D = SYS_DllSym( gl_h, symname );
    if( ! glCopyTexSubImage3D )
        GL_SymNotFound( symname );
 
    symname = "glHistogram";
    glHistogram = SYS_DllSym( gl_h, symname );
    if( ! glHistogram )
        GL_SymNotFound( symname );
 
    symname = "glResetHistogram";
    glResetHistogram = SYS_DllSym( gl_h, symname );
    if( ! glResetHistogram )
        GL_SymNotFound( symname );
 
    symname = "glGetHistogram";
    glGetHistogram = SYS_DllSym( gl_h, symname );
    if( ! glGetHistogram )
        GL_SymNotFound( symname );
 
    symname = "glGetHistogramParameterfv";
    glGetHistogramParameterfv = SYS_DllSym( gl_h, symname );
    if( ! glGetHistogramParameterfv )
        GL_SymNotFound( symname );
 
    symname = "glGetHistogramParameteriv";
    glGetHistogramParameteriv = SYS_DllSym( gl_h, symname );
    if( ! glGetHistogramParameteriv )
        GL_SymNotFound( symname );
 
    symname = "glMinmax";
    glMinmax = SYS_DllSym( gl_h, symname );
    if( ! glMinmax )
        GL_SymNotFound( symname );
 
    symname = "glResetMinmax";
    glResetMinmax = SYS_DllSym( gl_h, symname );
    if( ! glResetMinmax )
        GL_SymNotFound( symname );
 
    symname = "glGetMinmaxParameterfv";
    glGetMinmaxParameterfv = SYS_DllSym( gl_h, symname );
    if( ! glGetMinmaxParameterfv )
        GL_SymNotFound( symname );
 
    symname = "glGetMinmaxParameteriv";
    glGetMinmaxParameteriv = SYS_DllSym( gl_h, symname );
    if( ! glGetMinmaxParameteriv )
        GL_SymNotFound( symname );
 
    symname = "glConvolutionFilter1D";
    glConvolutionFilter1D = SYS_DllSym( gl_h, symname );
    if( ! glConvolutionFilter1D )
        GL_SymNotFound( symname );
 
    symname = "glConvolutionFilter2D";
    glConvolutionFilter2D = SYS_DllSym( gl_h, symname );
    if( ! glConvolutionFilter2D )
        GL_SymNotFound( symname );
 
    symname = "glConvolutionParameterf";
    glConvolutionParameterf = SYS_DllSym( gl_h, symname );
    if( ! glConvolutionParameterf )
        GL_SymNotFound( symname );
 
    symname = "glConvolutionParameterfv";
    glConvolutionParameterfv = SYS_DllSym( gl_h, symname );
    if( ! glConvolutionParameterfv )
        GL_SymNotFound( symname );
 
    symname = "glConvolutionParameteri";
    glConvolutionParameteri = SYS_DllSym( gl_h, symname );
    if( ! glConvolutionParameteri )
        GL_SymNotFound( symname );
 
    symname = "glConvolutionParameteriv";
    glConvolutionParameteriv = SYS_DllSym( gl_h, symname );
    if( ! glConvolutionParameteriv )
        GL_SymNotFound( symname );
 
    symname = "glCopyConvolutionFilter1D";
    glCopyConvolutionFilter1D = SYS_DllSym( gl_h, symname );
    if( ! glCopyConvolutionFilter1D )
        GL_SymNotFound( symname );
 
    symname = "glCopyConvolutionFilter2D";
    glCopyConvolutionFilter2D = SYS_DllSym( gl_h, symname );
    if( ! glCopyConvolutionFilter2D )
        GL_SymNotFound( symname );
 
    symname = "glGetConvolutionFilter";
    glGetConvolutionFilter = SYS_DllSym( gl_h, symname );
    if( ! glGetConvolutionFilter )
        GL_SymNotFound( symname );
 
    symname = "glGetConvolutionParameterfv";
    glGetConvolutionParameterfv = SYS_DllSym( gl_h, symname );
    if( ! glGetConvolutionParameterfv )
        GL_SymNotFound( symname );
 
    symname = "glGetConvolutionParameteriv";
    glGetConvolutionParameteriv = SYS_DllSym( gl_h, symname );
    if( ! glGetConvolutionParameteriv )
        GL_SymNotFound( symname );
 
    symname = "glSeparableFilter2D";
    glSeparableFilter2D = SYS_DllSym( gl_h, symname );
    if( ! glSeparableFilter2D )
        GL_SymNotFound( symname );
 
    symname = "glGetSeparableFilter";
    glGetSeparableFilter = SYS_DllSym( gl_h, symname );
    if( ! glGetSeparableFilter )
        GL_SymNotFound( symname );
 
    symname = "glCopyColorSubTable";
    glCopyColorSubTable = SYS_DllSym( gl_h, symname );
    if( ! glCopyColorSubTable )
        GL_SymNotFound( symname );
 
    symname = "glCopyColorTable";
    glCopyColorTable = SYS_DllSym( gl_h, symname );
    if( ! glCopyColorTable )
        GL_SymNotFound( symname );
 
    symname = "glCopyTexSubImage2D";
    glCopyTexSubImage2D = SYS_DllSym( gl_h, symname );
    if( ! glCopyTexSubImage2D )
        GL_SymNotFound( symname );
 
    symname = "glMap1d";
    glMap1d = SYS_DllSym( gl_h, symname );
    if( ! glMap1d )
        GL_SymNotFound( symname );
 
    symname = "glMap1f";
    glMap1f = SYS_DllSym( gl_h, symname );
    if( ! glMap1f )
        GL_SymNotFound( symname );
 
    symname = "glMap2d";
    glMap2d = SYS_DllSym( gl_h, symname );
    if( ! glMap2d )
        GL_SymNotFound( symname );
 
    symname = "glMap2f";
    glMap2f = SYS_DllSym( gl_h, symname );
    if( ! glMap2f )
        GL_SymNotFound( symname );
 
    symname = "glGetMapdv";
    glGetMapdv = SYS_DllSym( gl_h, symname );
    if( ! glGetMapdv )
        GL_SymNotFound( symname );
 
    symname = "glGetMapfv";
    glGetMapfv = SYS_DllSym( gl_h, symname );
    if( ! glGetMapfv )
        GL_SymNotFound( symname );
 
    symname = "glGetMapiv";
    glGetMapiv = SYS_DllSym( gl_h, symname );
    if( ! glGetMapiv )
        GL_SymNotFound( symname );
 
    symname = "glEvalCoord1d";
    glEvalCoord1d = SYS_DllSym( gl_h, symname );
    if( ! glEvalCoord1d )
        GL_SymNotFound( symname );
 
    symname = "glEvalCoord1f";
    glEvalCoord1f = SYS_DllSym( gl_h, symname );
    if( ! glEvalCoord1f )
        GL_SymNotFound( symname );
 
    symname = "glEvalCoord1dv";
    glEvalCoord1dv = SYS_DllSym( gl_h, symname );
    if( ! glEvalCoord1dv )
        GL_SymNotFound( symname );
 
    symname = "glEvalCoord1fv";
    glEvalCoord1fv = SYS_DllSym( gl_h, symname );
    if( ! glEvalCoord1fv )
        GL_SymNotFound( symname );
 
    symname = "glEvalCoord2d";
    glEvalCoord2d = SYS_DllSym( gl_h, symname );
    if( ! glEvalCoord2d )
        GL_SymNotFound( symname );
 
    symname = "glEvalCoord2f";
    glEvalCoord2f = SYS_DllSym( gl_h, symname );
    if( ! glEvalCoord2f )
        GL_SymNotFound( symname );
 
    symname = "glEvalCoord2dv";
    glEvalCoord2dv = SYS_DllSym( gl_h, symname );
    if( ! glEvalCoord2dv )
        GL_SymNotFound( symname );
 
    symname = "glEvalCoord2fv";
    glEvalCoord2fv = SYS_DllSym( gl_h, symname );
    if( ! glEvalCoord2fv )
        GL_SymNotFound( symname );
 
    symname = "glMapGrid1d";
    glMapGrid1d = SYS_DllSym( gl_h, symname );
    if( ! glMapGrid1d )
        GL_SymNotFound( symname );
 
    symname = "glMapGrid1f";
    glMapGrid1f = SYS_DllSym( gl_h, symname );
    if( ! glMapGrid1f )
        GL_SymNotFound( symname );
 
    symname = "glMapGrid2d";
    glMapGrid2d = SYS_DllSym( gl_h, symname );
    if( ! glMapGrid2d )
        GL_SymNotFound( symname );
 
    symname = "glMapGrid2f";
    glMapGrid2f = SYS_DllSym( gl_h, symname );
    if( ! glMapGrid2f )
        GL_SymNotFound( symname );
 
    symname = "glEvalPoint1";
    glEvalPoint1 = SYS_DllSym( gl_h, symname );
    if( ! glEvalPoint1 )
        GL_SymNotFound( symname );
 
    symname = "glEvalPoint2";
    glEvalPoint2 = SYS_DllSym( gl_h, symname );
    if( ! glEvalPoint2 )
        GL_SymNotFound( symname );
 
    symname = "glEvalMesh1";
    glEvalMesh1 = SYS_DllSym( gl_h, symname );
    if( ! glEvalMesh1 )
        GL_SymNotFound( symname );
 
    symname = "glEvalMesh2";
    glEvalMesh2 = SYS_DllSym( gl_h, symname );
    if( ! glEvalMesh2 )
        GL_SymNotFound( symname );
 
    symname = "glFogf";
    glFogf = SYS_DllSym( gl_h, symname );
    if( ! glFogf )
        GL_SymNotFound( symname );
 
    symname = "glFogi";
    glFogi = SYS_DllSym( gl_h, symname );
    if( ! glFogi )
        GL_SymNotFound( symname );
 
    symname = "glFogfv";
    glFogfv = SYS_DllSym( gl_h, symname );
    if( ! glFogfv )
        GL_SymNotFound( symname );
 
    symname = "glFogiv";
    glFogiv = SYS_DllSym( gl_h, symname );
    if( ! glFogiv )
        GL_SymNotFound( symname );
 
    symname = "glFeedbackBuffer";
    glFeedbackBuffer = SYS_DllSym( gl_h, symname );
    if( ! glFeedbackBuffer )
        GL_SymNotFound( symname );
 
    symname = "glPassThrough";
    glPassThrough = SYS_DllSym( gl_h, symname );
    if( ! glPassThrough )
        GL_SymNotFound( symname );
 
    symname = "glSelectBuffer";
    glSelectBuffer = SYS_DllSym( gl_h, symname );
    if( ! glSelectBuffer )
        GL_SymNotFound( symname );
 
    symname = "glInitNames";
    glInitNames = SYS_DllSym( gl_h, symname );
    if( ! glInitNames )
        GL_SymNotFound( symname );
 
    symname = "glLoadName";
    glLoadName = SYS_DllSym( gl_h, symname );
    if( ! glLoadName )
        GL_SymNotFound( symname );
 
    symname = "glPushName";
    glPushName = SYS_DllSym( gl_h, symname );
    if( ! glPushName )
        GL_SymNotFound( symname );
 
    symname = "glPopName";
    glPopName = SYS_DllSym( gl_h, symname );
    if( ! glPopName )
        GL_SymNotFound( symname );
 
    symname = "glActiveTextureARB";
    glActiveTextureARB = SYS_DllSym( gl_h, symname );
    if( ! glActiveTextureARB )
        GL_SymNotFound( symname );
 
    symname = "glClientActiveTextureARB";
    glClientActiveTextureARB = SYS_DllSym( gl_h, symname );
    if( ! glClientActiveTextureARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord1dARB";
    glMultiTexCoord1dARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord1dARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord1dvARB";
    glMultiTexCoord1dvARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord1dvARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord1fARB";
    glMultiTexCoord1fARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord1fARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord1fvARB";
    glMultiTexCoord1fvARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord1fvARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord1iARB";
    glMultiTexCoord1iARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord1iARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord1ivARB";
    glMultiTexCoord1ivARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord1ivARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord1sARB";
    glMultiTexCoord1sARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord1sARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord1svARB";
    glMultiTexCoord1svARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord1svARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord2dARB";
    glMultiTexCoord2dARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord2dARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord2dvARB";
    glMultiTexCoord2dvARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord2dvARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord2fARB";
    glMultiTexCoord2fARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord2fARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord2fvARB";
    glMultiTexCoord2fvARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord2fvARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord2iARB";
    glMultiTexCoord2iARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord2iARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord2ivARB";
    glMultiTexCoord2ivARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord2ivARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord2sARB";
    glMultiTexCoord2sARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord2sARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord2svARB";
    glMultiTexCoord2svARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord2svARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord3dARB";
    glMultiTexCoord3dARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord3dARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord3dvARB";
    glMultiTexCoord3dvARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord3dvARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord3fARB";
    glMultiTexCoord3fARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord3fARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord3fvARB";
    glMultiTexCoord3fvARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord3fvARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord3iARB";
    glMultiTexCoord3iARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord3iARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord3ivARB";
    glMultiTexCoord3ivARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord3ivARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord3sARB";
    glMultiTexCoord3sARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord3sARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord3svARB";
    glMultiTexCoord3svARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord3svARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord4dARB";
    glMultiTexCoord4dARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord4dARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord4dvARB";
    glMultiTexCoord4dvARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord4dvARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord4fARB";
    glMultiTexCoord4fARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord4fARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord4fvARB";
    glMultiTexCoord4fvARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord4fvARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord4iARB";
    glMultiTexCoord4iARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord4iARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord4ivARB";
    glMultiTexCoord4ivARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord4ivARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord4sARB";
    glMultiTexCoord4sARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord4sARB )
        GL_SymNotFound( symname );
 
    symname = "glMultiTexCoord4svARB";
    glMultiTexCoord4svARB = SYS_DllSym( gl_h, symname );
    if( ! glMultiTexCoord4svARB )
        GL_SymNotFound( symname );
 
    symname = "glPointParameterfEXT";
    glPointParameterfEXT = SYS_DllSym( gl_h, symname );
    if( ! glPointParameterfEXT )
        GL_SymNotFound( symname );
 
    symname = "glPointParameterfvEXT";
    glPointParameterfvEXT = SYS_DllSym( gl_h, symname );
    if( ! glPointParameterfvEXT )
        GL_SymNotFound( symname );
 
    symname = "glBlendFuncSeparateINGR";
    glBlendFuncSeparateINGR = SYS_DllSym( gl_h, symname );
    if( ! glBlendFuncSeparateINGR )
        GL_SymNotFound( symname );
 
    symname = "glDrawRangeElements";
    glDrawRangeElements = SYS_DllSym( gl_h, symname );
    if( ! glDrawRangeElements )
        GL_SymNotFound( symname );
 
    symname = "glTexImage3D";
    glTexImage3D = SYS_DllSym( gl_h, symname );
    if( ! glTexImage3D )
        GL_SymNotFound( symname );
 
    symname = "glTexSubImage3D";
    glTexSubImage3D = SYS_DllSym( gl_h, symname );
    if( ! glTexSubImage3D )
        GL_SymNotFound( symname );
 
    symname = "glCopyTexSubImage3D";
    glCopyTexSubImage3D = SYS_DllSym( gl_h, symname );
    if( ! glCopyTexSubImage3D )
        GL_SymNotFound( symname );
 
    symname = "glHistogram";
    glHistogram = SYS_DllSym( gl_h, symname );
    if( ! glHistogram )
        GL_SymNotFound( symname );
 
    symname = "glResetHistogram";
    glResetHistogram = SYS_DllSym( gl_h, symname );
    if( ! glResetHistogram )
        GL_SymNotFound( symname );
 
    symname = "glGetHistogram";
    glGetHistogram = SYS_DllSym( gl_h, symname );
    if( ! glGetHistogram )
        GL_SymNotFound( symname );
 
    symname = "glGetHistogramParameterfv";
    glGetHistogramParameterfv = SYS_DllSym( gl_h, symname );
    if( ! glGetHistogramParameterfv )
        GL_SymNotFound( symname );
 
    symname = "glGetHistogramParameteriv";
    glGetHistogramParameteriv = SYS_DllSym( gl_h, symname );
    if( ! glGetHistogramParameteriv )
        GL_SymNotFound( symname );
 
    symname = "glMinmax";
    glMinmax = SYS_DllSym( gl_h, symname );
    if( ! glMinmax )
        GL_SymNotFound( symname );
 
    symname = "glResetMinmax";
    glResetMinmax = SYS_DllSym( gl_h, symname );
    if( ! glResetMinmax )
        GL_SymNotFound( symname );
 
    symname = "glGetMinmaxParameterfv";
    glGetMinmaxParameterfv = SYS_DllSym( gl_h, symname );
    if( ! glGetMinmaxParameterfv )
        GL_SymNotFound( symname );
 
    symname = "glGetMinmaxParameteriv";
    glGetMinmaxParameteriv = SYS_DllSym( gl_h, symname );
    if( ! glGetMinmaxParameteriv )
        GL_SymNotFound( symname );
 
    symname = "glConvolutionFilter1D";
    glConvolutionFilter1D = SYS_DllSym( gl_h, symname );
    if( ! glConvolutionFilter1D )
        GL_SymNotFound( symname );
 
    symname = "glConvolutionFilter2D";
    glConvolutionFilter2D = SYS_DllSym( gl_h, symname );
    if( ! glConvolutionFilter2D )
        GL_SymNotFound( symname );
 
    symname = "glConvolutionParameterf";
    glConvolutionParameterf = SYS_DllSym( gl_h, symname );
    if( ! glConvolutionParameterf )
        GL_SymNotFound( symname );
 
    symname = "glConvolutionParameterfv";
    glConvolutionParameterfv = SYS_DllSym( gl_h, symname );
    if( ! glConvolutionParameterfv )
        GL_SymNotFound( symname );
 
    symname = "glConvolutionParameteri";
    glConvolutionParameteri = SYS_DllSym( gl_h, symname );
    if( ! glConvolutionParameteri )
        GL_SymNotFound( symname );
 
    symname = "glConvolutionParameteriv";
    glConvolutionParameteriv = SYS_DllSym( gl_h, symname );
    if( ! glConvolutionParameteriv )
        GL_SymNotFound( symname );
 
    symname = "glCopyConvolutionFilter1D";
    glCopyConvolutionFilter1D = SYS_DllSym( gl_h, symname );
    if( ! glCopyConvolutionFilter1D )
        GL_SymNotFound( symname );
 
    symname = "glCopyConvolutionFilter2D";
    glCopyConvolutionFilter2D = SYS_DllSym( gl_h, symname );
    if( ! glCopyConvolutionFilter2D )
        GL_SymNotFound( symname );
 
    symname = "glGetConvolutionFilter";
    glGetConvolutionFilter = SYS_DllSym( gl_h, symname );
    if( ! glGetConvolutionFilter )
        GL_SymNotFound( symname );
 
    symname = "glGetConvolutionParameterfv";
    glGetConvolutionParameterfv = SYS_DllSym( gl_h, symname );
    if( ! glGetConvolutionParameterfv )
        GL_SymNotFound( symname );
 
    symname = "glGetConvolutionParameteriv";
    glGetConvolutionParameteriv = SYS_DllSym( gl_h, symname );
    if( ! glGetConvolutionParameteriv )
        GL_SymNotFound( symname );
 
    symname = "glSeparableFilter2D";
    glSeparableFilter2D = SYS_DllSym( gl_h, symname );
    if( ! glSeparableFilter2D )
        GL_SymNotFound( symname );
 
    symname = "glGetSeparableFilter";
    glGetSeparableFilter = SYS_DllSym( gl_h, symname );
    if( ! glGetSeparableFilter )
        GL_SymNotFound( symname );
 
    symname = "glCopyColorSubTable";
    glCopyColorSubTable = SYS_DllSym( gl_h, symname );
    if( ! glCopyColorSubTable )
        GL_SymNotFound( symname );
 
    symname = "glCopyColorTable";
    glCopyColorTable = SYS_DllSym( gl_h, symname );
    if( ! glCopyColorTable )
        GL_SymNotFound( symname );
    
    
}
