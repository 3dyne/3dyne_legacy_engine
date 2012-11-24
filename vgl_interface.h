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



// vgl_interface.h
// vgl proc pointers and link list



#ifndef __vgl_interface_h
#define __vgl_interface_h

#ifdef win32_x86
  #include <Windows.h>
#endif
#include <GL/gl.h>




#include "gl_defs.h"

typedef struct
{
	const char	*name;

	void	**proc;
} vgl_field_t;

#ifdef vgl_notextern
// #warning vgl_notextern
#undef GLAPI
#define GLAPI
#else
#undef GLAPI
#define GLAPI extern
#endif


#if( !defined( win32_x86 ))
#define APIENTRY
#endif

/*
 * Miscellaneous
 */

typedef void ( APIENTRY * vglClearIndex_t ) ( GLfloat c );

typedef void ( APIENTRY * vglClearColor_t ) ( GLclampf red,
			  GLclampf green,
			  GLclampf blue,
			  GLclampf alpha );

typedef void ( APIENTRY * vglClear_t ) ( GLbitfield mask );

typedef void ( APIENTRY * vglIndexMask_t ) ( GLuint mask );

typedef void ( APIENTRY * vglColorMask_t ) ( GLboolean red, GLboolean green,
			 GLboolean blue, GLboolean alpha );

typedef void ( APIENTRY * vglAlphaFunc_t ) ( GLenum func, GLclampf ref );

typedef void ( APIENTRY * vglBlendFunc_t ) ( GLenum sfactor, GLenum dfactor );

typedef void ( APIENTRY * vglLogicOp_t ) ( GLenum opcode );

typedef void ( APIENTRY * vglCullFace_t ) ( GLenum mode );

typedef void ( APIENTRY * vglFrontFace_t ) ( GLenum mode );

typedef void ( APIENTRY * vglPointSize_t ) ( GLfloat size );

typedef void ( APIENTRY * vglLineWidth_t ) ( GLfloat width );

typedef void ( APIENTRY * vglLineStipple_t ) ( GLint factor, GLushort pattern );

typedef void ( APIENTRY * vglPolygonMode_t ) ( GLenum face, GLenum mode );

typedef void ( APIENTRY * vglPolygonOffset_t ) ( GLfloat factor, GLfloat units );

typedef void ( APIENTRY * vglPolygonStipple_t ) ( const GLubyte *mask );

typedef void ( APIENTRY * vglGetPolygonStipple_t ) ( GLubyte *mask );

typedef void ( APIENTRY * vglEdgeFlag_t ) ( GLboolean flag );

typedef void ( APIENTRY * vglEdgeFlagv_t ) ( const GLboolean *flag );

typedef void ( APIENTRY * vglScissor_t ) ( GLint x, GLint y,
                                   GLsizei width, GLsizei height);

typedef void ( APIENTRY * vglClipPlane_t ) ( GLenum plane, const GLdouble *equation );

typedef void ( APIENTRY * vglGetClipPlane_t ) ( GLenum plane, GLdouble *equation );

typedef void ( APIENTRY * vglDrawBuffer_t ) ( GLenum mode );

typedef void ( APIENTRY * vglReadBuffer_t ) ( GLenum mode );

typedef void ( APIENTRY * vglEnable_t ) ( GLenum cap );

typedef void ( APIENTRY * vglDisable_t ) ( GLenum cap );

typedef GLboolean ( APIENTRY * vglIsEnabled_t ) ( GLenum cap );


typedef void ( APIENTRY * vglEnableClientState_t ) ( GLenum cap );  /* v1.1 */

typedef void ( APIENTRY * vglDisableClientState_t ) ( GLenum cap );  /* v1.1 */


typedef void ( APIENTRY * vglGetBooleanv_t ) ( GLenum pname, GLboolean *params );

typedef void ( APIENTRY * vglGetDoublev_t ) ( GLenum pname, GLdouble *params );

typedef void ( APIENTRY * vglGetFloatv_t ) ( GLenum pname, GLfloat *params );

typedef void ( APIENTRY * vglGetIntegerv_t ) ( GLenum pname, GLint *params );


typedef void ( APIENTRY * vglPushAttrib_t ) ( GLbitfield mask );

typedef void ( APIENTRY * vglPopAttrib_t ) ( void );


typedef void ( APIENTRY * vglPushClientAttrib_t ) ( GLbitfield mask );  /* v1.1 */

typedef void ( APIENTRY * vglPopClientAttrib_t ) ( void );  /* v1.1 */


typedef GLint ( APIENTRY * vglRenderMode_t ) ( GLenum mode );

typedef GLenum ( APIENTRY * vglGetError_t ) ( void );

typedef const GLubyte* ( APIENTRY * vglGetString_t ) ( GLenum name );

typedef void ( APIENTRY * vglFinish_t ) ( void );

typedef void ( APIENTRY * vglFlush_t ) ( void );

typedef void ( APIENTRY * vglHint_t ) ( GLenum target, GLenum mode );



/*
 * Depth Buffer
 */

typedef void ( APIENTRY * vglClearDepth_t ) ( GLclampd depth );

typedef void ( APIENTRY * vglDepthFunc_t ) ( GLenum func );

typedef void ( APIENTRY * vglDepthMask_t ) ( GLboolean flag );

typedef void ( APIENTRY * vglDepthRange_t ) ( GLclampd near_val, GLclampd far_val );


/*
 * Accumulation Buffer
 */

typedef void ( APIENTRY * vglClearAccum_t ) ( GLfloat red, GLfloat green,
                                      GLfloat blue, GLfloat alpha );

typedef void ( APIENTRY * vglAccum_t ) ( GLenum op, GLfloat value );



/*
 * Transformation
 */

typedef void ( APIENTRY * vglMatrixMode_t ) ( GLenum mode );

typedef void ( APIENTRY * vglOrtho_t ) ( GLdouble left, GLdouble right,
                                 GLdouble bottom, GLdouble top,
                                 GLdouble near_val, GLdouble far_val );

typedef void ( APIENTRY * vglFrustum_t ) ( GLdouble left, GLdouble right,
                                   GLdouble bottom, GLdouble top,
                                   GLdouble near_val, GLdouble far_val );

typedef void ( APIENTRY * vglViewport_t ) ( GLint x, GLint y,
                                    GLsizei width, GLsizei height );

typedef void ( APIENTRY * vglPushMatrix_t ) ( void );

typedef void ( APIENTRY * vglPopMatrix_t ) ( void );

typedef void ( APIENTRY * vglLoadIdentity_t ) ( void );

typedef void ( APIENTRY * vglLoadMatrixd_t ) ( const GLdouble *m );
typedef void ( APIENTRY * vglLoadMatrixf_t ) ( const GLfloat *m );

typedef void ( APIENTRY * vglMultMatrixd_t ) ( const GLdouble *m );
typedef void ( APIENTRY * vglMultMatrixf_t ) ( const GLfloat *m );

typedef void ( APIENTRY * vglRotated_t ) ( GLdouble angle,
                                   GLdouble x, GLdouble y, GLdouble z );
typedef void ( APIENTRY * vglRotatef_t ) ( GLfloat angle,
                                   GLfloat x, GLfloat y, GLfloat z );

typedef void ( APIENTRY * vglScaled_t ) ( GLdouble x, GLdouble y, GLdouble z );
typedef void ( APIENTRY * vglScalef_t ) ( GLfloat x, GLfloat y, GLfloat z );

typedef void ( APIENTRY * vglTranslated_t ) ( GLdouble x, GLdouble y, GLdouble z );
typedef void ( APIENTRY * vglTranslatef_t ) ( GLfloat x, GLfloat y, GLfloat z );



/*
 * Display Lists
 */

typedef GLboolean ( APIENTRY * vglIsList_t ) ( GLuint list );

typedef void ( APIENTRY * vglDeleteLists_t ) ( GLuint list, GLsizei range );

typedef GLuint ( APIENTRY * vglGenLists_t ) ( GLsizei range );

typedef void ( APIENTRY * vglNewList_t ) ( GLuint list, GLenum mode );

typedef void ( APIENTRY * vglEndList_t ) ( void );

typedef void ( APIENTRY * vglCallList_t ) ( GLuint list );

typedef void ( APIENTRY * vglCallLists_t ) ( GLsizei n, GLenum type,
                                     const GLvoid *lists );

typedef void ( APIENTRY * vglListBase_t ) ( GLuint base );



/*
 * Drawing Functions
 */

typedef void ( APIENTRY * vglBegin_t ) ( GLenum mode );

typedef void ( APIENTRY * vglEnd_t ) ( void );


typedef void ( APIENTRY * vglVertex2d_t ) ( GLdouble x, GLdouble y );
typedef void ( APIENTRY * vglVertex2f_t ) ( GLfloat x, GLfloat y );
typedef void ( APIENTRY * vglVertex2i_t ) ( GLint x, GLint y );
typedef void ( APIENTRY * vglVertex2s_t ) ( GLshort x, GLshort y );

typedef void ( APIENTRY * vglVertex3d_t ) ( GLdouble x, GLdouble y, GLdouble z );
typedef void ( APIENTRY * vglVertex3f_t ) ( GLfloat x, GLfloat y, GLfloat z );
typedef void ( APIENTRY * vglVertex3i_t ) ( GLint x, GLint y, GLint z );
typedef void ( APIENTRY * vglVertex3s_t ) ( GLshort x, GLshort y, GLshort z );

typedef void ( APIENTRY * vglVertex4d_t ) ( GLdouble x, GLdouble y, GLdouble z, GLdouble w );
typedef void ( APIENTRY * vglVertex4f_t ) ( GLfloat x, GLfloat y, GLfloat z, GLfloat w );
typedef void ( APIENTRY * vglVertex4i_t ) ( GLint x, GLint y, GLint z, GLint w );
typedef void ( APIENTRY * vglVertex4s_t ) ( GLshort x, GLshort y, GLshort z, GLshort w );

typedef void ( APIENTRY * vglVertex2dv_t ) ( const GLdouble *v );
typedef void ( APIENTRY * vglVertex2fv_t ) ( const GLfloat *v );
typedef void ( APIENTRY * vglVertex2iv_t ) ( const GLint *v );
typedef void ( APIENTRY * vglVertex2sv_t ) ( const GLshort *v );

typedef void ( APIENTRY * vglVertex3dv_t ) ( const GLdouble *v );
typedef void ( APIENTRY * vglVertex3fv_t ) ( const GLfloat *v );
typedef void ( APIENTRY * vglVertex3iv_t ) ( const GLint *v );
typedef void ( APIENTRY * vglVertex3sv_t ) ( const GLshort *v );

typedef void ( APIENTRY * vglVertex4dv_t ) ( const GLdouble *v );
typedef void ( APIENTRY * vglVertex4fv_t ) ( const GLfloat *v );
typedef void ( APIENTRY * vglVertex4iv_t ) ( const GLint *v );
typedef void ( APIENTRY * vglVertex4sv_t ) ( const GLshort *v );


typedef void ( APIENTRY * vglNormal3b_t ) ( GLbyte nx, GLbyte ny, GLbyte nz );
typedef void ( APIENTRY * vglNormal3d_t ) ( GLdouble nx, GLdouble ny, GLdouble nz );
typedef void ( APIENTRY * vglNormal3f_t ) ( GLfloat nx, GLfloat ny, GLfloat nz );
typedef void ( APIENTRY * vglNormal3i_t ) ( GLint nx, GLint ny, GLint nz );
typedef void ( APIENTRY * vglNormal3s_t ) ( GLshort nx, GLshort ny, GLshort nz );

typedef void ( APIENTRY * vglNormal3bv_t ) ( const GLbyte *v );
typedef void ( APIENTRY * vglNormal3dv_t ) ( const GLdouble *v );
typedef void ( APIENTRY * vglNormal3fv_t ) ( const GLfloat *v );
typedef void ( APIENTRY * vglNormal3iv_t ) ( const GLint *v );
typedef void ( APIENTRY * vglNormal3sv_t ) ( const GLshort *v );


typedef void ( APIENTRY * vglIndexd_t ) ( GLdouble c );
typedef void ( APIENTRY * vglIndexf_t ) ( GLfloat c );
typedef void ( APIENTRY * vglIndexi_t ) ( GLint c );
typedef void ( APIENTRY * vglIndexs_t ) ( GLshort c );
typedef void ( APIENTRY * vglIndexub_t ) ( GLubyte c );  /* 1.1 */

typedef void ( APIENTRY * vglIndexdv_t ) ( const GLdouble *c );
typedef void ( APIENTRY * vglIndexfv_t ) ( const GLfloat *c );
typedef void ( APIENTRY * vglIndexiv_t ) ( const GLint *c );
typedef void ( APIENTRY * vglIndexsv_t ) ( const GLshort *c );
typedef void ( APIENTRY * vglIndexubv_t ) ( const GLubyte *c );  /* 1.1 */

typedef void ( APIENTRY * vglColor3b_t ) ( GLbyte red, GLbyte green, GLbyte blue );
typedef void ( APIENTRY * vglColor3d_t ) ( GLdouble red, GLdouble green, GLdouble blue );
typedef void ( APIENTRY * vglColor3f_t ) ( GLfloat red, GLfloat green, GLfloat blue );
typedef void ( APIENTRY * vglColor3i_t ) ( GLint red, GLint green, GLint blue );
typedef void ( APIENTRY * vglColor3s_t ) ( GLshort red, GLshort green, GLshort blue );
typedef void ( APIENTRY * vglColor3ub_t ) ( GLubyte red, GLubyte green, GLubyte blue );
typedef void ( APIENTRY * vglColor3ui_t ) ( GLuint red, GLuint green, GLuint blue );
typedef void ( APIENTRY * vglColor3us_t ) ( GLushort red, GLushort green, GLushort blue );

typedef void ( APIENTRY * vglColor4b_t ) ( GLbyte red, GLbyte green,
                                   GLbyte blue, GLbyte alpha );
typedef void ( APIENTRY * vglColor4d_t ) ( GLdouble red, GLdouble green,
                                   GLdouble blue, GLdouble alpha );
typedef void ( APIENTRY * vglColor4f_t ) ( GLfloat red, GLfloat green,
                                   GLfloat blue, GLfloat alpha );
typedef void ( APIENTRY * vglColor4i_t ) ( GLint red, GLint green,
                                   GLint blue, GLint alpha );
typedef void ( APIENTRY * vglColor4s_t ) ( GLshort red, GLshort green,
                                   GLshort blue, GLshort alpha );
typedef void ( APIENTRY * vglColor4ub_t ) ( GLubyte red, GLubyte green,
                                    GLubyte blue, GLubyte alpha );
typedef void ( APIENTRY * vglColor4ui_t ) ( GLuint red, GLuint green,
                                    GLuint blue, GLuint alpha );
typedef void ( APIENTRY * vglColor4us_t ) ( GLushort red, GLushort green,
                                    GLushort blue, GLushort alpha );


typedef void ( APIENTRY * vglColor3bv_t ) ( const GLbyte *v );
typedef void ( APIENTRY * vglColor3dv_t ) ( const GLdouble *v );
typedef void ( APIENTRY * vglColor3fv_t ) ( const GLfloat *v );
typedef void ( APIENTRY * vglColor3iv_t ) ( const GLint *v );
typedef void ( APIENTRY * vglColor3sv_t ) ( const GLshort *v );
typedef void ( APIENTRY * vglColor3ubv_t ) ( const GLubyte *v );
typedef void ( APIENTRY * vglColor3uiv_t ) ( const GLuint *v );
typedef void ( APIENTRY * vglColor3usv_t ) ( const GLushort *v );

typedef void ( APIENTRY * vglColor4bv_t ) ( const GLbyte *v );
typedef void ( APIENTRY * vglColor4dv_t ) ( const GLdouble *v );
typedef void ( APIENTRY * vglColor4fv_t ) ( const GLfloat *v );
typedef void ( APIENTRY * vglColor4iv_t ) ( const GLint *v );
typedef void ( APIENTRY * vglColor4sv_t ) ( const GLshort *v );
typedef void ( APIENTRY * vglColor4ubv_t ) ( const GLubyte *v );
typedef void ( APIENTRY * vglColor4uiv_t ) ( const GLuint *v );
typedef void ( APIENTRY * vglColor4usv_t ) ( const GLushort *v );


typedef void ( APIENTRY * vglTexCoord1d_t ) ( GLdouble s );
typedef void ( APIENTRY * vglTexCoord1f_t ) ( GLfloat s );
typedef void ( APIENTRY * vglTexCoord1i_t ) ( GLint s );
typedef void ( APIENTRY * vglTexCoord1s_t ) ( GLshort s );

typedef void ( APIENTRY * vglTexCoord2d_t ) ( GLdouble s, GLdouble t );
typedef void ( APIENTRY * vglTexCoord2f_t ) ( GLfloat s, GLfloat t );
typedef void ( APIENTRY * vglTexCoord2i_t ) ( GLint s, GLint t );
typedef void ( APIENTRY * vglTexCoord2s_t ) ( GLshort s, GLshort t );

typedef void ( APIENTRY * vglTexCoord3d_t ) ( GLdouble s, GLdouble t, GLdouble r );
typedef void ( APIENTRY * vglTexCoord3f_t ) ( GLfloat s, GLfloat t, GLfloat r );
typedef void ( APIENTRY * vglTexCoord3i_t ) ( GLint s, GLint t, GLint r );
typedef void ( APIENTRY * vglTexCoord3s_t ) ( GLshort s, GLshort t, GLshort r );

typedef void ( APIENTRY * vglTexCoord4d_t ) ( GLdouble s, GLdouble t, GLdouble r, GLdouble q );
typedef void ( APIENTRY * vglTexCoord4f_t ) ( GLfloat s, GLfloat t, GLfloat r, GLfloat q );
typedef void ( APIENTRY * vglTexCoord4i_t ) ( GLint s, GLint t, GLint r, GLint q );
typedef void ( APIENTRY * vglTexCoord4s_t ) ( GLshort s, GLshort t, GLshort r, GLshort q );

