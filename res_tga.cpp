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



// res_tga.c

#include "interfaces.h"
#include "g_shared.h"

#include "res_tga.h"
#include "res_tgadefs.h"


res_tga_register_t * Res_SetupRegisterTGA( char *path )
{
	res_tga_register_t	*reg;
	int		size;

	size = strlen( path ) + 1;

	reg = (res_tga_register_t*)NEWBYTES( size );
	strcpy( reg->path, path );

	return reg;
}

res_tga_cache_t * Res_NewTGA( int width, int height, int bpp )
{
        res_tga_cache_t         *tga;
        size_t             size;
        int             n;

        n = width * height * bpp;
        size = (size_t)&(((res_tga_cache_t *)0)->image[n]);
        tga = (res_tga_cache_t *) NEWBYTES( size );
        tga->width = width;
        tga->height = height;
        tga->bpp = bpp;

        return tga;
}

res_tga_cache_t * Res_CacheInTGA( /*char *path*/ res_tga_register_t *reg )
{
	ib_file_t	*h;
	int		filesize;
	char		*buf;

	char   ident_len;
	char   cmap_type;
	char   image_type;
	short  cmap_origin;
	short  cmap_len;
	char   centry_size;
	short  image_xorg;
	short  image_yorg;
	short  image_width;
	short  image_height;
	char   pixel_size;
	char   image_discr;

	unsigned char	alpha, red, green, blue;
	unsigned char	*image;

	int		i;
	res_tga_cache_t		*tga;
	int		pixels;

	h = IB_Open( reg->path );
	if ( !h )
		__error( "load of '%s' failed\n", reg->path );

	filesize = IB_GetSize( h );

	buf = (char*)alloca( filesize );
	
	if ( !buf )
		__error( "alloca for '%s' failed\n" );

	IB_Read( buf, filesize, 1, h );
	IB_Close( h );

	
	//
	// interpret buffer as tga
	//

	U_BeginUnpack( U_PACKED_BIN, buf, filesize );

	// extract tga header
	U_Unpacks8( &ident_len );
	U_Unpacks8( &cmap_type );
	U_Unpacks8( &image_type );
	U_Unpacks16( &cmap_origin );
	U_Unpacks16( &cmap_len );
	U_Unpacks8( &centry_size );
	U_Unpacks16( &image_xorg );
	U_Unpacks16( &image_yorg );
	U_Unpacks16( &image_width );
	U_Unpacks16( &image_height );
	U_Unpacks8( &pixel_size );
	U_Unpacks8( &image_discr );

	// skip ident
	U_UnpackSkip( ident_len );
	// skip cmap
	U_UnpackSkip( (cmap_len * centry_size) / 8 );

	tga = NULL;
	
	if ( image_type == 2 && pixel_size == 24 && image_discr == 32 )
	{
		// 24-bit-GBR image
		tga = Res_NewTGA( image_width, image_height, 3 );
		image = tga->image;

		pixels = tga->width * tga->height;
		for ( i = 0; i < pixels; i++ )
		{
			// blue
			U_Unpacks8( (char*)&blue );
			// green
			U_Unpacks8( (char*)&green );
			// red
			U_Unpacks8( (char*)&red );

			*image = red;
			image++;
			*image = green;
			image++;
			*image = blue;
			image++;			
		}

	}
	else if ( image_type == 2 && pixel_size == 32 && image_discr == 40 )
	{
		// 32-bit-GBRA image		
		tga = Res_NewTGA( image_width, image_height, 4 );
		image = tga->image;

		pixels = tga->width * tga->height;
		for ( i = 0; i < pixels; i++ )
		{
			// blue
			U_Unpacks8( (char*)&blue );
			// green
			U_Unpacks8( (char*)&green );
			// red
			U_Unpacks8( (char*)&red );
			// alpha
			U_Unpacks8( (char*)&alpha );

			*image = alpha;
			image++;
			*image = red;
			image++;
			*image = green;
			image++;
			*image = blue;
			image++;

		}
		
	}
	else
	{
		__named_message( "format not supported.\n" );
	}

	U_EndUnpack();
	
	return tga;
}

g_resource_t * Res_RegisterTGA( hobj_t *resobj )
{
	g_resource_t	*r;
	hpair_t		*name;	
	hpair_t		*path;

	name = FindHPair( resobj, "name" );
	if ( !name )
		return NULL;

	path = FindHPair( resobj, "path" );
	if ( !path )
		return NULL;

	r = G_NewResource( name->value, resobj->name );
	
	r->res_register = Res_SetupRegisterTGA( path->value );
	r->state = G_RESOURCE_STATE_REGISTERED;

	return r;
}

void Res_UnregisterTGA( g_resource_t *res )
{
	printf( "Res_UnregisterTGA: do nothing\n" );
}

void Res_CacheTGA( g_resource_t *res )
{
	res_tga_register_t	*res_register;

	if ( res->state != G_RESOURCE_STATE_REGISTERED )
		return;
	
	res_register = (res_tga_register_t *) res->res_register;
	
//	printf( "try to cache tga from path: %s\n", res_register->path );

	res->res_cache = Res_CacheInTGA( res_register );
	res->state = G_RESOURCE_STATE_CACHED;
}

void Res_UncacheTGA( g_resource_t *res )
{
	printf( "Res_UncacheTGA: do nothing\n" );
}
