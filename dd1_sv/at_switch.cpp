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



// at_switch.c

#include "sv_state.h"

#include "l_head.h"
#include "l_defs.h"

#include "s_mem.h"
#include "lib_hobj.h"
#include "shock.h"

#include "events.h"

typedef enum
{
	switchStyle_none,
	switchStyle_use,
	switchStyle_inside,

	switchStyle_num
} switchStyle;

typedef struct at_switch_s
{
	ati_t		atif;

	// private data
	switchStyle	style;
	bool_t		state;
	vec3d_t		origin;	
	fp_t		radius;		// bound sphere

	vec3d_t		rel_min;
	vec3d_t		rel_max;

	// running ...
	bool_t		inside1;
	bool_t		inside2;

	bool_t		use1;
	bool_t		use2;
} at_switch_t;

static void SomeoneIsInsideSwitch( at_switch_t *_switch )
{
	if ( _switch->style != switchStyle_inside )
	{
		// ignore 'inside' message
		return;
	}

	_switch->inside1 = true;
}

static void SomeoneUseSwitch( at_switch_t *_switch )
{
	if ( _switch->style != switchStyle_use )
	{
		// ignore 'use' message
		return;
	}

	_switch->use1 = true;
}

/*
  ==============================
  ComSwitch

  ==============================
*/
static const char * ComSwitch( ati_t *atif, const char *in )
{
	at_switch_t	*_switch;

	_switch = ADDR_OF_BASE( at_switch_t, atif, atif );

	if ( !strcmp( in, "inside" ) )
	{
		SomeoneIsInsideSwitch( _switch );
	}
	else if ( !strcmp( in, "use" ) )
	{
		SomeoneUseSwitch( _switch );
	}
	else
	{
		__error( "can't handle message '%s'\n", in );
	}

//	__named_message( "message '%s'\n", in );

	return "ok";
}

/*
  ==============================
  RunItem

  ==============================
*/
static void InitSwitchFromClass( at_switch_t *_switch, hobj_t *switch_obj )
{
	hpair_t		*pair;
	int		i;

	static struct { const char *key; switchStyle style; } style_rtbl[] = {
		{ "default", switchStyle_use },
		{ "use", switchStyle_use },
		{ "inside", switchStyle_inside },
		{ NULL, switchStyle_none }
	};

	EasyFindVec3d( _switch->origin, switch_obj, "origin" );

	EasyFindInt( &i, switch_obj, "state" );
	// normalize state
	_switch->state = true;
	if ( i == 0 )
	{
		_switch->state = false;
	}

	//
	// fixme: setup bound box from bound sphere dist is not the right way
	//
	EasyFindFloat( &_switch->radius, switch_obj, "dist" );
	for ( i = 0; i < 3; i++ )
	{
		_switch->rel_min[i] = -_switch->radius;
		_switch->rel_max[i] =  _switch->radius;
	}

	pair = FindHPair( switch_obj, "style" );
	if ( !pair )
	{
		__error( "missing key 'style' in server object '#%u'\n", _switch->atif.id );
	}
	
	for( i = 0; style_rtbl[i].key ; i++ )
		if ( !strcmp( style_rtbl[i].key, pair->value ) ) break;

	if ( !style_rtbl[i].key )
	{
		__error( "unknown switch style '%s'\n", pair->value );
	}

	_switch->style = style_rtbl[i].style;
}

static void SelectSwitchable_func( ati_t *self, ati_t *other )
{
	at_switch_t	*_switch;
// 	const char	*com_res;

	_switch = ADDR_OF_BASE( at_switch_t, atif, self );		

//	__named_message( "object [%s:%s] update object [%s:%s]\n", self->ati_obj->type, self->ati_obj->name, other->ati_obj->type, other->ati_obj->name );

	if ( _switch->state != 0 )
	{
		/*com_res =*/ other->ftbl->Com( other, "is_on" );
	}
	else
	{
		/*com_res =*/ other->ftbl->Com( other, "is_off" );
	}
}

