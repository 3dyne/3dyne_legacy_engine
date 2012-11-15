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



// r_light_sw.c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "lib_math.h"
#include "shock.h"

#include "r_state.h"
#include "shape_defs.h"
#include "r_sprite.h"
#include "interfaces.h"
#include "r_private.h"
#include "layer.h"
#include "unitex.h"
#include "imgcache.h"
#include "u_pixel.h"

#include "render.h"

#define MAX_DLIGHT_FALLOFF_NODES	( 4096 )
#define MAX_DLIGHT_FALLOFF_MAP_BYTES	( 128*128*4*8 )

// tmp falloff node heap
static int				r_falloff_node_num;
static dlight_falloff_node_t		r_falloff_nodes[MAX_DLIGHT_FALLOFF_NODES];

// sort r_falloff_nodes[] by layer's texobj
static dlight_falloff_node_t	       *r_falloff_node_refs[MAX_DLIGHT_FALLOFF_NODES];

// tmp falloff map heap
static int				r_falloff_map_byte_num;
static unsigned char			r_falloff_map_bytes[MAX_DLIGHT_FALLOFF_MAP_BYTES];

#define LIGHT_FALLOFF_WIDTH	( 32 )
#define LIGHT_FALLOFF_HEIGHT	( 32 )

static bool_t		r_have_falloff = false;
static unsigned char	r_falloff[LIGHT_FALLOFF_WIDTH][LIGHT_FALLOFF_HEIGHT];

static vec3d_t		r_ramp_color;
static bool_t		r_ramp_valid;

static unsigned int	rgba_ramp[256];
static unsigned char	i_ramps[16][256];

/*
  ==============================
  R_DlightClearTmpFalloffNodes

  ==============================
*/
void R_DlightClearTmpFalloffs( void )
{
//	if ( r_falloff_map_byte_num > 0 )
//		printf( " %.2fkb in %d falloff maps\n", r_falloff_map_byte_num/1024.0, r_falloff_node_num );

	r_falloff_node_num = 0;	
	r_falloff_map_byte_num = 0;
}

int R_DlightGetTmpFalloffNodeNum( void )
{
	return r_falloff_node_num;
}

int R_DlightGetTmpFalloffMapByteNum( void )
{
	return r_falloff_map_byte_num;
}

/*
  ==============================
  R_DlightGetTmpFalloffNode

  ==============================
*/
dlight_falloff_node_t * R_DlightGetTmpFalloffNode( void )
{
	dlight_falloff_node_t	*fn;

//	if ( r_falloff_node_num >= MAX_DLIGHT_FALLOFF_NODES )
//		__error( "reached MAX_DLIGHT_FALLOFF_NODES\n" );

	fn = &r_falloff_nodes[r_falloff_node_num++];
	return fn;
}

/*
  ==============================
  R_DlightGetTmpFalloffMapBytes

  ==============================
*/
static unsigned char * R_DlightGetTmpFalloffMapBytes( int byte_num )
{
	unsigned char	*fm;

	// make byte_num 4 byte aligned ...
	byte_num = (byte_num&(~3))+4;

	if ( r_falloff_map_byte_num + byte_num >= MAX_DLIGHT_FALLOFF_MAP_BYTES )
		__error( "reached MAX_DLIGHT_FALLOFF_MAP_BYTES\n" );

	fm = &r_falloff_map_bytes[r_falloff_map_byte_num];
	r_falloff_map_byte_num += byte_num;

	return fm;
}


/*
  ==============================
  R_DlightSetupReMapRamps

  ==============================
*/
void R_DlightSetupReMapRamps_sw( vec3d_t color )
{
	r_ramp_valid = false;
	Vec3dCopy( r_ramp_color, color );
}

