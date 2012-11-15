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



// u_pixel.c

#include <stdio.h>
#include <math.h>
#include <string.h>
#include "lib_math.h"
#include "u_pixel.h"

#include "shock.h"

/*
  ==============================
  U_ConvertPixel_565to888

  ==============================
*/
void U_ConvertPixel_565to888( void *out888, void *in565 )
{
	unsigned short	in;
	unsigned char	out[3];

	memcpy( &in, in565, 2 );
	
	out[0] = ((in>>11)&31)<<3;
	out[1] = ((in>>5)&63)<<2;
	out[2] = ((in)&31)<<3;

	memcpy( out888, out, 3 );
}

/*
  ==============================
  U_ConvertPixel_888to565

  ==============================
*/
void U_ConvertPixel_888to565( void *out565, void *in888 )
{
	unsigned short		out;
	unsigned char		in[3];

	unsigned int		r, g, b;

	memcpy( in, in888, 3 );

	r = in[0] >> 3;
	g = in[1] >> 2;
	b = in[2] >> 3;

	r <<= 11;
	g <<= 5;

	out = r | g | b;
	memcpy( out565, &out, 2 );
}

/*
  ==============================
  U_ConvertPixels_565to888

  ==============================
*/
void U_ConvertPixels_565to888( void *out888, void *in565, int num )
{
	unsigned short	*in;
	unsigned char	*out;
	int		i;

	in = (unsigned short *)in565;
	out = (unsigned char *)out888;

	for ( i = 0; i < num; i++ )
	{
		U_ConvertPixel_565to888( &out[i*3], &in[i] );	
	}
	
}

/*
  ==============================
  U_ConvertPixels_888to565

  ==============================
*/
void U_ConvertPixels_888to565( void *out565, void *in888, int num )
{
	unsigned short	*out;
	unsigned char	*in;
	int		i;

	in = (unsigned char *)in888;
	out = (unsigned short *)out565;

	for ( i = 0; i < num; i++ )
	{
		U_ConvertPixel_888to565( &out[i], &in[i*3] );	
	}
	
}


/*
  ==============================
  U_ConvertPixel_565to8888

  ==============================
*/
void U_ConvertPixel_565to8888( void *out8888, void *in565 )
{
	unsigned short	in;
	unsigned char	out[4];

	memcpy( &in, in565, 2 );
	
	out[0] = ((in>>11)&31)<<3;
	out[1] = ((in>>5)&63)<<2;
	out[2] = ((in)&31)<<3;
	out[3] = 255;

	memcpy( out8888, out, 4 );
}


/*
  ==============================
  U_ConvertPixels_565to8888

  ==============================
*/
void U_ConvertPixels_565to8888( void *out8888, void *in565, int num )
{
	unsigned short	*in;
	unsigned char	*out;
	int		i;

	in = (unsigned short *)in565;
	out = (unsigned char *)out8888;

	for ( i = 0; i < num; i++ )
	{
		U_ConvertPixel_565to8888( &out[i*4], &in[i] );	
	}
	
}

/*
  ==============================
  U_PixelAddWithSaturation

  ==============================
*/
void U_PixelAddWithSaturation_888( void *out, void *in1, void *in2 )
{
	unsigned char	i1[3], i2[3], o[3];
	unsigned int		r, g, b;

	memcpy( i1, in1, 3 );
	memcpy( i2, in2, 3 );

	r = i1[0] + i2[0];
	g = i1[1] + i2[1];
	b = i1[2] + i2[2];

	if ( r > 255 )
		r = 255;
	if ( g > 255 )
		g = 255;
	if ( b > 255 )
		b = 255;

	o[0] = r;
	o[1] = g;
	o[2] = b;

	memcpy( out, o, 3 );
}

/*
  ==============================
  U_PixelAddWithSaturation_8888

  ==============================
*/
void U_PixelAddWithSaturation_8888( void *out, void *in1, void *in2 )
{
	unsigned char	i1[4], i2[4], o[4];
	unsigned int		r, g, b, a;

	memcpy( i1, in1, 4 );
	memcpy( i2, in2, 4 );

	r = i1[0] + i2[0];
	g = i1[1] + i2[1];
	b = i1[2] + i2[2];
	a = i1[3] + i2[3];

	if ( r > 255 )
		r = 255;
	if ( g > 255 )
		g = 255;
	if ( b > 255 )
		b = 255;
	if ( a > 255 )
		a = 255;

	o[0] = r;
	o[1] = g;
	o[2] = b;
	o[3] = a;

	memcpy( out, o, 4 );	
}

