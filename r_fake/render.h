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



// render.h

#ifndef __render
#define __render

#include "r_interfaces.h"

#define LAYER_TEXTURE_1		( 1 )
#define LAYER_DIFFUSE_1		( 1024 )
#define LAYER_SPECULAR_1	( 2048 )

#define OFS_FOR_BASE( _type, _member ) ( (maddr_t)(&(((_type *)0)->_member)) )
#define ADDR_OF_BASE( _type, _member, _member_addr ) ( (_type *) ((maddr_t)(_member_addr) - (maddr_t)(&(((_type *)0)->_member)) ) )

#define LIGHTMAP_FORMAT_8888		// if this is changed, it have to be changed in imgcache.c too

//#define GL_LOCK_ARRAYS_EXT
//#define GL_ARB_MULTITEXTURE_EXT


#define RND	( (_Random()%1000)/1000.0 )

#define	SIZE_X		( 640 )
#define SIZE_Y		( 480 )

//
// a_main.c
//

extern g_map_t *a_map;

//
// r_misc.c
//

void ProjectVec3d( vec2d_t out, vec3d_t in, projectionType type );
void CalcVecsFromAngle( vec2d_t vecs[2], fp_t deg );
unsigned char* Image565ToImage888( unsigned short *in, int pixelnum );
GLuint Misc_GenTexture_TGA_8888( char *name );
void ProjectVec3d( vec2d_t out, vec3d_t in, projectionType type );
void CalcVertex( vec4d_t out, vec3d_t in );
void strcrpl( char *text, char that, char with );	// replace a character in a cstring

projectionType ProjectionTypeFromNorm( vec3d_t norm );

//
// r_field.c
//
void SetupField( char *plane_name, char *node_name );
void RunParticle( void );
void FieldTest( vec3d_t pos );

//
// r_initgl.c
//
void R_InitGL( void );

//
// render3.c
//
void BSP_VisitLeafsInBoundBox( vec3d_t min, vec3d_t max, void (*action_func)(mapnode_t *leaf) );

void J_ObjWarning( hobj_t *obj, const char *text );

//
// r_light.c
//
void R_BeginDlightFrame_sw( void );
void R_EndDlightFrame_sw( void );

//
// r_light_sw.c
//
void R_DlightSetupReMapRamps_sw( vec3d_t color );
void R_DlightCreateTmpFalloffs_sw( ri_local_light_t *dlight, shape_t *shp );
void R_MergeFalloffs_sw( int num, shape_t *shapes[] );
void R_SortFalloffs_sw( void );
void R_ApplyShapesFalloffs_sw( void );
void R_CleanUpShapesFalloffs_sw( void );

#endif
