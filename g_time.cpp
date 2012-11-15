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



// g_time.c

#include "interfaces.h"

/*
  ==============================
  G_NewVecIpo

  ==============================
*/
g_vec_ipo_t * G_NewVecIpo( unsigned int msec1, unsigned int msec2, vec3d_t pos, vec3d_t delta )
{
	g_vec_ipo_t	*ipo;

	ipo = NEWTYPE( g_vec_ipo_t );
	ipo->msec1 = msec1;
	ipo->msec2 = msec2;

	Vec3dCopy( ipo->pos, pos );
	Vec3dCopy( ipo->delta, delta );

	return ipo;
}

/*
  ==============================
  G_InitVecIpo

  ==============================
*/
void G_InitVecIpo( g_vec_ipo_t *ipo, unsigned int msec1, unsigned int msec2, vec3d_t pos, vec3d_t delta )
{
	ipo->msec1 = msec1;
	ipo->msec2 = msec2;

	Vec3dCopy( ipo->pos, pos );
	Vec3dCopy( ipo->delta, delta );
}


/*
  ==============================
  G_FreeVecIpo

  ==============================
*/
void G_FreeVecIpo( g_vec_ipo_t *ipo )
{
	FREE( ipo );
}


/*
  ==============================
  G_InitAngleIpo

  ==============================
*/
void G_InitAngleIpo( g_angle_ipo_t *ipo, unsigned int msec1, unsigned int msec2, fp_t lat, fp_t lat_delta, fp_t lon, fp_t lon_delta )
{
	ipo->msec1 = msec1;
	ipo->msec2 = msec2;

	ipo->lat = lat;
	ipo->lat_delta = lat_delta;

	ipo->lon = lon;
	ipo->lon_delta = lon_delta;
}

/*
  ==============================
  G_CalcVecIpo

  ==============================
*/
void G_CalcVecIpo( g_vec_ipo_t *ipo, unsigned int t, vec3d_t p )
{
	if ( t <= ipo->msec1 )
		Vec3dCopy( p, ipo->pos );
	else if ( t >= ipo->msec2 )
		Vec3dAdd( p, ipo->pos, ipo->delta );
	else
	{
		unsigned int		td;
		fp_t		scale;

		td = ipo->msec2 - ipo->msec1;
		scale = (((fp_t)(t-ipo->msec1)) / (fp_t)(td));
		Vec3dMA( p, scale, ipo->delta, ipo->pos  );
	}
}

/*
  ==============================
  G_CalcAngleIpo

  ==============================
*/
void G_CalcAngleIpo( g_angle_ipo_t *ipo, unsigned int t, fp_t *lat, fp_t *lon )
{
	if ( t <= ipo->msec1 )
	{
		*lat = ipo->lat;
		*lon = ipo->lon;
	}
	else if ( t >= ipo->msec2 )
	{
		*lat = ipo->lat+ipo->lat_delta;
		*lon = ipo->lon+ipo->lon_delta;
	}
	else
	{
		unsigned int	td;
		fp_t	scale;

		td = ipo->msec2 - ipo->msec1;
		scale = (((fp_t)(t-ipo->msec1)) / (fp_t)(td));

		*lat = scale * ipo->lat_delta + ipo->lat;
		*lon = scale * ipo->lon_delta + ipo->lon;
	}    
}

/*
  ==============================
  G_SetRenderHintOriginIpo

  ==============================
*/
void G_SetRenderHintOriginIpo( unsigned int msec_duration, vec3d_t pos, vec3d_t delta )
{
	unsigned int		msec1, msec2;

	msec1 = ms_wfbegin;
	msec2 = ms_wfbegin+100;
	
	G_InitVecIpo( &g_st->view_origin_ipo, msec1, msec2, pos, delta );
}

/*
  ==============================
  G_SetRenderHintAngleIpo

  ==============================
*/
void G_SetRenderHintAngleIpo( unsigned int msec_duration,fp_t lat, fp_t lat_delta, fp_t lon, fp_t lon_delta )
{
	unsigned int		msec1, msec2;

	msec1 = ms_wfbegin;
	msec2 = ms_wfbegin+100;
	
	G_InitAngleIpo( &g_st->view_angle_ipo, msec1, msec2, lat, lat_delta, lon, lon_delta );
}



