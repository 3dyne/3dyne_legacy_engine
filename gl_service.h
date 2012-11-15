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



// gl_service.h

// GS openGL interface implementation

#ifndef gl_service_h
#define gl_service_h

#include "gl_defs.h"

void GL_GetInfo( gl_info_t *info );

void GL_LoadGLApi( char * );
void GL_RejectGLApi();

#ifndef GL_LINKED

#ifndef GLAPI
#define GLAPI extern
#endif


/*
 * Miscellaneous
 */

GLAPI void (* glClearIndex ) ( GLfloat c );

GLAPI void (* glClearColor ) ( GLclampf red,
			  GLclampf green,
			  GLclampf blue,
			  GLclampf alpha );

GLAPI void (* glClear ) ( GLbitfield mask );

GLAPI void (* glIndexMask ) ( GLuint mask );

GLAPI void (* glColorMask ) ( GLboolean red, GLboolean green,
			 GLboolean blue, GLboolean alpha );

GLAPI void (* glAlphaFunc ) ( GLenum func, GLclampf ref );

GLAPI void (* glBlendFunc ) ( GLenum sfactor, GLenum dfactor );

GLAPI void (* glLogicOp ) ( GLenum opcode );

GLAPI void (* glCullFace ) ( GLenum mode );

GLAPI void (* glFrontFace ) ( GLenum mode );

GLAPI void (* glPointSize ) ( GLfloat size );

GLAPI void (* glLineWidth ) ( GLfloat width );

GLAPI void (* glLineStipple ) ( GLint factor, GLushort pattern );

GLAPI void (* glPolygonMode ) ( GLenum face, GLenum mode );

GLAPI void (* glPolygonOffset ) ( GLfloat factor, GLfloat units );

GLAPI void (* glPolygonStipple ) ( const GLubyte *mask );

GLAPI void (* glGetPolygonStipple ) ( GLubyte *mask );

GLAPI void (* glEdgeFlag ) ( GLboolean flag );

GLAPI void (* glEdgeFlagv ) ( const GLboolean *flag );

GLAPI void (* glScissor ) ( GLint x, GLint y,
                                   GLsizei width, GLsizei height);

GLAPI void (* glClipPlane ) ( GLenum plane, const GLdouble *equation );

GLAPI void (* glGetClipPlane ) ( GLenum plane, GLdouble *equation );

GLAPI void (* glDrawBuffer ) ( GLenum mode );

GLAPI void (* glReadBuffer ) ( GLenum mode );

GLAPI void (* glEnable ) ( GLenum cap );

GLAPI void (* glDisable ) ( GLenum cap );

GLAPI GLboolean (* glIsEnabled ) ( GLenum cap );


GLAPI void (* glEnableClientState ) ( GLenum cap );  /* 1.1 */

GLAPI void (* glDisableClientState ) ( GLenum cap );  /* 1.1 */


GLAPI void (* glGetBooleanv ) ( GLenum pname, GLboolean *params );

GLAPI void (* glGetDoublev ) ( GLenum pname, GLdouble *params );

GLAPI void (* glGetFloatv ) ( GLenum pname, GLfloat *params );

GLAPI void (* glGetIntegerv ) ( GLenum pname, GLint *params );


GLAPI void (* glPushAttrib ) ( GLbitfield mask );

GLAPI void (* glPopAttrib ) ( void );


GLAPI void (* glPushClientAttrib ) ( GLbitfield mask );  /* 1.1 */

GLAPI void (* glPopClientAttrib ) ( void );  /* 1.1 */


GLAPI GLint (* glRenderMode ) ( GLenum mode );

GLAPI GLenum (* glGetError ) ( void );

GLAPI const GLubyte* (* glGetString ) ( GLenum name );

GLAPI void (* glFinish ) ( void );

GLAPI void (* glFlush ) ( void );

GLAPI void (* glHint ) ( GLenum target, GLenum mode );



/*
 * Depth Buffer
 */

GLAPI void (* glClearDepth ) ( GLclampd depth );

GLAPI void (* glDepthFunc ) ( GLenum func );

GLAPI void (* glDepthMask ) ( GLboolean flag );

GLAPI void (* glDepthRange ) ( GLclampd near_val, GLclampd far_val );


/*
 * Accumulation Buffer
 */

GLAPI void (* glClearAccum ) ( GLfloat red, GLfloat green,
                                      GLfloat blue, GLfloat alpha );

GLAPI void (* glAccum ) ( GLenum op, GLfloat value );



/*
 * Transformation
 */

GLAPI void (* glMatrixMode ) ( GLenum mode );

GLAPI void (* glOrtho ) ( GLdouble left, GLdouble right,
                                 GLdouble bottom, GLdouble top,
                                 GLdouble near_val, GLdouble far_val );

GLAPI void (* glFrustum ) ( GLdouble left, GLdouble right,
                                   GLdouble bottom, GLdouble top,
                                   GLdouble near_val, GLdouble far_val );

GLAPI void (* glViewport ) ( GLint x, GLint y,
                                    GLsizei width, GLsizei height );

GLAPI void (* glPushMatrix ) ( void );

