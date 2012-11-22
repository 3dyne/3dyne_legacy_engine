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



#include <setjmp.h>
#include <stdio.h>
#include <jpeglib.h>
#include <string.h>
//#include <alloca.h>
#include <string.h>
#include <vector>

#include "ib_service.h"
#include "res_gltex.h"
#include "res_gltexdefs.h"
//#include "s_mem.h"
#include "shock.h"

void *_MM_Malloc( int );
void _MM_Free( void * );


#define MM_Malloc _MM_Malloc
#define MM_Free _MM_Free

#define NEWTYPE( x )	( (x *)(memset( (_MM_Malloc(sizeof(x)) ), 0, sizeof(x) ) ) )
/*
  ==============================
  Res_CacheInGltex_JPG

  ==============================
*/

// util funcs ...

#define INPUT_BUF_SIZE	4096 // should be good for us, too.

typedef struct {
	struct jpeg_source_mgr	pub;
	
	ib_file_t	*h;
	JOCTET	buffer[INPUT_BUF_SIZE];
} my_source_mgr;

typedef struct my_error_mgr {
	struct jpeg_error_mgr pub;    /* "public" fields */
	jmp_buf setjmp_buffer;        /* for return to caller */
} *my_error_ptr;

typedef my_source_mgr *my_src_ptr;


// I do this with the jpeglib style, although I don't really know, what I am doing.

METHODDEF( void )init_source( j_decompress_ptr cinfo )
{
	// what the hell are they doing in jdatasrc.c?
}

METHODDEF( boolean )fill_input_buf( j_decompress_ptr cinfo )
{
	my_src_ptr	src = (my_src_ptr) cinfo->src;
	int	num, i, c;

	__named_message( "\n" );

	if( !src->h )
	{
		__warning( "handle == NULL.\n" );
		// very faky.
		src->buffer[0] = (JOCTET) 0xFF;
		src->buffer[1] = (JOCTET) JPEG_EOI;
		num = 2;
	}

      
	for( i = 0; i < INPUT_BUF_SIZE; i++ )
	{
		c = IB_GetChar( src->h );


//		printf( "%d\n", c );
		if( c == EOF )
		{
			break;
		}


		src->buffer[i] =  c;

	}
       
	num = i;
	
	src->pub.next_input_byte = src->buffer;
	src->pub.bytes_in_buffer = num;
	
	return TRUE;
}	

METHODDEF( void ) skip_input_data( j_decompress_ptr cinfo, long num )
{
// 	int	i;

	my_src_ptr src = (my_src_ptr) cinfo->src;

//	__named_message( "%d\n", num );


	if (num > 0) {
		while (num > (long) src->pub.bytes_in_buffer) 
		{
			num -= (long) src->pub.bytes_in_buffer;
			fill_input_buf(cinfo);
			/* note we assume that fill_input_buffer will never return FALSE,         
       * so suspension need not be handled.                                     
       */                                                                       
		}
		src->pub.next_input_byte += (size_t) num;
		src->pub.bytes_in_buffer -= (size_t) num;
	}


}

METHODDEF( void ) term_source( j_decompress_ptr cinfo )
{
}

void jpeg_ibase_src( j_decompress_ptr cinfo, ib_file_t *h )
{
	my_src_ptr	src;

	if( cinfo->src != NULL )
	{
		__warning( "cinfo->src != NULL\n" );
	}
	cinfo->src = (struct jpeg_source_mgr *) NEWTYPE( my_source_mgr );
	//cinfo->src = (struct jpeg_source_mgr *) malloc( sizeof( my_source_mgr ));

	src = (my_src_ptr) cinfo->src; 
	
	src->pub.init_source = init_source;
	src->pub.fill_input_buffer = fill_input_buf;
	src->pub.skip_input_data = skip_input_data;
	src->pub.resync_to_restart = jpeg_resync_to_restart; /* use default method */ 
	src->pub.term_source = term_source;
	src->h = h;
	src->pub.bytes_in_buffer = 0; /* forces fill_input_buffer on first read */
	src->pub.next_input_byte = NULL; /* until buffer loaded */

}      