typedef void ( APIENTRY * vglTexCoord1dv_t ) ( const GLdouble *v );
typedef void ( APIENTRY * vglTexCoord1fv_t ) ( const GLfloat *v );
typedef void ( APIENTRY * vglTexCoord1iv_t ) ( const GLint *v );
typedef void ( APIENTRY * vglTexCoord1sv_t ) ( const GLshort *v );

typedef void ( APIENTRY * vglTexCoord2dv_t ) ( const GLdouble *v );
typedef void ( APIENTRY * vglTexCoord2fv_t ) ( const GLfloat *v );
typedef void ( APIENTRY * vglTexCoord2iv_t ) ( const GLint *v );
typedef void ( APIENTRY * vglTexCoord2sv_t ) ( const GLshort *v );

typedef void ( APIENTRY * vglTexCoord3dv_t ) ( const GLdouble *v );
typedef void ( APIENTRY * vglTexCoord3fv_t ) ( const GLfloat *v );
typedef void ( APIENTRY * vglTexCoord3iv_t ) ( const GLint *v );
typedef void ( APIENTRY * vglTexCoord3sv_t ) ( const GLshort *v );

typedef void ( APIENTRY * vglTexCoord4dv_t ) ( const GLdouble *v );
typedef void ( APIENTRY * vglTexCoord4fv_t ) ( const GLfloat *v );
typedef void ( APIENTRY * vglTexCoord4iv_t ) ( const GLint *v );
typedef void ( APIENTRY * vglTexCoord4sv_t ) ( const GLshort *v );


typedef void ( APIENTRY * vglRasterPos2d_t ) ( GLdouble x, GLdouble y );
typedef void ( APIENTRY * vglRasterPos2f_t ) ( GLfloat x, GLfloat y );
typedef void ( APIENTRY * vglRasterPos2i_t ) ( GLint x, GLint y );
typedef void ( APIENTRY * vglRasterPos2s_t ) ( GLshort x, GLshort y );

typedef void ( APIENTRY * vglRasterPos3d_t ) ( GLdouble x, GLdouble y, GLdouble z );
typedef void ( APIENTRY * vglRasterPos3f_t ) ( GLfloat x, GLfloat y, GLfloat z );
typedef void ( APIENTRY * vglRasterPos3i_t ) ( GLint x, GLint y, GLint z );
typedef void ( APIENTRY * vglRasterPos3s_t ) ( GLshort x, GLshort y, GLshort z );

typedef void ( APIENTRY * vglRasterPos4d_t ) ( GLdouble x, GLdouble y, GLdouble z, GLdouble w );
typedef void ( APIENTRY * vglRasterPos4f_t ) ( GLfloat x, GLfloat y, GLfloat z, GLfloat w );
typedef void ( APIENTRY * vglRasterPos4i_t ) ( GLint x, GLint y, GLint z, GLint w );
typedef void ( APIENTRY * vglRasterPos4s_t ) ( GLshort x, GLshort y, GLshort z, GLshort w );

typedef void ( APIENTRY * vglRasterPos2dv_t ) ( const GLdouble *v );
typedef void ( APIENTRY * vglRasterPos2fv_t ) ( const GLfloat *v );
typedef void ( APIENTRY * vglRasterPos2iv_t ) ( const GLint *v );
typedef void ( APIENTRY * vglRasterPos2sv_t ) ( const GLshort *v );

typedef void ( APIENTRY * vglRasterPos3dv_t ) ( const GLdouble *v );
typedef void ( APIENTRY * vglRasterPos3fv_t ) ( const GLfloat *v );
typedef void ( APIENTRY * vglRasterPos3iv_t ) ( const GLint *v );
typedef void ( APIENTRY * vglRasterPos3sv_t ) ( const GLshort *v );

typedef void ( APIENTRY * vglRasterPos4dv_t ) ( const GLdouble *v );
typedef void ( APIENTRY * vglRasterPos4fv_t ) ( const GLfloat *v );
typedef void ( APIENTRY * vglRasterPos4iv_t ) ( const GLint *v );
typedef void ( APIENTRY * vglRasterPos4sv_t ) ( const GLshort *v );


typedef void ( APIENTRY * vglRectd_t ) ( GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2 );
typedef void ( APIENTRY * vglRectf_t ) ( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 );
typedef void ( APIENTRY * vglRecti_t ) ( GLint x1, GLint y1, GLint x2, GLint y2 );
typedef void ( APIENTRY * vglRects_t ) ( GLshort x1, GLshort y1, GLshort x2, GLshort y2 );


typedef void ( APIENTRY * vglRectdv_t ) ( const GLdouble *v1, const GLdouble *v2 );
typedef void ( APIENTRY * vglRectfv_t ) ( const GLfloat *v1, const GLfloat *v2 );
typedef void ( APIENTRY * vglRectiv_t ) ( const GLint *v1, const GLint *v2 );
typedef void ( APIENTRY * vglRectsv_t ) ( const GLshort *v1, const GLshort *v2 );



/*
 * Vertex Arrays  _t ) (1.1)
 */

typedef void ( APIENTRY * vglVertexPointer_t ) ( GLint size, GLenum type,
                                         GLsizei stride, const GLvoid *ptr );

typedef void ( APIENTRY * vglNormalPointer_t ) ( GLenum type, GLsizei stride,
                                         const GLvoid *ptr );

typedef void ( APIENTRY * vglColorPointer_t ) ( GLint size, GLenum type,
                                        GLsizei stride, const GLvoid *ptr );

typedef void ( APIENTRY * vglIndexPointer_t ) ( GLenum type, GLsizei stride,
                                        const GLvoid *ptr );

typedef void ( APIENTRY * vglTexCoordPointer_t ) ( GLint size, GLenum type,
                                           GLsizei stride, const GLvoid *ptr );

typedef void ( APIENTRY * vglEdgeFlagPointer_t ) ( GLsizei stride, const GLvoid *ptr );

typedef void ( APIENTRY * vglGetPointerv_t ) ( GLenum pname, void **params );

typedef void ( APIENTRY * vglArrayElement_t ) ( GLint i );

typedef void ( APIENTRY * vglDrawArrays_t ) ( GLenum mode, GLint first,
                                      GLsizei count );

typedef void ( APIENTRY * vglDrawElements_t ) ( GLenum mode, GLsizei count,
                                        GLenum type, const GLvoid *indices );

typedef void ( APIENTRY * vglInterleavedArrays_t ) ( GLenum format, GLsizei stride,
                                             const GLvoid *pointer );


/*
 * Lighting
 */

typedef void ( APIENTRY * vglShadeModel_t ) ( GLenum mode );

typedef void ( APIENTRY * vglLightf_t ) ( GLenum light, GLenum pname, GLfloat param );
typedef void ( APIENTRY * vglLighti_t ) ( GLenum light, GLenum pname, GLint param );
typedef void ( APIENTRY * vglLightfv_t ) ( GLenum light, GLenum pname,
                                   const GLfloat *params );
typedef void ( APIENTRY * vglLightiv_t ) ( GLenum light, GLenum pname,
                                   const GLint *params );

typedef void ( APIENTRY * vglGetLightfv_t ) ( GLenum light, GLenum pname,
                                      GLfloat *params );
typedef void ( APIENTRY * vglGetLightiv_t ) ( GLenum light, GLenum pname,
                                      GLint *params );

typedef void ( APIENTRY * vglLightModelf_t ) ( GLenum pname, GLfloat param );
typedef void ( APIENTRY * vglLightModeli_t ) ( GLenum pname, GLint param );
typedef void ( APIENTRY * vglLightModelfv_t ) ( GLenum pname, const GLfloat *params );
typedef void ( APIENTRY * vglLightModeliv_t ) ( GLenum pname, const GLint *params );

typedef void ( APIENTRY * vglMaterialf_t ) ( GLenum face, GLenum pname, GLfloat param );
typedef void ( APIENTRY * vglMateriali_t ) ( GLenum face, GLenum pname, GLint param );
typedef void ( APIENTRY * vglMaterialfv_t ) ( GLenum face, GLenum pname, const GLfloat *params );
typedef void ( APIENTRY * vglMaterialiv_t ) ( GLenum face, GLenum pname, const GLint *params );

typedef void ( APIENTRY * vglGetMaterialfv_t ) ( GLenum face, GLenum pname, GLfloat *params );
typedef void ( APIENTRY * vglGetMaterialiv_t ) ( GLenum face, GLenum pname, GLint *params );

typedef void ( APIENTRY * vglColorMaterial_t ) ( GLenum face, GLenum mode );




/*
 * Raster functions
 */

typedef void ( APIENTRY * vglPixelZoom_t ) ( GLfloat xfactor, GLfloat yfactor );

typedef void ( APIENTRY * vglPixelStoref_t ) ( GLenum pname, GLfloat param );
typedef void ( APIENTRY * vglPixelStorei_t ) ( GLenum pname, GLint param );

typedef void ( APIENTRY * vglPixelTransferf_t ) ( GLenum pname, GLfloat param );
typedef void ( APIENTRY * vglPixelTransferi_t ) ( GLenum pname, GLint param );

typedef void ( APIENTRY * vglPixelMapfv_t ) ( GLenum map, GLint mapsize,
                                      const GLfloat *values );
typedef void ( APIENTRY * vglPixelMapuiv_t ) ( GLenum map, GLint mapsize,
                                       const GLuint *values );
typedef void ( APIENTRY * vglPixelMapusv_t ) ( GLenum map, GLint mapsize,
                                       const GLushort *values );

typedef void ( APIENTRY * vglGetPixelMapfv_t ) ( GLenum map, GLfloat *values );
typedef void ( APIENTRY * vglGetPixelMapuiv_t ) ( GLenum map, GLuint *values );
typedef void ( APIENTRY * vglGetPixelMapusv_t ) ( GLenum map, GLushort *values );

typedef void ( APIENTRY * vglBitmap_t ) ( GLsizei width, GLsizei height,
                                  GLfloat xorig, GLfloat yorig,
                                  GLfloat xmove, GLfloat ymove,
                                  const GLubyte *bitmap );

typedef void ( APIENTRY * vglReadPixels_t ) ( GLint x, GLint y,
                                      GLsizei width, GLsizei height,
                                      GLenum format, GLenum type,
                                      GLvoid *pixels );

typedef void ( APIENTRY * vglDrawPixels_t ) ( GLsizei width, GLsizei height,
                                      GLenum format, GLenum type,
                                      const GLvoid *pixels );

typedef void ( APIENTRY * vglCopyPixels_t ) ( GLint x, GLint y,
                                      GLsizei width, GLsizei height,
                                      GLenum type );



/*
 * Stenciling
 */

typedef void ( APIENTRY * vglStencilFunc_t ) ( GLenum func, GLint ref, GLuint mask );

typedef void ( APIENTRY * vglStencilMask_t ) ( GLuint mask );

typedef void ( APIENTRY * vglStencilOp_t ) ( GLenum fail, GLenum zfail, GLenum zpass );

typedef void ( APIENTRY * vglClearStencil_t ) ( GLint s );



/*
 * Texture mapping
 */

typedef void ( APIENTRY * vglTexGend_t ) ( GLenum coord, GLenum pname, GLdouble param );
typedef void ( APIENTRY * vglTexGenf_t ) ( GLenum coord, GLenum pname, GLfloat param );
typedef void ( APIENTRY * vglTexGeni_t ) ( GLenum coord, GLenum pname, GLint param );

typedef void ( APIENTRY * vglTexGendv_t ) ( GLenum coord, GLenum pname, const GLdouble *params );
typedef void ( APIENTRY * vglTexGenfv_t ) ( GLenum coord, GLenum pname, const GLfloat *params );
typedef void ( APIENTRY * vglTexGeniv_t ) ( GLenum coord, GLenum pname, const GLint *params );

typedef void ( APIENTRY * vglGetTexGendv_t ) ( GLenum coord, GLenum pname, GLdouble *params );
typedef void ( APIENTRY * vglGetTexGenfv_t ) ( GLenum coord, GLenum pname, GLfloat *params );
typedef void ( APIENTRY * vglGetTexGeniv_t ) ( GLenum coord, GLenum pname, GLint *params );


typedef void ( APIENTRY * vglTexEnvf_t ) ( GLenum target, GLenum pname, GLfloat param );
typedef void ( APIENTRY * vglTexEnvi_t ) ( GLenum target, GLenum pname, GLint param );

typedef void ( APIENTRY * vglTexEnvfv_t ) ( GLenum target, GLenum pname, const GLfloat *params );
typedef void ( APIENTRY * vglTexEnviv_t ) ( GLenum target, GLenum pname, const GLint *params );

typedef void ( APIENTRY * vglGetTexEnvfv_t ) ( GLenum target, GLenum pname, GLfloat *params );
typedef void ( APIENTRY * vglGetTexEnviv_t ) ( GLenum target, GLenum pname, GLint *params );


typedef void ( APIENTRY * vglTexParameterf_t ) ( GLenum target, GLenum pname, GLfloat param );
typedef void ( APIENTRY * vglTexParameteri_t ) ( GLenum target, GLenum pname, GLint param );

typedef void ( APIENTRY * vglTexParameterfv_t ) ( GLenum target, GLenum pname,
                                          const GLfloat *params );
typedef void ( APIENTRY * vglTexParameteriv_t ) ( GLenum target, GLenum pname,
                                          const GLint *params );

typedef void ( APIENTRY * vglGetTexParameterfv_t ) ( GLenum target,
                                             GLenum pname, GLfloat *params);
typedef void ( APIENTRY * vglGetTexParameteriv_t ) ( GLenum target,
                                             GLenum pname, GLint *params );

typedef void ( APIENTRY * vglGetTexLevelParameterfv_t ) ( GLenum target, GLint level,
                                                  GLenum pname, GLfloat *params );
typedef void ( APIENTRY * vglGetTexLevelParameteriv_t ) ( GLenum target, GLint level,
                                                  GLenum pname, GLint *params );


typedef void ( APIENTRY * vglTexImage1D_t ) ( GLenum target, GLint level,
                                      GLint internalFormat,
                                      GLsizei width, GLint border,
                                      GLenum format, GLenum type,
                                      const GLvoid *pixels );

typedef void ( APIENTRY * vglTexImage2D_t ) ( GLenum target, GLint level,
                                      GLint internalFormat,
                                      GLsizei width, GLsizei height,
                                      GLint border, GLenum format, GLenum type,
                                      const GLvoid *pixels );

typedef void ( APIENTRY * vglGetTexImage_t ) ( GLenum target, GLint level,
                                       GLenum format, GLenum type,
                                       GLvoid *pixels );



/* 1.1 functions */

typedef void ( APIENTRY * vglGenTextures_t ) ( GLsizei n, GLuint *textures );

typedef void ( APIENTRY * vglDeleteTextures_t ) ( GLsizei n, const GLuint *textures);

typedef void ( APIENTRY * vglBindTexture_t ) ( GLenum target, GLuint texture );

typedef void ( APIENTRY * vglPrioritizeTextures_t ) ( GLsizei n,
                                              const GLuint *textures,
                                              const GLclampf *priorities );

typedef GLboolean ( APIENTRY * vglAreTexturesResident_t ) ( GLsizei n,
                                                    const GLuint *textures,
                                                    GLboolean *residences );

typedef GLboolean ( APIENTRY * vglIsTexture_t ) ( GLuint texture );


typedef void ( APIENTRY * vglTexSubImage1D_t ) ( GLenum target, GLint level,
                                         GLint xoffset,
                                         GLsizei width, GLenum format,
                                         GLenum type, const GLvoid *pixels );


typedef void ( APIENTRY * vglTexSubImage2D_t ) ( GLenum target, GLint level,
                                         GLint xoffset, GLint yoffset,
                                         GLsizei width, GLsizei height,
                                         GLenum format, GLenum type,
                                         const GLvoid *pixels );


typedef void ( APIENTRY * vglCopyTexImage1D_t ) ( GLenum target, GLint level,
                                          GLenum internalformat,
                                          GLint x, GLint y,
                                          GLsizei width, GLint border );


typedef void ( APIENTRY * vglCopyTexImage2D_t ) ( GLenum target, GLint level,
                                          GLenum internalformat,
                                          GLint x, GLint y,
                                          GLsizei width, GLsizei height,
                                          GLint border );


typedef void ( APIENTRY * vglCopyTexSubImage1D_t ) ( GLenum target, GLint level,
                                             GLint xoffset, GLint x, GLint y,
                                             GLsizei width );


typedef void ( APIENTRY * vglCopyTexSubImage2D_t ) ( GLenum target, GLint level,
                                             GLint xoffset, GLint yoffset,
                                             GLint x, GLint y,
                                             GLsizei width, GLsizei height );




/*
 * Evaluators
 */

typedef void ( APIENTRY * vglMap1d_t ) ( GLenum target, GLdouble u1, GLdouble u2,
                                 GLint stride,
                                 GLint order, const GLdouble *points );
typedef void ( APIENTRY * vglMap1f_t ) ( GLenum target, GLfloat u1, GLfloat u2,
                                 GLint stride,
                                 GLint order, const GLfloat *points );

typedef void ( APIENTRY * vglMap2d_t ) ( GLenum target,
		     GLdouble u1, GLdouble u2, GLint ustride, GLint uorder,
		     GLdouble v1, GLdouble v2, GLint vstride, GLint vorder,
		     const GLdouble *points );
typedef void ( APIENTRY * vglMap2f_t ) ( GLenum target,
		     GLfloat u1, GLfloat u2, GLint ustride, GLint uorder,
		     GLfloat v1, GLfloat v2, GLint vstride, GLint vorder,
		     const GLfloat *points );

typedef void ( APIENTRY * vglGetMapdv_t ) ( GLenum target, GLenum query, GLdouble *v );
typedef void ( APIENTRY * vglGetMapfv_t ) ( GLenum target, GLenum query, GLfloat *v );
typedef void ( APIENTRY * vglGetMapiv_t ) ( GLenum target, GLenum query, GLint *v );

typedef void ( APIENTRY * vglEvalCoord1d_t ) ( GLdouble u );
typedef void ( APIENTRY * vglEvalCoord1f_t ) ( GLfloat u );

typedef void ( APIENTRY * vglEvalCoord1dv_t ) ( const GLdouble *u );
typedef void ( APIENTRY * vglEvalCoord1fv_t ) ( const GLfloat *u );