GLAPI void (* glPopMatrix ) ( void );

GLAPI void (* glLoadIdentity ) ( void );

GLAPI void (* glLoadMatrixd ) ( const GLdouble *m );
GLAPI void (* glLoadMatrixf ) ( const GLfloat *m );

GLAPI void (* glMultMatrixd ) ( const GLdouble *m );
GLAPI void (* glMultMatrixf ) ( const GLfloat *m );

GLAPI void (* glRotated ) ( GLdouble angle,
                                   GLdouble x, GLdouble y, GLdouble z );
GLAPI void (* glRotatef ) ( GLfloat angle,
                                   GLfloat x, GLfloat y, GLfloat z );

GLAPI void (* glScaled ) ( GLdouble x, GLdouble y, GLdouble z );
GLAPI void (* glScalef ) ( GLfloat x, GLfloat y, GLfloat z );

GLAPI void (* glTranslated ) ( GLdouble x, GLdouble y, GLdouble z );
GLAPI void (* glTranslatef ) ( GLfloat x, GLfloat y, GLfloat z );



/*
 * Display Lists
 */

GLAPI GLboolean (* glIsList ) ( GLuint list );

GLAPI void (* glDeleteLists ) ( GLuint list, GLsizei range );

GLAPI GLuint (* glGenLists ) ( GLsizei range );

GLAPI void (* glNewList ) ( GLuint list, GLenum mode );

GLAPI void (* glEndList ) ( void );

GLAPI void (* glCallList ) ( GLuint list );

GLAPI void (* glCallLists ) ( GLsizei n, GLenum type,
                                     const GLvoid *lists );

GLAPI void (* glListBase ) ( GLuint base );



/*
 * Drawing Functions
 */

GLAPI void (* glBegin ) ( GLenum mode );

GLAPI void (* glEnd ) ( void );


GLAPI void (* glVertex2d ) ( GLdouble x, GLdouble y );
GLAPI void (* glVertex2f ) ( GLfloat x, GLfloat y );
GLAPI void (* glVertex2i ) ( GLint x, GLint y );
GLAPI void (* glVertex2s ) ( GLshort x, GLshort y );

GLAPI void (* glVertex3d ) ( GLdouble x, GLdouble y, GLdouble z );
GLAPI void (* glVertex3f ) ( GLfloat x, GLfloat y, GLfloat z );
GLAPI void (* glVertex3i ) ( GLint x, GLint y, GLint z );
GLAPI void (* glVertex3s ) ( GLshort x, GLshort y, GLshort z );

GLAPI void (* glVertex4d ) ( GLdouble x, GLdouble y, GLdouble z, GLdouble w );
GLAPI void (* glVertex4f ) ( GLfloat x, GLfloat y, GLfloat z, GLfloat w );
GLAPI void (* glVertex4i ) ( GLint x, GLint y, GLint z, GLint w );
GLAPI void (* glVertex4s ) ( GLshort x, GLshort y, GLshort z, GLshort w );

GLAPI void (* glVertex2dv ) ( const GLdouble *v );
GLAPI void (* glVertex2fv ) ( const GLfloat *v );
GLAPI void (* glVertex2iv ) ( const GLint *v );
GLAPI void (* glVertex2sv ) ( const GLshort *v );

GLAPI void (* glVertex3dv ) ( const GLdouble *v );
GLAPI void (* glVertex3fv ) ( const GLfloat *v );
GLAPI void (* glVertex3iv ) ( const GLint *v );
GLAPI void (* glVertex3sv ) ( const GLshort *v );

GLAPI void (* glVertex4dv ) ( const GLdouble *v );
GLAPI void (* glVertex4fv ) ( const GLfloat *v );
GLAPI void (* glVertex4iv ) ( const GLint *v );
GLAPI void (* glVertex4sv ) ( const GLshort *v );


GLAPI void (* glNormal3b ) ( GLbyte nx, GLbyte ny, GLbyte nz );
GLAPI void (* glNormal3d ) ( GLdouble nx, GLdouble ny, GLdouble nz );
GLAPI void (* glNormal3f ) ( GLfloat nx, GLfloat ny, GLfloat nz );
GLAPI void (* glNormal3i ) ( GLint nx, GLint ny, GLint nz );
GLAPI void (* glNormal3s ) ( GLshort nx, GLshort ny, GLshort nz );

GLAPI void (* glNormal3bv ) ( const GLbyte *v );
GLAPI void (* glNormal3dv ) ( const GLdouble *v );
GLAPI void (* glNormal3fv ) ( const GLfloat *v );
GLAPI void (* glNormal3iv ) ( const GLint *v );
GLAPI void (* glNormal3sv ) ( const GLshort *v );


GLAPI void (* glIndexd ) ( GLdouble c );
GLAPI void (* glIndexf ) ( GLfloat c );
GLAPI void (* glIndexi ) ( GLint c );
GLAPI void (* glIndexs ) ( GLshort c );
GLAPI void (* glIndexub ) ( GLubyte c );  /* 1.1 */