static void R_DlightCalcReMapRampsForReal( void )
{
	int		i;

	int	r_f;
	int	g_f;
	int	b_f;
	int	r_step_f;
	int	g_step_f;
	int	b_step_f;

	if ( r_ramp_valid )
		return;

	r_ramp_valid = true;
	
	r_step_f = (int)(r_ramp_color[0] * ( 65536.0 ));
	g_step_f = (int)(r_ramp_color[1] * ( 65536.0 ));
	b_step_f = (int)(r_ramp_color[2] * ( 65536.0 ));
	
	r_f = 0;
	g_f = 0;
	b_f = 0;

	for ( i = 0; i < 256; i++ )
	{
		unsigned int	tmp;

		tmp = 255;	// ignore alpha ramp;
		tmp <<= 8;
		tmp |= (b_f >> 16)&0xff;
		tmp <<= 8;
		tmp |= (g_f >> 16)&0xff;
		tmp <<= 8;
		tmp |= (r_f >> 16)&0xff;

		rgba_ramp[i] = tmp;

		r_f += r_step_f;
		g_f += g_step_f;
		b_f += b_step_f;

	}
}

/*
  ==================================================
  test stuff

  ==================================================
*/

static void Light_InitFalloff( void )
{
	int		i, x, y;
	res_tga_cache_t		*cache;
	g_resource_t *tga;

	tga = G_ResourceSearch( g_rs, "tga.fx.falloff32x32" );
	if ( !tga )
		__error( "can't get resource 'tga.fx.falloff' \n" );	

	cache = (res_tga_cache_t *) tga->res_cache;
	if ( cache->width != LIGHT_FALLOFF_WIDTH || cache->height != LIGHT_FALLOFF_HEIGHT )
		__error( "wrong size\n" );

	i = 0;
	for ( y = 0; y < cache->height; y++ )
	{
		for ( x = 0; x < cache->width; x++ )
		{
			r_falloff[x][y] = cache->image[i++];
			i++;
			i++;
//			r_falloff[x][y][1] = cache->image[i++];
//			r_falloff[x][y][2] = cache->image[i++];
		}
	}
}


/*
  ==================================================
  falloff source

  ==================================================
*/

static void R_SetupFalloffMapValues( int fx1, int fy1, int fx2, int fy2,
			      int cx1, int cy1, int cx2, int cy2, unsigned char *falloff_values )
{
	int		x, y;
	
	fp_t		stepx, stepy;
	
	fx2 -= fx1;
	fy2 -= fy1;
	cx1 -= fx1;
	cy1 -= fy1;
	cx2 -= fx1;
	cy2 -= fy1;
	fx1 = 0;
	fy1 = 0;

#if 0	// version 1
	for ( y = cy1; y <= cy2; y++ )
	{
		ty = (LIGHT_FALLOFF_WIDTH-1)*((fp_t)(y)/(fy2-fy1));

		for ( x = cx1; x <= cx2; x++ )
		{
			tx = (LIGHT_FALLOFF_WIDTH-1)*((fp_t)(x)/(fx2-fx1));
			
			falloff_values[(x-cx1)+((cx2-cx1)+1)*(y-cy1)] = r_falloff[((int)rint(tx))&63][((int)rint(ty))&63];
		}
	}
#endif
#if 0	// version 2
	stepx = (1.0/(fx2-fx1))*(LIGHT_FALLOFF_WIDTH-1.0);
	stepy = (1.0/(fy2-fy1))*(LIGHT_FALLOFF_HEIGHT-1.0);
	
	ty = ((fp_t)(cy1))*stepy;
	startx = ((fp_t)(cx1))*stepx;
	for ( y = cy1; y <= cy2; y++ )
	{	
		tx = startx;
		for ( x = cx1; x <= cx2; x++ )
		{
			int	xx, yy;
			xx = ((int)(tx))&63; //*(LIGHT_FALLOFF_WIDTH-1.0);
			yy = ((int)(ty))&63; //*(LIGHT_FALLOFF_HEIGHT-1.0);
			falloff_values[(x-cx1)+((cx2-cx1)+1)*(y-cy1)] = r_falloff[xx][yy];
			tx += stepx;
		}
		ty += stepy;
	}
#endif
#if 1	// version 3
	{
		int	stepx_f, stepy_f;
		int	ty_f, tx_f, startx_f;
		int	xx, yy;
		int	width, height;
		int		pos;

		width = (cx2-cx1)+1;
		height = (cy2-cy1)+1;

		stepx = (1.0/(fx2-fx1))*(LIGHT_FALLOFF_WIDTH-1.0);
		stepy = (1.0/(fy2-fy1))*(LIGHT_FALLOFF_HEIGHT-1.0);
		
		stepx_f = (int)(stepx * ( 65536.0 ));
		stepy_f = (int)(stepy * ( 65536.0 ));
		
		ty_f = cy1*stepy_f;
		startx_f = cx1*stepx_f;
		for ( y = 0, pos = 0; y < height; y++ )
		{	
			tx_f = startx_f;
			yy = ((ty_f+0x8000) >> 16 );
			for ( x = 0; x < width; x++ )
			{
				xx = ((tx_f+0x8000) >> 16 );
				falloff_values[pos] = r_falloff[xx][yy];
				tx_f += stepx_f;
				pos++;
			}
			ty_f += stepy_f;
		}
	}
#endif
}


