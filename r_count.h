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



// r_count.h

#ifndef __r_count
#define __r_count

#define COUNT_RESET( x ) ( (x) = 0 )
#define COUNT_SET( x, y ) ( (x) = (y) )
#define COUNT_INCR( x ) ( (x)++ )
#define COUNT_DECR( x ) ( (x)++ )
#define COUNT_ADD( x, y ) ( (x)+=(y) )

extern unsigned int	count_rf_current_mapnode;
extern unsigned int	count_rf_num_visportal;
extern unsigned int	count_rf_num_vis_mapnode;		// num mapnode from pvs
extern unsigned int	count_rf_num_passed_mapnode;		// num mapnode passed frustum check
extern unsigned int	count_rf_num_vis_shape;

extern unsigned int	count_rf_num_run_job;
extern unsigned int	count_rf_num_destroy_job;
extern unsigned int	count_rf_num_vis_job;

extern unsigned int	count_rf_num_shader_bindtex;
extern unsigned int	count_rf_num_shader_blendfunc;
extern unsigned int	count_rf_num_shader_layer;

extern unsigned int	count_rf_num_fpsys;		// num fpsys registerd for rf
extern unsigned int	count_rf_num_fpsys_visible;	// num fpsys registerd passed vis
extern unsigned int	count_rf_num_fpsys_bindtex;
extern unsigned int	count_rf_num_fpsys_blendfunc;
extern unsigned int	count_rf_num_fpsys_particle;	// num of rendered particles

extern unsigned int	count_be_num_transform;
extern unsigned int	count_be_num_begin;
extern unsigned int	count_be_num_flush;
extern unsigned int	count_be_num_tri;

#endif