GLAPI void (* glIndexdv ) ( const GLdouble *c );
GLAPI void (* glIndexfv ) ( const GLfloat *c );
GLAPI void (* glIndexiv ) ( const GLint *c );
GLAPI void (* glIndexsv ) ( const GLshort *c );
GLAPI void (* glIndexubv ) ( const GLubyte *c );  /* 1.1 */

GLAPI void (* glColor3b ) ( GLbyte red, GLbyte green, GLbyte blue );
GLAPI void (* glColor3d ) ( GLdouble red, GLdouble green, GLdouble blue );
GLAPI void (* glColor3f ) ( GLfloat red, GLfloat green, GLfloat blue );
GLAPI void (* glColor3i ) ( GLint red, GLint green, GLint blue );
GLAPI void (* glColor3s ) ( GLshort red, GLshort green, GLshort blue );
GLAPI void (* glColor3ub ) ( GLubyte red, GLubyte green, GLubyte blue );
GLAPI void (* glColor3ui ) ( GLuint red, GLuint green, GLuint blue );
GLAPI void (* glColor3us ) ( GLushort red, GLushort green, GLushort blue );

GLAPI void (* glColor4b ) ( GLbyte red, GLbyte green,
                                   GLbyte blue, GLbyte alpha );
GLAPI void (* glColor4d ) ( GLdouble red, GLdouble green,
                                   GLdouble blue, GLdouble alpha );
GLAPI void (* glColor4f ) ( GLfloat red, GLfloat green,
                                   GLfloat blue, GLfloat alpha );
GLAPI void (* glColor4i ) ( GLint red, GLint green,
                                   GLint blue, GLint alpha );
GLAPI void (* glColor4s ) ( GLshort red, GLshort green,
                                   GLshort blue, GLshort alpha );
GLAPI void (* glColor4ub ) ( GLubyte red, GLubyte green,
                                    GLubyte blue, GLubyte alpha );
GLAPI void (* glColor4ui ) ( GLuint red, GLuint green,
                                    GLuint blue, GLuint alpha );
GLAPI void (* glColor4us ) ( GLushort red, GLushort green,
                                    GLushort blue, GLushort alpha );


GLAPI void (* glColor3bv ) ( const GLbyte *v );
GLAPI void (* glColor3dv ) ( const GLdouble *v );
GLAPI void (* glColor3fv ) ( const GLfloat *v );
GLAPI void (* glColor3iv ) ( const GLint *v );
GLAPI void (* glColor3sv ) ( const GLshort *v );
GLAPI void (* glColor3ubv ) ( const GLubyte *v );
GLAPI void (* glColor3uiv ) ( const GLuint *v );
GLAPI void (* glColor3usv ) ( const GLushort *v );

GLAPI void (* glColor4bv ) ( const GLbyte *v );
GLAPI void (* glColor4dv ) ( const GLdouble *v );
GLAPI void (* glColor4fv ) ( const GLfloat *v );
GLAPI void (* glColor4iv ) ( const GLint *v );
GLAPI void (* glColor4sv ) ( const GLshort *v );
GLAPI void (* glColor4ubv ) ( const GLubyte *v );
GLAPI void (* glColor4uiv ) ( const GLuint *v );
GLAPI void (* glColor4usv ) ( const GLushort *v );


GLAPI void (* glTexCoord1d ) ( GLdouble s );
GLAPI void (* glTexCoord1f ) ( GLfloat s );
GLAPI void (* glTexCoord1i ) ( GLint s );
GLAPI void (* glTexCoord1s ) ( GLshort s );

GLAPI void (* glTexCoord2d ) ( GLdouble s, GLdouble t );
GLAPI void (* glTexCoord2f ) ( GLfloat s, GLfloat t );
GLAPI void (* glTexCoord2i ) ( GLint s, GLint t );
GLAPI void (* glTexCoord2s ) ( GLshort s, GLshort t );

GLAPI void (* glTexCoord3d ) ( GLdouble s, GLdouble t, GLdouble r );
GLAPI void (* glTexCoord3f ) ( GLfloat s, GLfloat t, GLfloat r );
GLAPI void (* glTexCoord3i ) ( GLint s, GLint t, GLint r );
GLAPI void (* glTexCoord3s ) ( GLshort s, GLshort t, GLshort r );

GLAPI void (* glTexCoord4d ) ( GLdouble s, GLdouble t, GLdouble r, GLdouble q );
GLAPI void (* glTexCoord4f ) ( GLfloat s, GLfloat t, GLfloat r, GLfloat q );
GLAPI void (* glTexCoord4i ) ( GLint s, GLint t, GLint r, GLint q );
GLAPI void (* glTexCoord4s ) ( GLshort s, GLshort t, GLshort r, GLshort q );

GLAPI void (* glTexCoord1dv ) ( const GLdouble *v );
GLAPI void (* glTexCoord1fv ) ( const GLfloat *v );
GLAPI void (* glTexCoord1iv ) ( const GLint *v );
GLAPI void (* glTexCoord1sv ) ( const GLshort *v );

GLAPI void (* glTexCoord2dv ) ( const GLdouble *v );
GLAPI void (* glTexCoord2fv ) ( const GLfloat *v );
GLAPI void (* glTexCoord2iv ) ( const GLint *v );
GLAPI void (* glTexCoord2sv ) ( const GLshort *v );