void R_DlightCreateTmpFalloffs_sw( ri_local_light_t *dlight, shape_t *shp )
{
	int		i, j;
//	int		num;
	u_list_iter_t	iter;
	layer_t		*layer;
	ic_subimage_t	*sub;

	fp_t		dist, b;
	fp_t		scale;
	vec3d_t		proj1;
	vec2d_t		proj2;

	fp_t		xx[4], yy[4], bb[4];

	int		x, y;       

	int		i_ramp, sum;

	unsigned char	falloff_values[128*128];

	if ( dlight->value < 1.0 )
		return;

	if ( r_have_falloff == false )
	{
		Light_InitFalloff();

		//
		// setup the 16 intensity ramps
		//

		for ( i = 0; i < 16; i++ )
		{
			for ( j = 0; j < 256; j++ )
			{
				fp_t		b;
				int		c;

				b = ((fp_t) j * (i/15.0));
				c = (int) b;
				i_ramps[i][j] = (unsigned char) c;

			}
		}

		r_have_falloff = true;
	}

	if ( /*!shp->valid*/ !( shp->flags & SHAPE_FLAGS_IS_VALID ) )
	{
		return;
	}
	
	if ( shp->tess_name != ShapeTesselation_simple_face )
	{
		return;
	}

	// search diffuse layer 
	U_ListIterInit( &iter, shp->layer_list );
	for ( ; ( layer = (layer_t *)U_ListIterNext( &iter ) ) ; )
	{
		if ( layer->ordinal == LAYER_DIFFUSE_1 )
			break;
	}

	if ( !layer )
		return;       
		
	sub = UniTex_IsReal_subimage( layer->real_source );
	if ( !sub )
		return;
	
	// project light 'origin' onto sface plane
	
	dist = Vec3dProjectOnPlane( proj1, shp->norm, shp->dist, dlight->origin );
	
	if ( dist <= 0.0 )
		return;		// too near or behind
	
	if ( dist < 64.0 )
		dist = 64.0;

	if ( dist > 256.0 )
		return;		// too far
	
	Layer_CreateTexCoord_projection( layer, proj1, proj2 );
	
	xx[0] = floor( proj2[0] );
	yy[0] = floor( proj2[1] );

	xx[1] = floor( proj2[0] ) + 1;
	yy[1] = floor( proj2[1] );

	xx[2] = floor( proj2[0] ) + 1;
	yy[2] = floor( proj2[1] ) + 1;

	xx[3] = floor( proj2[0] ) + 0;
	yy[3] = floor( proj2[1] ) + 1;

	bb[0] = fabs(((xx[1]-proj2[0]))*((yy[3]-proj2[1])));
	bb[1] = fabs(((xx[0]-proj2[0]))*((yy[2]-proj2[1])));
	bb[2] = fabs(((xx[3]-proj2[0]))*((yy[1]-proj2[1])));
	bb[3] = fabs(((xx[2]-proj2[0]))*((yy[0]-proj2[1])));

	sum = 0;

	// disable 4 time sample
	bb[0] = 1.0;
	for ( i = 0; i < 1; i++ )
	{
		// falloff rectangle
		int		fx1, fy1;
		int		fx2, fy2;
		int		fwidth, fheight;
		
		// subimage rectangle
		int		lx1, ly1;
		int		lx2, ly2;
// 		int		lwidth, lheight;
		
		// intersction rectangle
		int		cx1, cy1;
		int		cx2, cy2;
	
		x = (int)xx[i];
		y = (int)yy[i];		
	
		scale = 1.0/(dist/64.0);
		b = (dlight->value*dlight->value)/(dist*dist);
		b *= 255.0;
		if ( b > 255 )
			b = 255.0;			       
		
//		scale *= LIGHT_FALLOFF_WIDTH/16.0;
		scale = 1.0/scale;
		
//		printf( "dist: %f, scale: %f\n", dist, scale );
		
		lx1 = 0;
		ly1 = 0;
		lx2 = sub->width-1;
		ly2 = sub->height-1;
// 		lwidth = sub->width;
// 		lheight = sub->height;
//			printf( "subimage: (%d,%d)-(%d,%d)\n", lx1, ly1, lx2, ly2 );
		
		fwidth = 16.0*scale;
		fheight = 16.0*scale;
		fwidth = (fwidth&(~1)) - 1;
		fheight = (fheight&(~1)) - 1;
		fx1 = x - (((fwidth-1)/2));
		fy1 = y - (((fheight-1)/2));
		fx2 = fx1 + fwidth-1;
		fy2 = fy1 + fheight-1;

//		printf( "falloff: (%d,%d) (%d,%d)-(%d,%d)\n", fwidth, fheight, fx1, fy1, fx2, fy2 );
		
		cx1 = (lx1>fx1)?lx1:fx1;
		cy1 = (ly1>fy1)?ly1:fy1;
		cx2 = (lx2<fx2)?lx2:fx2;
		cy2 = (ly2<fy2)?ly2:fy2;
		
//			printf( "(%d,%d)-(%d,%d)\n", cx1, cy1, cx2, cy2 );
		
		if ( cx1 > cx2 || cy1 > cy2 )
		{
			
		}
		else
		{
			dlight_falloff_node_t	*fn;
			unsigned char		*fm;

			int		cwidth, cheight;

			cwidth = (cx2-cx1)+1;
			cheight = (cy2-cy1)+1;

			fn = R_DlightGetTmpFalloffNode();
			
			R_SetupFalloffMapValues( fx1, fy1, fx2, fy2, cx1, cy1, cx2, cy2, falloff_values );

			i_ramp = b / 16;

			if ( i_ramp < 0 )
				i_ramp = 0;
			if ( i_ramp > 15 )
				i_ramp = 15;

			if ( i <= 2 )
			{
				i_ramp = floor((fp_t)(i_ramp)*bb[i]);
				sum+=i_ramp;
			}
			else
			{
				i_ramp = i_ramp-sum;
			}
//			printf( "%d\n", i_ramp );

			U_PixelsReMap_8_to_8( falloff_values, falloff_values, cwidth*cheight, i_ramps[i_ramp] );


			if ( !r_ramp_valid )
				R_DlightCalcReMapRampsForReal();

#ifdef LIGHTMAP_FORMAT_888			
			fm = R_DlightGetTmpFalloffMapBytes( cwidth*cheight*3 );
			U_PixelsReMap_8_to_888( fm, falloff_values, cwidth*cheight, rgba_ramp );
#else
			fm = R_DlightGetTmpFalloffMapBytes( cwidth*cheight*4 );
			U_PixelsReMap_8_to_8888( fm, falloff_values, cwidth*cheight, rgba_ramp );
#endif			

			fn->xofs = cx1;
			fn->yofs = cy1;
			fn->width = cwidth;
			fn->height = cheight;
			fn->falloff_pixels = fm;
			fn->layer = layer;
			fn->texobj = IC_SubImageGetImageGLtexobj( sub );
			// insert the falloff node into the shape
			fn->next = shp->falloff_nodes;
			shp->falloff_nodes = fn;
		}
	}
	
	shp->has_light = r_frame_count;
//	num++;
}



