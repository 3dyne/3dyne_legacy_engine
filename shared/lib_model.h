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



// lib_model.h

#ifndef __lib_model
#define __lib_model

#include "u_defs.h"
#include "lib_modeldefs.h"
#include "lib_math.h"
#include "res_submddefs.h"

md_coordsys_t * MD_NewCoordsys( void );
void MD_FreeCoordsys( md_coordsys_t *cs );
void MD_CoordsysInsertCoordsys( md_coordsys_t *parent, md_coordsys_t *child );


fp_t * MD_MoveAccess( md_move_t *mv, unsigned int step, int param );
unsigned int MD_MoveGetStepnum( md_move_t *mv );

int MD_CalcStep( md_move_t *mv, msec_t msec_local );

void MD_MoveGetParameter( md_move_t *mv, vec3d_t origin, fp_t *lat, fp_t *lon, fp_t *yaw );
void MD_CoordsysSetMove( md_coordsys_t *cs, md_move_t *mv );
void MD_CoordsysInsertSubmodel( md_coordsys_t *cs, md_submodel_t *submodel );

void MD_CalcAxisPoint( vec3d_t out, vec3d_t in, vec3d_t axis[3] );

#endif