GLAPI void (* glTexCoord3dv ) ( const GLdouble *v );
GLAPI void (* glTexCoord3fv ) ( const GLfloat *v );
GLAPI void (* glTexCoord3iv ) ( const GLint *v );
GLAPI void (* glTexCoord3sv ) ( const GLshort *v );

GLAPI void (* glTexCoord4dv ) ( const GLdouble *v );
GLAPI void (* glTexCoord4fv ) ( const GLfloat *v );
GLAPI void (* glTexCoord4iv ) ( const GLint *v );
GLAPI void (* glTexCoord4sv ) ( const GLshort *v );


GLAPI void (* glRasterPos2d ) ( GLdouble x, GLdouble y );
GLAPI void (* glRasterPos2f ) ( GLfloat x, GLfloat y );
GLAPI void (* glRasterPos2i ) ( GLint x, GLint y );
GLAPI void (* glRasterPos2s ) ( GLshort x, GLshort y );

GLAPI void (* glRasterPos3d ) ( GLdouble x, GLdouble y, GLdouble z );
GLAPI void (* glRasterPos3f ) ( GLfloat x, GLfloat y, GLfloat z );
GLAPI void (* glRasterPos3i ) ( GLint x, GLint y, GLint z );
GLAPI void (* glRasterPos3s ) ( GLshort x, GLshort y, GLshort z );

GLAPI void (* glRasterPos4d ) ( GLdouble x, GLdouble y, GLdouble z, GLdouble w );
GLAPI void (* glRasterPos4f ) ( GLfloat x, GLfloat y, GLfloat z, GLfloat w );
GLAPI void (* glRasterPos4i ) ( GLint x, GLint y, GLint z, GLint w );
GLAPI void (* glRasterPos4s ) ( GLshort x, GLshort y, GLshort z, GLshort w );

GLAPI void (* glRasterPos2dv ) ( const GLdouble *v );
GLAPI void (* glRasterPos2fv ) ( const GLfloat *v );
GLAPI void (* glRasterPos2iv ) ( const GLint *v );
GLAPI void (* glRasterPos2sv ) ( const GLshort *v );

GLAPI void (* glRasterPos3dv ) ( const GLdouble *v );
GLAPI void (* glRasterPos3fv ) ( const GLfloat *v );
GLAPI void (* glRasterPos3iv ) ( const GLint *v );
GLAPI void (* glRasterPos3sv ) ( const GLshort *v );

GLAPI void (* glRasterPos4dv ) ( const GLdouble *v );
GLAPI void (* glRasterPos4fv ) ( const GLfloat *v );
GLAPI void (* glRasterPos4iv ) ( const GLint *v );
GLAPI void (* glRasterPos4sv ) ( const GLshort *v );


GLAPI void (* glRectd ) ( GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2 );
GLAPI void (* glRectf ) ( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 );
GLAPI void (* glRecti ) ( GLint x1, GLint y1, GLint x2, GLint y2 );
GLAPI void (* glRects ) ( GLshort x1, GLshort y1, GLshort x2, GLshort y2 );


GLAPI void (* glRectdv ) ( const GLdouble *v1, const GLdouble *v2 );
GLAPI void (* glRectfv ) ( const GLfloat *v1, const GLfloat *v2 );
GLAPI void (* glRectiv ) ( const GLint *v1, const GLint *v2 );
GLAPI void (* glRectsv ) ( const GLshort *v1, const GLshort *v2 );



/*
 * Vertex Arrays   ) (1.1)
 */

GLAPI void (* glVertexPointer ) ( GLint size, GLenum type,
                                         GLsizei stride, const GLvoid *ptr );

GLAPI void (* glNormalPointer ) ( GLenum type, GLsizei stride,
                                         const GLvoid *ptr );

GLAPI void (* glColorPointer ) ( GLint size, GLenum type,
                                        GLsizei stride, const GLvoid *ptr );

GLAPI void (* glIndexPointer ) ( GLenum type, GLsizei stride,
                                        const GLvoid *ptr );

GLAPI void (* glTexCoordPointer ) ( GLint size, GLenum type,
                                           GLsizei stride, const GLvoid *ptr );

GLAPI void (* glEdgeFlagPointer ) ( GLsizei stride, const GLvoid *ptr );

GLAPI void (* glGetPointerv ) ( GLenum pname, void **params );

GLAPI void (* glArrayElement ) ( GLint i );

GLAPI void (* glDrawArrays ) ( GLenum mode, GLint first,
                                      GLsizei count );

GLAPI void (* glDrawElements ) ( GLenum mode, GLsizei count,
                                        GLenum type, const GLvoid *indices );

GLAPI void (* glInterleavedArrays ) ( GLenum format, GLsizei stride,
                                             const GLvoid *pointer );


/*
 * Lighting
 */

GLAPI void (* glShadeModel ) ( GLenum mode );

GLAPI void (* glLightf ) ( GLenum light, GLenum pname, GLfloat param );
GLAPI void (* glLighti ) ( GLenum light, GLenum pname, GLint param );
GLAPI void (* glLightfv ) ( GLenum light, GLenum pname,
                                   const GLfloat *params );
