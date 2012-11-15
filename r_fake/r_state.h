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



// r_state.h

#ifndef __r_state
#define __r_state

//#include "render.h"
#include "r_defs2.h"
#include "lib_math.h"
#include "g_defs.h"

//
// gc_service.c
//
extern unsigned int	ms_rfbegin;

//
// render3.c
//

extern g_map_t		*a_map;

extern int		ri_local_light_num;
extern ri_local_light_t	**ri_local_lights;

extern int		r_local_count;
extern int		r_frame_count;

extern u_list_t		*r_job_list;

extern icu_upload_frame_t	r_upload_frame;

extern shape_t		*r_visible_shapes[];
extern int		r_visible_shape_num;

extern fp_t	r_curlod;

//
// r_data.c
//

extern int		r_leafrefnum;
extern leafref_t	r_leafrefs[];

extern int		r_mapnodenum;
extern mapnode_t	r_mapnodes[];

extern int		r_visleafnum;
extern int		r_leafbitpos[];

// plane revison
extern int		r_planenum;
extern cplane_t		*r_planes;

// visleaf stuff
extern int	r_pointnum ;
extern int	r_portalnum;
extern int	r_visleafnum;
extern vec3d_t	r_points[];
extern portal_t r_portals[];
extern visleaf_t r_visleafs[];

// mapnode
extern int		r_mapnodenum;
extern mapnode_t	r_mapnodes[];

extern int		r_visleafnum;
extern int		r_leafbitpos[];


extern int		r_frame_count;	// render3.c
extern int		r_tri_count;

extern vec3d_t		r_origin;
extern matrix3_t	r_matrix;

//
// r_initgl.c
//

extern gl_extensions_t		gl_ext;

//
// r_shapes.c
//
extern int		r_cfaceshapenum;
extern shape_t		*r_cfaceshapes[];

extern int		r_csurfshapenum;
extern shape_t		*r_csurfshapes[];

extern int	r_sfaceshapenum;
extern shape_t	*r_sfaceshapes[];

extern int		r_meshtileshapenum;
extern shape_t		*r_meshtileshapes[];

#endif
