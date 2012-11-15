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



// imgcache.c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <alloca.h>

#include "lib_math.h"
#include "lib_container.h"

#include "r_fake/r_math.h"
#include "g_shared.h"		// for macros
#include "ca_service.h"		// for malloc
#include "shock.h"		// for errors

#include "r_private.h"		// for opengl
#include "u_pixel.h"		// for pixel format convert

#include "imgcache_defs.h"
#include "imgcache.h"

#define IC_INTERNAL_FORMAT	( GL_RGB )
#define IC_MEMORY_FORMAT	( GL_RGBA )

ic_rect_t * IC_NewRectangle( int width, int height, int xofs, int yofs );
void IC_FreeRectangle( ic_rect_t *rect );
ic_image_t * IC_NewImage( int width, int height );
void IC_FreeImage( ic_image_t *img );
int IC_ImageCalcEmptyArea( ic_image_t *img );
int IC_ImageTestEmptyRectangles( ic_image_t *img, int width, int height );
void IC_ImageTakeEmptyRectangle( ic_image_t *img, int width, int height, int *xofs, int *yofs );

/*
  ==================================================
  IC_Mgr...

  ==================================================
*/

/*
  ==============================
  IC_NewMgr

  ==============================
*/
ic_mgr_t * IC_NewMgr( int width, int height, int max )
{
	ic_mgr_t	*mgr;

	mgr = NEWTYPE( ic_mgr_t );

	mgr->width = width;
	mgr->height = height;
	mgr->maxnum = max;

	mgr->image_list = U_NewList();

	return mgr;
}

/*
  ==============================
  IC_FreeMgr

  ==============================
*/
void IC_FreeMgr( ic_mgr_t * mgr )
{
	ic_image_t	*img;

	for( ; U_ListIsNotEmpty( mgr->image_list ) ; )
	{
		img = (ic_image_t *)U_ListRemoveAtHead( mgr->image_list );
		IC_FreeImage( img );
	}

	U_FreeList( mgr->image_list );

	FREE( mgr );
}

/*
  ==============================
  IC_DumpMgr

  ==============================
*/
void IC_DumpMgr( ic_mgr_t * mgr )
{
	u_list_iter_t	iter;
	ic_image_t	*img;
	int		total_empty_area;
	int		total_image_area;

	printf( "IC_DumpMgr:\n" );
	printf( " width: %d\n", mgr->width );
	printf( " height: %d\n", mgr->height );
	printf( " max images: %d\n", mgr->maxnum );
	printf( " used images: %d\n", U_ListLength( mgr->image_list ) );

	if ( U_ListLength( mgr->image_list ) == 0 )
		return;
	
	total_empty_area = 0;
	U_ListIterInit( &iter, mgr->image_list );
	for ( ; ( img = (ic_image_t *)U_ListIterNext( &iter ) ) ; )
	{
		total_empty_area += IC_ImageCalcEmptyArea( img );
	}

	total_image_area = mgr->width * mgr->height * U_ListLength( mgr->image_list );

	printf( " empty area: %.2f%%\n", (total_empty_area*100.0)/(total_image_area*1.0) );
}

/*
  ==============================
  IC_CleanUpMgr

  ==============================
*/
void IC_MgrCleanUp( ic_mgr_t *mgr )
{
	ic_image_t	*img;
	
	for( ; U_ListIsNotEmpty( mgr->image_list ) ; )
	{
		img = (ic_image_t *)U_ListRemoveAtHead( mgr->image_list );
		IC_FreeImage( img );
	}	
}