GLAPI void (* glLightiv ) ( GLenum light, GLenum pname,
                                   const GLint *params );

GLAPI void (* glGetLightfv ) ( GLenum light, GLenum pname,
                                      GLfloat *params );
GLAPI void (* glGetLightiv ) ( GLenum light, GLenum pname,
                                      GLint *params );

GLAPI void (* glLightModelf ) ( GLenum pname, GLfloat param );
GLAPI void (* glLightModeli ) ( GLenum pname, GLint param );
GLAPI void (* glLightModelfv ) ( GLenum pname, const GLfloat *params );
GLAPI void (* glLightModeliv ) ( GLenum pname, const GLint *params );

GLAPI void (* glMaterialf ) ( GLenum face, GLenum pname, GLfloat param );
GLAPI void (* glMateriali ) ( GLenum face, GLenum pname, GLint param );
GLAPI void (* glMaterialfv ) ( GLenum face, GLenum pname, const GLfloat *params );
GLAPI void (* glMaterialiv ) ( GLenum face, GLenum pname, const GLint *params );

GLAPI void (* glGetMaterialfv ) ( GLenum face, GLenum pname, GLfloat *params );
GLAPI void (* glGetMaterialiv ) ( GLenum face, GLenum pname, GLint *params );

GLAPI void (* glColorMaterial ) ( GLenum face, GLenum mode );




/*
 * Raster functions
 */

GLAPI void (* glPixelZoom ) ( GLfloat xfactor, GLfloat yfactor );

GLAPI void (* glPixelStoref ) ( GLenum pname, GLfloat param );
GLAPI void (* glPixelStorei ) ( GLenum pname, GLint param );

GLAPI void (* glPixelTransferf ) ( GLenum pname, GLfloat param );
GLAPI void (* glPixelTransferi ) ( GLenum pname, GLint param );

GLAPI void (* glPixelMapfv ) ( GLenum map, GLint mapsize,
                                      const GLfloat *values );
GLAPI void (* glPixelMapuiv ) ( GLenum map, GLint mapsize,
                                       const GLuint *values );
GLAPI void (* glPixelMapusv ) ( GLenum map, GLint mapsize,
                                       const GLushort *values );

GLAPI void (* glGetPixelMapfv ) ( GLenum map, GLfloat *values );
GLAPI void (* glGetPixelMapuiv ) ( GLenum map, GLuint *values );
GLAPI void (* glGetPixelMapusv ) ( GLenum map, GLushort *values );

GLAPI void (* glBitmap ) ( GLsizei width, GLsizei height,
                                  GLfloat xorig, GLfloat yorig,
                                  GLfloat xmove, GLfloat ymove,
                                  const GLubyte *bitmap );

GLAPI void (* glReadPixels ) ( GLint x, GLint y,
                                      GLsizei width, GLsizei height,
                                      GLenum format, GLenum type,
                                      GLvoid *pixels );

GLAPI void (* glDrawPixels ) ( GLsizei width, GLsizei height,
                                      GLenum format, GLenum type,
                                      const GLvoid *pixels );

GLAPI void (* glCopyPixels ) ( GLint x, GLint y,
                                      GLsizei width, GLsizei height,
                                      GLenum type );



/*
 * Stenciling
 */

GLAPI void (* glStencilFunc ) ( GLenum func, GLint ref, GLuint mask );

GLAPI void (* glStencilMask ) ( GLuint mask );

GLAPI void (* glStencilOp ) ( GLenum fail, GLenum zfail, GLenum zpass );

GLAPI void (* glClearStencil ) ( GLint s );



/*
 * Texture mapping
 */

GLAPI void (* glTexGend ) ( GLenum coord, GLenum pname, GLdouble param );
GLAPI void (* glTexGenf ) ( GLenum coord, GLenum pname, GLfloat param );
GLAPI void (* glTexGeni ) ( GLenum coord, GLenum pname, GLint param );

GLAPI void (* glTexGendv ) ( GLenum coord, GLenum pname, const GLdouble *params );
GLAPI void (* glTexGenfv ) ( GLenum coord, GLenum pname, const GLfloat *params );
GLAPI void (* glTexGeniv ) ( GLenum coord, GLenum pname, const GLint *params );

GLAPI void (* glGetTexGendv ) ( GLenum coord, GLenum pname, GLdouble *params );
GLAPI void (* glGetTexGenfv ) ( GLenum coord, GLenum pname, GLfloat *params );
GLAPI void (* glGetTexGeniv ) ( GLenum coord, GLenum pname, GLint *params );


GLAPI void (* glTexEnvf ) ( GLenum target, GLenum pname, GLfloat param );
GLAPI void (* glTexEnvi ) ( GLenum target, GLenum pname, GLint param );

GLAPI void (* glTexEnvfv ) ( GLenum target, GLenum pname, const GLfloat *params );
GLAPI void (* glTexEnviv ) ( GLenum target, GLenum pname, const GLint *params );

GLAPI void (* glGetTexEnvfv ) ( GLenum target, GLenum pname, GLfloat *params );
GLAPI void (* glGetTexEnviv ) ( GLenum target, GLenum pname, GLint *params );


