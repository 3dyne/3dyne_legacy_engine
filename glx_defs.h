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



// glx_defs.h

#ifndef glx_defs_h
#define glx_defs_h

#define GLX_VERSION_1_1		1


/*
 * Tokens for glXChooseVisual and glXGetConfig:
 */
enum _GLX_CONFIGS {
	GLX_USE_GL		= 1,
	GLX_BUFFER_SIZE		= 2,
	GLX_LEVEL		= 3,
	GLX_RGBA		= 4,
	GLX_DOUBLEBUFFER	= 5, 
	GLX_STEREO		= 6,
	GLX_AUX_BUFFERS		= 7,
	GLX_RED_SIZE		= 8,
	GLX_GREEN_SIZE		= 9,
	GLX_BLUE_SIZE		= 10,
	GLX_ALPHA_SIZE		= 11,
	GLX_DEPTH_SIZE		= 12,
	GLX_STENCIL_SIZE	= 13,
	GLX_ACCUM_RED_SIZE	= 14,
	GLX_ACCUM_GREEN_SIZE	= 15,
	GLX_ACCUM_BLUE_SIZE	= 16,
	GLX_ACCUM_ALPHA_SIZE	= 17,

	/* GLX_EXT_visual_info extension */
	GLX_X_VISUAL_TYPE_EXT		= 0x22,
	GLX_TRANSPARENT_TYPE_EXT	= 0x23,
	GLX_TRANSPARENT_INDEX_VALUE_EXT	= 0x24,
	GLX_TRANSPARENT_RED_VALUE_EXT	= 0x25,
	GLX_TRANSPARENT_GREEN_VALUE_EXT	= 0x26,
	GLX_TRANSPARENT_BLUE_VALUE_EXT	= 0x27,
	GLX_TRANSPARENT_ALPHA_VALUE_EXT	= 0x28
};


/*
 * Error codes returned by glXGetConfig:
 */
#define GLX_BAD_SCREEN		1
#define GLX_BAD_ATTRIBUTE	2
#define GLX_NO_EXTENSION	3
#define GLX_BAD_VISUAL		4
#define GLX_BAD_CONTEXT		5
#define GLX_BAD_VALUE       	6
#define GLX_BAD_ENUM		7


/*
 * GLX 1.1 and later:
 */
#define GLX_VENDOR		1
#define GLX_VERSION		2
#define GLX_EXTENSIONS 		3


/*
 * GLX_visual_info extension
 */
#define GLX_TRUE_COLOR_EXT		0x8002
#define GLX_DIRECT_COLOR_EXT		0x8003
#define GLX_PSEUDO_COLOR_EXT		0x8004
#define GLX_STATIC_COLOR_EXT		0x8005
#define GLX_GRAY_SCALE_EXT		0x8006
#define GLX_STATIC_GRAY_EXT		0x8007
#define GLX_NONE_EXT			0x8000
#define GLX_TRANSPARENT_RGB_EXT		0x8008
#define GLX_TRANSPARENT_INDEX_EXT	0x8009

typedef void * GLXContext;
typedef XID GLXPixmap;
typedef XID GLXDrawable;
typedef XID GLXContextID;
#endif