static void SwitchUpdateSwitchables( at_switch_t *_switch )
{
	//
	// state update of server side switchables
	//

	L_SwitchableMapSelectAtiWhereSwitch( &_switch->atif, SelectSwitchable_func );	

	//
	// state update of client side switch
	// no typeinfo is needed, switches are created by the client itself
	//
	{
		byte_iter_t	frm;
		PackFrame_Begin( bi_out, &frm );
		ByteIter_Packi( &frm, _switch->atif.id );
		ByteIter_Packi( &frm, eventType_client_obj_update_state );
		ByteIter_Packi( &frm, _switch->state );
		PackFrame_End( bi_out, &frm );
	}		
}

static void RunSwitch( ati_t *atif, atiRun run )
{
	at_switch_t	*_switch;

	_switch = ADDR_OF_BASE( at_switch_t, atif, atif );	

	if ( run == atiRun_think1 )
	{
		if ( atif->frame_count == 0 )
		{
			// first call, init switch from class
			InitSwitchFromClass( _switch, atif->ati_obj );

			// make id public
			L_AtiRegisterId( atif );
		}
		
		//
		// add switch object to collide map
		//
		L_CollideMapAddAtiWithRelBoundBox( atif, _switch->origin, _switch->rel_min, _switch->rel_max );
	}
	else if ( run == atiRun_think2 )
	{	
		//
		// send switch state to switchable
		//
		if ( (atif->frame_count&31)==0 )
		{
			SwitchUpdateSwitchables( _switch );	
		}


		//
		// fixme: hardcoded state changes
		//
		if ( _switch->inside1 && !_switch->inside2 )
		{
//			__named_message( "trigger 'raising edge'\n" );
			
			_switch->state ^= true;
			SwitchUpdateSwitchables( _switch );
		}

		if ( !_switch->inside1 && _switch->inside2 )
		{
//			__named_message( "trigger 'falling edge'\n" );

			_switch->state ^= true;
			SwitchUpdateSwitchables( _switch );
		}

		if ( _switch->use1 && !_switch->use2 )
		{
//			__named_message( "trigger 'raising edge'\n" );

			_switch->state ^= true;
			SwitchUpdateSwitchables( _switch );
		}

		if ( !_switch->use1 && _switch->use2 )
		{
//			__named_message( "trigger 'falling edge'\n" );

			// ignore falling edge of use_style
		}

		_switch->inside2 = _switch->inside1;		
		_switch->inside1 = false;
		_switch->use2 = _switch->use1;
		_switch->use1 = false;

		atif->frame_count++;
	}
}

/*
  ==============================
  DestroySwitch

  ==============================
*/
static void DestroySwitch( ati_t *atif )
{
	at_switch_t	*_switch;

	DeepDestroyClass( atif->ati_obj );

	_switch = ADDR_OF_BASE( at_switch_t, atif, atif );
	FREE( _switch );
}

/*
  ==================================================
  ATI_CreateSwitch

  ==================================================
*/
static bool_t		ftbl_valid = false;
static ati_ftbl_t	ftbl;

ati_t * ATI_CreateSwitch( hobj_t *switch_obj )
{
	at_switch_t	*_switch;
	
	if ( strcmp( switch_obj->type, "switch" ) )
		__error( "switch_obj is not of type 'switch'\n" );

	if ( ftbl_valid == false )
	{
		ftbl_valid = true;
		INITTYPE( &ftbl, ati_ftbl_t );

		ftbl.Destroy = DestroySwitch;
		ftbl.Com = ComSwitch;
		ftbl.UnpackEvent = NULL;
		ftbl.Run = RunSwitch;
	}

	_switch = NEWTYPE( at_switch_t );

	_switch->atif.ftbl = &ftbl;
	_switch->atif.id = StringToUnique( switch_obj->name );
	_switch->atif.ati_obj = switch_obj;
	_switch->atif.frame_count = 0;

	return &_switch->atif;
}