/*
  ==============================
  R_MergeFalloffs

  ==============================
*/
static void R_MergeFalloffsInList( dlight_falloff_node_t *head )
{
	dlight_falloff_node_t		*fn1, *fn2;
	int	x1_mn, y1_mn, x1_mx, y1_mx;
	int	x2_mn, y2_mn, x2_mx, y2_mx;
	int	cx1, cy1, cx2, cy2;
	
	for ( fn1 = head; fn1 ; fn1=fn1->next )
	{
		x1_mn = fn1->xofs;
		y1_mn = fn1->yofs;
		x1_mx = x1_mn + (fn1->width-1);
		y1_mx = y1_mn + (fn1->height-1);

		for ( fn2 = fn1->next; fn2 ; fn2=fn2->next )
		{			
			if ( fn1->layer != fn2->layer )
				continue;

			x2_mn = fn2->xofs;
			y2_mn = fn2->yofs;
			x2_mx = x2_mn + (fn2->width-1);
			y2_mx = y2_mn + (fn2->height-1);

			if ( x1_mn > x2_mx || x1_mx < x2_mn ||
			     y1_mn > y2_mx || y1_mx < y2_mn )
			{
				continue;
			}
			
			// ok, mix the common area of both falloffs
			cx1 = (x1_mn>x2_mn)?x1_mn:x2_mn;
			cy1 = (y1_mn>y2_mn)?y1_mn:y2_mn;
			cx2 = (x1_mx<x2_mx)?x1_mx:x2_mx;
			cy2 = (y1_mx<y2_mx)?y1_mx:y2_mx;

#ifdef LIGHTMAP_FORMAT_888			
			U_MixSubRectangles( fn1->falloff_pixels, fn1->width, cx1-fn1->xofs, cy1-fn1->yofs,
					    fn2->falloff_pixels, fn2->width, cx1-fn2->xofs, cy1-fn2->yofs,
					    (cx2-cx1)+1, (cy2-cy1)+1, 3 );
#else
			U_MixSubRectangles( fn1->falloff_pixels, fn1->width, cx1-fn1->xofs, cy1-fn1->yofs,
					    fn2->falloff_pixels, fn2->width, cx1-fn2->xofs, cy1-fn2->yofs,
					    (cx2-cx1)+1, (cy2-cy1)+1, 4 );			
#endif

		}
	}
}

