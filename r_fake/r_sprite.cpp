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



// r_sprite.c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "r_private.h"
#include "r_sprite.h"
#include "render.h"

void R_DrawSprite( g_resource_t *gltex, vec3d_t pos, fp_t width, fp_t height )
{
	vec3d_t		tmp;
	vec4d_t		v;
	res_gltex_cache_t	*cache;
	int		texobj;

	cache = ( res_gltex_cache_t * ) gltex->res_cache;
	texobj = cache->texobj;

	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glBindTexture( GL_TEXTURE_2D, texobj );
	glEnable( GL_TEXTURE_2D );
	glEnable ( GL_BLEND );                                          
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );	

	Vec3dScale( tmp, 1.0/16.0, pos );
	CalcVertex( v, tmp );

	glColor4f( 1, 1, 1, 1 );

	glBegin( GL_TRIANGLE_FAN );
	glTexCoord2f( 0,0 );
	glVertex3f( (v[0]+width)/v[3], (v[1]-height)/v[3], 1.0/v[3] );
	glTexCoord2f( 1,0 );
	glVertex3f( (v[0]-width)/v[3], (v[1]-height)/v[3], 1.0/v[3] );
	glTexCoord2f( 1,1 );
	glVertex3f( (v[0]-width)/v[3], (v[1]+height)/v[3], 1.0/v[3] );
	glTexCoord2f( 0,1 );
	glVertex3f( (v[0]+width)/v[3], (v[1]+height)/v[3], 1.0/v[3] );
	glEnd();	
	
}

void R_DrawHaloSprite( g_resource_t *gltex, vec3d_t pos, fp_t width, fp_t height )
{
	vec3d_t		tmp;
	vec4d_t		v;

	Vec3dScale( tmp, 1.0/16.0, pos );
	CalcVertex( v, tmp );

//	glColor4f( 1, 1, 1, 1 );

	glBegin( GL_TRIANGLE_FAN );
	glTexCoord2f( 0,0 );
	glVertex3f( (v[0]/v[3]+width), (v[1]/v[3]-height), 1.0/v[3] );
	glTexCoord2f( 1,0 );
	glVertex3f( (v[0]/v[3]-width), (v[1]/v[3]-height), 1.0/v[3]  );
	glTexCoord2f( 1,1 );
	glVertex3f( (v[0]/v[3]-width), (v[1]/v[3]+height), 1.0/v[3]  );
	glTexCoord2f( 0,1 );
	glVertex3f( (v[0]/v[3]+width), (v[1]/v[3]+height), 1.0/v[3]  );
	glEnd();	
	
}

void R_DrawSprite2( g_resource_t *gltex, vec3d_t pos, fp_t width, fp_t height )
{
	vec3d_t		tmp;
	vec4d_t		v;
	res_gltex_cache_t	*cache;
	int		texobj;

	cache = ( res_gltex_cache_t * ) gltex->res_cache;
	texobj = cache->texobj;

	glBindTexture( GL_TEXTURE_2D, texobj );

	Vec3dScale( tmp, 1.0/16.0, pos );
	CalcVertex( v, tmp );

//	glColor4f( 1, 1, 1, 1 );

	glBegin( GL_TRIANGLE_FAN );
	glTexCoord2f( 0,0 );
	glVertex3f( (v[0]+width)/v[3], (v[1]-height)/v[3], 1.0/v[3] );
	glTexCoord2f( 1,0 );
	glVertex3f( (v[0]-width)/v[3], (v[1]-height)/v[3], 1.0/v[3] );
	glTexCoord2f( 1,1 );
	glVertex3f( (v[0]-width)/v[3], (v[1]+height)/v[3], 1.0/v[3] );
	glTexCoord2f( 0,1 );
	glVertex3f( (v[0]+width)/v[3], (v[1]+height)/v[3], 1.0/v[3] );
	glEnd();		
}

#define SPRITE_ROTATATION_STEPS		( 32 )

static bool_t	r_rotates_valid = false;
static fp_t	r_rotates[SPRITE_ROTATATION_STEPS][4][2];