/*
  ==============================
  U_PixelsAddWithSaturation_888

  ==============================
*/
void U_PixelsAddWithSaturation_888( unsigned char out[][3], unsigned char in1[][3], unsigned char in2[][3], int num )
{

#if 0
	int		i;
	unsigned int	tmp;
	
	for ( i = 0; i < num; i++ )
	{
		unsigned int		sum;

		sum = in1[i][0] + in2[i][0];
		if ( sum > 255 )
			sum = 255;
		out[i][0] = ( unsigned char ) sum;

		sum = in1[i][1] + in2[i][1];
		if ( sum > 255 )
			sum = 255;
		out[i][1] = ( unsigned char ) sum;

		sum = in1[i][2] + in2[i][2];
		if ( sum > 255 )
			sum = 255;
		out[i][2] = ( unsigned char ) sum;
	}
#else
	
	int		i;
	int		bytes;
	int		num2;
	unsigned int	*out_32;
	unsigned int	*in1_32;
	unsigned int	*in2_32;
	unsigned int	tmp1, tmp2, tmp3;
	unsigned int		sum;

	bytes = num*3;

	num2 = (bytes&3);
	num = (bytes-num2)/4;

	out_32 = (unsigned int*) &out[0][0];
	in1_32 = (unsigned int*) &in1[0][0];
	in2_32 = (unsigned int*) &in2[0][0];
	
	// get 4 bytes each time
	for ( i = 0; i < num; i++ )
	{
		tmp1 = in1_32[i];
		tmp2 = in2_32[i];

		sum = (tmp1&0x00ff00ff) + (tmp2&0x00ff00ff);
		if ( (sum & 0x0000ff00) )
			sum |= 0x000000ff;
		if ( (sum & 0xff000000) )
			sum |= 0x00ff0000;

		sum &= 0x00ff00ff;
		tmp3 = sum;

		tmp1 >>= 8;
		tmp2 >>= 8;

		sum = (tmp1&0x00ff00ff) + (tmp2&0x00ff00ff);
		if ( (sum & 0x0000ff00) )
			sum |= 0x000000ff;
		if ( (sum & 0xff000000) )
			sum |= 0x00ff0000;
		sum &= 0x00ff00ff;
		tmp3 |= sum<<8;

		out_32[i] = tmp3;
	}

#if 1
	i *= 4;

	for ( ; i < bytes; i++ )
	{		
		sum = in1[0][i] + in2[0][i];
		if ( sum > 255 )
			sum = 255;
		out[0][i] = ( unsigned char ) sum;	
	}
#endif
#endif
}


/*
  ==============================
  U_PixelsScale_888

  ==============================
*/
void U_PixelsScale_888( void *out, void *in, int num, unsigned char scale )
{
	int		j;
	unsigned char	*o, *i;
	
	o = (unsigned char *)out;
	i = (unsigned char *)in;

	for ( j = 0; j < num*3; j++ )
	{
		o[j] = (unsigned char) (((unsigned int)(i[j])) * ((unsigned int)(scale)) >> 8 );
	}	
}

/*
  ==============================
  U_PixelsScale_8

  ==============================
*/
void U_PixelsScale_8( unsigned char out[], unsigned char in[], int num, unsigned char scale )
{
	int		j;
	
	for ( j = 0; j < num*3; j++ )
	{
		out[j] = (unsigned char) (((unsigned int)(in[j])) * ((unsigned int)(scale)) >> 8 );
	}	
}



/*
  ==============================
  U_PixelRectangleGetSubRectangle_888

  ==============================
*/
void U_PixelRectangleGetSubRectangle_888( unsigned char out[][3], unsigned char in[][3], int s_width, int s_xofs, int s_yofs, int d_width, int d_height )
{
	int		src;
	int		dst;
	int		x;
	int		y;

	src = s_width * s_yofs + s_xofs;
	dst = 0;

	for ( y = 0; y < d_height; y++ )
	{
		for ( x = 0; x < d_width; x++ )
		{
			out[dst][0] = in[src][0];
			out[dst][1] = in[src][1];
			out[dst][2] = in[src][2];			

			dst++;
			src++;
		}

		src += (s_width-d_width);
	}
}