void R_MergeFalloffs_sw( int num, shape_t *shapes[] )
{
	int		i;
	shape_t		*shp;

	for ( i = 0; i < num; i++ )
	{
		shp = shapes[i];

		if ( !shp->falloff_nodes )
			continue;

		R_MergeFalloffsInList( shp->falloff_nodes );
	}
}


/*
  ==============================
  R_SortFalloffs

  ==============================
*/

static int QSort_FalloffCompareFunc( const void *f1, const void *f2 )
{
	dlight_falloff_node_t	*fn1;
	dlight_falloff_node_t	*fn2;

	int	texobj1;
	int	texobj2;

	fn1 = (*(dlight_falloff_node_t**)(f1));
	fn2 = (*(dlight_falloff_node_t**)(f2));

	texobj1 = fn1->texobj;
	texobj2 = fn2->texobj;

	if ( texobj1 > texobj2 )
		return 1;
	else if ( texobj1 < texobj2 )
		return -1;
	return 0;		
}

void R_SortFalloffs_sw( void )
{
	int		i;
	
	for ( i = 0; i < r_falloff_node_num; i++ )
	{
		r_falloff_node_refs[i] = &r_falloff_nodes[i];	
	}

	qsort( (void *)&r_falloff_node_refs[0], r_falloff_node_num, sizeof( dlight_falloff_node_t * ), QSort_FalloffCompareFunc );
}