/*
  ==============================
  IC_MgrCacheSubImage

  ==============================
*/
icMgrCacheRes IC_MgrCacheSubImage( ic_mgr_t *mgr, ic_subimage_t *sub )
{
	int		num;
	u_list_iter_t	iter;
	ic_image_t	*img;

	int		gap_area;
	int		best_gap_area;
	ic_image_t	*best_img;

//	printf( "imgcache request\n" );

	if ( sub->width > mgr->width || sub->height > mgr->height )
	{
		Error( "IC_MgrUpdateSubImage: subimage exceed maximum size\n" );
		return ICMgrCacheRes_invalid_size;
	}

	// first search in all already allocated images for empty areas

	best_gap_area = 0xffffff;
	best_img = NULL;

	U_ListIterInit( &iter, mgr->image_list );
	for ( num = 0; ( img = (ic_image_t *)U_ListIterNext( &iter ) ) ; num++ )
	{
		gap_area = IC_ImageTestEmptyRectangles( img, sub->width, sub->height );

		if ( gap_area != -1 )
		{
			if ( gap_area < best_gap_area )
			{
				best_gap_area = gap_area;
				best_img = img;
			}
		}
	}
	
	if ( best_img )
	{
		IC_ImageTakeEmptyRectangle( best_img, sub->width, sub->height, &sub->xofs, &sub->yofs );
		sub->texobj = best_img->texobj;

//		IC_SubImageInsertIntoImage( sub, sub->pixels );
		return ICMgrCacheRes_ok;
	}

	// none of the empty areas is up to standard, allocate new ic_image
	
	if ( num >= mgr->maxnum )
	{
		// reached maximal number of ic_images
		// __error( "reached maximal number of ic_images\n" );
		return ICMgrCacheRes_cache_full;
	}

	img = IC_NewImage( mgr->width, mgr->height );
	U_ListInsertAtHead( mgr->image_list, img );

	IC_ImageTestEmptyRectangles( img, sub->width, sub->height );
	IC_ImageTakeEmptyRectangle( img, sub->width, sub->height, &sub->xofs, &sub->yofs );
	sub->texobj = img->texobj;

//	IC_SubImageInsertIntoImage( sub, sub->pixels );
	return ICMgrCacheRes_ok;
}

/*
  ==================================================
  IC_Image...

  ==================================================
*/

ic_rect_t * IC_NewRectangle( int width, int height, int xofs, int yofs )
{
	ic_rect_t	*rect;
	
	rect = NEWTYPE( ic_rect_t );

	rect->width = width;
	rect->height = height;

	rect->xofs = xofs;
	rect->yofs = yofs;

	return rect;
}

void IC_FreeRectangle( ic_rect_t *rect )
{
	FREE( rect );
}


/*
  ==============================
  IC_NewImage

  ==============================
*/
ic_image_t * IC_NewImage( int width, int height )
{
	ic_image_t	*img;
	ic_rect_t	*rect;
	unsigned char	*tmpimg;
	int		img_bytes;

	img = NEWTYPE( ic_image_t );
	img->width = width;
	img->height = height;

//	glEnable( GL_TEXTURE_2D );	// is this necessary ???
	glGenTextures( 1, &img->texobj );
	glBindTexture( GL_TEXTURE_2D, img->texobj );

	// create gl texture
	if ( IC_MEMORY_FORMAT == GL_RGB )
	{
		img_bytes = width*height*3;
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	}
	else if ( IC_MEMORY_FORMAT == GL_RGBA )
	{
		img_bytes = width*height*4;
		glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	}

	tmpimg = (unsigned char *)alloca( img_bytes );
	memset( tmpimg, 0, img_bytes );

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
#if 1
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#else
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
#endif


	glTexImage2D( GL_TEXTURE_2D, 0, IC_INTERNAL_FORMAT, width, height, 0, IC_MEMORY_FORMAT, GL_UNSIGNED_BYTE, tmpimg );
	if ( glGetError() != GL_NO_ERROR )
	{
		__error( "glTexImage2D failed\n" );
	}
//	glDisable( GL_TEXTURE_2D );

	// create list for empty rectangles
	img->empty_rect_list = U_NewList();

	// create an initial empty rectangle from image size
	rect = IC_NewRectangle( width, height, 0, 0 );

	U_ListInsertAtHead( img->empty_rect_list, rect );

	return img;
}

/*
  ==============================
  IC_FreeImage

  ==============================
*/
void IC_FreeImage( ic_image_t *img )
{
	ic_rect_t		*rect;

	for ( ; U_ListIsNotEmpty( img->empty_rect_list ) ; )
	{
		rect = (ic_rect_t *)U_ListRemoveAtHead( img->empty_rect_list );
		IC_FreeRectangle( rect );
	}
	U_FreeList( img->empty_rect_list );
	
	glDeleteTextures( 1, &img->texobj );

	FREE( img );
}

/*
  ==============================
  IC_ImageCalcEmptyArea

  ==============================
*/
int IC_ImageCalcEmptyArea( ic_image_t *img )
{
	u_list_iter_t	iter;
	ic_rect_t	*rect;
	int		empty_area;

	empty_area = 0;

	U_ListIterInit( &iter, img->empty_rect_list );
	for ( ; ( rect = (ic_rect_t *)U_ListIterNext( &iter ) ) ; )
	{
		empty_area += rect->width * rect->height;
	}
	
	return empty_area;
}

