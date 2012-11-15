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



// r_light.c

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
#include "r_interface.h"

static int			r_dlight_shaperef_count = 0;
static ri_local_light_t		*r_current_dlight_info;

/*
  =============================================================================
  dlight setup for software lightmap rendering

  =============================================================================
*/

/*
  ==============================
  R_SetupDlights_sw

  ==============================
*/

//  called by BSP_VisitLeafsInBoundBox
static void DlightLeafFunc_sw( mapnode_t *leaf )
{
	shape_ref_t	*sr;
	shape_t		*shp;

	for ( sr = leaf->shaperefs; sr ; sr=sr->next )
	{
		shp = sr->shape;
		if ( shp->last_rf == r_frame_count )
		{
			if ( shp->last_local != r_local_count )
			{
				// from now the current dlight ignores the shape
				shp->last_local = r_local_count;

				// final boundbox check
				if ( Vec3dBBDoIntersect( shp->min, shp->max, r_current_dlight_info->min, r_current_dlight_info->max ) )
				{
					// ok, this shape is hit by the current dlight					
					// create a falloff for each layer, that should be lit

					// insert the falloffs into shape's falloff node list
					R_DlightCreateTmpFalloffs_sw( r_current_dlight_info, shp );

					r_dlight_shaperef_count++;	
				}
			}
		}
	}
}

void R_SetupDlights_sw( void )
{
	int			i;

	r_dlight_shaperef_count = 0;	

	// forget all falloff nodes and maps
	R_DlightClearTmpFalloffs();

	for ( i = 0; i < ri_local_light_num; i++ )
	{
		/*
		  for each dlight:
		  drop boundbox into bsptree and collect all shapes that
		  are visible
		*/

		r_current_dlight_info = ri_local_lights[i];

		R_DlightSetupReMapRamps_sw( r_current_dlight_info->color );

		// call DlightLeafFunc for each visited leaf
		BSP_VisitLeafsInBoundBox( r_current_dlight_info->min, r_current_dlight_info->max, DlightLeafFunc_sw );		

		// advance total number of ever processed dlights
		r_local_count++;
	}
}


void R_BeginDlightFrame_sw( void )
{
	R_SetupDlights_sw();	// each shape gets a list of falloffs

        R_MergeFalloffs_sw( r_visible_shape_num, r_visible_shapes );
	R_SortFalloffs_sw();
	R_ApplyShapesFalloffs_sw();	// apply the falloffs to the layers subimages

}

void R_EndDlightFrame_sw( void )
{
	R_CleanUpShapesFalloffs_sw();
}



/*
  =============================================================================
  dlight setup for opengl lightmap rendering

  =============================================================================
*/


/*
  ==============================
  R_SetupDlights_gl

  ==============================
*/

//  called by BSP_VisitLeafsInBoundBox
static void DlightLeafFunc_gl( mapnode_t *leaf )
{
	shape_ref_t	*sr;
	shape_t		*shp;

	for ( sr = leaf->shaperefs; sr ; sr=sr->next )
	{
		shp = sr->shape;
		if ( shp->last_rf == r_frame_count )
		{
			if ( shp->last_local != r_local_count )
			{
				// from now the current dlight ignores the shape
				shp->last_local = r_local_count;

				// final boundbox check
				if ( Vec3dBBDoIntersect( shp->min, shp->max, r_current_dlight_info->min, r_current_dlight_info->max ) )
				{
					// ok, this shape is hit by the current dlight					
					// create a falloff for each layer, that should be lit

					// insert the falloffs into shape's falloff node list
					R_DlightCreateTmpFalloffs_gl( r_current_dlight_info, shp );

					r_dlight_shaperef_count++;	
				}
			}
		}
	}
}

void R_SetupDlights_gl( void )
{
	int			i;

	r_dlight_shaperef_count = 0;	

	// forget all falloff nodes and maps
//	R_DlightClearTmpFalloffs();

	for ( i = 0; i < ri_local_light_num; i++ )
	{
		/*
		  for each dlight:
		  drop boundbox into bsptree and collect all shapes that
		  are visible
		*/

		r_current_dlight_info = ri_local_lights[i];

//		R_DlightSetupReMapRamps_sw( r_current_dlight_info->color );

		// call DlightLeafFunc for each visited leaf
		BSP_VisitLeafsInBoundBox( r_current_dlight_info->min, r_current_dlight_info->max, DlightLeafFunc_gl );		

		// advance total number of ever processed dlights
		r_local_count++;
	}

//	printf( "r_dlight_shaperef_count = %d\n", r_dlight_shaperef_count );
}



void R_BeginDlightFrame_gl( void )
{
//	printf( "R_BeginDlightFrame_gl\n" );
	R_SetupDlights_gl();
}

void R_EndDlightFrame_gl( void )
{
//	printf( "R_EndDlightFrame_gl\n" );
}