void R_ApplyShapesFalloffs_sw( void )
{
	int		i;
	dlight_falloff_node_t	*fn;
	ic_subimage_t	*sub;
	int		texobj, last_texobj;

	last_texobj = -1;
	for ( i = 0; i < r_falloff_node_num; i++ )
	{
		fn = r_falloff_node_refs[i];
		texobj = fn->texobj;

		sub = UniTex_IsReal_subimage( fn->layer->real_source );
		if ( !sub )
			continue;

#ifdef LIGHTMAP_FORMAT_888
		U_MixSubRectangleToPixels( sub->pixels, sub->width, fn->xofs, fn->yofs, fn->width, fn->height, 3, fn->falloff_pixels );
#else
		U_MixSubRectangleToPixels( (unsigned char *)sub->pixels, sub->width, fn->xofs, fn->yofs, fn->width, fn->height, 4, fn->falloff_pixels );
#endif

		// some ic_subimage violation ...

		if ( last_texobj != texobj )
		{
			last_texobj = texobj;
			glBindTexture( GL_TEXTURE_2D, texobj );
		}
		
#ifdef LIGHTMAP_FORMAT_888
		glTexSubImage2D( GL_TEXTURE_2D, 0, sub->xofs+fn->xofs, sub->yofs+fn->yofs, fn->width, fn->height,
				 GL_RGB, GL_UNSIGNED_BYTE, /*tmp_map*/ fn->falloff_pixels );
#else
		glTexSubImage2D( GL_TEXTURE_2D, 0, sub->xofs+fn->xofs, sub->yofs+fn->yofs, fn->width, fn->height,
				 GL_RGBA, GL_UNSIGNED_BYTE, /*tmp_map*/ fn->falloff_pixels );
#endif
		
	}
}

void R_CleanUpShapesFalloffs_sw( void )
{
	int		i;
	dlight_falloff_node_t	*fn;
	ic_subimage_t	*sub;
	int		texobj, last_texobj;
	unsigned char	tmp_map[128*128][4];

	last_texobj = -1;
	for ( i = 0; i < r_falloff_node_num; i++ )
	{
		fn = r_falloff_node_refs[i];

		sub = UniTex_IsReal_subimage( fn->layer->real_source );
		if ( !sub )
			continue;
		texobj = fn->texobj;

#ifdef LIGHTMAP_FORMAT_888
		U_PixelRectangleGetSubRectangle_888( tmp_map, sub->pixels, sub->width, 
						     fn->xofs, fn->yofs, fn->width, fn->height );		       
#else
		U_PixelRectangleGetSubRectangle_8888( &tmp_map[0][0], (unsigned char *) sub->pixels, sub->width,
						     fn->xofs, fn->yofs, fn->width, fn->height );		       
#endif		

		// some ic_subimage violation ...

		if ( last_texobj != texobj )
		{
			last_texobj = texobj;
			glBindTexture( GL_TEXTURE_2D, texobj );
		}	
		
#ifdef LIGHTMAP_FORMAT_888
		glTexSubImage2D( GL_TEXTURE_2D, 0, sub->xofs+fn->xofs, sub->yofs+fn->yofs, fn->width, fn->height,
				 GL_RGB, GL_UNSIGNED_BYTE, tmp_map );	
#else
		glTexSubImage2D( GL_TEXTURE_2D, 0, sub->xofs+fn->xofs, sub->yofs+fn->yofs, fn->width, fn->height,
				 GL_RGBA, GL_UNSIGNED_BYTE, tmp_map );			
#endif
	}
}