/*
  ==============================
  IC_DumpImage

  ==============================
*/
void IC_DumpImage( ic_image_t *img )
{
	printf( "ID_DumpImage:\n" );
	printf( " width: %d\n", img->width );
	printf( " height: %d\n", img->height );
	
	printf( " empty rectangles: %d\n", U_ListLength( img->empty_rect_list) );
	printf( " empty area: %.2f%%\n", (IC_ImageCalcEmptyArea( img )*100.0)/(img->width*img->height*1.0) );
}

/*
  ==============================
  IC_SubImageInsertPixelIntoImage_hack

  ==============================
*/
void IC_SubImageInsertPixelIntoImage_hack( ic_subimage_t *sub, int x, int y, unsigned char *colors )
{

	if ( sub->xofs < 0 ||
	     sub->yofs < 0 )
		__error( "try to insert a invalid subimage\n" );

	glBindTexture( GL_TEXTURE_2D, sub->texobj );
//	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	if ( x < 0 || y < 0 || x >= sub->width || y >= sub->height )
		__error( "invalid pixel pos in subimage\n" );

	glTexSubImage2D( GL_TEXTURE_2D, 0, sub->xofs+x, sub->yofs+y, 1, 1,
			 IC_MEMORY_FORMAT, GL_UNSIGNED_BYTE, colors );
	
	if ( glGetError() != GL_NO_ERROR )
	{
		__error( "glTexSubImage2D failed\n" );
	}

	glFinish();
}

/*
  ==============================
  IC_ImageInsertSubImage

  ==============================
*/
void IC_SubImageInsertIntoImage( ic_subimage_t *sub, void *pixels )
{

	if ( sub->xofs < 0 ||
	     sub->yofs < 0 )
		__error( "try to insert a invalid subimage\n" );

	glBindTexture( GL_TEXTURE_2D, sub->texobj );
//	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	if ( sub->width < 1 || sub->height < 1 )
		__error( "invalid subimage size\n" );

	glTexSubImage2D( GL_TEXTURE_2D, 0, sub->xofs, sub->yofs, sub->width, sub->height,
			 IC_MEMORY_FORMAT, GL_UNSIGNED_BYTE, pixels );
	
	if ( glGetError() != GL_NO_ERROR )
	{
		__error( "glTexSubImage2D failed\n" );
	}

//	glFinish();
}

/*
  ==============================
  IC_ImageInsertSubImage

  ==============================
*/
void IC_SubSubImageInsertIntoImage( ic_subimage_t *sub, int xofs, int yofs, int width, int height, void *pixels )
{

	if ( sub->xofs < 0 ||
	     sub->yofs < 0 )
		__error( "try to insert a invalid subimage\n" );       

	glBindTexture( GL_TEXTURE_2D, sub->texobj );
//	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	if ( sub->width < 1 || sub->height < 1 )
		__error( "invalid subimage size\n" );

	glTexSubImage2D( GL_TEXTURE_2D, 0, sub->xofs+xofs, sub->yofs+yofs, width, height,
			 IC_MEMORY_FORMAT, GL_UNSIGNED_BYTE, pixels );
	
	if ( glGetError() != GL_NO_ERROR )
	{
		__error( "glTexSubImage2D failed\n" );
	}

//	glFinish();
}



/*
  ==============================
  IC_ImageTestEmptyRectangle

  ==============================
*/
int IC_ImageTestEmptyRectangles( ic_image_t *img, int width, int height )
{
	u_list_iter_t		iter;
	ic_rect_t		*rect;
	ic_rect_t		*best;

	int	mindw, mindh;
	int	dw, dh;

	mindw = 0xffff;
	mindh = 0xffff;

	best = NULL;

	U_ListIterInit( &iter, img->empty_rect_list );
	for ( ; ( rect = (ic_rect_t *)U_ListIterNext( &iter ) ) ; )
	{
		dw = rect->width - width;
		dh = rect->height - height;

		if ( dw < 0 || dh < 0 )
			continue;

		if ( dw <= mindw && dh <= mindh )
		{
			mindw = dw;
			mindh = dh;
			best = rect;
		}
	}

	if ( !best )
	{
		img->best_last_rect = NULL;
		return -1;
	}

	img->best_last_rect = best;

	return ( mindw*mindh );
}

