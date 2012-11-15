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



// g_time.h

#ifndef __g_time
#define __g_time

g_vec_ipo_t * G_NewVecIpo( unsigned int msec1, unsigned int msec2, vec3d_t pos, vec3d_t delta );
//g_angle_ipo_t * G_NewAngleIpo( unsigned int msec1, unsigned int msec2, fp_t lat, fp_t lat_delta, fp_t lon, fp_t lon_delta );
void G_InitVecIpo( g_vec_ipo_t *ipo, unsigned int msec1, unsigned int msec2, vec3d_t pos, vec3d_t delta );
void G_InitAngleIpo( g_angle_ipo_t *ipo, unsigned int msec1, unsigned int msec2, fp_t lat, fp_t lat_delta, fp_t lon, fp_t lon_delta );
void G_FreeVecIpo( g_vec_ipo_t *ipo );
//void G_FreeAngleIpo( g_angle_ipo_t *ipo );

void G_CalcVecIpo( g_vec_ipo_t *ipo, unsigned int t, vec3d_t p );
void G_CalcAngleIpo( g_angle_ipo_t *ipo, unsigned int t, fp_t *lat, fp_t *lon );

// for testing
void G_SetRenderHintOriginIpo( unsigned int msec_duration, vec3d_t pos, vec3d_t delta );
void G_SetRenderHintAngleIpo( unsigned int msec_duration, fp_t lat, fp_t lat_delta, fp_t lon, fp_t lon_delta );

#endif