static void jpeg_error_exit( j_common_ptr cinfo)
{
	my_error_ptr myerr = (my_error_ptr) cinfo->err;
	(*cinfo->err->output_message) (cinfo);

	longjmp (myerr->setjmp_buffer, 1);
}

void Res_CreateGLTEX_rgb( int width, int height, unsigned char *color_buf );
res_gltex_cache_t * Res_CacheInGLTEX_jpg( res_gltex_register_t *reg )
{
	struct jpeg_decompress_struct cinfo;

	ib_file_t	*h;
	unsigned char *buffer;
	int row_stride;
	struct my_error_mgr jerr;
// 	FILE	*b;

	int	pixels;
	unsigned char	*image, *ptr;
	res_gltex_cache_t		*gltex;
	

	h = IB_Open( reg->path );
//	b = fopen( "test.jpg", "rb" );

	if( !h )
	{
		__error( "load of '%s' failed\n", reg->path );
	}

//	memset( &cinfo, 0, sizeof( struct jpeg_decompress_struct ));

	cinfo.err = jpeg_std_error(&jerr.pub);
	jerr.pub.error_exit = jpeg_error_exit;
	
	if (setjmp (jerr.setjmp_buffer))                                              
	{

		jpeg_destroy_decompress (&cinfo);
		if( h )
			IB_Close( h );
		__error( "XJT: JPEG load error\n");
	       
	}



	printf( "%p\n", cinfo.err );


	// step 1

	jpeg_create_decompress(&cinfo);	
//	cinfo.err = jpeg_std_error (&jerr.pub);                                       
//	printf( "%p\n", cinfo.err );

	// step 2
	jpeg_ibase_src(&cinfo, h);
//	jpeg_stdio_src( &cinfo, b );

//	printf( "back\n" );
	
	// step 3
	jpeg_read_header(&cinfo, TRUE);


/*
	if( cinfo.jpeg_color_space != JCS_RGB )
	{
		__error( "colorspace is not RGB\n" );
	}
*/


	jpeg_start_decompress( &cinfo );
//	printf( "jpeg: %d %d %d\n", cinfo.image_width, cinfo.image_height, cinfo.output_components );
	if( cinfo.output_components != 3 )
	{
		__error( "jpeg file '%s' is not RGB\n", reg->path );
	}

	pixels = cinfo.output_width * cinfo.output_height;
	image = (unsigned char *)MM_Malloc( pixels * 3 );
       


	row_stride = cinfo.output_width * 3;

	ptr = image + ( row_stride * (cinfo.output_height - 1) );

	std::vector<unsigned char>buffer_data( row_stride );
	buffer = buffer_data.data();
	//buffer = (unsigned char *)alloca( row_stride );

//	printf( "row_stride: %d\n", row_stride );

       

	while( cinfo.output_scanline < cinfo.output_height )
	{
//		__named_message( "%d\n", cinfo.output_scanline );
		jpeg_read_scanlines( &cinfo, &buffer, 1 ); 
	
		memcpy( ptr, buffer, row_stride );
		
		ptr -= row_stride;

	}

//	memset( image, 0, pixels *3 );

	jpeg_finish_decompress( &cinfo );
	jpeg_destroy_decompress( &cinfo );
	IB_Close( h );

	gltex = NEWTYPE( res_gltex_cache_t );
	gltex->width = cinfo.output_width;
	gltex->height = cinfo.output_height;

	__message( "%s: %d %d\n", reg->path, gltex->width, gltex->height );

	gltex->comp = resGltexComponents_rgb;
	Res_CreateGLTEX_rgb( cinfo.output_width, cinfo.output_height, image );
//	__error( "good\n" );

	return gltex;
}
	

	