/*
  ==============================
  IC_ImageTakeEmptyRectangle

  ==============================
*/
void IC_ImageTakeEmptyRectangle( ic_image_t *img, int width, int height, int *xofs, int *yofs )
{
	int	gapw;
	int	gaph;
	ic_rect_t	*split;		
	ic_rect_t	*rect;
	u_list_iter_t	iter;
		
	
	if ( !img->best_last_rect )
		Error( "IC_ImageTakeEmptyRectangle: best_last_rect is (null)\n" );

	gapw = img->best_last_rect->width - width;
	gaph = img->best_last_rect->height - height;

	if ( gapw < 0 || gaph < 0 )
		Error( "IC_ImageTakeEmptyRectangle: best_last_rect doesn't fit the requested size\n" );
	
	*xofs = img->best_last_rect->xofs;
	*yofs = img->best_last_rect->yofs;

	// remove best_last rectangle from empty_rect list ...
	
	U_ListIterInit( &iter, img->empty_rect_list );
	for ( ; ( rect = (ic_rect_t *)U_ListIterNext( &iter ) ) ; )
	{
		if ( rect == img->best_last_rect )
			break;
	}

	if ( !rect )
		Error( "IC_ImageTakeEmptyRectangle: odd, can't find best_last_rect in list\n" );

	U_ListIterRemoveGoNext( &iter );

	// ... and split it up, if necessary

	// upper left is the requested rectangle

	if ( gapw > 0 )
	{
		// upper right
		split = IC_NewRectangle( gapw, height,
					 img->best_last_rect->xofs + width,
					 img->best_last_rect->yofs );		

		U_ListInsertAtHead( img->empty_rect_list, split );
	}

	if ( gaph > 0 )
	{
		// lower left
		split = IC_NewRectangle( width, gaph,
					 img->best_last_rect->xofs,
					 img->best_last_rect->yofs + height );

		U_ListInsertAtHead( img->empty_rect_list, split );
	}

	if ( gapw > 0 && gaph > 0 )
	{
		// lower right
		split = IC_NewRectangle( gapw, gaph,
					 img->best_last_rect->xofs + width,
					 img->best_last_rect->yofs + height );

		U_ListInsertAtHead( img->empty_rect_list, split );
	}

	// the requested rectangle is not needed any more
	IC_FreeRectangle( img->best_last_rect );
	img->best_last_rect = NULL;	
}


/*
  ==================================================
  IC_SubImage...

  ==================================================
*/

/*
  ==============================
  IC_NewSubImage

  ==============================
*/
ic_subimage_t * IC_NewSubImage( int width, int height )
{
	ic_subimage_t	*sub;

	sub = NEWTYPE( ic_subimage_t );

	sub->width = width;
	sub->height = height;

	// a new subimage is not valid       
	sub->xofs = -1;
	sub->yofs = -1;

	return sub;
}

/*
  ==============================
  IC_FreeSubImage

  ==============================
*/
void IC_FreeSubImage( ic_subimage_t *sub )
{
	// fixme: free resources
	FREE( sub );
}

/*
  ==============================
  IC_SubImageSetPixels

  ==============================
*/
void IC_SubImageSetPixels( ic_subimage_t *sub, void *pixels )
{
	sub->pixels = pixels;
}

/*
  ==============================
  IC_SubImageIsValid

  ==============================
*/
bool_t IC_SubImageIsCached( ic_subimage_t *sub )
{
	if ( sub->xofs != -1 )
		return true;
	return false;
}

/*
  ==============================
  IC_SubImageInvalidate

  ==============================
*/
void IC_SubImageInvalidate( ic_subimage_t *sub )
{
	sub->xofs = -1;
	sub->yofs = -1;
}

/*
  ==============================
  IC_SubImageGetImageGLtexobj

  ==============================
*/
int IC_SubImageGetImageGLtexobj( ic_subimage_t *sub )
{
	if ( sub->xofs == -1 )
	{
		__error( "subimage is not cached\n" );
	}
	return sub->texobj;
}

