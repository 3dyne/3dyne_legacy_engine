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



// at_testobjI.c

/*
  =============================================================================
  model instance for testobj - impl

  =============================================================================
*/

#include "at_testobjFD.h"
#include "at_testobjID.h"

extern mdf_testobj_t	mdf_testobj;

/*
  ==============================
  MDI_Initialize_testobj

  ==============================
*/
void MDI_Initialize_testobj( mdi_testobj_t *mdi )
{
	MD_CoordsysSetMove( mdi->cs_root, mdf_testobj.mv_F_base_frame_C_root );
	MD_CoordsysInsertSubmodel( mdi->cs_root, mdf_testobj.submd_C_root_S_box->res_cache );
}



/*
  ==============================
  MDI_GetRootCoordsys_testobj

  ==============================
*/
md_coordsys_t * MDI_GetRootCoordsys_testobj( mdi_testobj_t *mdi )
{
	return mdi->cs_root;
}
