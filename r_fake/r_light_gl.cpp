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



// r_light_gl.c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "lib_math.h"
#include "shock.h"

#include "render.h"

#include "r_state.h"
#include "shape_defs.h"
#include "r_sprite.h"
#include "interfaces.h"
#include "r_private.h"
#include "layer.h"
#include "unitex.h"
#include "imgcache.h"
#include "u_pixel.h"

static int	r_falloff_texobj = -1;

/*
  ==============================
  R_InitFalloff_gl

  ==============================
*/
void R_InitFalloff_gl( void )
{
	g_resource_t		*gltex_res;
	res_gltex_cache_t	*gltex_cache;

	gltex_res = G_ResourceSearch( g_rs, "gltex.fx.falloff16x16" );
	if ( !gltex_res )
		__error( "can't get 'gltex.fx.falloff16x16'\n" );

	gltex_cache = (res_gltex_cache_t *) gltex_res->res_cache;

	r_falloff_texobj = gltex_cache->texobj;
}

static void R_SetupFalloffTexCoords( int fx1, int fy1, int fx2, int fy2,
				     int cx1, int cy1, int cx2, int cy2, 
				     fp_t *tx1, fp_t *ty1, fp_t *tx2, fp_t *ty2 )
{
	fx2 -= fx1;
	fy2 -= fy1;
	cx1 -= fx1;
	cy1 -= fy1;
	cx2 -= fx1;
	cy2 -= fy1;
	fx1 = 0;
	fy1 = 0;

//	printf( "falloff: (%d,%d)-(%d,%d), clip: (%d,%d)-(%d,%d)\n", fx1, fy1, fx2, fy2, cx1, cy1, cx2, cy2 );

	*tx1 = (fp_t)(cx1)/(fp_t)(fx2-fx1);
	*ty1 = (fp_t)(cy1)/(fp_t)(fy2-fy1);

	*tx2 = (fp_t)(cx2)/(fp_t)(fx2-fx1);
	*ty2 = (fp_t)(cy2)/(fp_t)(fy2-fy1);

}

/*
  ==============================
  R_DlightCreateTmpFalloffs_gl

  ==============================
*/
void R_DlightCreateTmpFalloffs_gl( ri_local_light_t *dlight, shape_t *shp )
{
//	int		num;
	layer_t		*layer;
	ic_subimage_t	*sub;

	fp_t		dist, b;
	fp_t		scale;
	vec3d_t		proj1;
	vec2d_t		proj2;

	int		x, y;

	if ( r_falloff_texobj == -1) 
	{
		R_InitFalloff_gl();	       
	}

	if ( dlight->value < 1.0 )
		return;

	if ( /*!shp->valid*/ !( shp->flags & SHAPE_FLAGS_IS_VALID ) )
	{
		return;
	}
	
	if ( shp->tess_name != ShapeTesselation_simple_face )
	{
		return;
	}
	
	if ( shp->la->layernum < 2 )
		return;
	
	layer = shp->la->layers[1];
	
	if ( layer->ordinal != LAYER_DIFFUSE_1 )
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
	
	x = (int) floor( proj2[0] );
	y = (int) floor( proj2[1] );

#if 1
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
			int	texobj;
			fp_t	tx1, ty1, tx2, ty2;

			R_SetupFalloffTexCoords( fx1, fy1, fx2, fy2, cx1, cy1, cx2, cy2, &tx1, &ty1, &tx2, &ty2 );

//			printf( "(%.2f,%.2f)-(%.2f,%.2f)\n", tx1, ty1, tx2, ty2 );

			IC_SubImageInsertIntoImage( sub, sub->pixels );

			texobj = IC_SubImageGetImageGLtexobj( sub );

			glDisable( GL_TEXTURE_2D );
			
			glEnable( GL_TEXTURE_2D );

			// draw subimage
			glBindTexture( GL_TEXTURE_2D, texobj );
			glDisable( GL_BLEND );
			glColor3f( 1, 1, 1 );
			glBegin( GL_QUADS );

			glTexCoord2f( (sub->xofs)/128.0, (sub->yofs)/128.0 );
			glVertex2i( 0, 0 );

			glTexCoord2f( (sub->xofs+sub->width)/128.0, (sub->yofs)/128.0 );
			glVertex2i( sub->width, 0 );

			glTexCoord2f( (sub->xofs+sub->width)/128.0, (sub->yofs+sub->height)/128.0 );
			glVertex2i( sub->width, sub->height );

			glTexCoord2f( (sub->xofs)/128.0, (sub->yofs+sub->height)/128.0 );
			glVertex2i( 0, sub->height );
			glEnd();			
			

			// draw falloff
			glBindTexture( GL_TEXTURE_2D, r_falloff_texobj );
			glEnable( GL_BLEND );
			glBlendFunc( GL_ONE, GL_ONE );

			glBegin( GL_QUADS );

			glTexCoord2f( tx1, ty1 );
			glColor3f( 1, 1, 1 );
			glVertex2i( cx1, cy1 );

//			glTexCoord2f( (sub->xofs+(sub->width-1))/127.0, (sub->yofs+(sub->height-1)*0)/127.0 );
			glTexCoord2f( tx2, ty1 );
//			glColor3f( 1, 0, 0 );
			glVertex2i( cx2, cy1 );

//			glTexCoord2f( (sub->xofs+(sub->width-1))/127.0, (sub->yofs+(sub->height-1))/127.0 );
			glTexCoord2f( tx2, ty2 );
//			glColor3f( 0, 1, 0 );
			glVertex2i( cx2, cy2 );

//			glTexCoord2f( (sub->xofs+(sub->width-1)*0)/127.0, (sub->yofs+(sub->height-1))/127.0 );
			glTexCoord2f( tx1, ty2 );
//			glColor3f( 0, 0, 1 );
			glVertex2i( cx1, cy2 );
			glEnd();

//			glFinish();
			
			glBindTexture( GL_TEXTURE_2D, texobj );
			glCopyTexSubImage2D( GL_TEXTURE_2D, 0, sub->xofs+cx1, sub->yofs+cy1, cx1, cy1, (cx2-cx1)+1, (cy2-cy1)+1);
//			glFinish();

#if 0
			dlight_falloff_node_t	*fn;
			unsigned char		*fm;

			int		cwidth, cheight;

			cwidth = (cx2-cx1)+1;
			cheight = (cy2-cy1)+1;

			fn = R_DlightGetTmpFalloffNode();
			fm = R_DlightGetTmpFalloffMapBytes( cwidth*cheight*3 );
			
			R_SetupFalloffMapValues( fx1, fy1, fx2, fy2, cx1, cy1, cx2, cy2, falloff_values );

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
#endif
		}
	}
#endif
	
	shp->has_light = r_frame_count;
//	num++;
}
