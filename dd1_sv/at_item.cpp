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



// at_item.c

#include "sv_state.h"

#include "l_head.h"
#include "l_defs.h"

#include "s_mem.h"
#include "lib_hobj.h"
#include "shock.h"

#include "events.h"

typedef struct at_item_s
{
	ati_t		atif;

	// private data
	itemStyle	style;
	vec3d_t		origin;

	vec3d_t		rel_min;
	vec3d_t		rel_max;

} at_item_t;

/*
  ==============================
  ComItem

  ==============================
*/
static const char * ComItem( ati_t *atif, const char *in )
{
// 	at_item_t	*item;

// 	item = ADDR_OF_BASE( at_item_t, atif, atif );
	
//	__named_message( "id '#%u', got '%s'\n", atif->id, in );
	return "ok";
}

/*
  ==============================
  RunItem

  ==============================
*/
static void InitItemFromClass( at_item_t *item, hobj_t *item_obj )
{
	hpair_t		*pair;
	int		i;

	static struct { const char *key; itemStyle style; } resolves[] = {
		{ "rocket", itemStyle_rocket },
		{ "rlaunch", itemStyle_rlaunch },
		{ NULL, itemStyle_none }
	};

//	__named_message( "\n" );
	
	EasyFindVec3d( item->origin, item_obj, "origin" );
	
	pair = FindHPair( item_obj, "style" );
	if ( !pair )
		__error( "missing key 'style' in item_obj '%s'\n", item_obj->name );
	
	for ( i = 0; resolves[i].key ; i++ )
		if ( !strcmp( resolves[i].key, pair->value ) ) break;
	
	if ( !resolves[i].key )
	{
		__error( "unknown item style '%s'\n", pair->value );
	}
	else
	{
		item->style = resolves[i].style;
	}

	//
	// fixme: is this the right place for setup the bound box
	//
	Vec3dInit( item->rel_min, -32.0, -32.0, -32.0 );
	Vec3dInit( item->rel_max, 32.0, 32.0, 32.0 );

}

static void RunItem( ati_t *atif, atiRun run )
{
	at_item_t	*item;

	item = ADDR_OF_BASE( at_item_t, atif, atif );
	
	if ( run == atiRun_think1 )
	{
		if ( atif->frame_count == 0)
		{
			// first call, init at_item
			InitItemFromClass( item, atif->ati_obj );
			
			// make id public
			L_AtiRegisterId( atif );
		}

		//
		// add item object to collide map
		//
		L_CollideMapAddAtiWithRelBoundBox( atif, item->origin, item->rel_min, item->rel_max );
	}
	else if ( run == atiRun_think2 )
	{
		if ( !( atif->frame_count&31) )
		{
			byte_iter_t	frm;
			
			// emit client typeinfo event			
			PackFrame_Begin( bi_out, &frm );
			ByteIter_Packi( &frm, atif->id );
			ByteIter_Packi( &frm, eventType_client_typeinfo_item );
			PackFrame_End( bi_out, &frm );
			
			// emit client item style update event
			PackFrame_Begin( bi_out, &frm );
			ByteIter_Packi( &frm, atif->id );
			ByteIter_Packi( &frm, eventType_client_obj_update_style );
			ByteIter_Packi( &frm, item->style );
			PackFrame_End( bi_out, &frm );
		}
				
		{
			byte_iter_t	frm;
			
			// emit client item pos update event
			PackFrame_Begin( bi_out, &frm );
			ByteIter_Packi( &frm, atif->id );
			ByteIter_Packi( &frm, eventType_client_obj_update_pos );
			ByteIter_Pack3fv( &frm, item->origin );
			PackFrame_End( bi_out, &frm );
			
		}
		
		atif->frame_count++;
	}
}
/*
  ==============================
  DestroyItem
  
  ==============================
*/
static void DestroyItem( ati_t *atif )
{
	at_item_t	*item;

	DeepDestroyClass( atif->ati_obj );

	item = ADDR_OF_BASE( at_item_t, atif, atif );
	FREE( item );
}

/*
  ==================================================
  ATI_CreateItem

  ==================================================
*/
static bool_t		ftbl_valid = false;
static ati_ftbl_t	ftbl;

ati_t * ATI_CreateItem( hobj_t *item_obj )
{
	at_item_t		*item;

	if ( strcmp( item_obj->type, "item" ) )
		__error( "item_obj is not of type 'item'\n" );

	if ( ftbl_valid == false )
	{
		ftbl_valid = true;
		INITTYPE( &ftbl, ati_ftbl_t );

		ftbl.Destroy = DestroyItem;
		ftbl.Com = ComItem;
		ftbl.UnpackEvent = NULL;
		ftbl.Run = RunItem;
	}

	item = NEWTYPE( at_item_t );

	item->atif.ftbl = &ftbl;
	item->atif.id = StringToUnique( item_obj->name );
	item->atif.ati_obj = item_obj;
	item->atif.frame_count = 0;       

	return &item->atif;
}
