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



// r_misc.c

#include "render.h"

#include "interfaces.h" 

/*
  ==================================================
  string stuff

  ==================================================
*/
void strcrpl( char *text, char that, char with )
{
	char	*ptr;
	
	ptr = text;
	for ( ; *ptr ; ptr++ )
	{
		if ( *ptr == that )
			*ptr = with;
	}
}

/*
  ==================================================
  image convert

  ==================================================
*/
unsigned char* Image565ToImage888( unsigned short *in, int pixelnum )
{

	int		i;
	unsigned short		pixel565;
	unsigned char		*pixel888;
	unsigned char		*rgb888;
	
	rgb888 = (unsigned char*) malloc( pixelnum * 3 );
	pixel888 = rgb888;

	for ( i = 0; i < pixelnum; i++ )
	{
		pixel565 = *in++;

		*pixel888++ = ((pixel565>>11)&31)<<3;
		*pixel888++ = ((pixel565>>5)&63)<<2;
		*pixel888++ = ((pixel565&31))<<3;
	}

	return rgb888;
}

#if 0
GLuint Misc_GenTexture_TGA_8888( char *name )
{
	ca_tga_t	*tga;
	int		i, pixels;
	GLuint		texobj;
	unsigned char            *tmp, *ptr;

	
	i = CA_CacheFile( name, CA_LoadTga, CA_TGA );
	
	tga = CA_GetPtr( i );

	if ( !tga )
		__error( "Misc_GenTexture_TGA: tga failed.\n" );

	glGenTextures( 1, &texobj );
	glBindTexture( GL_TEXTURE_2D, texobj );
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);


	tmp = (unsigned char *) alloca( tga->width*tga->height*tga->bpp );
	ptr = tmp;
	                              
	pixels = tga->width*tga->height;

        for ( i = 0; i < pixels; i++ )                               
        {                                                                       
                *ptr++ = tga->image[4*i+1];                                     
                *ptr++ = tga->image[4*i+2];                                   
                *ptr++ = tga->image[4*i+3];                                    
		*ptr++ = tga->image[4*i];  
        }

//	gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGBA, tga->image_width, tga->image_height, GL_RGBA, GL_UNSIGNED_BYTE, tmp );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, tga->width, tga->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp);

	return texobj;
}
#endif

/*
  ==================================================
  vertex stuff

  ==================================================
*/

void CalcVertex( vec4d_t out, vec3d_t in )
{
        vec3d_t         tmp; 
	Vec3dScale( tmp, 1.0/16.0, r_origin );
        Vec3dSub( tmp, in, tmp /*r_origin*/ );
        Matrix3Vec3dRotate( tmp, tmp, r_matrix );
        out[0] = tmp[0];
        out[1] = tmp[1]*1.33;
        out[3] = 1.0 + tmp[2] / 1.0;
        out[2] = 1.0; // !!!
}

void ProjectVec3d( vec2d_t out, vec3d_t in, projectionType type )
{
	if ( ( type & ProjectionType_Mask ) == ProjectionType_X )
	{
		out[0] = in[2];
		out[1] = in[1];		
	}
	else if ( ( type & ProjectionType_Mask ) == ProjectionType_Y )
	{
		out[0] = in[0];
		out[1] = in[2];
	}
	else if ( ( type & ProjectionType_Mask ) == ProjectionType_Z )
	{
		out[0] = in[0];
		out[1] = in[1];
	}
	else
	{
		__error( "ProjectVec3d: unkown projection type (%u): ", type );
		Vec3dPrint( in );
		out[0] = 0;
		out[1] = 0;	
	}
}


/*
  ==============================
  CalcVecsFromAngle

  ==============================
*/
void CalcVecsFromAngle( vec2d_t vecs[2], fp_t deg )
{
	fp_t	rad;
	fp_t		rad_sin;
	fp_t		rad_cos;
	int		i;
	vec2d_t		axis[2] = { {1,0}, {0,1} };

	rad = deg * D2R;
	rad_sin = sin( rad );
	rad_cos = cos( rad );

	for ( i = 0; i < 2; i++ )
	{
		vecs[i][0] = rad_cos*axis[i][0] - rad_sin*axis[i][1];
		vecs[i][1] = rad_sin*axis[i][0] + rad_cos*axis[i][1];
	}       
}


/*
  ==============================
  ProjectionTypeFromNorm

  ==============================
*/
projectionType ProjectionTypeFromNorm( vec3d_t norm )
{
	vec3d_t		a;       

	a[0] = fabs(norm[0]);
	a[1] = fabs(norm[1]);
	a[2] = fabs(norm[2]);
	
	if ( a[0] >= a[1] && a[0] >= a[2] )
		return ProjectionType_X;
	else if ( a[1] >= a[0] && a[1] >= a[2] )
		return ProjectionType_Y;
	else if ( a[2] >= a[0] && a[2] >= a[1] )
		return ProjectionType_Z;

	__error( "odd: can't get projection type of normal\n" );
	return (projectionType)-1;
}