GLAPI void (* glTexParameterf ) ( GLenum target, GLenum pname, GLfloat param );
GLAPI void (* glTexParameteri ) ( GLenum target, GLenum pname, GLint param );

GLAPI void (* glTexParameterfv ) ( GLenum target, GLenum pname,
                                          const GLfloat *params );
GLAPI void (* glTexParameteriv ) ( GLenum target, GLenum pname,
                                          const GLint *params );

GLAPI void (* glGetTexParameterfv ) ( GLenum target,
                                             GLenum pname, GLfloat *params);
GLAPI void (* glGetTexParameteriv ) ( GLenum target,
                                             GLenum pname, GLint *params );

GLAPI void (* glGetTexLevelParameterfv ) ( GLenum target, GLint level,
                                                  GLenum pname, GLfloat *params );
GLAPI void (* glGetTexLevelParameteriv ) ( GLenum target, GLint level,
                                                  GLenum pname, GLint *params );


GLAPI void (* glTexImage1D ) ( GLenum target, GLint level,
                                      GLint internalFormat,
                                      GLsizei width, GLint border,
                                      GLenum format, GLenum type,
                                      const GLvoid *pixels );

GLAPI void (* glTexImage2D ) ( GLenum target, GLint level,
                                      GLint internalFormat,
                                      GLsizei width, GLsizei height,
                                      GLint border, GLenum format, GLenum type,
                                      const GLvoid *pixels );

GLAPI void (* glGetTexImage ) ( GLenum target, GLint level,
                                       GLenum format, GLenum type,
                                       GLvoid *pixels );



/* 1.1 functions */

GLAPI void (* glGenTextures ) ( GLsizei n, GLuint *textures );

GLAPI void (* glDeleteTextures ) ( GLsizei n, const GLuint *textures);

GLAPI void (* glBindTexture ) ( GLenum target, GLuint texture );

GLAPI void (* glPrioritizeTextures ) ( GLsizei n,
                                              const GLuint *textures,
                                              const GLclampf *priorities );

GLAPI GLboolean (* glAreTexturesResident ) ( GLsizei n,
                                                    const GLuint *textures,
                                                    GLboolean *residences );

GLAPI GLboolean (* glIsTexture ) ( GLuint texture );


GLAPI void (* glTexSubImage1D ) ( GLenum target, GLint level,
                                         GLint xoffset,
                                         GLsizei width, GLenum format,
                                         GLenum type, const GLvoid *pixels );


GLAPI void (* glTexSubImage2D ) ( GLenum target, GLint level,
                                         GLint xoffset, GLint yoffset,
                                         GLsizei width, GLsizei height,
                                         GLenum format, GLenum type,
                                         const GLvoid *pixels );


GLAPI void (* glCopyTexImage1D ) ( GLenum target, GLint level,
                                          GLenum internalformat,
                                          GLint x, GLint y,
                                          GLsizei width, GLint border );


GLAPI void (* glCopyTexImage2D ) ( GLenum target, GLint level,
                                          GLenum internalformat,
                                          GLint x, GLint y,
                                          GLsizei width, GLsizei height,
                                          GLint border );


GLAPI void (* glCopyTexSubImage1D ) ( GLenum target, GLint level,
                                             GLint xoffset, GLint x, GLint y,
                                             GLsizei width );


GLAPI void (* glCopyTexSubImage2D ) ( GLenum target, GLint level,
                                             GLint xoffset, GLint yoffset,
                                             GLint x, GLint y,
                                             GLsizei width, GLsizei height );




/*
 * Evaluators
 */

GLAPI void (* glMap1d ) ( GLenum target, GLdouble u1, GLdouble u2,
                                 GLint stride,
                                 GLint order, const GLdouble *points );
GLAPI void (* glMap1f ) ( GLenum target, GLfloat u1, GLfloat u2,
                                 GLint stride,
                                 GLint order, const GLfloat *points );

GLAPI void (* glMap2d ) ( GLenum target,
		     GLdouble u1, GLdouble u2, GLint ustride, GLint uorder,
		     GLdouble v1, GLdouble v2, GLint vstride, GLint vorder,
		     const GLdouble *points );
GLAPI void (* glMap2f ) ( GLenum target,
		     GLfloat u1, GLfloat u2, GLint ustride, GLint uorder,
		     GLfloat v1, GLfloat v2, GLint vstride, GLint vorder,
		     const GLfloat *points );

GLAPI void (* glGetMapdv ) ( GLenum target, GLenum query, GLdouble *v );
GLAPI void (* glGetMapfv ) ( GLenum target, GLenum query, GLfloat *v );
GLAPI void (* glGetMapiv ) ( GLenum target, GLenum query, GLint *v );

GLAPI void (* glEvalCoord1d ) ( GLdouble u );
GLAPI void (* glEvalCoord1f ) ( GLfloat u );

GLAPI void (* glEvalCoord1dv ) ( const GLdouble *u );
GLAPI void (* glEvalCoord1fv ) ( const GLfloat *u );

