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



// u_pixel.h

#ifndef __u_pixel
#define __u_pixel

//
// pixel format concversion
//

void U_ConvertPixel_565to888( void *out888, void *in565 );
void U_ConvertPixel_888to565( void *out565, void *in888 );
void U_ConvertPixel_565to8888( void *out8888, void *in565 );

void U_ConvertPixels_565to888( void *out888, void *in565, int num );
void U_ConvertPixels_888to565( void *out565, void *in888, int num );
void U_ConvertPixels_565to8888( void *out8888, void *in565, int num );


//
// pixel misc
//

void U_PixelAddWithSaturation_888( void *out, void *in1, void *in2 );
void U_PixelAddWithSaturation_8888( void *out, void *in1, void *in2 );
void U_PixelsAddWithSaturation_888( unsigned char out[][3], unsigned char in1[][3], unsigned char in2[][3], int num );


void U_PixelsScale_888( void *out, void *in, int num, unsigned char scale );
void U_PixelsScale_8( unsigned char out[], unsigned char in[], int num, unsigned char scale );

void U_PixelRectangleGetSubRectangle_888( unsigned char out[][3], unsigned char in[][3], int s_width, int s_xofs, int s_yofs, int d_width, int d_height );
//void U_PixelRectangleGetSubRectangle_8888( unsigned char out[][4], unsigned char in[][4], int s_width, int s_xofs, int s_yofs, int d_width, int d_height );
void U_PixelRectangleGetSubRectangle_8888( unsigned char *out, unsigned char *in, int s_width, int s_xofs, int s_yofs, int d_width, int d_height );
void U_PixelRectangleGetSubRectangle_8( unsigned char out[], unsigned char in[], int s_width, int s_xofs, int s_yofs, int d_width, int d_height );

void U_PixelRectanglePutSubRectangle_888( unsigned char out[][3], unsigned char in[][3], int s_width, int s_xofs, int s_yofs, int d_width, int d_height );

void U_PixelRectangleScale_888( unsigned char dst[][3], unsigned char src[][3], int s_width, int s_height, int d_width, int d_height );
void U_PixelRectangleScale_8( unsigned char dst[], unsigned char src[], int s_width, int s_height, int d_width, int d_height );

void U_PixelsReMap_8_to_888( unsigned char dst[][3], unsigned char src[], int num, unsigned int rgba_ramp[256] );

void U_PixelsReMap_8_to_8888( unsigned char *dst, unsigned char src[], int num, unsigned int rgba_ramp[256] );

void U_PixelsReMap_8_to_8( unsigned char dst[], unsigned char src[], int num, unsigned char i_ramp[] );

void U_MixSubRectangles( unsigned char *map1, int map1_width, int map1_xofs, int map1_yofs,
			 unsigned char *map2, int map2_width, int map2_xofs, int map2_yofs,
			 int sub_width, int sub_height, int bpp );

void U_MixSubRectangleToPixels( unsigned char *map1, int map1_width, int map1_xofs, int map1_yofs,
				int sub_width, int sub_height, int bpp,
				unsigned char *pixels );
#endif
