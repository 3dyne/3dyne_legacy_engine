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



// at_testobjF.c

/*
  =============================================================================
  model factory for testobj - impl

  =============================================================================
*/

#include "l_locals.h"
#include "at_testobjFD.h"
#include "at_testobjID.h"

mdf_testobj_t	mdf_testobj;


/*
  ==============================
  MDF_Init_testobj

  ==============================
*/
void MDF_Init_testobj( void )
{
	li.SH_Printf( " for 'testobj' ..." );

	memset( &mdf_testobj, 0, sizeof( mdf_testobj_t ) );

	//
	// moves
	//

	mdf_testobj.res_moves = li.GI_ResourceSearch( "moves.testobj" );
	
	if ( !mdf_testobj.res_moves )
		goto L_init_failed_testobj;

	{	    
		res_moves_cache_t	*moves;
		md_move_t		*move;

		moves = (res_moves_cache_t *) mdf_testobj.res_moves->res_cache;

		move = U_MapSearch( &moves->moves_map, "base_frame.root" );
		__chkptr( move );

		mdf_testobj.mv_F_base_frame_C_root = move;
	}

	//
	// submodels
	//

	{
		mdf_testobj.submd_C_root_S_box = li.GI_ResourceSearch( "submd.testobj.box" );
		if ( !mdf_testobj.submd_C_root_S_box )
			goto L_init_failed_testobj;
	}
	

	return;

L_init_failed_testobj:
	__error( "failed\n" );
}


/*
  ==============================
  MDF_Reject_testobj

  ==============================
*/
void MDF_Reject_testobj( void )
{

}

/*
  ==============================
  MDF_CreateInstance_testobj

  ==============================
*/
mdi_testobj_t * MDF_CreateInstance_testobj( void )
{
	mdi_testobj_t	*mdi;

	mdi = NEWTYPE( mdi_testobj_t );

	//
	// setup coordsystems
	//

	mdi->cs_root = MD_NewCoordsys();

	return mdi;
}

/*
  ==============================
  MDF_DestroyInstance_testobj

  ==============================
*/
void MDF_DestroyInstance_testobj( mdi_testobj_t *mdi )
{
	
}