/*
  ==============================
  IC_SubImageGetSubImageOffset

  ==============================
*/
void IC_SubImageGetSubImageOffset( ic_subimage_t *sub, vec2d_t ofs )
{
	if ( sub->xofs == -1 )
	{
		__error( "subimage is not cached\n" );
	}

	ofs[0] = (fp_t)(sub->xofs);
	ofs[1] = (fp_t)(sub->yofs);
}

/*
  ==============================
  IC_SubImageGetSubImageSize

  ==============================
*/
void IC_SubImageGetSubImageSize( ic_subimage_t *sub, vec2d_t size )
{
	size[0] = (fp_t)(sub->width);
	size[1] = (fp_t)(sub->height);
}

/*
  ==============================
  IC_SubImageGetImageSize

  ==============================
*/
void IC_SubImageGetImageSize( ic_subimage_t *sub, vec2d_t size )
{	
	if ( sub->xofs == -1 )
	{
		__error( "subimage is not cached\n" );
	}

//	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, sub->texobj );
	glGetTexLevelParameterfv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &size[0] );
	glGetTexLevelParameterfv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &size[1] );		
//	glDisable( GL_TEXTURE_2D );
}

/*
  ==================================================
  subimage upload frame util

  ==================================================
*/

/*
  ==============================
  ICU_BeginSubImageUploadFrame

  ==============================
*/
void ICU_BeginSubImageUploadFrame( icu_upload_frame_t *frame )
{
	frame->sub_num = 0;
}

/*
  ==============================
  ICU_EndSubImageUploadFrame

  ==============================
*/

int QSort_SubimageCompare( const void *s1, const void *s2 )
{
	ic_subimage_t	*sub1, *sub2;
	maddr_t		a1, a2;


	sub1 = (*(ic_subimage_t **)(s1));
	sub2 = (*(ic_subimage_t **)(s2));
	// sort by texobj

	if ( sub1->texobj > sub2->texobj )
		return 1;
	else if ( sub1->texobj < sub2->texobj )
		return -1;
	else
	{
		// sort by subimage
		a1 = (maddr_t) sub1;
		a2 = (maddr_t) sub2;
		
		if ( a1 > a2 )
			return 1;
		else if ( a1 < a2 )
			return -1;
		else
		{
			return 0;
		}
	}

	return 0;
}

void ICU_EndSubImageUploadFrame( icu_upload_frame_t *frame )
{
	int		i;
	int		texobj;
	int		last_texobj;
	int		count_texobj;
	int		count_multiple;

	ic_subimage_t	*last_sub;
	ic_subimage_t	*sub;

	//
	// sort subimage refs by texobj
	//

	qsort( (void *)&frame->sub_refs[0], frame->sub_num, sizeof( ic_subimage_t * ), QSort_SubimageCompare );

	//
	// upload subimage pixels 
	//

	last_texobj = -1;
	last_sub = NULL;
	count_texobj = 0;
	count_multiple = 0;

	for ( i = 0; i < frame->sub_num; i++ )
	{
		sub = frame->sub_refs[i];

		if ( sub == last_sub )
		{
			count_multiple++;
			continue;
		}
		last_sub = sub;

		if ( IC_SubImageIsCached( sub ) == false )
		{
//			__warning( "ignore upload for uncached subimage\n" );
			continue;
		}

		texobj = sub->texobj;

		if ( texobj != last_texobj )
		{
			glBindTexture( GL_TEXTURE_2D, texobj );
			last_texobj = texobj;
			count_texobj++;
		}

		glTexSubImage2D( GL_TEXTURE_2D, 0, sub->xofs, sub->yofs, sub->width, sub->height,
				 IC_MEMORY_FORMAT, GL_UNSIGNED_BYTE, sub->pixels );
	
	}

	if ( frame->sub_num )
	{
//		__named_message( " %d subimages ( %d multiples removed ) uploaded on %d texobj\n", frame->sub_num, count_multiple, count_texobj );
	}
}

/*
  ==============================
  ICU_UploadFrameAddSubImage

  ==============================
*/
void ICU_UploadFrameAddSubImage( icu_upload_frame_t *frame, ic_subimage_t *sub )
{
	if ( frame->sub_num >= ICU_UPLOAD_FRAME_MAX_SUBIMAGES )
		__error( "reached ICU_UPLOAD_FRAME_MAX_SUBIMAGES ( %d )\n", frame->sub_num );

	frame->sub_refs[frame->sub_num] = sub;
	frame->sub_num++;
}


