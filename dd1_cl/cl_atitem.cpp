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



// cl_atitem.c

#include "cl_state.h"
#include "cl_head.h"

#include "s_mem.h"
#include "shock.h"

#include "events.h"

typedef struct cl_at_item_s
{
	cl_ati_t	atif;

	itemStyle	style;
	vec3d_t		origin;

	// render info
	ri_sprite_t	ri_sprite;
	ri_model_t	ri_model;
} cl_at_item_t;

/*
  ==============================
  UnpackEventItem

  ==============================
*/
static void UnpackEventItem( cl_ati_t *atif, unique_t id, eventType type, byte_iter_t *bi )
{
	cl_at_item_t	*item;
//	__named_message( "\n" );
	item = ADDR_OF_BASE( cl_at_item_t, atif, atif );	

	if ( type == eventType_client_obj_update_pos )
	{
		ByteIter_Unpack3fv( bi, item->origin );
	}
	else if ( type == eventType_client_obj_update_style )
	{		
		itemStyle	style;
		style = (itemStyle)ByteIter_Unpacki( bi );
		
		if ( style != item->style )
		{
			item->style = style;
			if ( item->style == itemStyle_rocket )
			{
				item->ri_model.md = CL_CreateModelFromResourceName( "rocket1" );
				CL_ModelSetMoveByName( item->ri_model.md, "base_frame" );
			}
			else if ( item->style == itemStyle_rlaunch )
			{
				item->ri_model.md = CL_CreateModelFromResourceName( "rlaunch1" );
				CL_ModelSetMoveByName( item->ri_model.md, "base_frame" );
			}
			else
			{
				__error( "unknown item style '%d'\n", item->style );
			}		
		}
	}
	else
	{
		__warning( "unkown event for client object '#%u'\n", id );
	}
}

/*
  ==============================
  RunItem

  ==============================
*/
static void RunItem( cl_ati_t *atif )
{
	cl_at_item_t	*item;

	item = ADDR_OF_BASE( cl_at_item_t, atif, atif );		

	if ( item->style != itemStyle_none )
	{
		int             i;                                                      
		vec3d_t         axis[3] = { {1,0,0}, {0,1,0}, {0,0,1} };                

		Vec3dCopy( item->ri_model.ref_origin, item->origin );                    
		for ( i = 0; i < 3; i++ )                                               
			Vec3dCopy( item->ri_model.ref_axis[i], axis[i] );                
		
		g_api.Render_Model( &item->ri_model );
	}
}

/*
  ==============================
  DestroyItem

  ==============================
*/
static void DestroyItem( cl_ati_t *atif )
{
	cl_at_item_t	*item;
//	__named_message( "\n" );
	item = ADDR_OF_BASE( cl_at_item_t, atif, atif );	
	FREE( item );
}

/*
  ==================================================
  create client item

  ==================================================
*/
static bool_t		ftbl_valid = false;
static cl_ati_ftbl_t	ftbl;

cl_ati_t * CL_CreateItem( unique_t id, byte_iter_t *bi )
{
	cl_at_item_t		*item;

	if ( ftbl_valid == false )
	{
		ftbl_valid = true;
		INITTYPE( &ftbl, cl_ati_ftbl_t );

		ftbl.Destroy = DestroyItem;
		ftbl.UnpackEvent = UnpackEventItem;
		ftbl.Run = RunItem;
	}

	item = NEWTYPE( cl_at_item_t );
	item->atif.ftbl = &ftbl;
	item->atif.id = id;

	// init private data from byte_iter event
	item->style = itemStyle_none;

	return &item->atif;
}