typedef void ( APIENTRY * vglEvalCoord2d_t ) ( GLdouble u, GLdouble v );
typedef void ( APIENTRY * vglEvalCoord2f_t ) ( GLfloat u, GLfloat v );

typedef void ( APIENTRY * vglEvalCoord2dv_t ) ( const GLdouble *u );
typedef void ( APIENTRY * vglEvalCoord2fv_t ) ( const GLfloat *u );

typedef void ( APIENTRY * vglMapGrid1d_t ) ( GLint un, GLdouble u1, GLdouble u2 );
typedef void ( APIENTRY * vglMapGrid1f_t ) ( GLint un, GLfloat u1, GLfloat u2 );

typedef void ( APIENTRY * vglMapGrid2d_t ) ( GLint un, GLdouble u1, GLdouble u2,
                                     GLint vn, GLdouble v1, GLdouble v2 );
typedef void ( APIENTRY * vglMapGrid2f_t ) ( GLint un, GLfloat u1, GLfloat u2,
                                     GLint vn, GLfloat v1, GLfloat v2 );

typedef void ( APIENTRY * vglEvalPoint1_t ) ( GLint i );

typedef void ( APIENTRY * vglEvalPoint2_t ) ( GLint i, GLint j );

typedef void ( APIENTRY * vglEvalMesh1_t ) ( GLenum mode, GLint i1, GLint i2 );

typedef void ( APIENTRY * vglEvalMesh2_t ) ( GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2 );



/*
 * Fog
 */

typedef void ( APIENTRY * vglFogf_t ) ( GLenum pname, GLfloat param );

typedef void ( APIENTRY * vglFogi_t ) ( GLenum pname, GLint param );

typedef void ( APIENTRY * vglFogfv_t ) ( GLenum pname, const GLfloat *params );

typedef void ( APIENTRY * vglFogiv_t ) ( GLenum pname, const GLint *params );



/*
 * Selection and Feedback
 */

typedef void ( APIENTRY * vglFeedbackBuffer_t ) ( GLsizei size, GLenum type, GLfloat *buffer );

typedef void ( APIENTRY * vglPassThrough_t ) ( GLfloat token );

typedef void ( APIENTRY * vglSelectBuffer_t ) ( GLsizei size, GLuint *buffer );

typedef void ( APIENTRY * vglInitNames_t ) ( void );

typedef void ( APIENTRY * vglLoadName_t ) ( GLuint name );

typedef void ( APIENTRY * vglPushName_t ) ( GLuint name );

typedef void ( APIENTRY * vglPopName_t ) ( void );





