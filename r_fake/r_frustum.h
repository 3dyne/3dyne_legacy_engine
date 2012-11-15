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



// r_frustum.h

/*
  =============================================================================
  frustum setup and clipping functions

  =============================================================================
*/

#ifndef __r_frustum
#define __r_frustum

#include "r_math.h"
#include "r_defs2.h"
#include "r_state.h"
//#include "render.h"

#include "r_frustumdefs.h"

#define FRUSTUM_ON_EPSILON	( 0.01 )


// frustum setup
void R_InitBaseFrustum( frustum_t *f, fp_t width, fp_t height, fp_t viewdist );
void R_FrustumCalcPlanes( frustum_t *f );
void R_FrustumRotate( frustum_t *f, matrix3_t rotmat );
void R_FrustumTranslate( frustum_t *f, vec3d_t vec );
void R_FrustumGetNearpoly( frustum_t *f, vec3d_t nearpoly[] );
void R_FrustumDump( frustum_t *f );

// frustum test
bool_t R_FrustumTestPoint( frustum_t *f, vec3d_t p );
frustumClipMask R_FrustumTestBB( frustum_t *f, vec3d_t min, vec3d_t max );

// frustum clip
void R_FrustumClipVertex3Buffer_devel( frustum_t *f, void *out, void *in );
void R_FrustumClipVertex5Buffer_devel( frustum_t *f, void *out, void *in );



#endif
