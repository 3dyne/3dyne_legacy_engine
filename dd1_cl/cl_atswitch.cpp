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



// cl_atswitch.c

#include "cl_state.h"
#include "cl_head.h"

#include "s_mem.h"
#include "lib_hobj.h"
#include "shock.h"

#include "events.h"

typedef struct cl_at_switch_s
{
	cl_ati_t	atif;

	bool_t		ev_valid;
	bool_t		ev_state;

	// private data
	bool_t		state;

} cl_at_switch_t;

/*
  ==============================
  UnpackEventSwitch

  ==============================
*/
static void UnpackEventSwitch( cl_ati_t *atif, unique_t id, eventType type, byte_iter_t *bi )
{
	cl_at_switch_t	*_switch;

	_switch = ADDR_OF_BASE( cl_at_switch_t, atif, atif );

	if ( _switch->ev_valid )
	{
//		__warning( "override event\n" );
	}

	if ( type == eventType_client_obj_update_state )
	{
		_switch->ev_state = ByteIter_Unpacki( bi );
	}
	else
	{
		__warning( "unkown event for client object '#%u'\n", id );
	}
	
	_switch->ev_valid = true;
}

/*
  ==============================
  RunSwitch

  ==============================
*/
static void SelectSwitchable_func( cl_ati_t *self, cl_ati_t *other )
{
	cl_at_switch_t	*_switch;
// 	const char	*com_res;

	_switch = ADDR_OF_BASE( cl_at_switch_t, atif, self );		

	if ( _switch->state != 0 )
	{
		/*com_res =*/ other->ftbl->Com( other, "is_on" );
	}
	else
	{
		/*com_res =*/ other->ftbl->Com( other, "is_off" );
	}
}

static void SwitchUpdateSwitchables( cl_at_switch_t *_switch )
{
	CL_SwitchableMapSelectAtiWhereSwitch( &_switch->atif, SelectSwitchable_func );
}

static void RunSwitch( cl_ati_t *atif )
{
	cl_at_switch_t	*_switch;

	_switch = ADDR_OF_BASE( cl_at_switch_t, atif, atif );

	if ( atif->frame_count == 0 )
	{
		
	}

	if ( _switch->ev_valid == true )
	{
		_switch->ev_valid = false;

//		if ( _switch->ev_state != _switch->state )
		{
			_switch->state = _switch->ev_state;

			SwitchUpdateSwitchables( _switch );
		}
	}

	atif->frame_count++;
}

/*
  ==============================
  DestroySwitch

  ==============================
*/
static void DestroySwitch( cl_ati_t *atif )
{
	cl_at_switch_t	*_switch;

	DeepDestroyClass( atif->ati_obj);

	_switch = ADDR_OF_BASE( cl_at_switch_t, atif, atif );
	FREE( _switch );	
}

/*
  ==================================================
  create client switch

  ==================================================
*/
static bool_t		ftbl_valid = false;
static cl_ati_ftbl_t	ftbl;

cl_ati_t * CL_CreateSwitch( hobj_t *obj_switch )
{
	cl_at_switch_t	*_switch;

	if ( ftbl_valid == false )
	{
		ftbl_valid = true;
		INITTYPE( &ftbl, cl_ati_ftbl_t );

		ftbl.Destroy = DestroySwitch;
		ftbl.UnpackEvent = UnpackEventSwitch;
		ftbl.Run = RunSwitch;
	}

	_switch = NEWTYPE( cl_at_switch_t );
	_switch->atif.ftbl = &ftbl;
	_switch->atif.id = StringToUnique( obj_switch->name );
	_switch->atif.ati_obj = obj_switch;

	_switch->atif.frame_count = 0;

	return &_switch->atif;
}
