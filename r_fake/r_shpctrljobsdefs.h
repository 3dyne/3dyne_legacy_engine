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



// r_shp_ctrl_jobs_defs.h

#ifndef __r_shp_ctrl_jobs_defs
#define __r_shp_ctrl_jobs_defs

#include "shape_ctrl_job_defs.h"		// shape control job interface

/*
  ==================================================
  reference impl job: ValidateShape

  ==================================================
*/
typedef struct shape_ctrl_job_validate_shape_s
{
	shape_ctrl_job_t	shape_ctrl_job_if;		// inherits shape_ctrl_job interface

	// private data:
	shape_t			*shape;		// this shape is target of the job

} shape_ctrl_job_validate_shape_t;

//
// UpdateLayerSubimage
//
typedef struct shape_ctrl_job_UpdateLayerSubimage_s
{
	shape_ctrl_job_t	shape_ctrl_job_if;		// inherits shape_ctrl_job interface

	// private data:
	shape_t			*shape;		// this shape is target of the job
	int			layer_ordinal;
	void			*update_pixels;
} shape_ctrl_job_UpdateLayerSubimage_t;

//
// LightmapStateWatch
//
typedef struct shape_ctrl_job_LightmapStateWatch_s
{
	shape_ctrl_job_t	shape_ctrl_job_if;

	// private data:
	shape_t			*shape;		// this shape is target of the job
	lightmap_t		*lightmap_head;
	int			run_count;
	msec_t			ms_start;	// job was started at this world time ( for local time styles )
	
	bool_t			instable_states;

	int			cache_pos;
	struct pixel_cache_s { int key; void *pixel; }	cache[32];

} shape_ctrl_job_LightmapStateWatch_t;


#endif
