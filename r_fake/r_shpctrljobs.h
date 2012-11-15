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



// r_shpctrljobs.h

#ifndef __r_shp_ctrl_jobs
#define __r_shp_ctrl_jobs

#include "r_shpctrljobsdefs.h"

/*
  ==================================================
  reference impl job: ValidateShape

  ==================================================
*/

shape_ctrl_job_t * ShapeCtrlJob_Create_ValidateShape( shape_t *shp );

//
// UpdateLayerSubimage
//

shape_ctrl_job_t * ShapeCtrlJob_Create_UpdateLayerSubimage( shape_t *shp, int layer_ordinal, void *pixels );

//
// LightmapStateWatch
//

shape_ctrl_job_t * ShapeCtrlJob_Create_LightmapStateWatch( shape_t *shp );

//
// AmbientColor
//

shape_ctrl_job_t * ShapeCtrlJob_Create_AmbientColor( shape_t *shp, wf_shape_info_t *info );

//
// ModulateAlpha
//

shape_ctrl_job_t * ShapeCtrlJob_Create_ModulateAlpha( hobj_t *job_obj, layer_t *layer );

//
// ModulateColor
//

shape_ctrl_job_t * ShapeCtrlJob_Create_ModulateColor( hobj_t *job_obj, layer_t *layer );

//
// ShiftTexCoord
//

shape_ctrl_job_t * ShapeCtrlJob_Create_ShiftTexCoord( hobj_t *job_obj, layer_t *layer );

//
// EnvMap
//

shape_ctrl_job_t * ShapeCtrlJob_Create_EnvMap( hobj_t *job_obj, layer_t *layer );

//
// DetailMap
//

shape_ctrl_job_t * ShapeCtrlJob_Create_DetailMap( hobj_t *job_obj, layer_t *layer );

//
// SurfaceDeform
//
 
shape_ctrl_job_t * ShapeCtrlJob_Create_SurfaceDeform( hobj_t *job_obj, shape_t *shape );

#endif