/*
  ==============================
  U_PixelRectangleGetSubRectangle_888

  ==============================
*/
void U_PixelRectangleGetSubRectangle_8888( unsigned char *out, unsigned char *in, int s_width, int s_xofs, int s_yofs, int d_width, int d_height )
{
	int		src;
	int		dst;
	int		x;
	int		y;

	src = s_width * s_yofs + s_xofs;
	dst = 0;

	for ( y = 0; y < d_height; y++ )
	{
		for ( x = 0; x < d_width; x++ )
		{
			for ( int i = 0; i < 4; i++ ) {
				out[dst*4+i] = in[src*4+i];
			}
//			out[dst][0] = in[src][0];
//			out[dst][1] = in[src][1];
//			out[dst][2] = in[src][2];
//			out[dst][3] = in[src][3];

			dst++;
			src++;
		}

		src += (s_width-d_width);
	}
}

/*
  ==============================
  U_PixelRectanglePutSubRectangle_888

  ==============================
*/
void U_PixelRectanglePutSubRectangle_888( unsigned char dst[][3], unsigned char src[][3], int d_width, int d_xofs, int d_yofs, int s_width, int s_height )
{
	int		s;
	int		d;
	int		x;
	int		y;

	d = d_width * d_yofs + d_xofs;
	s = 0;

	for ( y = 0; y < s_height; y++ )
	{
		for ( x = 0; x < s_width; x++ )
		{
			dst[d][0] = src[s][0];
			dst[d][1] = src[s][1];
			dst[d][2] = src[s][2];			
			
			d++;
			s++;
		}

		d += (d_width-s_width);
	}
}


/*
  ==============================
  U_PixelRectangleGetSubRectangle_8

  ==============================
*/
void U_PixelRectangleGetSubRectangle_8( unsigned char out[], unsigned char in[], int s_width, int s_xofs, int s_yofs, int d_width, int d_height )
{
	int		src;
	int		dst;
	int		x;
	int		y;

	src = s_width * s_yofs + s_xofs;
	dst = 0;

//	printf( "(%d,%d)-(%d,%d)\n", s_xofs, s_yofs, s_xofs+(d_width-1), s_yofs+(d_height-1) );

	for ( y = 0; y < d_height; y++ )
	{
		for ( x = 0; x < d_width; x++ )
		{
			out[dst] = in[src];

			dst++;
			src++;
		}

		src += (s_width-d_width);
	}
}



/*
  ==============================
  U_PixelRectangleScale_888

  ==============================
*/
void U_PixelSpanCopy_888( unsigned char dst[][3], unsigned char src[][3], int dst_width, int src_width )
{
	int		i;
	fp_t	step;
	fp_t	x;
	int	xx;

	step = (src_width*1.0)/(dst_width*1.0);

	x = 0.0;
	for ( i = 0; i < dst_width; i++ )
	{
		xx = (int) x;
		
		dst[i][0] = src[xx][0];
		dst[i][1] = src[xx][1];
		dst[i][2] = src[xx][2];

		x += step;
	}
}

void U_PixelRectangleScale_888( unsigned char dst[][3], unsigned char src[][3], int s_width, int s_height, int d_width, int d_height )
{
	int		i;
	fp_t	step;
	fp_t	y;
	int	yy;

	step = (s_height*1.0)/(d_height*1.0);

	y = 0.0;
	for ( i = 0; i < d_height; i++ )
	{
		yy = (int) y;
		U_PixelSpanCopy_888( &dst[i*d_width], &src[yy*s_width], d_width, s_width );
		y += step;
	}
}


/*
  ==============================
  U_PixelRectangleScale_8

  ==============================
*/
void U_PixelSpanCopy_8( unsigned char dst[], unsigned char src[], int dst_width, int src_width )
{
	int		i;
	fp_t	step;
	fp_t	x;
	int	xx;

	step = (src_width*1.0-1.0)/(dst_width*1.0-1.0);

	x = 0.0;
	for ( i = 0; i < dst_width; i++ )
	{
		xx = (int) floor(x+0.5);
		dst[i] = src[xx];
		x += step;
	}
}

void U_PixelRectangleScale_8( unsigned char dst[], unsigned char src[], int s_width, int s_height, int d_width, int d_height )
{
	int		i;
	fp_t	step;
	fp_t	y;
	int	yy;

	step = (s_height*1.0-1.0)/(d_height*1.0-1.0);

	y = 0.0;
	for ( i = 0; i < d_height; i++ )
	{
		yy = (int) floor(y+0.5);
        // SIM: changed from: don't know if this is actually the right thing to do
        __error( "unchecked strange code\n" );
        //U_PixelSpanCopy_8( &dst[i*d_width][0], &src[yy*s_width][0], d_width, s_width );
		U_PixelSpanCopy_8( &dst[i*d_width], &src[yy*s_width], d_width, s_width );
		y += step;
	}
}


