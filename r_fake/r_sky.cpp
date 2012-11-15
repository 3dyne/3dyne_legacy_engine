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



// r_sky.c

#include "render.h"
#include "r_interfaces.h"
#include "interfaces.h"

extern matrix3_t	sky_matrix;

void CalcSkyVertex( vec4d_t out, vec3d_t in )
{
        vec3d_t         tmp; 
//	Vec3dScale( tmp, 1.0/16.0, r_origin );
//        Vec3dSub( tmp, in, tmp /*r_origin*/ );
        Matrix3Vec3dRotate( tmp, in, sky_matrix );
        out[0] = tmp[0];
        out[1] = tmp[1]*1.33;
        out[3] = 1.0 + tmp[2] / 1.0;
        out[2] = 1.0; //0.0001 * out[3]; // 1 !!!	
}

void R_RenderSkyBox( void )
{
#if 1
	
	int	i, j;

	static int skyobjs[6] = { -1, -1, -1, -1, -1, -1 };
	static char skyparts[6][3] = { "lt", "ft", "rt", "bk", "up", "dn" };
	static char skyname[] = "hollow2";

	static vec5d_t		skydef[6*4] =
	{
		// 
		// left side
		//
		{ -1.0,  1.0, -1.0, 0.0, 0.0 },
		{ -1.0,  1.0,  1.0, 1.0, 0.0 },
		{ -1.0, -1.0,  1.0, 1.0, 1.0 },
		{ -1.0, -1.0, -1.0, 0.0, 1.0 },
		
		
		//
		// front side
		//
		{ -1.0,  1.0,  1.0, 0.0, 0.0 },
		{  1.0,  1.0,  1.0, 1.0, 0.0 },
		{  1.0, -1.0,  1.0, 1.0, 1.0 },
		{ -1.0, -1.0,  1.0, 0.0, 1.0 },
		
		
		//
		// right side
		//
		{  1.0,  1.0,  1.0, 0.0, 0.0 },
		{  1.0,  1.0, -1.0, 1.0, 0.0 },
		{  1.0, -1.0, -1.0, 1.0, 1.0 },
		{  1.0, -1.0,  1.0, 0.0, 1.0 },
		
		
		//
		// back side
		//
		{  1.0,  1.0, -1.0, 0.0, 0.0 },
		{ -1.0,  1.0, -1.0, 1.0, 0.0 },
		{ -1.0, -1.0, -1.0, 1.0, 1.0 },
		{  1.0, -1.0, -1.0, 0.0, 1.0 },
		
		//
		// top side
		//
		{ -1.0,  1.0, -1.0, 0.0, 0.0 },
		{  1.0,  1.0, -1.0, 1.0, 0.0 },
		{  1.0,  1.0,  1.0, 1.0, 1.0 },
		{ -1.0,  1.0,  1.0, 0.0, 1.0 },
//		{ -1.0,  1.0, -1.0, 0.0, 1.0 },
//		{  1.0,  1.0, -1.0, 0.0, 0.0 },
//		{  1.0,  1.0,  1.0, 1.0, 0.0 },
//		{ -1.0,  1.0,  1.0, 1.0, 1.0 },
		
		//
		// bottom side
		//
		{ -1.0, -1.0,  1.0, 0.0, 0.0 },
		{  1.0, -1.0,  1.0, 1.0, 0.0 },
		{  1.0, -1.0, -1.0, 1.0, 1.0 },
		{ -1.0, -1.0, -1.0, 0.0, 1.0 }	

//		{ -1.0, -1.0,  1.0, 1.0, 0.0 },
//		{  1.0, -1.0,  1.0, 1.0, 1.0 },
//		{  1.0, -1.0, -1.0, 0.0, 1.0 },
//		{ -1.0, -1.0, -1.0, 0.0, 0.0 }	
	};


	//
	// init
	//
	

	if ( skyobjs[0] == -1 )
	{
		printf( "R_RenderSkyBox: init ...\n" );

		for ( i = 0; i < 6; i++ )
		{
			char	str[256];
			g_resource_t	*res;
			res_gltex_cache_t	*cache;

			sprintf( str, "gltex.sky.%s.%s", skyname, skyparts[i] );

			res = G_ResourceSearch( g_rs, str );
			if ( !res )
			{
				__error( "can't get resource '%s'\n", str );
			}

			cache = (res_gltex_cache_t *) res->res_cache;

			skyobjs[i] = cache->texobj;
		}
	}


	//
	// draw
	// 
	glEnable(GL_DEPTH_TEST);
	glDepthFunc( GL_ALWAYS );
//	glDepthMask(GL_FALSE);
	glEnable( GL_TEXTURE_2D );
	glDisable(GL_BLEND);
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
	for ( i = 0; i < 6; i++ )
	{
		glBindTexture( GL_TEXTURE_2D, skyobjs[i] );
		glBegin( GL_TRIANGLE_FAN );
		glColor3f( 1.0, 1.0, 1.0 );
		for ( j = 0; j < 4; j++ )
		{
			vec4d_t		w;
			vec3d_t		v;
			Vec3dScale( v, 10240.0, &skydef[i*4+j][0] );
			CalcSkyVertex( w, v );
			glTexCoord2f( skydef[i*4+j][3], skydef[i*4+j][4] );
//			glVertex3f( w[0]/w[3], w[1]/w[3], 0.0 );
			glVertex4fv( w );
//			glVertex3f( skydef[i*4+j][0]*2.0, skydef[i*4+j][1]*2.0, skydef[i*4+j][2]*2.0 );
		}
		glEnd();
	}
//	glDepthMask(GL_TRUE);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc( GL_GEQUAL );
#endif
}


