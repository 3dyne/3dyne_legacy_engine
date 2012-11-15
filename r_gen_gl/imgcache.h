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



// imgcache.h

#ifndef __imgcache
#define __imgcache

#include "imgcache_defs.h"

//
// user interface of the image cache
//

// image cache manager stuff

ic_mgr_t *	IC_NewMgr( int width, int height, int max );
void		IC_MgrCleanUp( ic_mgr_t *mgr );			// flush cache and free resources
void		IC_FreeMgr( ic_mgr_t *mgr );			// free mgr and don't care about unfreed resources


icMgrCacheRes	IC_MgrCacheSubImage( ic_mgr_t *mgr, ic_subimage_t *sub );

// subimage stuff

ic_subimage_t *	IC_NewSubImage( int width, int height );
void		IC_FreeSubImage( ic_subimage_t *sub );
void		IC_SubImageSetPixels( ic_subimage_t *sub, void *pixels );
void		IC_SubImageInsertIntoImage( ic_subimage_t *sub, void *pixels );
void		IC_SubSubImageInsertIntoImage( ic_subimage_t *sub, int xofs, int yofs, int width, int height, void *pixels );

// dynamic light test hack
void		IC_SubImageInsertPixelIntoImage_hack( ic_subimage_t *sub, int x, int y, unsigned char *colors );
//void		IC_SubImageReloadOriginalIntoImage_hack( ic_subimage_t *sub );

bool_t		IC_SubImageIsCached( ic_subimage_t *sub );
void		IC_SubImageInvalidate( ic_subimage_t *sub );	// doesn't really remove from cache, just sets values that it looks like
int		IC_SubImageGetImageGLtexobj( ic_subimage_t *sub );
void		IC_SubImageGetSubImageOffset( ic_subimage_t *sub, vec2d_t ofs );
void		IC_SubImageGetSubImageSize( ic_subimage_t *sub, vec2d_t size );
void		IC_SubImageGetImageSize( ic_subimage_t *sub, vec2d_t size );

// subimage upload frame util
void		ICU_BeginSubImageUploadFrame( icu_upload_frame_t *frame );
void		ICU_EndSubImageUploadFrame( icu_upload_frame_t *frame );
void		ICU_UploadFrameAddSubImage( icu_upload_frame_t *frame, ic_subimage_t *sub );

#endif