GLAPI void (* glEvalCoord2d ) ( GLdouble u, GLdouble v );
GLAPI void (* glEvalCoord2f ) ( GLfloat u, GLfloat v );

GLAPI void (* glEvalCoord2dv ) ( const GLdouble *u );
GLAPI void (* glEvalCoord2fv ) ( const GLfloat *u );

GLAPI void (* glMapGrid1d ) ( GLint un, GLdouble u1, GLdouble u2 );
GLAPI void (* glMapGrid1f ) ( GLint un, GLfloat u1, GLfloat u2 );

GLAPI void (* glMapGrid2d ) ( GLint un, GLdouble u1, GLdouble u2,
                                     GLint vn, GLdouble v1, GLdouble v2 );
GLAPI void (* glMapGrid2f ) ( GLint un, GLfloat u1, GLfloat u2,
                                     GLint vn, GLfloat v1, GLfloat v2 );

GLAPI void (* glEvalPoint1 ) ( GLint i );

GLAPI void (* glEvalPoint2 ) ( GLint i, GLint j );

GLAPI void (* glEvalMesh1 ) ( GLenum mode, GLint i1, GLint i2 );

GLAPI void (* glEvalMesh2 ) ( GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2 );



/*
 * Fog
 */

GLAPI void (* glFogf ) ( GLenum pname, GLfloat param );

GLAPI void (* glFogi ) ( GLenum pname, GLint param );

GLAPI void (* glFogfv ) ( GLenum pname, const GLfloat *params );

GLAPI void (* glFogiv ) ( GLenum pname, const GLint *params );



/*
 * Selection and Feedback
 */

GLAPI void (* glFeedbackBuffer ) ( GLsizei size, GLenum type, GLfloat *buffer );

GLAPI void (* glPassThrough ) ( GLfloat token );

GLAPI void (* glSelectBuffer ) ( GLsizei size, GLuint *buffer );

GLAPI void (* glInitNames ) ( void );

GLAPI void (* glLoadName ) ( GLuint name );

GLAPI void (* glPushName ) ( GLuint name );

GLAPI void (* glPopName ) ( void );