/*
  ==============================
  U_PixelsReMap_8_to_888

  ==============================
*/
void U_PixelsReMap_8_to_888( unsigned char dst[][3], unsigned char src[], int num, unsigned int rgba_ramp[256] )
{
	int		i;
	int		k;

	unsigned char	*ptr;

	ptr = &dst[0][0];

	for ( i = 0; i < num; i++ )
	{
		k = (int)(src[i]);

		*ptr++ = rgba_ramp[k] & 0xff;
		*ptr++ = (rgba_ramp[k] >> 8) & 0xff;
		*ptr++ = (rgba_ramp[k] >> 16) & 0xff;
	}
}


/*
  ==============================
  U_PixelsReMap_8_to_8888

  ==============================
*/
void U_PixelsReMap_8_to_8888( unsigned char* dst, unsigned char src[], int num, unsigned int rgba_ramp[256] )
{
	int		i;
	int		k;

	unsigned int	*ptr;

	__chkalign( dst, 4 );
    
    // SIM: warning: changed from ptr = (unsigned int *)&dst[0][0];
	ptr = (unsigned int *)&dst[0];

	for ( i = 0; i < num; i++ )
	{
		
		k = (int)(src[i]);			
		*ptr++ = rgba_ramp[k];
	}
}


/*
  ==============================
  U_PixelsReMap_8_to_8

  ==============================
*/
void U_PixelsReMap_8_to_8( unsigned char dst[], unsigned char src[], int num, unsigned char i_ramp[] )
{
	int		i;
	int		k;

	for ( i = 0; i < num; i++ )
	{
		k = (int)(src[i]);

		dst[i] = i_ramp[k];
	}	
}



/*
  ==============================
  U_SubRectanglesMix

  ==============================
*/

void U_MixSubRectangles( unsigned char *map1, int map1_width, int map1_xofs, int map1_yofs,
			 unsigned char *map2, int map2_width, int map2_xofs, int map2_yofs,
			 int sub_width, int sub_height, int bpp )
{
	int		x, y, i;
	unsigned char	*ptr1, *ptr2;
	int		map1_stride, map2_stride;

	ptr1 = map1 + (map1_width*map1_yofs+map1_xofs)*bpp;
	ptr2 = map2 + (map2_width*map2_yofs+map2_xofs)*bpp;

	map1_stride = (map1_width-sub_width)*bpp;
	map2_stride = (map2_width-sub_width)*bpp;

	for ( y = 0; y < sub_height; y++ )
	{
		for ( x = 0; x < sub_width; x++ )
		{
			for ( i = 0; i < bpp; i++ )
			{
				unsigned int	c1, c2;				

				c1 = (int)*ptr1;
				c2 = (int)*ptr2;

				c1 = c1+c2;
				if ( c1 > 255 )
					c1 = 255;

				*ptr1 = (unsigned int) c1;
				*ptr2 = (unsigned int) c1;

				ptr1++;
				ptr2++;
			}
		}
		ptr1 += map1_stride;
		ptr2 += map2_stride;
	}
}

/*
  ==============================
  U_MixSubRectangleToPixels

  ==============================
*/
void U_PixelsAddSaturation_p( unsigned char *pixels_inout, unsigned char *pixels_in, int num )
{
	int		i;
	unsigned int	c1, c2;	


	for ( i = 0; i < num; i++ )
	{
		c1 = (int)*pixels_inout;
		c2 = (int)*pixels_in;	

		c1 = c1+c2;
		if ( c1 > 255 )
			c1 = 255;
		
		*pixels_inout = (unsigned char) c1;

		pixels_inout++;
		pixels_in++;
	}	
}

void U_PixelsAddSaturation( unsigned char *pixels_inout, unsigned char *pixels_in, int num )
{
	int		i;
	unsigned int	c1, c2;	
	unsigned int	c3, c4;	

	int		num2;
	unsigned short	*short_inout;
	unsigned short	*short_in;       

	num2 = num&(~1);
//	num = num-num2;

	short_inout = (unsigned short*) pixels_inout;
	short_in = (unsigned short*) pixels_in;
	
	for ( i = 0; i < num2; i+=2 )
	{
		c1 = (int)*short_inout;
		c2 = (int)*short_in;

		c3 = (c1&0xff) | ((c1&0xff00)<<8);
		c4 = (c2&0xff) | ((c2&0xff00)<<8); 

		c3 = c3+c4;

		if ( (c3 & 0x0000ff00) )
			c3 |= 0x000000ff;
		if ( (c3 & 0xff000000) )
			c3 |= 0x00ff0000;

		c3 &= 0x00ff00ff;
	       
		c1 = (c3&0xff) | ((c3>>8)&0xff00);
		
		*short_inout = (unsigned short) c1;

		short_inout++;
		short_in++;
	}	

#if 1
	if ( num != num2 )
	{
		i = num-1;
		
		c1 =  pixels_inout[i] + pixels_in[i];
		if ( c1 > 255 )
			c1 = 255;
		pixels_inout[i] = ( unsigned char ) c1;	
	}
#endif
}