typedef void ( APIENTRY * vglActiveTextureARB_t ) (GLenum texture);
typedef void ( APIENTRY * vglClientActiveTextureARB_t ) (GLenum texture);
typedef void ( APIENTRY * vglMultiTexCoord1dARB_t ) (GLenum target, GLdouble s);
typedef void ( APIENTRY * vglMultiTexCoord1dvARB_t ) (GLenum target, const GLdouble *v);
typedef void ( APIENTRY * vglMultiTexCoord1fARB_t ) (GLenum target, GLfloat s);
typedef void ( APIENTRY * vglMultiTexCoord1fvARB_t ) (GLenum target, const GLfloat *v);
typedef void ( APIENTRY * vglMultiTexCoord1iARB_t ) (GLenum target, GLint s);
typedef void ( APIENTRY * vglMultiTexCoord1ivARB_t ) (GLenum target, const GLint *v);
typedef void ( APIENTRY * vglMultiTexCoord1sARB_t ) (GLenum target, GLshort s);
typedef void ( APIENTRY * vglMultiTexCoord1svARB_t ) (GLenum target, const GLshort *v);
typedef void ( APIENTRY * vglMultiTexCoord2dARB_t ) (GLenum target, GLdouble s, GLdouble t);
typedef void ( APIENTRY * vglMultiTexCoord2dvARB_t ) (GLenum target, const GLdouble *v);
typedef void ( APIENTRY * vglMultiTexCoord2fARB_t ) (GLenum target, GLfloat s, GLfloat t);
typedef void ( APIENTRY * vglMultiTexCoord2fvARB_t ) (GLenum target, const GLfloat *v);
typedef void ( APIENTRY * vglMultiTexCoord2iARB_t ) (GLenum target, GLint s, GLint t);
typedef void ( APIENTRY * vglMultiTexCoord2ivARB_t ) (GLenum target, const GLint *v);
typedef void ( APIENTRY * vglMultiTexCoord2sARB_t ) (GLenum target, GLshort s, GLshort t);
typedef void ( APIENTRY * vglMultiTexCoord2svARB_t ) (GLenum target, const GLshort *v);
typedef void ( APIENTRY * vglMultiTexCoord3dARB_t ) (GLenum target, GLdouble s, GLdouble t, GLdouble r);
typedef void ( APIENTRY * vglMultiTexCoord3dvARB_t ) (GLenum target, const GLdouble *v);
typedef void ( APIENTRY * vglMultiTexCoord3fARB_t ) (GLenum target, GLfloat s, GLfloat t, GLfloat r);
typedef void ( APIENTRY * vglMultiTexCoord3fvARB_t ) (GLenum target, const GLfloat *v);
typedef void ( APIENTRY * vglMultiTexCoord3iARB_t ) (GLenum target, GLint s, GLint t, GLint r);
typedef void ( APIENTRY * vglMultiTexCoord3ivARB_t ) (GLenum target, const GLint *v);
typedef void ( APIENTRY * vglMultiTexCoord3sARB_t ) (GLenum target, GLshort s, GLshort t, GLshort r);
typedef void ( APIENTRY * vglMultiTexCoord3svARB_t ) (GLenum target, const GLshort *v);
typedef void ( APIENTRY * vglMultiTexCoord4dARB_t ) (GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
typedef void ( APIENTRY * vglMultiTexCoord4dvARB_t ) (GLenum target, const GLdouble *v);
typedef void ( APIENTRY * vglMultiTexCoord4fARB_t ) (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
typedef void ( APIENTRY * vglMultiTexCoord4fvARB_t ) (GLenum target, const GLfloat *v);
typedef void ( APIENTRY * vglMultiTexCoord4iARB_t ) (GLenum target, GLint s, GLint t, GLint r, GLint q);
typedef void ( APIENTRY * vglMultiTexCoord4ivARB_t ) (GLenum target, const GLint *v);
typedef void ( APIENTRY * vglMultiTexCoord4sARB_t ) (GLenum target, GLshort s, GLshort t, GLshort r, GLshort q);
typedef void ( APIENTRY * vglMultiTexCoord4svARB_t ) (GLenum target, const GLshort *v);



/* GL_EXT_point_parameters */
typedef void ( APIENTRY * vglPointParameterfEXT_t ) ( GLenum pname, GLfloat param );
typedef void ( APIENTRY * vglPointParameterfvEXT_t ) ( GLenum pname,
                                               const GLfloat *params );



/* GL_INGR_blend_func_separate */
typedef void ( APIENTRY * vglBlendFuncSeparateINGR_t ) ( GLenum sfactorRGB,
                                               GLenum dfactorRGB,
                                               GLenum sfactorAlpha, 
                                               GLenum dfactorAlpha );



/* 1.2 functions */
typedef void ( APIENTRY * vglDrawRangeElements_t ) ( GLenum mode, GLuint start,
	GLuint end, GLsizei count, GLenum type, const GLvoid *indices );

typedef void ( APIENTRY * vglTexImage3D_t ) ( GLenum target, GLint level,
                                      GLint internalFormat,
                                      GLsizei width, GLsizei height,
                                      GLsizei depth, GLint border,
                                      GLenum format, GLenum type,
                                      const GLvoid *pixels );

typedef void ( APIENTRY * vglTexSubImage3D_t ) ( GLenum target, GLint level,
                                         GLint xoffset, GLint yoffset,
                                         GLint zoffset, GLsizei width,
                                         GLsizei height, GLsizei depth,
                                         GLenum format,
                                         GLenum type, const GLvoid *pixels);

typedef void ( APIENTRY * vglCopyTexSubImage3D_t ) ( GLenum target, GLint level,
                                             GLint xoffset, GLint yoffset,
                                             GLint zoffset, GLint x,
                                             GLint y, GLsizei width,
                                             GLsizei height );


/* 1.2 imaging extension functions */

typedef void ( APIENTRY * vglHistogram_t ) ( GLenum target, GLsizei width,
				   GLenum internalformat, GLboolean sink );

typedef void ( APIENTRY * vglResetHistogram_t ) ( GLenum target );

typedef void ( APIENTRY * vglGetHistogram_t ) ( GLenum target, GLboolean reset,
				      GLenum format, GLenum type,
				      GLvoid *values );

typedef void ( APIENTRY * vglGetHistogramParameterfv_t ) ( GLenum target, GLenum pname,
						 GLfloat *params );

typedef void ( APIENTRY * vglGetHistogramParameteriv_t ) ( GLenum target, GLenum pname,
						 GLint *params );

typedef void ( APIENTRY * vglMinmax_t ) ( GLenum target, GLenum internalformat,
				GLboolean sink );

typedef void ( APIENTRY * vglResetMinmax_t ) ( GLenum target );

typedef void ( APIENTRY * vglGetMinmaxParameterfv_t ) ( GLenum target, GLenum pname,
					      GLfloat *params );

typedef void ( APIENTRY * vglGetMinmaxParameteriv_t ) ( GLenum target, GLenum pname,
					      GLint *params );

typedef void ( APIENTRY * vglConvolutionFilter1D_t ) ( GLenum target,
	GLenum internalformat, GLsizei width, GLenum format, GLenum type,
	const GLvoid *image );

typedef void ( APIENTRY * vglConvolutionFilter2D_t ) ( GLenum target,
	GLenum internalformat, GLsizei width, GLsizei height, GLenum format,
	GLenum type, const GLvoid *image );

typedef void ( APIENTRY * vglConvolutionParameterf_t ) ( GLenum target, GLenum pname,
	GLfloat params );

typedef void ( APIENTRY * vglConvolutionParameterfv_t ) ( GLenum target, GLenum pname,
	const GLfloat *params );

typedef void ( APIENTRY * vglConvolutionParameteri_t ) ( GLenum target, GLenum pname,
	GLint params );

typedef void ( APIENTRY * vglConvolutionParameteriv_t ) ( GLenum target, GLenum pname,
	const GLint *params );

typedef void ( APIENTRY * vglCopyConvolutionFilter1D_t ) ( GLenum target,
	GLenum internalformat, GLint x, GLint y, GLsizei width );

typedef void ( APIENTRY * vglCopyConvolutionFilter2D_t ) ( GLenum target,
	GLenum internalformat, GLint x, GLint y, GLsizei width,
	GLsizei height);

typedef void ( APIENTRY * vglGetConvolutionFilter_t ) ( GLenum target, GLenum format,
	GLenum type, GLvoid *image );

typedef void ( APIENTRY * vglGetConvolutionParameterfv_t ) ( GLenum target, GLenum pname,
	GLfloat *params );

typedef void ( APIENTRY * vglGetConvolutionParameteriv_t ) ( GLenum target, GLenum pname,
	GLint *params );

typedef void ( APIENTRY * vglSeparableFilter2D_t ) ( GLenum target,
	GLenum internalformat, GLsizei width, GLsizei height, GLenum format,
	GLenum type, const GLvoid *row, const GLvoid *column );

typedef void ( APIENTRY * vglGetSeparableFilter_t ) ( GLenum target, GLenum format,
	GLenum type, GLvoid *row, GLvoid *column, GLvoid *span );

typedef void ( APIENTRY * vglCopyColorSubTable_t ) ( GLenum target, GLsizei start,
	GLint x, GLint y, GLsizei width );

typedef void ( APIENTRY * vglCopyColorTable_t ) ( GLenum target, GLenum internalformat,
	GLint x, GLint y, GLsizei width );



// ============================================================
//
// normal gl api
//
// ============================================================

/*
 * Miscellaneous
 */

typedef void ( APIENTRY * glClearIndex_t ) ( GLfloat c );

typedef void ( APIENTRY * glClearColor_t ) ( GLclampf red,
			  GLclampf green,
			  GLclampf blue,
			  GLclampf alpha );

typedef void ( APIENTRY * glClear_t ) ( GLbitfield mask );

typedef void ( APIENTRY * glIndexMask_t ) ( GLuint mask );

typedef void ( APIENTRY * glColorMask_t ) ( GLboolean red, GLboolean green,
			 GLboolean blue, GLboolean alpha );

typedef void ( APIENTRY * glAlphaFunc_t ) ( GLenum func, GLclampf ref );

typedef void ( APIENTRY * glBlendFunc_t ) ( GLenum sfactor, GLenum dfactor );

typedef void ( APIENTRY * glLogicOp_t ) ( GLenum opcode );

typedef void ( APIENTRY * glCullFace_t ) ( GLenum mode );

typedef void ( APIENTRY * glFrontFace_t ) ( GLenum mode );

typedef void ( APIENTRY * glPointSize_t ) ( GLfloat size );

typedef void ( APIENTRY * glLineWidth_t ) ( GLfloat width );

typedef void ( APIENTRY * glLineStipple_t ) ( GLint factor, GLushort pattern );

typedef void ( APIENTRY * glPolygonMode_t ) ( GLenum face, GLenum mode );

typedef void ( APIENTRY * glPolygonOffset_t ) ( GLfloat factor, GLfloat units );

typedef void ( APIENTRY * glPolygonStipple_t ) ( const GLubyte *mask );

typedef void ( APIENTRY * glGetPolygonStipple_t ) ( GLubyte *mask );

typedef void ( APIENTRY * glEdgeFlag_t ) ( GLboolean flag );

typedef void ( APIENTRY * glEdgeFlagv_t ) ( const GLboolean *flag );

typedef void ( APIENTRY * glScissor_t ) ( GLint x, GLint y,
                                   GLsizei width, GLsizei height);

typedef void ( APIENTRY * glClipPlane_t ) ( GLenum plane, const GLdouble *equation );

typedef void ( APIENTRY * glGetClipPlane_t ) ( GLenum plane, GLdouble *equation );

typedef void ( APIENTRY * glDrawBuffer_t ) ( GLenum mode );

typedef void ( APIENTRY * glReadBuffer_t ) ( GLenum mode );

typedef void ( APIENTRY * glEnable_t ) ( GLenum cap );

typedef void ( APIENTRY * glDisable_t ) ( GLenum cap );

typedef GLboolean ( APIENTRY * glIsEnabled_t ) ( GLenum cap );


typedef void ( APIENTRY * glEnableClientState_t ) ( GLenum cap );  /* v1.1 */

typedef void ( APIENTRY * glDisableClientState_t ) ( GLenum cap );  /* v1.1 */


typedef void ( APIENTRY * glGetBooleanv_t ) ( GLenum pname, GLboolean *params );

typedef void ( APIENTRY * glGetDoublev_t ) ( GLenum pname, GLdouble *params );

typedef void ( APIENTRY * glGetFloatv_t ) ( GLenum pname, GLfloat *params );

typedef void ( APIENTRY * glGetIntegerv_t ) ( GLenum pname, GLint *params );


typedef void ( APIENTRY * glPushAttrib_t ) ( GLbitfield mask );

typedef void ( APIENTRY * glPopAttrib_t ) ( void );


typedef void ( APIENTRY * glPushClientAttrib_t ) ( GLbitfield mask );  /* v1.1 */

typedef void ( APIENTRY * glPopClientAttrib_t ) ( void );  /* v1.1 */


typedef GLint ( APIENTRY * glRenderMode_t ) ( GLenum mode );

typedef GLenum ( APIENTRY * glGetError_t ) ( void );

typedef const GLubyte* ( APIENTRY * glGetString_t ) ( GLenum name );

typedef void ( APIENTRY * glFinish_t ) ( void );

typedef void ( APIENTRY * glFlush_t ) ( void );

typedef void ( APIENTRY * glHint_t ) ( GLenum target, GLenum mode );



/*
 * Depth Buffer
 */

typedef void ( APIENTRY * glClearDepth_t ) ( GLclampd depth );

typedef void ( APIENTRY * glDepthFunc_t ) ( GLenum func );

typedef void ( APIENTRY * glDepthMask_t ) ( GLboolean flag );

typedef void ( APIENTRY * glDepthRange_t ) ( GLclampd near_val, GLclampd far_val );


/*
 * Accumulation Buffer
 */

typedef void ( APIENTRY * glClearAccum_t ) ( GLfloat red, GLfloat green,
                                      GLfloat blue, GLfloat alpha );

typedef void ( APIENTRY * glAccum_t ) ( GLenum op, GLfloat value );



/*
 * Transformation
 */

typedef void ( APIENTRY * glMatrixMode_t ) ( GLenum mode );

typedef void ( APIENTRY * glOrtho_t ) ( GLdouble left, GLdouble right,
                                 GLdouble bottom, GLdouble top,
                                 GLdouble near_val, GLdouble far_val );

typedef void ( APIENTRY * glFrustum_t ) ( GLdouble left, GLdouble right,
                                   GLdouble bottom, GLdouble top,
                                   GLdouble near_val, GLdouble far_val );

typedef void ( APIENTRY * glViewport_t ) ( GLint x, GLint y,
                                    GLsizei width, GLsizei height );

typedef void ( APIENTRY * glPushMatrix_t ) ( void );

typedef void ( APIENTRY * glPopMatrix_t ) ( void );

typedef void ( APIENTRY * glLoadIdentity_t ) ( void );

typedef void ( APIENTRY * glLoadMatrixd_t ) ( const GLdouble *m );
typedef void ( APIENTRY * glLoadMatrixf_t ) ( const GLfloat *m );

typedef void ( APIENTRY * glMultMatrixd_t ) ( const GLdouble *m );
typedef void ( APIENTRY * glMultMatrixf_t ) ( const GLfloat *m );

typedef void ( APIENTRY * glRotated_t ) ( GLdouble angle,
                                   GLdouble x, GLdouble y, GLdouble z );
typedef void ( APIENTRY * glRotatef_t ) ( GLfloat angle,
                                   GLfloat x, GLfloat y, GLfloat z );

typedef void ( APIENTRY * glScaled_t ) ( GLdouble x, GLdouble y, GLdouble z );
typedef void ( APIENTRY * glScalef_t ) ( GLfloat x, GLfloat y, GLfloat z );

typedef void ( APIENTRY * glTranslated_t ) ( GLdouble x, GLdouble y, GLdouble z );
typedef void ( APIENTRY * glTranslatef_t ) ( GLfloat x, GLfloat y, GLfloat z );



/*
 * Display Lists
 */

typedef GLboolean ( APIENTRY * glIsList_t ) ( GLuint list );

typedef void ( APIENTRY * glDeleteLists_t ) ( GLuint list, GLsizei range );

typedef GLuint ( APIENTRY * glGenLists_t ) ( GLsizei range );

typedef void ( APIENTRY * glNewList_t ) ( GLuint list, GLenum mode );

typedef void ( APIENTRY * glEndList_t ) ( void );

typedef void ( APIENTRY * glCallList_t ) ( GLuint list );

typedef void ( APIENTRY * glCallLists_t ) ( GLsizei n, GLenum type,
                                     const GLvoid *lists );

typedef void ( APIENTRY * glListBase_t ) ( GLuint base );



/*
 * Drawing Functions
 */

typedef void ( APIENTRY * glBegin_t ) ( GLenum mode );

typedef void ( APIENTRY * glEnd_t ) ( void );


typedef void ( APIENTRY * glVertex2d_t ) ( GLdouble x, GLdouble y );
typedef void ( APIENTRY * glVertex2f_t ) ( GLfloat x, GLfloat y );
typedef void ( APIENTRY * glVertex2i_t ) ( GLint x, GLint y );
typedef void ( APIENTRY * glVertex2s_t ) ( GLshort x, GLshort y );

typedef void ( APIENTRY * glVertex3d_t ) ( GLdouble x, GLdouble y, GLdouble z );
typedef void ( APIENTRY * glVertex3f_t ) ( GLfloat x, GLfloat y, GLfloat z );
typedef void ( APIENTRY * glVertex3i_t ) ( GLint x, GLint y, GLint z );
typedef void ( APIENTRY * glVertex3s_t ) ( GLshort x, GLshort y, GLshort z );

typedef void ( APIENTRY * glVertex4d_t ) ( GLdouble x, GLdouble y, GLdouble z, GLdouble w );
typedef void ( APIENTRY * glVertex4f_t ) ( GLfloat x, GLfloat y, GLfloat z, GLfloat w );
typedef void ( APIENTRY * glVertex4i_t ) ( GLint x, GLint y, GLint z, GLint w );
typedef void ( APIENTRY * glVertex4s_t ) ( GLshort x, GLshort y, GLshort z, GLshort w );

typedef void ( APIENTRY * glVertex2dv_t ) ( const GLdouble *v );
typedef void ( APIENTRY * glVertex2fv_t ) ( const GLfloat *v );
typedef void ( APIENTRY * glVertex2iv_t ) ( const GLint *v );
typedef void ( APIENTRY * glVertex2sv_t ) ( const GLshort *v );

typedef void ( APIENTRY * glVertex3dv_t ) ( const GLdouble *v );
typedef void ( APIENTRY * glVertex3fv_t ) ( const GLfloat *v );
typedef void ( APIENTRY * glVertex3iv_t ) ( const GLint *v );
typedef void ( APIENTRY * glVertex3sv_t ) ( const GLshort *v );

typedef void ( APIENTRY * glVertex4dv_t ) ( const GLdouble *v );
typedef void ( APIENTRY * glVertex4fv_t ) ( const GLfloat *v );
typedef void ( APIENTRY * glVertex4iv_t ) ( const GLint *v );
typedef void ( APIENTRY * glVertex4sv_t ) ( const GLshort *v );


typedef void ( APIENTRY * glNormal3b_t ) ( GLbyte nx, GLbyte ny, GLbyte nz );
typedef void ( APIENTRY * glNormal3d_t ) ( GLdouble nx, GLdouble ny, GLdouble nz );
typedef void ( APIENTRY * glNormal3f_t ) ( GLfloat nx, GLfloat ny, GLfloat nz );
typedef void ( APIENTRY * glNormal3i_t ) ( GLint nx, GLint ny, GLint nz );
typedef void ( APIENTRY * glNormal3s_t ) ( GLshort nx, GLshort ny, GLshort nz );

typedef void ( APIENTRY * glNormal3bv_t ) ( const GLbyte *v );
typedef void ( APIENTRY * glNormal3dv_t ) ( const GLdouble *v );
typedef void ( APIENTRY * glNormal3fv_t ) ( const GLfloat *v );
typedef void ( APIENTRY * glNormal3iv_t ) ( const GLint *v );
typedef void ( APIENTRY * glNormal3sv_t ) ( const GLshort *v );


typedef void ( APIENTRY * glIndexd_t ) ( GLdouble c );
typedef void ( APIENTRY * glIndexf_t ) ( GLfloat c );
typedef void ( APIENTRY * glIndexi_t ) ( GLint c );
typedef void ( APIENTRY * glIndexs_t ) ( GLshort c );
typedef void ( APIENTRY * glIndexub_t ) ( GLubyte c );  /* 1.1 */

typedef void ( APIENTRY * glIndexdv_t ) ( const GLdouble *c );
typedef void ( APIENTRY * glIndexfv_t ) ( const GLfloat *c );
typedef void ( APIENTRY * glIndexiv_t ) ( const GLint *c );
typedef void ( APIENTRY * glIndexsv_t ) ( const GLshort *c );
typedef void ( APIENTRY * glIndexubv_t ) ( const GLubyte *c );  /* 1.1 */

typedef void ( APIENTRY * glColor3b_t ) ( GLbyte red, GLbyte green, GLbyte blue );
typedef void ( APIENTRY * glColor3d_t ) ( GLdouble red, GLdouble green, GLdouble blue );
typedef void ( APIENTRY * glColor3f_t ) ( GLfloat red, GLfloat green, GLfloat blue );
typedef void ( APIENTRY * glColor3i_t ) ( GLint red, GLint green, GLint blue );
typedef void ( APIENTRY * glColor3s_t ) ( GLshort red, GLshort green, GLshort blue );
typedef void ( APIENTRY * glColor3ub_t ) ( GLubyte red, GLubyte green, GLubyte blue );
typedef void ( APIENTRY * glColor3ui_t ) ( GLuint red, GLuint green, GLuint blue );
typedef void ( APIENTRY * glColor3us_t ) ( GLushort red, GLushort green, GLushort blue );

typedef void ( APIENTRY * glColor4b_t ) ( GLbyte red, GLbyte green,
                                   GLbyte blue, GLbyte alpha );
typedef void ( APIENTRY * glColor4d_t ) ( GLdouble red, GLdouble green,
                                   GLdouble blue, GLdouble alpha );
typedef void ( APIENTRY * glColor4f_t ) ( GLfloat red, GLfloat green,
                                   GLfloat blue, GLfloat alpha );
typedef void ( APIENTRY * glColor4i_t ) ( GLint red, GLint green,
                                   GLint blue, GLint alpha );
typedef void ( APIENTRY * glColor4s_t ) ( GLshort red, GLshort green,
                                   GLshort blue, GLshort alpha );
typedef void ( APIENTRY * glColor4ub_t ) ( GLubyte red, GLubyte green,
                                    GLubyte blue, GLubyte alpha );
typedef void ( APIENTRY * glColor4ui_t ) ( GLuint red, GLuint green,
                                    GLuint blue, GLuint alpha );
typedef void ( APIENTRY * glColor4us_t ) ( GLushort red, GLushort green,
                                    GLushort blue, GLushort alpha );


typedef void ( APIENTRY * glColor3bv_t ) ( const GLbyte *v );
typedef void ( APIENTRY * glColor3dv_t ) ( const GLdouble *v );
typedef void ( APIENTRY * glColor3fv_t ) ( const GLfloat *v );
typedef void ( APIENTRY * glColor3iv_t ) ( const GLint *v );
typedef void ( APIENTRY * glColor3sv_t ) ( const GLshort *v );
typedef void ( APIENTRY * glColor3ubv_t ) ( const GLubyte *v );
typedef void ( APIENTRY * glColor3uiv_t ) ( const GLuint *v );
typedef void ( APIENTRY * glColor3usv_t ) ( const GLushort *v );

typedef void ( APIENTRY * glColor4bv_t ) ( const GLbyte *v );
typedef void ( APIENTRY * glColor4dv_t ) ( const GLdouble *v );
typedef void ( APIENTRY * glColor4fv_t ) ( const GLfloat *v );
typedef void ( APIENTRY * glColor4iv_t ) ( const GLint *v );
typedef void ( APIENTRY * glColor4sv_t ) ( const GLshort *v );
typedef void ( APIENTRY * glColor4ubv_t ) ( const GLubyte *v );
typedef void ( APIENTRY * glColor4uiv_t ) ( const GLuint *v );
typedef void ( APIENTRY * glColor4usv_t ) ( const GLushort *v );


typedef void ( APIENTRY * glTexCoord1d_t ) ( GLdouble s );
typedef void ( APIENTRY * glTexCoord1f_t ) ( GLfloat s );
typedef void ( APIENTRY * glTexCoord1i_t ) ( GLint s );
typedef void ( APIENTRY * glTexCoord1s_t ) ( GLshort s );

typedef void ( APIENTRY * glTexCoord2d_t ) ( GLdouble s, GLdouble t );
typedef void ( APIENTRY * glTexCoord2f_t ) ( GLfloat s, GLfloat t );
typedef void ( APIENTRY * glTexCoord2i_t ) ( GLint s, GLint t );
typedef void ( APIENTRY * glTexCoord2s_t ) ( GLshort s, GLshort t );

typedef void ( APIENTRY * glTexCoord3d_t ) ( GLdouble s, GLdouble t, GLdouble r );
typedef void ( APIENTRY * glTexCoord3f_t ) ( GLfloat s, GLfloat t, GLfloat r );
typedef void ( APIENTRY * glTexCoord3i_t ) ( GLint s, GLint t, GLint r );
typedef void ( APIENTRY * glTexCoord3s_t ) ( GLshort s, GLshort t, GLshort r );

typedef void ( APIENTRY * glTexCoord4d_t ) ( GLdouble s, GLdouble t, GLdouble r, GLdouble q );
typedef void ( APIENTRY * glTexCoord4f_t ) ( GLfloat s, GLfloat t, GLfloat r, GLfloat q );
typedef void ( APIENTRY * glTexCoord4i_t ) ( GLint s, GLint t, GLint r, GLint q );
typedef void ( APIENTRY * glTexCoord4s_t ) ( GLshort s, GLshort t, GLshort r, GLshort q );

typedef void ( APIENTRY * glTexCoord1dv_t ) ( const GLdouble *v );
typedef void ( APIENTRY * glTexCoord1fv_t ) ( const GLfloat *v );
typedef void ( APIENTRY * glTexCoord1iv_t ) ( const GLint *v );
typedef void ( APIENTRY * glTexCoord1sv_t ) ( const GLshort *v );

typedef void ( APIENTRY * glTexCoord2dv_t ) ( const GLdouble *v );
typedef void ( APIENTRY * glTexCoord2fv_t ) ( const GLfloat *v );
typedef void ( APIENTRY * glTexCoord2iv_t ) ( const GLint *v );
typedef void ( APIENTRY * glTexCoord2sv_t ) ( const GLshort *v );

typedef void ( APIENTRY * glTexCoord3dv_t ) ( const GLdouble *v );
typedef void ( APIENTRY * glTexCoord3fv_t ) ( const GLfloat *v );
typedef void ( APIENTRY * glTexCoord3iv_t ) ( const GLint *v );
typedef void ( APIENTRY * glTexCoord3sv_t ) ( const GLshort *v );

typedef void ( APIENTRY * glTexCoord4dv_t ) ( const GLdouble *v );
typedef void ( APIENTRY * glTexCoord4fv_t ) ( const GLfloat *v );
typedef void ( APIENTRY * glTexCoord4iv_t ) ( const GLint *v );
typedef void ( APIENTRY * glTexCoord4sv_t ) ( const GLshort *v );


typedef void ( APIENTRY * glRasterPos2d_t ) ( GLdouble x, GLdouble y );
typedef void ( APIENTRY * glRasterPos2f_t ) ( GLfloat x, GLfloat y );
typedef void ( APIENTRY * glRasterPos2i_t ) ( GLint x, GLint y );
typedef void ( APIENTRY * glRasterPos2s_t ) ( GLshort x, GLshort y );

typedef void ( APIENTRY * glRasterPos3d_t ) ( GLdouble x, GLdouble y, GLdouble z );
typedef void ( APIENTRY * glRasterPos3f_t ) ( GLfloat x, GLfloat y, GLfloat z );
typedef void ( APIENTRY * glRasterPos3i_t ) ( GLint x, GLint y, GLint z );
typedef void ( APIENTRY * glRasterPos3s_t ) ( GLshort x, GLshort y, GLshort z );

typedef void ( APIENTRY * glRasterPos4d_t ) ( GLdouble x, GLdouble y, GLdouble z, GLdouble w );
typedef void ( APIENTRY * glRasterPos4f_t ) ( GLfloat x, GLfloat y, GLfloat z, GLfloat w );
typedef void ( APIENTRY * glRasterPos4i_t ) ( GLint x, GLint y, GLint z, GLint w );
typedef void ( APIENTRY * glRasterPos4s_t ) ( GLshort x, GLshort y, GLshort z, GLshort w );

typedef void ( APIENTRY * glRasterPos2dv_t ) ( const GLdouble *v );
typedef void ( APIENTRY * glRasterPos2fv_t ) ( const GLfloat *v );
typedef void ( APIENTRY * glRasterPos2iv_t ) ( const GLint *v );
typedef void ( APIENTRY * glRasterPos2sv_t ) ( const GLshort *v );

typedef void ( APIENTRY * glRasterPos3dv_t ) ( const GLdouble *v );
typedef void ( APIENTRY * glRasterPos3fv_t ) ( const GLfloat *v );
typedef void ( APIENTRY * glRasterPos3iv_t ) ( const GLint *v );
typedef void ( APIENTRY * glRasterPos3sv_t ) ( const GLshort *v );

typedef void ( APIENTRY * glRasterPos4dv_t ) ( const GLdouble *v );
typedef void ( APIENTRY * glRasterPos4fv_t ) ( const GLfloat *v );
typedef void ( APIENTRY * glRasterPos4iv_t ) ( const GLint *v );
typedef void ( APIENTRY * glRasterPos4sv_t ) ( const GLshort *v );


typedef void ( APIENTRY * glRectd_t ) ( GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2 );
typedef void ( APIENTRY * glRectf_t ) ( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 );
typedef void ( APIENTRY * glRecti_t ) ( GLint x1, GLint y1, GLint x2, GLint y2 );
typedef void ( APIENTRY * glRects_t ) ( GLshort x1, GLshort y1, GLshort x2, GLshort y2 );


typedef void ( APIENTRY * glRectdv_t ) ( const GLdouble *v1, const GLdouble *v2 );
typedef void ( APIENTRY * glRectfv_t ) ( const GLfloat *v1, const GLfloat *v2 );
typedef void ( APIENTRY * glRectiv_t ) ( const GLint *v1, const GLint *v2 );
typedef void ( APIENTRY * glRectsv_t ) ( const GLshort *v1, const GLshort *v2 );



/*
 * Vertex Arrays  _t ) (1.1)
 */

typedef void ( APIENTRY * glVertexPointer_t ) ( GLint size, GLenum type,
                                         GLsizei stride, const GLvoid *ptr );

typedef void ( APIENTRY * glNormalPointer_t ) ( GLenum type, GLsizei stride,
                                         const GLvoid *ptr );

typedef void ( APIENTRY * glColorPointer_t ) ( GLint size, GLenum type,
                                        GLsizei stride, const GLvoid *ptr );

typedef void ( APIENTRY * glIndexPointer_t ) ( GLenum type, GLsizei stride,
                                        const GLvoid *ptr );

typedef void ( APIENTRY * glTexCoordPointer_t ) ( GLint size, GLenum type,
                                           GLsizei stride, const GLvoid *ptr );

typedef void ( APIENTRY * glEdgeFlagPointer_t ) ( GLsizei stride, const GLvoid *ptr );

typedef void ( APIENTRY * glGetPointerv_t ) ( GLenum pname, void **params );

typedef void ( APIENTRY * glArrayElement_t ) ( GLint i );

typedef void ( APIENTRY * glDrawArrays_t ) ( GLenum mode, GLint first,
                                      GLsizei count );

typedef void ( APIENTRY * glDrawElements_t ) ( GLenum mode, GLsizei count,
                                        GLenum type, const GLvoid *indices );

typedef void ( APIENTRY * glInterleavedArrays_t ) ( GLenum format, GLsizei stride,
                                             const GLvoid *pointer );


/*
 * Lighting
 */

typedef void ( APIENTRY * glShadeModel_t ) ( GLenum mode );

typedef void ( APIENTRY * glLightf_t ) ( GLenum light, GLenum pname, GLfloat param );
typedef void ( APIENTRY * glLighti_t ) ( GLenum light, GLenum pname, GLint param );
typedef void ( APIENTRY * glLightfv_t ) ( GLenum light, GLenum pname,
                                   const GLfloat *params );
typedef void ( APIENTRY * glLightiv_t ) ( GLenum light, GLenum pname,
                                   const GLint *params );

typedef void ( APIENTRY * glGetLightfv_t ) ( GLenum light, GLenum pname,
                                      GLfloat *params );
typedef void ( APIENTRY * glGetLightiv_t ) ( GLenum light, GLenum pname,
                                      GLint *params );

typedef void ( APIENTRY * glLightModelf_t ) ( GLenum pname, GLfloat param );
typedef void ( APIENTRY * glLightModeli_t ) ( GLenum pname, GLint param );
typedef void ( APIENTRY * glLightModelfv_t ) ( GLenum pname, const GLfloat *params );
typedef void ( APIENTRY * glLightModeliv_t ) ( GLenum pname, const GLint *params );

typedef void ( APIENTRY * glMaterialf_t ) ( GLenum face, GLenum pname, GLfloat param );
typedef void ( APIENTRY * glMateriali_t ) ( GLenum face, GLenum pname, GLint param );
typedef void ( APIENTRY * glMaterialfv_t ) ( GLenum face, GLenum pname, const GLfloat *params );
typedef void ( APIENTRY * glMaterialiv_t ) ( GLenum face, GLenum pname, const GLint *params );

typedef void ( APIENTRY * glGetMaterialfv_t ) ( GLenum face, GLenum pname, GLfloat *params );
typedef void ( APIENTRY * glGetMaterialiv_t ) ( GLenum face, GLenum pname, GLint *params );

typedef void ( APIENTRY * glColorMaterial_t ) ( GLenum face, GLenum mode );




/*
 * Raster functions
 */

typedef void ( APIENTRY * glPixelZoom_t ) ( GLfloat xfactor, GLfloat yfactor );

typedef void ( APIENTRY * glPixelStoref_t ) ( GLenum pname, GLfloat param );
typedef void ( APIENTRY * glPixelStorei_t ) ( GLenum pname, GLint param );

typedef void ( APIENTRY * glPixelTransferf_t ) ( GLenum pname, GLfloat param );
typedef void ( APIENTRY * glPixelTransferi_t ) ( GLenum pname, GLint param );

typedef void ( APIENTRY * glPixelMapfv_t ) ( GLenum map, GLint mapsize,
                                      const GLfloat *values );
typedef void ( APIENTRY * glPixelMapuiv_t ) ( GLenum map, GLint mapsize,
                                       const GLuint *values );
typedef void ( APIENTRY * glPixelMapusv_t ) ( GLenum map, GLint mapsize,
                                       const GLushort *values );

typedef void ( APIENTRY * glGetPixelMapfv_t ) ( GLenum map, GLfloat *values );
typedef void ( APIENTRY * glGetPixelMapuiv_t ) ( GLenum map, GLuint *values );
typedef void ( APIENTRY * glGetPixelMapusv_t ) ( GLenum map, GLushort *values );

typedef void ( APIENTRY * glBitmap_t ) ( GLsizei width, GLsizei height,
                                  GLfloat xorig, GLfloat yorig,
                                  GLfloat xmove, GLfloat ymove,
                                  const GLubyte *bitmap );

typedef void ( APIENTRY * glReadPixels_t ) ( GLint x, GLint y,
                                      GLsizei width, GLsizei height,
                                      GLenum format, GLenum type,
                                      GLvoid *pixels );

typedef void ( APIENTRY * glDrawPixels_t ) ( GLsizei width, GLsizei height,
                                      GLenum format, GLenum type,
                                      const GLvoid *pixels );

typedef void ( APIENTRY * glCopyPixels_t ) ( GLint x, GLint y,
                                      GLsizei width, GLsizei height,
                                      GLenum type );



/*
 * Stenciling
 */

typedef void ( APIENTRY * glStencilFunc_t ) ( GLenum func, GLint ref, GLuint mask );

typedef void ( APIENTRY * glStencilMask_t ) ( GLuint mask );

typedef void ( APIENTRY * glStencilOp_t ) ( GLenum fail, GLenum zfail, GLenum zpass );

typedef void ( APIENTRY * glClearStencil_t ) ( GLint s );



/*
 * Texture mapping
 */

typedef void ( APIENTRY * glTexGend_t ) ( GLenum coord, GLenum pname, GLdouble param );
typedef void ( APIENTRY * glTexGenf_t ) ( GLenum coord, GLenum pname, GLfloat param );
typedef void ( APIENTRY * glTexGeni_t ) ( GLenum coord, GLenum pname, GLint param );

typedef void ( APIENTRY * glTexGendv_t ) ( GLenum coord, GLenum pname, const GLdouble *params );
typedef void ( APIENTRY * glTexGenfv_t ) ( GLenum coord, GLenum pname, const GLfloat *params );
typedef void ( APIENTRY * glTexGeniv_t ) ( GLenum coord, GLenum pname, const GLint *params );

typedef void ( APIENTRY * glGetTexGendv_t ) ( GLenum coord, GLenum pname, GLdouble *params );
typedef void ( APIENTRY * glGetTexGenfv_t ) ( GLenum coord, GLenum pname, GLfloat *params );
typedef void ( APIENTRY * glGetTexGeniv_t ) ( GLenum coord, GLenum pname, GLint *params );


typedef void ( APIENTRY * glTexEnvf_t ) ( GLenum target, GLenum pname, GLfloat param );
typedef void ( APIENTRY * glTexEnvi_t ) ( GLenum target, GLenum pname, GLint param );

typedef void ( APIENTRY * glTexEnvfv_t ) ( GLenum target, GLenum pname, const GLfloat *params );
typedef void ( APIENTRY * glTexEnviv_t ) ( GLenum target, GLenum pname, const GLint *params );

typedef void ( APIENTRY * glGetTexEnvfv_t ) ( GLenum target, GLenum pname, GLfloat *params );
typedef void ( APIENTRY * glGetTexEnviv_t ) ( GLenum target, GLenum pname, GLint *params );


typedef void ( APIENTRY * glTexParameterf_t ) ( GLenum target, GLenum pname, GLfloat param );
typedef void ( APIENTRY * glTexParameteri_t ) ( GLenum target, GLenum pname, GLint param );

typedef void ( APIENTRY * glTexParameterfv_t ) ( GLenum target, GLenum pname,
                                          const GLfloat *params );
typedef void ( APIENTRY * glTexParameteriv_t ) ( GLenum target, GLenum pname,
                                          const GLint *params );

typedef void ( APIENTRY * glGetTexParameterfv_t ) ( GLenum target,
                                             GLenum pname, GLfloat *params);
typedef void ( APIENTRY * glGetTexParameteriv_t ) ( GLenum target,
                                             GLenum pname, GLint *params );

typedef void ( APIENTRY * glGetTexLevelParameterfv_t ) ( GLenum target, GLint level,
                                                  GLenum pname, GLfloat *params );
typedef void ( APIENTRY * glGetTexLevelParameteriv_t ) ( GLenum target, GLint level,
                                                  GLenum pname, GLint *params );


typedef void ( APIENTRY * glTexImage1D_t ) ( GLenum target, GLint level,
                                      GLint internalFormat,
                                      GLsizei width, GLint border,
                                      GLenum format, GLenum type,
                                      const GLvoid *pixels );

typedef void ( APIENTRY * glTexImage2D_t ) ( GLenum target, GLint level,
                                      GLint internalFormat,
                                      GLsizei width, GLsizei height,
                                      GLint border, GLenum format, GLenum type,
                                      const GLvoid *pixels );

typedef void ( APIENTRY * glGetTexImage_t ) ( GLenum target, GLint level,
                                       GLenum format, GLenum type,
                                       GLvoid *pixels );



/* 1.1 functions */

typedef void ( APIENTRY * glGenTextures_t ) ( GLsizei n, GLuint *textures );

typedef void ( APIENTRY * glDeleteTextures_t ) ( GLsizei n, const GLuint *textures);

typedef void ( APIENTRY * glBindTexture_t ) ( GLenum target, GLuint texture );

typedef void ( APIENTRY * glPrioritizeTextures_t ) ( GLsizei n,
                                              const GLuint *textures,
                                              const GLclampf *priorities );

typedef GLboolean ( APIENTRY * glAreTexturesResident_t ) ( GLsizei n,
                                                    const GLuint *textures,
                                                    GLboolean *residences );

typedef GLboolean ( APIENTRY * glIsTexture_t ) ( GLuint texture );


typedef void ( APIENTRY * glTexSubImage1D_t ) ( GLenum target, GLint level,
                                         GLint xoffset,
                                         GLsizei width, GLenum format,
                                         GLenum type, const GLvoid *pixels );


typedef void ( APIENTRY * glTexSubImage2D_t ) ( GLenum target, GLint level,
                                         GLint xoffset, GLint yoffset,
                                         GLsizei width, GLsizei height,
                                         GLenum format, GLenum type,
                                         const GLvoid *pixels );


typedef void ( APIENTRY * glCopyTexImage1D_t ) ( GLenum target, GLint level,
                                          GLenum internalformat,
                                          GLint x, GLint y,
                                          GLsizei width, GLint border );


typedef void ( APIENTRY * glCopyTexImage2D_t ) ( GLenum target, GLint level,
                                          GLenum internalformat,
                                          GLint x, GLint y,
                                          GLsizei width, GLsizei height,
                                          GLint border );


typedef void ( APIENTRY * glCopyTexSubImage1D_t ) ( GLenum target, GLint level,
                                             GLint xoffset, GLint x, GLint y,
                                             GLsizei width );


typedef void ( APIENTRY * glCopyTexSubImage2D_t ) ( GLenum target, GLint level,
                                             GLint xoffset, GLint yoffset,
                                             GLint x, GLint y,
                                             GLsizei width, GLsizei height );




/*
 * Evaluators
 */

typedef void ( APIENTRY * glMap1d_t ) ( GLenum target, GLdouble u1, GLdouble u2,
                                 GLint stride,
                                 GLint order, const GLdouble *points );
typedef void ( APIENTRY * glMap1f_t ) ( GLenum target, GLfloat u1, GLfloat u2,
                                 GLint stride,
                                 GLint order, const GLfloat *points );

typedef void ( APIENTRY * glMap2d_t ) ( GLenum target,
		     GLdouble u1, GLdouble u2, GLint ustride, GLint uorder,
		     GLdouble v1, GLdouble v2, GLint vstride, GLint vorder,
		     const GLdouble *points );
typedef void ( APIENTRY * glMap2f_t ) ( GLenum target,
		     GLfloat u1, GLfloat u2, GLint ustride, GLint uorder,
		     GLfloat v1, GLfloat v2, GLint vstride, GLint vorder,
		     const GLfloat *points );

typedef void ( APIENTRY * glGetMapdv_t ) ( GLenum target, GLenum query, GLdouble *v );
typedef void ( APIENTRY * glGetMapfv_t ) ( GLenum target, GLenum query, GLfloat *v );
typedef void ( APIENTRY * glGetMapiv_t ) ( GLenum target, GLenum query, GLint *v );

typedef void ( APIENTRY * glEvalCoord1d_t ) ( GLdouble u );
typedef void ( APIENTRY * glEvalCoord1f_t ) ( GLfloat u );

typedef void ( APIENTRY * glEvalCoord1dv_t ) ( const GLdouble *u );
typedef void ( APIENTRY * glEvalCoord1fv_t ) ( const GLfloat *u );

typedef void ( APIENTRY * glEvalCoord2d_t ) ( GLdouble u, GLdouble v );
typedef void ( APIENTRY * glEvalCoord2f_t ) ( GLfloat u, GLfloat v );

typedef void ( APIENTRY * glEvalCoord2dv_t ) ( const GLdouble *u );
typedef void ( APIENTRY * glEvalCoord2fv_t ) ( const GLfloat *u );

typedef void ( APIENTRY * glMapGrid1d_t ) ( GLint un, GLdouble u1, GLdouble u2 );
typedef void ( APIENTRY * glMapGrid1f_t ) ( GLint un, GLfloat u1, GLfloat u2 );

typedef void ( APIENTRY * glMapGrid2d_t ) ( GLint un, GLdouble u1, GLdouble u2,
                                     GLint vn, GLdouble v1, GLdouble v2 );
typedef void ( APIENTRY * glMapGrid2f_t ) ( GLint un, GLfloat u1, GLfloat u2,
                                     GLint vn, GLfloat v1, GLfloat v2 );

typedef void ( APIENTRY * glEvalPoint1_t ) ( GLint i );

typedef void ( APIENTRY * glEvalPoint2_t ) ( GLint i, GLint j );

typedef void ( APIENTRY * glEvalMesh1_t ) ( GLenum mode, GLint i1, GLint i2 );

typedef void ( APIENTRY * glEvalMesh2_t ) ( GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2 );



/*
 * Fog
 */

typedef void ( APIENTRY * glFogf_t ) ( GLenum pname, GLfloat param );

typedef void ( APIENTRY * glFogi_t ) ( GLenum pname, GLint param );

typedef void ( APIENTRY * glFogfv_t ) ( GLenum pname, const GLfloat *params );

typedef void ( APIENTRY * glFogiv_t ) ( GLenum pname, const GLint *params );



/*
 * Selection and Feedback
 */

typedef void ( APIENTRY * glFeedbackBuffer_t ) ( GLsizei size, GLenum type, GLfloat *buffer );

typedef void ( APIENTRY * glPassThrough_t ) ( GLfloat token );

typedef void ( APIENTRY * glSelectBuffer_t ) ( GLsizei size, GLuint *buffer );

typedef void ( APIENTRY * glInitNames_t ) ( void );

typedef void ( APIENTRY * glLoadName_t ) ( GLuint name );

typedef void ( APIENTRY * glPushName_t ) ( GLuint name );

typedef void ( APIENTRY * glPopName_t ) ( void );





typedef void ( APIENTRY * glActiveTextureARB_t ) (GLenum texture);
typedef void ( APIENTRY * glClientActiveTextureARB_t ) (GLenum texture);
typedef void ( APIENTRY * glMultiTexCoord1dARB_t ) (GLenum target, GLdouble s);
typedef void ( APIENTRY * glMultiTexCoord1dvARB_t ) (GLenum target, const GLdouble *v);
typedef void ( APIENTRY * glMultiTexCoord1fARB_t ) (GLenum target, GLfloat s);
typedef void ( APIENTRY * glMultiTexCoord1fvARB_t ) (GLenum target, const GLfloat *v);
typedef void ( APIENTRY * glMultiTexCoord1iARB_t ) (GLenum target, GLint s);
typedef void ( APIENTRY * glMultiTexCoord1ivARB_t ) (GLenum target, const GLint *v);
typedef void ( APIENTRY * glMultiTexCoord1sARB_t ) (GLenum target, GLshort s);
typedef void ( APIENTRY * glMultiTexCoord1svARB_t ) (GLenum target, const GLshort *v);
typedef void ( APIENTRY * glMultiTexCoord2dARB_t ) (GLenum target, GLdouble s, GLdouble t);
typedef void ( APIENTRY * glMultiTexCoord2dvARB_t ) (GLenum target, const GLdouble *v);
typedef void ( APIENTRY * glMultiTexCoord2fARB_t ) (GLenum target, GLfloat s, GLfloat t);
typedef void ( APIENTRY * glMultiTexCoord2fvARB_t ) (GLenum target, const GLfloat *v);
typedef void ( APIENTRY * glMultiTexCoord2iARB_t ) (GLenum target, GLint s, GLint t);
typedef void ( APIENTRY * glMultiTexCoord2ivARB_t ) (GLenum target, const GLint *v);
typedef void ( APIENTRY * glMultiTexCoord2sARB_t ) (GLenum target, GLshort s, GLshort t);
typedef void ( APIENTRY * glMultiTexCoord2svARB_t ) (GLenum target, const GLshort *v);
typedef void ( APIENTRY * glMultiTexCoord3dARB_t ) (GLenum target, GLdouble s, GLdouble t, GLdouble r);
typedef void ( APIENTRY * glMultiTexCoord3dvARB_t ) (GLenum target, const GLdouble *v);
typedef void ( APIENTRY * glMultiTexCoord3fARB_t ) (GLenum target, GLfloat s, GLfloat t, GLfloat r);
typedef void ( APIENTRY * glMultiTexCoord3fvARB_t ) (GLenum target, const GLfloat *v);
typedef void ( APIENTRY * glMultiTexCoord3iARB_t ) (GLenum target, GLint s, GLint t, GLint r);
typedef void ( APIENTRY * glMultiTexCoord3ivARB_t ) (GLenum target, const GLint *v);
typedef void ( APIENTRY * glMultiTexCoord3sARB_t ) (GLenum target, GLshort s, GLshort t, GLshort r);
typedef void ( APIENTRY * glMultiTexCoord3svARB_t ) (GLenum target, const GLshort *v);
typedef void ( APIENTRY * glMultiTexCoord4dARB_t ) (GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q);
typedef void ( APIENTRY * glMultiTexCoord4dvARB_t ) (GLenum target, const GLdouble *v);
typedef void ( APIENTRY * glMultiTexCoord4fARB_t ) (GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q);
typedef void ( APIENTRY * glMultiTexCoord4fvARB_t ) (GLenum target, const GLfloat *v);
typedef void ( APIENTRY * glMultiTexCoord4iARB_t ) (GLenum target, GLint s, GLint t, GLint r, GLint q);
typedef void ( APIENTRY * glMultiTexCoord4ivARB_t ) (GLenum target, const GLint *v);
typedef void ( APIENTRY * glMultiTexCoord4sARB_t ) (GLenum target, GLshort s, GLshort t, GLshort r, GLshort q);
typedef void ( APIENTRY * glMultiTexCoord4svARB_t ) (GLenum target, const GLshort *v);



/* GL_EXT_point_parameters */
typedef void ( APIENTRY * glPointParameterfEXT_t ) ( GLenum pname, GLfloat param );
typedef void ( APIENTRY * glPointParameterfvEXT_t ) ( GLenum pname,
                                               const GLfloat *params );



/* GL_INGR_blend_func_separate */
typedef void ( APIENTRY * glBlendFuncSeparateINGR_t ) ( GLenum sfactorRGB,
                                               GLenum dfactorRGB,
                                               GLenum sfactorAlpha, 
                                               GLenum dfactorAlpha );



/* 1.2 functions */
typedef void ( APIENTRY * glDrawRangeElements_t ) ( GLenum mode, GLuint start,
	GLuint end, GLsizei count, GLenum type, const GLvoid *indices );

typedef void ( APIENTRY * glTexImage3D_t ) ( GLenum target, GLint level,
                                      GLint internalFormat,
                                      GLsizei width, GLsizei height,
                                      GLsizei depth, GLint border,
                                      GLenum format, GLenum type,
                                      const GLvoid *pixels );

typedef void ( APIENTRY * glTexSubImage3D_t ) ( GLenum target, GLint level,
                                         GLint xoffset, GLint yoffset,
                                         GLint zoffset, GLsizei width,
                                         GLsizei height, GLsizei depth,
                                         GLenum format,
                                         GLenum type, const GLvoid *pixels);

typedef void ( APIENTRY * glCopyTexSubImage3D_t ) ( GLenum target, GLint level,
                                             GLint xoffset, GLint yoffset,
                                             GLint zoffset, GLint x,
                                             GLint y, GLsizei width,
                                             GLsizei height );


/* 1.2 imaging extension functions */

typedef void ( APIENTRY * glHistogram_t ) ( GLenum target, GLsizei width,
				   GLenum internalformat, GLboolean sink );

typedef void ( APIENTRY * glResetHistogram_t ) ( GLenum target );

typedef void ( APIENTRY * glGetHistogram_t ) ( GLenum target, GLboolean reset,
				      GLenum format, GLenum type,
				      GLvoid *values );

typedef void ( APIENTRY * glGetHistogramParameterfv_t ) ( GLenum target, GLenum pname,
						 GLfloat *params );

typedef void ( APIENTRY * glGetHistogramParameteriv_t ) ( GLenum target, GLenum pname,
						 GLint *params );

typedef void ( APIENTRY * glMinmax_t ) ( GLenum target, GLenum internalformat,
				GLboolean sink );

typedef void ( APIENTRY * glResetMinmax_t ) ( GLenum target );

typedef void ( APIENTRY * glGetMinmaxParameterfv_t ) ( GLenum target, GLenum pname,
					      GLfloat *params );

typedef void ( APIENTRY * glGetMinmaxParameteriv_t ) ( GLenum target, GLenum pname,
					      GLint *params );

typedef void ( APIENTRY * glConvolutionFilter1D_t ) ( GLenum target,
	GLenum internalformat, GLsizei width, GLenum format, GLenum type,
	const GLvoid *image );

typedef void ( APIENTRY * glConvolutionFilter2D_t ) ( GLenum target,
	GLenum internalformat, GLsizei width, GLsizei height, GLenum format,
	GLenum type, const GLvoid *image );

typedef void ( APIENTRY * glConvolutionParameterf_t ) ( GLenum target, GLenum pname,
	GLfloat params );

typedef void ( APIENTRY * glConvolutionParameterfv_t ) ( GLenum target, GLenum pname,
	const GLfloat *params );

typedef void ( APIENTRY * glConvolutionParameteri_t ) ( GLenum target, GLenum pname,
	GLint params );

typedef void ( APIENTRY * glConvolutionParameteriv_t ) ( GLenum target, GLenum pname,
	const GLint *params );

typedef void ( APIENTRY * glCopyConvolutionFilter1D_t ) ( GLenum target,
	GLenum internalformat, GLint x, GLint y, GLsizei width );

typedef void ( APIENTRY * glCopyConvolutionFilter2D_t ) ( GLenum target,
	GLenum internalformat, GLint x, GLint y, GLsizei width,
	GLsizei height);

typedef void ( APIENTRY * glGetConvolutionFilter_t ) ( GLenum target, GLenum format,
	GLenum type, GLvoid *image );

typedef void ( APIENTRY * glGetConvolutionParameterfv_t ) ( GLenum target, GLenum pname,
	GLfloat *params );

typedef void ( APIENTRY * glGetConvolutionParameteriv_t ) ( GLenum target, GLenum pname,
	GLint *params );

typedef void ( APIENTRY * glSeparableFilter2D_t ) ( GLenum target,
	GLenum internalformat, GLsizei width, GLsizei height, GLenum format,
	GLenum type, const GLvoid *row, const GLvoid *column );

typedef void ( APIENTRY * glGetSeparableFilter_t ) ( GLenum target, GLenum format,
	GLenum type, GLvoid *row, GLvoid *column, GLvoid *span );

typedef void ( APIENTRY * glCopyColorSubTable_t ) ( GLenum target, GLsizei start,
	GLint x, GLint y, GLsizei width );

typedef void ( APIENTRY * glCopyColorTable_t ) ( GLenum target, GLenum internalformat,
	GLint x, GLint y, GLsizei width );


#if 0
// real prototypes
// vgl api

GLAPI vglClearIndex_t vglClearIndex;
GLAPI vglClearColor_t vglClearColor;
GLAPI vglClear_t vglClear;
GLAPI vglIndexMask_t vglIndexMask;
GLAPI vglColorMask_t vglColorMask;
GLAPI vglAlphaFunc_t vglAlphaFunc;
GLAPI vglBlendFunc_t vglBlendFunc;
GLAPI vglLogicOp_t vglLogicOp;
GLAPI vglCullFace_t vglCullFace;
GLAPI vglFrontFace_t vglFrontFace;
GLAPI vglPointSize_t vglPointSize;
GLAPI vglLineWidth_t vglLineWidth;
GLAPI vglLineStipple_t vglLineStipple;
GLAPI vglPolygonMode_t vglPolygonMode;
GLAPI vglPolygonOffset_t vglPolygonOffset;
GLAPI vglPolygonStipple_t vglPolygonStipple;
GLAPI vglGetPolygonStipple_t vglGetPolygonStipple;
GLAPI vglEdgeFlag_t vglEdgeFlag;
GLAPI vglEdgeFlagv_t vglEdgeFlagv;
GLAPI vglScissor_t vglScissor;
GLAPI vglClipPlane_t vglClipPlane;
GLAPI vglGetClipPlane_t vglGetClipPlane;
GLAPI vglDrawBuffer_t vglDrawBuffer;
GLAPI vglReadBuffer_t vglReadBuffer;
GLAPI vglEnable_t vglEnable;
GLAPI vglDisable_t vglDisable;
GLAPI vglIsEnabled_t vglIsEnabled;
GLAPI vglEnableClientState_t vglEnableClientState;
GLAPI vglDisableClientState_t vglDisableClientState;
GLAPI vglGetBooleanv_t vglGetBooleanv;
GLAPI vglGetDoublev_t vglGetDoublev;
GLAPI vglGetFloatv_t vglGetFloatv;
GLAPI vglGetIntegerv_t vglGetIntegerv;
GLAPI vglPushAttrib_t vglPushAttrib;
GLAPI vglPopAttrib_t vglPopAttrib;
GLAPI vglPushClientAttrib_t vglPushClientAttrib;
GLAPI vglPopClientAttrib_t vglPopClientAttrib;
GLAPI vglRenderMode_t vglRenderMode;
GLAPI vglGetError_t vglGetError;
GLAPI vglGetString_t vglGetString;
GLAPI vglFinish_t vglFinish;
GLAPI vglFlush_t vglFlush;
GLAPI vglHint_t vglHint;
GLAPI vglClearDepth_t vglClearDepth;
GLAPI vglDepthFunc_t vglDepthFunc;
GLAPI vglDepthMask_t vglDepthMask;
GLAPI vglDepthRange_t vglDepthRange;
GLAPI vglClearAccum_t vglClearAccum;
GLAPI vglAccum_t vglAccum;
GLAPI vglMatrixMode_t vglMatrixMode;
GLAPI vglOrtho_t vglOrtho;
GLAPI vglFrustum_t vglFrustum;
GLAPI vglViewport_t vglViewport;
GLAPI vglPushMatrix_t vglPushMatrix;
GLAPI vglPopMatrix_t vglPopMatrix;
GLAPI vglLoadIdentity_t vglLoadIdentity;
GLAPI vglLoadMatrixd_t vglLoadMatrixd;
GLAPI vglLoadMatrixf_t vglLoadMatrixf;
GLAPI vglMultMatrixd_t vglMultMatrixd;
GLAPI vglMultMatrixf_t vglMultMatrixf;
GLAPI vglRotated_t vglRotated;
GLAPI vglRotatef_t vglRotatef;
GLAPI vglScaled_t vglScaled;
GLAPI vglScalef_t vglScalef;
GLAPI vglTranslated_t vglTranslated;
GLAPI vglTranslatef_t vglTranslatef;
GLAPI vglIsList_t vglIsList;
GLAPI vglDeleteLists_t vglDeleteLists;
GLAPI vglGenLists_t vglGenLists;
GLAPI vglNewList_t vglNewList;
GLAPI vglEndList_t vglEndList;
GLAPI vglCallList_t vglCallList;
GLAPI vglCallLists_t vglCallLists;
GLAPI vglListBase_t vglListBase;
GLAPI vglBegin_t vglBegin;
GLAPI vglEnd_t vglEnd;
GLAPI vglVertex2d_t vglVertex2d;
GLAPI vglVertex2f_t vglVertex2f;
GLAPI vglVertex2i_t vglVertex2i;
GLAPI vglVertex2s_t vglVertex2s;
GLAPI vglVertex3d_t vglVertex3d;
GLAPI vglVertex3f_t vglVertex3f;
GLAPI vglVertex3i_t vglVertex3i;
GLAPI vglVertex3s_t vglVertex3s;
GLAPI vglVertex4d_t vglVertex4d;
GLAPI vglVertex4f_t vglVertex4f;
GLAPI vglVertex4i_t vglVertex4i;
GLAPI vglVertex4s_t vglVertex4s;
GLAPI vglVertex2dv_t vglVertex2dv;
GLAPI vglVertex2fv_t vglVertex2fv;
GLAPI vglVertex2iv_t vglVertex2iv;
GLAPI vglVertex2sv_t vglVertex2sv;
GLAPI vglVertex3dv_t vglVertex3dv;
GLAPI vglVertex3fv_t vglVertex3fv;
GLAPI vglVertex3iv_t vglVertex3iv;
GLAPI vglVertex3sv_t vglVertex3sv;
GLAPI vglVertex4dv_t vglVertex4dv;
GLAPI vglVertex4fv_t vglVertex4fv;
GLAPI vglVertex4iv_t vglVertex4iv;
GLAPI vglVertex4sv_t vglVertex4sv;
GLAPI vglNormal3b_t vglNormal3b;
GLAPI vglNormal3d_t vglNormal3d;
GLAPI vglNormal3f_t vglNormal3f;
GLAPI vglNormal3i_t vglNormal3i;
GLAPI vglNormal3s_t vglNormal3s;
GLAPI vglNormal3bv_t vglNormal3bv;
GLAPI vglNormal3dv_t vglNormal3dv;
GLAPI vglNormal3fv_t vglNormal3fv;
GLAPI vglNormal3iv_t vglNormal3iv;
GLAPI vglNormal3sv_t vglNormal3sv;
GLAPI vglIndexd_t vglIndexd;
GLAPI vglIndexf_t vglIndexf;
GLAPI vglIndexi_t vglIndexi;
GLAPI vglIndexs_t vglIndexs;
GLAPI vglIndexub_t vglIndexub;
GLAPI vglIndexdv_t vglIndexdv;
GLAPI vglIndexfv_t vglIndexfv;
GLAPI vglIndexiv_t vglIndexiv;
GLAPI vglIndexsv_t vglIndexsv;
GLAPI vglIndexubv_t vglIndexubv;
GLAPI vglColor3b_t vglColor3b;
GLAPI vglColor3d_t vglColor3d;
GLAPI vglColor3f_t vglColor3f;
GLAPI vglColor3i_t vglColor3i;
GLAPI vglColor3s_t vglColor3s;
GLAPI vglColor3ub_t vglColor3ub;
GLAPI vglColor3ui_t vglColor3ui;
GLAPI vglColor3us_t vglColor3us;
GLAPI vglColor4b_t vglColor4b;
GLAPI vglColor4d_t vglColor4d;
GLAPI vglColor4f_t vglColor4f;
GLAPI vglColor4i_t vglColor4i;
GLAPI vglColor4s_t vglColor4s;
GLAPI vglColor4ub_t vglColor4ub;
GLAPI vglColor4ui_t vglColor4ui;
GLAPI vglColor4us_t vglColor4us;
GLAPI vglColor3bv_t vglColor3bv;
GLAPI vglColor3dv_t vglColor3dv;
GLAPI vglColor3fv_t vglColor3fv;
GLAPI vglColor3iv_t vglColor3iv;
GLAPI vglColor3sv_t vglColor3sv;
GLAPI vglColor3ubv_t vglColor3ubv;
GLAPI vglColor3uiv_t vglColor3uiv;
GLAPI vglColor3usv_t vglColor3usv;
GLAPI vglColor4bv_t vglColor4bv;
GLAPI vglColor4dv_t vglColor4dv;
GLAPI vglColor4fv_t vglColor4fv;
GLAPI vglColor4iv_t vglColor4iv;
GLAPI vglColor4sv_t vglColor4sv;
GLAPI vglColor4ubv_t vglColor4ubv;
GLAPI vglColor4uiv_t vglColor4uiv;
GLAPI vglColor4usv_t vglColor4usv;
GLAPI vglTexCoord1d_t vglTexCoord1d;
GLAPI vglTexCoord1f_t vglTexCoord1f;
GLAPI vglTexCoord1i_t vglTexCoord1i;
GLAPI vglTexCoord1s_t vglTexCoord1s;
GLAPI vglTexCoord2d_t vglTexCoord2d;
GLAPI vglTexCoord2f_t vglTexCoord2f;
GLAPI vglTexCoord2i_t vglTexCoord2i;
GLAPI vglTexCoord2s_t vglTexCoord2s;
GLAPI vglTexCoord3d_t vglTexCoord3d;
GLAPI vglTexCoord3f_t vglTexCoord3f;
GLAPI vglTexCoord3i_t vglTexCoord3i;
GLAPI vglTexCoord3s_t vglTexCoord3s;
GLAPI vglTexCoord4d_t vglTexCoord4d;
GLAPI vglTexCoord4f_t vglTexCoord4f;
GLAPI vglTexCoord4i_t vglTexCoord4i;
GLAPI vglTexCoord4s_t vglTexCoord4s;
GLAPI vglTexCoord1dv_t vglTexCoord1dv;
GLAPI vglTexCoord1fv_t vglTexCoord1fv;
GLAPI vglTexCoord1iv_t vglTexCoord1iv;
GLAPI vglTexCoord1sv_t vglTexCoord1sv;
GLAPI vglTexCoord2dv_t vglTexCoord2dv;
GLAPI vglTexCoord2fv_t vglTexCoord2fv;
GLAPI vglTexCoord2iv_t vglTexCoord2iv;
GLAPI vglTexCoord2sv_t vglTexCoord2sv;
GLAPI vglTexCoord3dv_t vglTexCoord3dv;
GLAPI vglTexCoord3fv_t vglTexCoord3fv;
GLAPI vglTexCoord3iv_t vglTexCoord3iv;
GLAPI vglTexCoord3sv_t vglTexCoord3sv;
GLAPI vglTexCoord4dv_t vglTexCoord4dv;
GLAPI vglTexCoord4fv_t vglTexCoord4fv;
GLAPI vglTexCoord4iv_t vglTexCoord4iv;
GLAPI vglTexCoord4sv_t vglTexCoord4sv;
GLAPI vglRasterPos2d_t vglRasterPos2d;
GLAPI vglRasterPos2f_t vglRasterPos2f;
GLAPI vglRasterPos2i_t vglRasterPos2i;
GLAPI vglRasterPos2s_t vglRasterPos2s;
GLAPI vglRasterPos3d_t vglRasterPos3d;
GLAPI vglRasterPos3f_t vglRasterPos3f;
GLAPI vglRasterPos3i_t vglRasterPos3i;
GLAPI vglRasterPos3s_t vglRasterPos3s;
GLAPI vglRasterPos4d_t vglRasterPos4d;
GLAPI vglRasterPos4f_t vglRasterPos4f;
GLAPI vglRasterPos4i_t vglRasterPos4i;
GLAPI vglRasterPos4s_t vglRasterPos4s;
GLAPI vglRasterPos2dv_t vglRasterPos2dv;
GLAPI vglRasterPos2fv_t vglRasterPos2fv;
GLAPI vglRasterPos2iv_t vglRasterPos2iv;
GLAPI vglRasterPos2sv_t vglRasterPos2sv;
GLAPI vglRasterPos3dv_t vglRasterPos3dv;
GLAPI vglRasterPos3fv_t vglRasterPos3fv;
GLAPI vglRasterPos3iv_t vglRasterPos3iv;
GLAPI vglRasterPos3sv_t vglRasterPos3sv;
GLAPI vglRasterPos4dv_t vglRasterPos4dv;
GLAPI vglRasterPos4fv_t vglRasterPos4fv;
GLAPI vglRasterPos4iv_t vglRasterPos4iv;
GLAPI vglRasterPos4sv_t vglRasterPos4sv;
GLAPI vglRectd_t vglRectd;
GLAPI vglRectf_t vglRectf;
GLAPI vglRecti_t vglRecti;
GLAPI vglRects_t vglRects;
GLAPI vglRectdv_t vglRectdv;
GLAPI vglRectfv_t vglRectfv;
GLAPI vglRectiv_t vglRectiv;
GLAPI vglRectsv_t vglRectsv;
GLAPI vglVertexPointer_t vglVertexPointer;
GLAPI vglNormalPointer_t vglNormalPointer;
GLAPI vglColorPointer_t vglColorPointer;
GLAPI vglIndexPointer_t vglIndexPointer;
GLAPI vglTexCoordPointer_t vglTexCoordPointer;
GLAPI vglEdgeFlagPointer_t vglEdgeFlagPointer;
GLAPI vglGetPointerv_t vglGetPointerv;
GLAPI vglArrayElement_t vglArrayElement;
GLAPI vglDrawArrays_t vglDrawArrays;
GLAPI vglDrawElements_t vglDrawElements;
GLAPI vglInterleavedArrays_t vglInterleavedArrays;
GLAPI vglShadeModel_t vglShadeModel;
GLAPI vglLightf_t vglLightf;
GLAPI vglLighti_t vglLighti;
GLAPI vglLightfv_t vglLightfv;
GLAPI vglLightiv_t vglLightiv;
GLAPI vglGetLightfv_t vglGetLightfv;
GLAPI vglGetLightiv_t vglGetLightiv;
GLAPI vglLightModelf_t vglLightModelf;
GLAPI vglLightModeli_t vglLightModeli;
GLAPI vglLightModelfv_t vglLightModelfv;
GLAPI vglLightModeliv_t vglLightModeliv;
GLAPI vglMaterialf_t vglMaterialf;
GLAPI vglMateriali_t vglMateriali;
GLAPI vglMaterialfv_t vglMaterialfv;
GLAPI vglMaterialiv_t vglMaterialiv;
GLAPI vglGetMaterialfv_t vglGetMaterialfv;
GLAPI vglGetMaterialiv_t vglGetMaterialiv;
GLAPI vglColorMaterial_t vglColorMaterial;
GLAPI vglPixelZoom_t vglPixelZoom;
GLAPI vglPixelStoref_t vglPixelStoref;
GLAPI vglPixelStorei_t vglPixelStorei;
GLAPI vglPixelTransferf_t vglPixelTransferf;
GLAPI vglPixelTransferi_t vglPixelTransferi;
GLAPI vglPixelMapfv_t vglPixelMapfv;
GLAPI vglPixelMapuiv_t vglPixelMapuiv;
GLAPI vglPixelMapusv_t vglPixelMapusv;
GLAPI vglGetPixelMapfv_t vglGetPixelMapfv;
GLAPI vglGetPixelMapuiv_t vglGetPixelMapuiv;
GLAPI vglGetPixelMapusv_t vglGetPixelMapusv;
GLAPI vglBitmap_t vglBitmap;
GLAPI vglReadPixels_t vglReadPixels;
GLAPI vglDrawPixels_t vglDrawPixels;
GLAPI vglCopyPixels_t vglCopyPixels;
GLAPI vglStencilFunc_t vglStencilFunc;
GLAPI vglStencilMask_t vglStencilMask;
GLAPI vglStencilOp_t vglStencilOp;
GLAPI vglClearStencil_t vglClearStencil;
GLAPI vglTexGend_t vglTexGend;
GLAPI vglTexGenf_t vglTexGenf;
GLAPI vglTexGeni_t vglTexGeni;
GLAPI vglTexGendv_t vglTexGendv;
GLAPI vglTexGenfv_t vglTexGenfv;
GLAPI vglTexGeniv_t vglTexGeniv;
GLAPI vglGetTexGendv_t vglGetTexGendv;
GLAPI vglGetTexGenfv_t vglGetTexGenfv;
GLAPI vglGetTexGeniv_t vglGetTexGeniv;
GLAPI vglTexEnvf_t vglTexEnvf;
GLAPI vglTexEnvi_t vglTexEnvi;
GLAPI vglTexEnvfv_t vglTexEnvfv;
GLAPI vglTexEnviv_t vglTexEnviv;
GLAPI vglGetTexEnvfv_t vglGetTexEnvfv;
GLAPI vglGetTexEnviv_t vglGetTexEnviv;
GLAPI vglTexParameterf_t vglTexParameterf;
GLAPI vglTexParameteri_t vglTexParameteri;
GLAPI vglTexParameterfv_t vglTexParameterfv;
GLAPI vglTexParameteriv_t vglTexParameteriv;
GLAPI vglGetTexParameterfv_t vglGetTexParameterfv;
GLAPI vglGetTexParameteriv_t vglGetTexParameteriv;
GLAPI vglGetTexLevelParameterfv_t vglGetTexLevelParameterfv;
GLAPI vglGetTexLevelParameteriv_t vglGetTexLevelParameteriv;
GLAPI vglTexImage1D_t vglTexImage1D;
GLAPI vglTexImage2D_t vglTexImage2D;
GLAPI vglGetTexImage_t vglGetTexImage;
GLAPI vglGenTextures_t vglGenTextures;
GLAPI vglDeleteTextures_t vglDeleteTextures;
GLAPI vglBindTexture_t vglBindTexture;
GLAPI vglPrioritizeTextures_t vglPrioritizeTextures;
GLAPI vglAreTexturesResident_t vglAreTexturesResident;
GLAPI vglIsTexture_t vglIsTexture;
GLAPI vglTexSubImage1D_t vglTexSubImage1D;
GLAPI vglTexSubImage2D_t vglTexSubImage2D;
GLAPI vglCopyTexImage1D_t vglCopyTexImage1D;
GLAPI vglCopyTexImage2D_t vglCopyTexImage2D;
GLAPI vglCopyTexSubImage1D_t vglCopyTexSubImage1D;
GLAPI vglCopyTexSubImage2D_t vglCopyTexSubImage2D;
GLAPI vglMap1d_t vglMap1d;
GLAPI vglMap1f_t vglMap1f;
GLAPI vglMap2d_t vglMap2d;
GLAPI vglMap2f_t vglMap2f;
GLAPI vglGetMapdv_t vglGetMapdv;
GLAPI vglGetMapfv_t vglGetMapfv;
GLAPI vglGetMapiv_t vglGetMapiv;
GLAPI vglEvalCoord1d_t vglEvalCoord1d;
GLAPI vglEvalCoord1f_t vglEvalCoord1f;
GLAPI vglEvalCoord1dv_t vglEvalCoord1dv;
GLAPI vglEvalCoord1fv_t vglEvalCoord1fv;
GLAPI vglEvalCoord2d_t vglEvalCoord2d;
GLAPI vglEvalCoord2f_t vglEvalCoord2f;
GLAPI vglEvalCoord2dv_t vglEvalCoord2dv;
GLAPI vglEvalCoord2fv_t vglEvalCoord2fv;
GLAPI vglMapGrid1d_t vglMapGrid1d;
GLAPI vglMapGrid1f_t vglMapGrid1f;
GLAPI vglMapGrid2d_t vglMapGrid2d;
GLAPI vglMapGrid2f_t vglMapGrid2f;
GLAPI vglEvalPoint1_t vglEvalPoint1;
GLAPI vglEvalPoint2_t vglEvalPoint2;
GLAPI vglEvalMesh1_t vglEvalMesh1;
GLAPI vglEvalMesh2_t vglEvalMesh2;
GLAPI vglFogf_t vglFogf;
GLAPI vglFogi_t vglFogi;
GLAPI vglFogfv_t vglFogfv;
GLAPI vglFogiv_t vglFogiv;
GLAPI vglFeedbackBuffer_t vglFeedbackBuffer;
GLAPI vglPassThrough_t vglPassThrough;
GLAPI vglSelectBuffer_t vglSelectBuffer;
GLAPI vglInitNames_t vglInitNames;
GLAPI vglLoadName_t vglLoadName;
GLAPI vglPushName_t vglPushName;
GLAPI vglPopName_t vglPopName;
GLAPI vglActiveTextureARB_t vglActiveTextureARB;
GLAPI vglClientActiveTextureARB_t vglClientActiveTextureARB;
GLAPI vglMultiTexCoord1dARB_t vglMultiTexCoord1dARB;
GLAPI vglMultiTexCoord1dvARB_t vglMultiTexCoord1dvARB;
GLAPI vglMultiTexCoord1fARB_t vglMultiTexCoord1fARB;
GLAPI vglMultiTexCoord1fvARB_t vglMultiTexCoord1fvARB;
GLAPI vglMultiTexCoord1iARB_t vglMultiTexCoord1iARB;
GLAPI vglMultiTexCoord1ivARB_t vglMultiTexCoord1ivARB;
GLAPI vglMultiTexCoord1sARB_t vglMultiTexCoord1sARB;
GLAPI vglMultiTexCoord1svARB_t vglMultiTexCoord1svARB;
GLAPI vglMultiTexCoord2dARB_t vglMultiTexCoord2dARB;
GLAPI vglMultiTexCoord2dvARB_t vglMultiTexCoord2dvARB;
GLAPI vglMultiTexCoord2fARB_t vglMultiTexCoord2fARB;
GLAPI vglMultiTexCoord2fvARB_t vglMultiTexCoord2fvARB;
GLAPI vglMultiTexCoord2iARB_t vglMultiTexCoord2iARB;
GLAPI vglMultiTexCoord2ivARB_t vglMultiTexCoord2ivARB;
GLAPI vglMultiTexCoord2sARB_t vglMultiTexCoord2sARB;
GLAPI vglMultiTexCoord2svARB_t vglMultiTexCoord2svARB;
GLAPI vglMultiTexCoord3dARB_t vglMultiTexCoord3dARB;
GLAPI vglMultiTexCoord3dvARB_t vglMultiTexCoord3dvARB;
GLAPI vglMultiTexCoord3fARB_t vglMultiTexCoord3fARB;
GLAPI vglMultiTexCoord3fvARB_t vglMultiTexCoord3fvARB;
GLAPI vglMultiTexCoord3iARB_t vglMultiTexCoord3iARB;
GLAPI vglMultiTexCoord3ivARB_t vglMultiTexCoord3ivARB;
GLAPI vglMultiTexCoord3sARB_t vglMultiTexCoord3sARB;
GLAPI vglMultiTexCoord3svARB_t vglMultiTexCoord3svARB;
GLAPI vglMultiTexCoord4dARB_t vglMultiTexCoord4dARB;
GLAPI vglMultiTexCoord4dvARB_t vglMultiTexCoord4dvARB;
GLAPI vglMultiTexCoord4fARB_t vglMultiTexCoord4fARB;
GLAPI vglMultiTexCoord4fvARB_t vglMultiTexCoord4fvARB;
GLAPI vglMultiTexCoord4iARB_t vglMultiTexCoord4iARB;
GLAPI vglMultiTexCoord4ivARB_t vglMultiTexCoord4ivARB;
GLAPI vglMultiTexCoord4sARB_t vglMultiTexCoord4sARB;
GLAPI vglMultiTexCoord4svARB_t vglMultiTexCoord4svARB;
GLAPI vglPointParameterfEXT_t vglPointParameterfEXT;
GLAPI vglPointParameterfvEXT_t vglPointParameterfvEXT;
GLAPI vglBlendFuncSeparateINGR_t vglBlendFuncSeparateINGR;
GLAPI vglDrawRangeElements_t vglDrawRangeElements;
GLAPI vglTexImage3D_t vglTexImage3D;
GLAPI vglTexSubImage3D_t vglTexSubImage3D;
GLAPI vglCopyTexSubImage3D_t vglCopyTexSubImage3D;
GLAPI vglHistogram_t vglHistogram;
GLAPI vglResetHistogram_t vglResetHistogram;
GLAPI vglGetHistogram_t vglGetHistogram;
GLAPI vglGetHistogramParameterfv_t vglGetHistogramParameterfv;
GLAPI vglGetHistogramParameteriv_t vglGetHistogramParameteriv;
GLAPI vglMinmax_t vglMinmax;
GLAPI vglResetMinmax_t vglResetMinmax;
GLAPI vglGetMinmaxParameterfv_t vglGetMinmaxParameterfv;
GLAPI vglGetMinmaxParameteriv_t vglGetMinmaxParameteriv;
GLAPI vglConvolutionFilter1D_t vglConvolutionFilter1D;
GLAPI vglConvolutionFilter2D_t vglConvolutionFilter2D;
GLAPI vglConvolutionParameterf_t vglConvolutionParameterf;
GLAPI vglConvolutionParameterfv_t vglConvolutionParameterfv;
GLAPI vglConvolutionParameteri_t vglConvolutionParameteri;
GLAPI vglConvolutionParameteriv_t vglConvolutionParameteriv;
GLAPI vglCopyConvolutionFilter1D_t vglCopyConvolutionFilter1D;
GLAPI vglCopyConvolutionFilter2D_t vglCopyConvolutionFilter2D;
GLAPI vglGetConvolutionFilter_t vglGetConvolutionFilter;
GLAPI vglGetConvolutionParameterfv_t vglGetConvolutionParameterfv;
GLAPI vglGetConvolutionParameteriv_t vglGetConvolutionParameteriv;
GLAPI vglSeparableFilter2D_t vglSeparableFilter2D;
GLAPI vglGetSeparableFilter_t vglGetSeparableFilter;
GLAPI vglCopyColorSubTable_t vglCopyColorSubTable;
GLAPI vglCopyColorTable_t vglCopyColorTable;

// old gl api

GLAPI glClearIndex_t glClearIndex;
GLAPI glClearColor_t glClearColor;
GLAPI glClear_t glClear;
GLAPI glIndexMask_t glIndexMask;
GLAPI glColorMask_t glColorMask;
GLAPI glAlphaFunc_t glAlphaFunc;
GLAPI glBlendFunc_t glBlendFunc;
GLAPI glLogicOp_t glLogicOp;
GLAPI glCullFace_t glCullFace;
GLAPI glFrontFace_t glFrontFace;
GLAPI glPointSize_t glPointSize;
GLAPI glLineWidth_t glLineWidth;
GLAPI glLineStipple_t glLineStipple;
GLAPI glPolygonMode_t glPolygonMode;
GLAPI glPolygonOffset_t glPolygonOffset;
GLAPI glPolygonStipple_t glPolygonStipple;
GLAPI glGetPolygonStipple_t glGetPolygonStipple;
GLAPI glEdgeFlag_t glEdgeFlag;
GLAPI glEdgeFlagv_t glEdgeFlagv;
GLAPI glScissor_t glScissor;
GLAPI glClipPlane_t glClipPlane;
GLAPI glGetClipPlane_t glGetClipPlane;
GLAPI glDrawBuffer_t glDrawBuffer;
GLAPI glReadBuffer_t glReadBuffer;
GLAPI glEnable_t glEnable;
GLAPI glDisable_t glDisable;
GLAPI glIsEnabled_t glIsEnabled;
GLAPI glEnableClientState_t glEnableClientState;
GLAPI glDisableClientState_t glDisableClientState;
GLAPI glGetBooleanv_t glGetBooleanv;
GLAPI glGetDoublev_t glGetDoublev;
GLAPI glGetFloatv_t glGetFloatv;
GLAPI glGetIntegerv_t glGetIntegerv;
GLAPI glPushAttrib_t glPushAttrib;
GLAPI glPopAttrib_t glPopAttrib;
GLAPI glPushClientAttrib_t glPushClientAttrib;
GLAPI glPopClientAttrib_t glPopClientAttrib;
GLAPI glRenderMode_t glRenderMode;
GLAPI glGetError_t glGetError;
GLAPI glGetString_t glGetString;
GLAPI glFinish_t glFinish;
GLAPI glFlush_t glFlush;
GLAPI glHint_t glHint;
GLAPI glClearDepth_t glClearDepth;
GLAPI glDepthFunc_t glDepthFunc;
GLAPI glDepthMask_t glDepthMask;
GLAPI glDepthRange_t glDepthRange;
GLAPI glClearAccum_t glClearAccum;
GLAPI glAccum_t glAccum;
GLAPI glMatrixMode_t glMatrixMode;
GLAPI glOrtho_t glOrtho;
GLAPI glFrustum_t glFrustum;
GLAPI glViewport_t glViewport;
GLAPI glPushMatrix_t glPushMatrix;
GLAPI glPopMatrix_t glPopMatrix;
GLAPI glLoadIdentity_t glLoadIdentity;
GLAPI glLoadMatrixd_t glLoadMatrixd;
GLAPI glLoadMatrixf_t glLoadMatrixf;
GLAPI glMultMatrixd_t glMultMatrixd;
GLAPI glMultMatrixf_t glMultMatrixf;
GLAPI glRotated_t glRotated;
GLAPI glRotatef_t glRotatef;
GLAPI glScaled_t glScaled;
GLAPI glScalef_t glScalef;
GLAPI glTranslated_t glTranslated;
GLAPI glTranslatef_t glTranslatef;
GLAPI glIsList_t glIsList;
GLAPI glDeleteLists_t glDeleteLists;
GLAPI glGenLists_t glGenLists;
GLAPI glNewList_t glNewList;
GLAPI glEndList_t glEndList;
GLAPI glCallList_t glCallList;
GLAPI glCallLists_t glCallLists;
GLAPI glListBase_t glListBase;
GLAPI glBegin_t glBegin;
GLAPI glEnd_t glEnd;
GLAPI glVertex2d_t glVertex2d;
GLAPI glVertex2f_t glVertex2f;
GLAPI glVertex2i_t glVertex2i;
GLAPI glVertex2s_t glVertex2s;
GLAPI glVertex3d_t glVertex3d;
GLAPI glVertex3f_t glVertex3f;
GLAPI glVertex3i_t glVertex3i;
GLAPI glVertex3s_t glVertex3s;
GLAPI glVertex4d_t glVertex4d;
GLAPI glVertex4f_t glVertex4f;
GLAPI glVertex4i_t glVertex4i;
GLAPI glVertex4s_t glVertex4s;
GLAPI glVertex2dv_t glVertex2dv;
GLAPI glVertex2fv_t glVertex2fv;
GLAPI glVertex2iv_t glVertex2iv;
GLAPI glVertex2sv_t glVertex2sv;
GLAPI glVertex3dv_t glVertex3dv;
GLAPI glVertex3fv_t glVertex3fv;
GLAPI glVertex3iv_t glVertex3iv;
GLAPI glVertex3sv_t glVertex3sv;
GLAPI glVertex4dv_t glVertex4dv;
GLAPI glVertex4fv_t glVertex4fv;
GLAPI glVertex4iv_t glVertex4iv;
GLAPI glVertex4sv_t glVertex4sv;
GLAPI glNormal3b_t glNormal3b;
GLAPI glNormal3d_t glNormal3d;
GLAPI glNormal3f_t glNormal3f;
GLAPI glNormal3i_t glNormal3i;
GLAPI glNormal3s_t glNormal3s;
GLAPI glNormal3bv_t glNormal3bv;
GLAPI glNormal3dv_t glNormal3dv;
GLAPI glNormal3fv_t glNormal3fv;
GLAPI glNormal3iv_t glNormal3iv;
GLAPI glNormal3sv_t glNormal3sv;
GLAPI glIndexd_t glIndexd;
GLAPI glIndexf_t glIndexf;
GLAPI glIndexi_t glIndexi;
GLAPI glIndexs_t glIndexs;
GLAPI glIndexub_t glIndexub;
GLAPI glIndexdv_t glIndexdv;
GLAPI glIndexfv_t glIndexfv;
GLAPI glIndexiv_t glIndexiv;
GLAPI glIndexsv_t glIndexsv;
GLAPI glIndexubv_t glIndexubv;
GLAPI glColor3b_t glColor3b;
GLAPI glColor3d_t glColor3d;
GLAPI glColor3f_t glColor3f;
GLAPI glColor3i_t glColor3i;
GLAPI glColor3s_t glColor3s;
GLAPI glColor3ub_t glColor3ub;
GLAPI glColor3ui_t glColor3ui;
GLAPI glColor3us_t glColor3us;
GLAPI glColor4b_t glColor4b;
GLAPI glColor4d_t glColor4d;
GLAPI glColor4f_t glColor4f;
GLAPI glColor4i_t glColor4i;
GLAPI glColor4s_t glColor4s;
GLAPI glColor4ub_t glColor4ub;
GLAPI glColor4ui_t glColor4ui;
GLAPI glColor4us_t glColor4us;
GLAPI glColor3bv_t glColor3bv;
GLAPI glColor3dv_t glColor3dv;
GLAPI glColor3fv_t glColor3fv;
GLAPI glColor3iv_t glColor3iv;
GLAPI glColor3sv_t glColor3sv;
GLAPI glColor3ubv_t glColor3ubv;
GLAPI glColor3uiv_t glColor3uiv;
GLAPI glColor3usv_t glColor3usv;
GLAPI glColor4bv_t glColor4bv;
GLAPI glColor4dv_t glColor4dv;
GLAPI glColor4fv_t glColor4fv;
GLAPI glColor4iv_t glColor4iv;
GLAPI glColor4sv_t glColor4sv;
GLAPI glColor4ubv_t glColor4ubv;
GLAPI glColor4uiv_t glColor4uiv;
GLAPI glColor4usv_t glColor4usv;
GLAPI glTexCoord1d_t glTexCoord1d;
GLAPI glTexCoord1f_t glTexCoord1f;
GLAPI glTexCoord1i_t glTexCoord1i;
GLAPI glTexCoord1s_t glTexCoord1s;
GLAPI glTexCoord2d_t glTexCoord2d;
GLAPI glTexCoord2f_t glTexCoord2f;
GLAPI glTexCoord2i_t glTexCoord2i;
GLAPI glTexCoord2s_t glTexCoord2s;
GLAPI glTexCoord3d_t glTexCoord3d;
GLAPI glTexCoord3f_t glTexCoord3f;
GLAPI glTexCoord3i_t glTexCoord3i;
GLAPI glTexCoord3s_t glTexCoord3s;
GLAPI glTexCoord4d_t glTexCoord4d;
GLAPI glTexCoord4f_t glTexCoord4f;
GLAPI glTexCoord4i_t glTexCoord4i;
GLAPI glTexCoord4s_t glTexCoord4s;
GLAPI glTexCoord1dv_t glTexCoord1dv;
GLAPI glTexCoord1fv_t glTexCoord1fv;
GLAPI glTexCoord1iv_t glTexCoord1iv;
GLAPI glTexCoord1sv_t glTexCoord1sv;
GLAPI glTexCoord2dv_t glTexCoord2dv;
GLAPI glTexCoord2fv_t glTexCoord2fv;
GLAPI glTexCoord2iv_t glTexCoord2iv;
GLAPI glTexCoord2sv_t glTexCoord2sv;
GLAPI glTexCoord3dv_t glTexCoord3dv;
GLAPI glTexCoord3fv_t glTexCoord3fv;
GLAPI glTexCoord3iv_t glTexCoord3iv;
GLAPI glTexCoord3sv_t glTexCoord3sv;
GLAPI glTexCoord4dv_t glTexCoord4dv;
GLAPI glTexCoord4fv_t glTexCoord4fv;
GLAPI glTexCoord4iv_t glTexCoord4iv;
GLAPI glTexCoord4sv_t glTexCoord4sv;
GLAPI glRasterPos2d_t glRasterPos2d;
GLAPI glRasterPos2f_t glRasterPos2f;
GLAPI glRasterPos2i_t glRasterPos2i;
GLAPI glRasterPos2s_t glRasterPos2s;
GLAPI glRasterPos3d_t glRasterPos3d;
GLAPI glRasterPos3f_t glRasterPos3f;
GLAPI glRasterPos3i_t glRasterPos3i;
GLAPI glRasterPos3s_t glRasterPos3s;
GLAPI glRasterPos4d_t glRasterPos4d;
GLAPI glRasterPos4f_t glRasterPos4f;
GLAPI glRasterPos4i_t glRasterPos4i;
GLAPI glRasterPos4s_t glRasterPos4s;
GLAPI glRasterPos2dv_t glRasterPos2dv;
GLAPI glRasterPos2fv_t glRasterPos2fv;
GLAPI glRasterPos2iv_t glRasterPos2iv;
GLAPI glRasterPos2sv_t glRasterPos2sv;
GLAPI glRasterPos3dv_t glRasterPos3dv;
GLAPI glRasterPos3fv_t glRasterPos3fv;
GLAPI glRasterPos3iv_t glRasterPos3iv;
GLAPI glRasterPos3sv_t glRasterPos3sv;
GLAPI glRasterPos4dv_t glRasterPos4dv;
GLAPI glRasterPos4fv_t glRasterPos4fv;
GLAPI glRasterPos4iv_t glRasterPos4iv;
GLAPI glRasterPos4sv_t glRasterPos4sv;
GLAPI glRectd_t glRectd;
GLAPI glRectf_t glRectf;
GLAPI glRecti_t glRecti;
GLAPI glRects_t glRects;
GLAPI glRectdv_t glRectdv;
GLAPI glRectfv_t glRectfv;
GLAPI glRectiv_t glRectiv;
GLAPI glRectsv_t glRectsv;
GLAPI glVertexPointer_t glVertexPointer;
GLAPI glNormalPointer_t glNormalPointer;
GLAPI glColorPointer_t glColorPointer;
GLAPI glIndexPointer_t glIndexPointer;
GLAPI glTexCoordPointer_t glTexCoordPointer;
GLAPI glEdgeFlagPointer_t glEdgeFlagPointer;
GLAPI glGetPointerv_t glGetPointerv;
GLAPI glArrayElement_t glArrayElement;
GLAPI glDrawArrays_t glDrawArrays;
GLAPI glDrawElements_t glDrawElements;
GLAPI glInterleavedArrays_t glInterleavedArrays;
GLAPI glShadeModel_t glShadeModel;
GLAPI glLightf_t glLightf;
GLAPI glLighti_t glLighti;
GLAPI glLightfv_t glLightfv;
GLAPI glLightiv_t glLightiv;
GLAPI glGetLightfv_t glGetLightfv;
GLAPI glGetLightiv_t glGetLightiv;
GLAPI glLightModelf_t glLightModelf;
GLAPI glLightModeli_t glLightModeli;
GLAPI glLightModelfv_t glLightModelfv;
GLAPI glLightModeliv_t glLightModeliv;
GLAPI glMaterialf_t glMaterialf;
GLAPI glMateriali_t glMateriali;
GLAPI glMaterialfv_t glMaterialfv;
GLAPI glMaterialiv_t glMaterialiv;
GLAPI glGetMaterialfv_t glGetMaterialfv;
GLAPI glGetMaterialiv_t glGetMaterialiv;
GLAPI glColorMaterial_t glColorMaterial;
GLAPI glPixelZoom_t glPixelZoom;
GLAPI glPixelStoref_t glPixelStoref;
GLAPI glPixelStorei_t glPixelStorei;
GLAPI glPixelTransferf_t glPixelTransferf;
GLAPI glPixelTransferi_t glPixelTransferi;
GLAPI glPixelMapfv_t glPixelMapfv;
GLAPI glPixelMapuiv_t glPixelMapuiv;
GLAPI glPixelMapusv_t glPixelMapusv;
GLAPI glGetPixelMapfv_t glGetPixelMapfv;
GLAPI glGetPixelMapuiv_t glGetPixelMapuiv;
GLAPI glGetPixelMapusv_t glGetPixelMapusv;
GLAPI glBitmap_t glBitmap;
GLAPI glReadPixels_t glReadPixels;
GLAPI glDrawPixels_t glDrawPixels;
GLAPI glCopyPixels_t glCopyPixels;
GLAPI glStencilFunc_t glStencilFunc;
GLAPI glStencilMask_t glStencilMask;
GLAPI glStencilOp_t glStencilOp;
GLAPI glClearStencil_t glClearStencil;
GLAPI glTexGend_t glTexGend;
GLAPI glTexGenf_t glTexGenf;
GLAPI glTexGeni_t glTexGeni;
GLAPI glTexGendv_t glTexGendv;
GLAPI glTexGenfv_t glTexGenfv;
GLAPI glTexGeniv_t glTexGeniv;
GLAPI glGetTexGendv_t glGetTexGendv;
GLAPI glGetTexGenfv_t glGetTexGenfv;
GLAPI glGetTexGeniv_t glGetTexGeniv;
GLAPI glTexEnvf_t glTexEnvf;
GLAPI glTexEnvi_t glTexEnvi;
GLAPI glTexEnvfv_t glTexEnvfv;
GLAPI glTexEnviv_t glTexEnviv;
GLAPI glGetTexEnvfv_t glGetTexEnvfv;
GLAPI glGetTexEnviv_t glGetTexEnviv;
GLAPI glTexParameterf_t glTexParameterf;
GLAPI glTexParameteri_t glTexParameteri;
GLAPI glTexParameterfv_t glTexParameterfv;
GLAPI glTexParameteriv_t glTexParameteriv;
GLAPI glGetTexParameterfv_t glGetTexParameterfv;
GLAPI glGetTexParameteriv_t glGetTexParameteriv;
GLAPI glGetTexLevelParameterfv_t glGetTexLevelParameterfv;
GLAPI glGetTexLevelParameteriv_t glGetTexLevelParameteriv;
GLAPI glTexImage1D_t glTexImage1D;
GLAPI glTexImage2D_t glTexImage2D;
GLAPI glGetTexImage_t glGetTexImage;
GLAPI glGenTextures_t glGenTextures;
GLAPI glDeleteTextures_t glDeleteTextures;
GLAPI glBindTexture_t glBindTexture;
GLAPI glPrioritizeTextures_t glPrioritizeTextures;
GLAPI glAreTexturesResident_t glAreTexturesResident;
GLAPI glIsTexture_t glIsTexture;
GLAPI glTexSubImage1D_t glTexSubImage1D;
GLAPI glTexSubImage2D_t glTexSubImage2D;
GLAPI glCopyTexImage1D_t glCopyTexImage1D;
GLAPI glCopyTexImage2D_t glCopyTexImage2D;
GLAPI glCopyTexSubImage1D_t glCopyTexSubImage1D;
GLAPI glCopyTexSubImage2D_t glCopyTexSubImage2D;
GLAPI glMap1d_t glMap1d;
GLAPI glMap1f_t glMap1f;
GLAPI glMap2d_t glMap2d;
GLAPI glMap2f_t glMap2f;
GLAPI glGetMapdv_t glGetMapdv;
GLAPI glGetMapfv_t glGetMapfv;
GLAPI glGetMapiv_t glGetMapiv;
GLAPI glEvalCoord1d_t glEvalCoord1d;
GLAPI glEvalCoord1f_t glEvalCoord1f;
GLAPI glEvalCoord1dv_t glEvalCoord1dv;
GLAPI glEvalCoord1fv_t glEvalCoord1fv;
GLAPI glEvalCoord2d_t glEvalCoord2d;
GLAPI glEvalCoord2f_t glEvalCoord2f;
GLAPI glEvalCoord2dv_t glEvalCoord2dv;
GLAPI glEvalCoord2fv_t glEvalCoord2fv;
GLAPI glMapGrid1d_t glMapGrid1d;
GLAPI glMapGrid1f_t glMapGrid1f;
GLAPI glMapGrid2d_t glMapGrid2d;
GLAPI glMapGrid2f_t glMapGrid2f;
GLAPI glEvalPoint1_t glEvalPoint1;
GLAPI glEvalPoint2_t glEvalPoint2;
GLAPI glEvalMesh1_t glEvalMesh1;
GLAPI glEvalMesh2_t glEvalMesh2;
GLAPI glFogf_t glFogf;
GLAPI glFogi_t glFogi;
GLAPI glFogfv_t glFogfv;
GLAPI glFogiv_t glFogiv;
GLAPI glFeedbackBuffer_t glFeedbackBuffer;
GLAPI glPassThrough_t glPassThrough;
GLAPI glSelectBuffer_t glSelectBuffer;
GLAPI glInitNames_t glInitNames;
GLAPI glLoadName_t glLoadName;
GLAPI glPushName_t glPushName;
GLAPI glPopName_t glPopName;
GLAPI glMultiTexCoord1dARB_t glMultiTexCoord1dARB;
GLAPI glMultiTexCoord1dvARB_t glMultiTexCoord1dvARB;
GLAPI glMultiTexCoord1fARB_t glMultiTexCoord1fARB;
GLAPI glMultiTexCoord1fvARB_t glMultiTexCoord1fvARB;
GLAPI glMultiTexCoord1iARB_t glMultiTexCoord1iARB;
GLAPI glMultiTexCoord1ivARB_t glMultiTexCoord1ivARB;
GLAPI glMultiTexCoord1sARB_t glMultiTexCoord1sARB;
GLAPI glMultiTexCoord1svARB_t glMultiTexCoord1svARB;
GLAPI glMultiTexCoord2dARB_t glMultiTexCoord2dARB;
GLAPI glMultiTexCoord2dvARB_t glMultiTexCoord2dvARB;

GLAPI glMultiTexCoord2iARB_t glMultiTexCoord2iARB;
GLAPI glMultiTexCoord2ivARB_t glMultiTexCoord2ivARB;
GLAPI glMultiTexCoord2sARB_t glMultiTexCoord2sARB;
GLAPI glMultiTexCoord2svARB_t glMultiTexCoord2svARB;
GLAPI glMultiTexCoord3dARB_t glMultiTexCoord3dARB;
GLAPI glMultiTexCoord3dvARB_t glMultiTexCoord3dvARB;
GLAPI glMultiTexCoord3fARB_t glMultiTexCoord3fARB;
GLAPI glMultiTexCoord3fvARB_t glMultiTexCoord3fvARB;
GLAPI glMultiTexCoord3iARB_t glMultiTexCoord3iARB;
GLAPI glMultiTexCoord3ivARB_t glMultiTexCoord3ivARB;
GLAPI glMultiTexCoord3sARB_t glMultiTexCoord3sARB;
GLAPI glMultiTexCoord3svARB_t glMultiTexCoord3svARB;
GLAPI glMultiTexCoord4dARB_t glMultiTexCoord4dARB;
GLAPI glMultiTexCoord4dvARB_t glMultiTexCoord4dvARB;
GLAPI glMultiTexCoord4fARB_t glMultiTexCoord4fARB;
GLAPI glMultiTexCoord4fvARB_t glMultiTexCoord4fvARB;
GLAPI glMultiTexCoord4iARB_t glMultiTexCoord4iARB;
GLAPI glMultiTexCoord4ivARB_t glMultiTexCoord4ivARB;
GLAPI glMultiTexCoord4sARB_t glMultiTexCoord4sARB;
GLAPI glMultiTexCoord4svARB_t glMultiTexCoord4svARB;
GLAPI glPointParameterfEXT_t glPointParameterfEXT;
GLAPI glPointParameterfvEXT_t glPointParameterfvEXT;
GLAPI glBlendFuncSeparateINGR_t glBlendFuncSeparateINGR;
GLAPI glDrawRangeElements_t glDrawRangeElements;
GLAPI glTexImage3D_t glTexImage3D;
GLAPI glTexSubImage3D_t glTexSubImage3D;
GLAPI glCopyTexSubImage3D_t glCopyTexSubImage3D;
GLAPI glHistogram_t glHistogram;
GLAPI glResetHistogram_t glResetHistogram;
GLAPI glGetHistogram_t glGetHistogram;
GLAPI glGetHistogramParameterfv_t glGetHistogramParameterfv;
GLAPI glGetHistogramParameteriv_t glGetHistogramParameteriv;
GLAPI glMinmax_t glMinmax;
GLAPI glResetMinmax_t glResetMinmax;
GLAPI glGetMinmaxParameterfv_t glGetMinmaxParameterfv;
GLAPI glGetMinmaxParameteriv_t glGetMinmaxParameteriv;
GLAPI glConvolutionFilter1D_t glConvolutionFilter1D;
GLAPI glConvolutionFilter2D_t glConvolutionFilter2D;
GLAPI glConvolutionParameterf_t glConvolutionParameterf;
GLAPI glConvolutionParameterfv_t glConvolutionParameterfv;
GLAPI glConvolutionParameteri_t glConvolutionParameteri;
GLAPI glConvolutionParameteriv_t glConvolutionParameteriv;
GLAPI glCopyConvolutionFilter1D_t glCopyConvolutionFilter1D;
GLAPI glCopyConvolutionFilter2D_t glCopyConvolutionFilter2D;
GLAPI glGetConvolutionFilter_t glGetConvolutionFilter;
GLAPI glGetConvolutionParameterfv_t glGetConvolutionParameterfv;
GLAPI glGetConvolutionParameteriv_t glGetConvolutionParameteriv;
GLAPI glSeparableFilter2D_t glSeparableFilter2D;
GLAPI glGetSeparableFilter_t glGetSeparableFilter;
GLAPI glCopyColorSubTable_t glCopyColorSubTable;
GLAPI glCopyColorTable_t glCopyColorTable;

#endif

#endif

GLAPI vglActiveTextureARB_t vglActiveTextureARB;
GLAPI vglClientActiveTextureARB_t vglClientActiveTextureARB;
GLAPI vglMultiTexCoord2fARB_t vglMultiTexCoord2fARB;
GLAPI vglMultiTexCoord2fvARB_t vglMultiTexCoord2fvARB;

#ifdef win32_x86
GLAPI glActiveTextureARB_t glActiveTextureARB;
GLAPI glClientActiveTextureARB_t glClientActiveTextureARB;
GLAPI glMultiTexCoord2fARB_t glMultiTexCoord2fARB;
GLAPI glMultiTexCoord2fvARB_t glMultiTexCoord2fvARB;
#endif