GLAPI void (* glActiveTextureARB ) (GLenum texture);
GLAPI void (* glClientActiveTextureARB ) (GLenum texture);
GLAPI void (* glMultiTexCoord1dARB ) (GLenum target, GLdouble s);
GLAPI void (* glMultiTexCoord1dvARB ) (GLenum target, const GLdouble *v);
GLAPI void (* glMultiTexCoord1fARB ) (GLenum target, GLfloat s);
GLAPI void (* glMultiTexCoord1fvARB ) (GLenum target, const GLfloat *v);
GLAPI void (* glMultiTexCoord1iARB ) (GLenum target, GLint s);
GLAPI void (* glMultiTexCoord1ivARB ) (GLenum target, const GLint *v);
GLAPI void (* glMultiTexCoord1sARB ) (GLenum target, GLshort s);
GLAPI void (* glMultiTexCoord1svARB ) (GLenum target, const GLshort *v);
GLAPI void (* glMultiTexCoord2dARB ) (GLenum target, GLdouble s, GLdouble t);
GLAPI void (* glMultiTexCoord2dvARB ) (GLenum target, const GLdouble *v);
GLAPI void (* glMultiTexCoord2fARB ) (GLenum target, GLfloat s, GLfloat t);
GLAPI void (* glMultiTexCoord2fvARB ) (GLenum target, const GLfloat *v);
GLAPI void (* glMultiTexCoord2iARB ) (GLenum target, GLint s, GLint t);
GLAPI void (* glMultiTexCoord2ivARB ) (GLenum target, const GLint *v);
GLAPI void (* glMultiTexCoord2sARB ) (GLenum target, GLshort s, GLshort t);
GLAPI void (* glMultiTexCoord2svARB ) (GLenum target, const GLshort *v);
GLAPI void (* glMultiTexCoord3dARB ) (GLenum target, GLdouble s, GLdouble t, GLdouble r);
GLAPI void (* glMultiTexCoord3dvARB ) (GLenum target, const GLdouble *v);
GLAPI void (* glMultiTexCoord3fARB ) (GLenum target, GLfloat s, GLfloat t, GLfloat r);
GLAPI void (* glMultiTexCoord3fvARB ) (GLenum target, const GLfloat *v);
GLAPI void (* glMultiTexCoord3iARB ) (GLenum target, GLint s, GLint t, GLint r);
GLAPI void (* glMultiTexCoord3ivARB ) (GLenum target, const GLint *v);
GLAPI void (* glMultiTexCoord3sARB ) (GLenum target, GLshort s, GLshort t, GLshort r);
GLAPI void (* glMultiTexCoord3svARB ) (GLenum target, const GLshort *v);
GLAPI void (* glMultiTexCoord4dARB ) (GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
GLAPI void (* glMultiTexCoord4dvARB ) (GLenum target, const GLdouble *v);
GLAPI void (* glMultiTexCoord4fARB ) (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
GLAPI void (* glMultiTexCoord4fvARB ) (GLenum target, const GLfloat *v);
GLAPI void (* glMultiTexCoord4iARB ) (GLenum target, GLint s, GLint t, GLint r, GLint q);
GLAPI void (* glMultiTexCoord4ivARB ) (GLenum target, const GLint *v);
GLAPI void (* glMultiTexCoord4sARB ) (GLenum target, GLshort s, GLshort t, GLshort r, GLshort q);
GLAPI void (* glMultiTexCoord4svARB ) (GLenum target, const GLshort *v);



/* GL_EXT_point_parameters */
GLAPI void (* glPointParameterfEXT ) ( GLenum pname, GLfloat param );
GLAPI void (* glPointParameterfvEXT ) ( GLenum pname,
                                               const GLfloat *params );



/* GL_INGR_blend_func_separate */
GLAPI void (* glBlendFuncSeparateINGR ) ( GLenum sfactorRGB,
                                               GLenum dfactorRGB,
                                               GLenum sfactorAlpha, 
                                               GLenum dfactorAlpha );



/* 1.2 functions */
GLAPI void (* glDrawRangeElements ) ( GLenum mode, GLuint start,
	GLuint end, GLsizei count, GLenum type, const GLvoid *indices );

GLAPI void (* glTexImage3D ) ( GLenum target, GLint level,
                                      GLint internalFormat,
                                      GLsizei width, GLsizei height,
                                      GLsizei depth, GLint border,
                                      GLenum format, GLenum type,
                                      const GLvoid *pixels );

GLAPI void (* glTexSubImage3D ) ( GLenum target, GLint level,
                                         GLint xoffset, GLint yoffset,
                                         GLint zoffset, GLsizei width,
                                         GLsizei height, GLsizei depth,
                                         GLenum format,
                                         GLenum type, const GLvoid *pixels);

GLAPI void (* glCopyTexSubImage3D ) ( GLenum target, GLint level,
                                             GLint xoffset, GLint yoffset,
                                             GLint zoffset, GLint x,
                                             GLint y, GLsizei width,
                                             GLsizei height );


/* 1.2 imaging extension functions */

GLAPI void (* glHistogram ) ( GLenum target, GLsizei width,
				   GLenum internalformat, GLboolean sink );

GLAPI void (* glResetHistogram ) ( GLenum target );

GLAPI void (* glGetHistogram ) ( GLenum target, GLboolean reset,
				      GLenum format, GLenum type,
				      GLvoid *values );

GLAPI void (* glGetHistogramParameterfv ) ( GLenum target, GLenum pname,
						 GLfloat *params );

GLAPI void (* glGetHistogramParameteriv ) ( GLenum target, GLenum pname,
						 GLint *params );

GLAPI void (* glMinmax ) ( GLenum target, GLenum internalformat,
				GLboolean sink );

GLAPI void (* glResetMinmax ) ( GLenum target );

GLAPI void (* glGetMinmaxParameterfv ) ( GLenum target, GLenum pname,
					      GLfloat *params );

GLAPI void (* glGetMinmaxParameteriv ) ( GLenum target, GLenum pname,
					      GLint *params );

GLAPI void (* glConvolutionFilter1D ) ( GLenum target,
	GLenum internalformat, GLsizei width, GLenum format, GLenum type,
	const GLvoid *image );

GLAPI void (* glConvolutionFilter2D ) ( GLenum target,
	GLenum internalformat, GLsizei width, GLsizei height, GLenum format,
	GLenum type, const GLvoid *image );

GLAPI void (* glConvolutionParameterf ) ( GLenum target, GLenum pname,
	GLfloat params );

GLAPI void (* glConvolutionParameterfv ) ( GLenum target, GLenum pname,
	const GLfloat *params );

GLAPI void (* glConvolutionParameteri ) ( GLenum target, GLenum pname,
	GLint params );

GLAPI void (* glConvolutionParameteriv ) ( GLenum target, GLenum pname,
	const GLint *params );

GLAPI void (* glCopyConvolutionFilter1D ) ( GLenum target,
	GLenum internalformat, GLint x, GLint y, GLsizei width );

GLAPI void (* glCopyConvolutionFilter2D ) ( GLenum target,
	GLenum internalformat, GLint x, GLint y, GLsizei width,
	GLsizei height);

GLAPI void (* glGetConvolutionFilter ) ( GLenum target, GLenum format,
	GLenum type, GLvoid *image );

GLAPI void (* glGetConvolutionParameterfv ) ( GLenum target, GLenum pname,
	GLfloat *params );

GLAPI void (* glGetConvolutionParameteriv ) ( GLenum target, GLenum pname,
	GLint *params );

GLAPI void (* glSeparableFilter2D ) ( GLenum target,
	GLenum internalformat, GLsizei width, GLsizei height, GLenum format,
	GLenum type, const GLvoid *row, const GLvoid *column );

GLAPI void (* glGetSeparableFilter ) ( GLenum target, GLenum format,
	GLenum type, GLvoid *row, GLvoid *column, GLvoid *span );

GLAPI void (* glCopyColorSubTable ) ( GLenum target, GLsizei start,
	GLint x, GLint y, GLsizei width );

GLAPI void (* glCopyColorTable ) ( GLenum target, GLenum internalformat,
	GLint x, GLint y, GLsizei width );

#endif
#endif
