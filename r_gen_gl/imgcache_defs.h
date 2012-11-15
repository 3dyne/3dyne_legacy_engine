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



// imgcache_defs.h

#ifndef __imgcache_defs
#define __imgcache_defs

#include "lib_containerdefs.h"
#include "lib_math.h"
#include <gl_defs.h>

// IC_... internal types

typedef struct ic_rect_s
{     
	int	width;
	int	height;

	// valid after caching
	int	xofs;
	int	yofs;
} ic_rect_t;

typedef struct ic_image_s
{
	GLuint	texobj;

	// not really needed
	int	width;
	int	height;

	// unused rectangles in this image
	u_list_t	*empty_rect_list;

	// best empty rectangle, set by IC_ImageTestEmptytRectangles
	// IC_ImageTakeEmptyRectangle will use this one if called
	ic_rect_t	*best_last_rect;

} ic_image_t;

// IC_... public types

typedef struct ic_subimage_s
{
	int	width;
	int	height;
	void	*pixels;

	// set by IC_MgrCacheSubImage
	int	xofs;		// -1 not cached
	int	yofs;
	int	texobj;
} ic_subimage_t;


typedef struct ic_mgr_s
{
	// internal cache images description
	int	width;
	int	height;
	int	maxnum;

	// the cache
	u_list_t	*image_list;

} ic_mgr_t;

typedef enum
{
	ICMgrCacheRes_ok,
	ICMgrCacheRes_invalid_size,
	ICMgrCacheRes_cache_full,
	ICMgrCacheRes_num
} icMgrCacheRes;

/*
  ==================================================
  imgcache utils

  ==================================================
*/

#define ICU_UPLOAD_FRAME_MAX_SUBIMAGES		( 1024*8 )

typedef struct icu_upload_frame_s
{
	int		sub_num;
	ic_subimage_t	*sub_refs[ICU_UPLOAD_FRAME_MAX_SUBIMAGES];
} icu_upload_frame_t;


#endif