void R_DrawSpriteWithRotation( g_resource_t *gltex, vec3d_t pos, fp_t width, fp_t height, fp_t rotate )
{
	vec3d_t		tmp;
	vec4d_t		v;
	res_gltex_cache_t	*cache;
	int		texobj;
	int		rot;

	if ( r_rotates_valid == false )
	{
		fp_t	pts[4][2]={ {1.0, -1.0}, {-1.0,-1.0}, { -1.0, 1.0 }, { 1.0, 1.0 } };
		int	j, k;
		fp_t	s, c, angle;
		for ( j = 0; j < SPRITE_ROTATATION_STEPS; j++ )
		{
			angle = (j/(fp_t)(SPRITE_ROTATATION_STEPS)) * M_PI*2.0; //(j*(360.0/16.0)) / (180.0*M_PI);
			s = sin( angle );
			c = cos( angle );
			for ( k = 0; k < 4; k++ )
			{
				r_rotates[j][k][0] = c*pts[k][0] - s*pts[k][1];
				r_rotates[j][k][1] = s*pts[k][0] + c*pts[k][1];
			}
		}		
		r_rotates_valid = true;
	}
	
	rot = ((int)(rotate/(360.0/(fp_t)(SPRITE_ROTATATION_STEPS))))%SPRITE_ROTATATION_STEPS;
//	printf( "%d ", rot );
//	rot = 0;

	cache = ( res_gltex_cache_t * ) gltex->res_cache;
	texobj = cache->texobj;

	glBindTexture( GL_TEXTURE_2D, texobj );

	Vec3dScale( tmp, 1.0/16.0, pos );
	CalcVertex( v, tmp );

//	glColor4f( 1, 1, 1, 1 );

	if ( v[3] <= 0.0 )
		return;

	v[3] = 1.0/v[3];


	glBegin( GL_TRIANGLE_FAN );
	glTexCoord2f( 0,0 );
	glVertex3f( (v[0]+r_rotates[rot][0][0]*width)*v[3], (v[1]+r_rotates[rot][0][1]*height)*v[3], 1.0*v[3] );
	glTexCoord2f( 1,0 );
	glVertex3f( (v[0]+r_rotates[rot][1][0]*width)*v[3], (v[1]+r_rotates[rot][1][1]*height)*v[3], 1.0*v[3] );
	glTexCoord2f( 1,1 );
	glVertex3f( (v[0]+r_rotates[rot][2][0]*width)*v[3], (v[1]+r_rotates[rot][2][1]*height)*v[3], 1.0*v[3] );
	glTexCoord2f( 0,1 );
	glVertex3f( (v[0]+r_rotates[rot][3][0]*width)*v[3], (v[1]+r_rotates[rot][3][1]*height)*v[3], 1.0*v[3] );
	glEnd();		
}

void R_DrawTexImage2DWithRotation( vec3d_t pos, fp_t width, fp_t height, fp_t rotate )
{
	vec3d_t		tmp;
	vec4d_t		v;
	unsigned int		rot;

	if ( r_rotates_valid == false )
	{
		fp_t	pts[4][2]={ {1.0, -1.0}, {-1.0,-1.0}, { -1.0, 1.0 }, { 1.0, 1.0 } };
		int	j, k;
		fp_t	s, c, angle;
		for ( j = 0; j < SPRITE_ROTATATION_STEPS; j++ )
		{
			angle = (j/(fp_t)(SPRITE_ROTATATION_STEPS)) * M_PI*2.0; //(j*(360.0/16.0)) / (180.0*M_PI);
			s = sin( angle );
			c = cos( angle );
			for ( k = 0; k < 4; k++ )
			{
				r_rotates[j][k][0] = c*pts[k][0] - s*pts[k][1];
				r_rotates[j][k][1] = s*pts[k][0] + c*pts[k][1];
			}
		}		
		r_rotates_valid = true;
	}
	
	rot = ((unsigned int)(rotate/(360.0/(fp_t)(SPRITE_ROTATATION_STEPS))))%SPRITE_ROTATATION_STEPS;
//	printf( "%d ", rot );
//	rot = 0;

	Vec3dScale( tmp, 1.0/16.0, pos );
	CalcVertex( v, tmp );

//	glColor4f( 1, 1, 1, 1 );

	if ( v[3] <= 0.0 )
		return;
	
	v[3] = 1.0/v[3];	

	glBegin( GL_TRIANGLE_FAN );
	glTexCoord2f( 0,0 );
	glVertex3f( (v[0]+r_rotates[rot][0][0]*width)*v[3], (v[1]+r_rotates[rot][0][1]*height)*v[3], v[3] );
	glTexCoord2f( 1,0 );
	glVertex3f( (v[0]+r_rotates[rot][1][0]*width)*v[3], (v[1]+r_rotates[rot][1][1]*height)*v[3], v[3] );
	glTexCoord2f( 1,1 );
	glVertex3f( (v[0]+r_rotates[rot][2][0]*width)*v[3], (v[1]+r_rotates[rot][2][1]*height)*v[3], v[3] );
	glTexCoord2f( 0,1 );
	glVertex3f( (v[0]+r_rotates[rot][3][0]*width)*v[3], (v[1]+r_rotates[rot][3][1]*height)*v[3], v[3] );
	glEnd();		
}