void U_PixelsAddSaturation_o( unsigned char *pixels_inout, unsigned char *pixels_in, int num )
{
	int		i;
	unsigned int	c1, c2;	

	unsigned int	*src, *dst;
	size_t		sofs, dofs;
	unsigned int		s, d;

	int	dshift, sshift;

	src = (unsigned int *)((size_t)(pixels_in)&(~3));
	dst = (unsigned int *)((size_t)(pixels_inout)&(~3));
	
	sofs = (size_t)(pixels_in)&3;
	dofs = (size_t)(pixels_inout)&3;

	s = *src;
	d = *dst;

	dshift = (dofs&3)*8;
	sshift = (sofs&3)*8;

	for ( i = 0; i < num; i++ )
	{
		c1 = (unsigned int)((d>>dshift)&0xff);
		c2 = (unsigned int)((s>>sshift)&0xff);

		c1 = c1+c2;
		if ( c1 > 255 )
			c1 = 255;
	
		c1 = c1<<dshift;
		c2 = (0xff)<<dshift;
		d &= (~c2);
		d |= c1;

		sofs++;
		dofs++;

		dshift = (dofs&3);
		sshift = (sofs&3);

		if ( sshift == 0 )
		{
			src++;	
			s = *src;       
		}

		if ( dshift == 0 )
		{
			*dst = d;
			dst++;
			d = *dst;
		}

		dshift *= 8;
		sshift *= 8;

	}

	*src = s;
	*dst = d;

}


void U_PixelsAddSaturation_8888( unsigned int *inout, unsigned int *in, int num )
{
	int		i;
	unsigned int	tmp1, tmp2, tmp3, sum;

	for ( i = 0; i < num; i++ )
	{
		tmp1 = inout[i];
		tmp2 = in[i];

		sum = (tmp1&0x00ff00ff) + (tmp2&0x00ff00ff);
		if ( (sum & 0x0000ff00) )
			sum |= 0x000000ff;
		if ( (sum & 0xff000000) )
			sum |= 0x00ff0000;

		sum &= 0x00ff00ff;
		tmp3 = sum;

		tmp1 >>= 8;
		tmp2 >>= 8;

		sum = (tmp1&0x00ff00ff) + (tmp2&0x00ff00ff);
		if ( (sum & 0x0000ff00) )
			sum |= 0x000000ff;
		if ( (sum & 0xff000000) )
			sum |= 0x00ff0000;
		sum &= 0x00ff00ff;
		tmp3 |= sum<<8;

		inout[i] = tmp3;		

	}
}

void U_MixSubRectangleToPixels( unsigned char *map1, int map1_width, int map1_xofs, int map1_yofs,
				int sub_width, int sub_height, int bpp,
				unsigned char *pixels )
{
	int		y;
	int		sub_xbytes, map_xbytes;
	unsigned char	*ptr1;	

	if ( bpp == 4 )
	{
		__chkalign( map1, 4 );
		__chkalign( pixels, 4 );
	
		ptr1 = map1 + (map1_width*map1_yofs+map1_xofs)*bpp;
		sub_xbytes = sub_width*bpp;
		map_xbytes = map1_width*bpp;
		
		for ( y = 0; y < sub_height; y++ )
		{
			U_PixelsAddSaturation_8888( (unsigned int *)pixels, (unsigned int *)ptr1, sub_width );
			pixels += sub_xbytes;
			ptr1 += map_xbytes;
		}		
	}

	if ( bpp != 4 )
	{
	
		ptr1 = map1 + (map1_width*map1_yofs+map1_xofs)*bpp;
		sub_xbytes = sub_width*bpp;
		map_xbytes = map1_width*bpp;
		
		for ( y = 0; y < sub_height; y++ )
		{
			U_PixelsAddSaturation( pixels, ptr1, sub_xbytes );
			pixels += sub_xbytes;
			ptr1 += map_xbytes;
		}			
	}

}
