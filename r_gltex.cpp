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



// r_gltex.c

/*
  =============================================================================
  setup textures for opengl
  
  =============================================================================
*/


#include "r_private.h"
#include "interfaces.h"


// glu fake
GLint APIENTRY gluBuild2DMipmaps( GLenum a, GLint b, GLsizei c, GLsizei d, GLenum e, GLenum f, const void * g)
{
	return 0;
}

/*
  ====================
  R_SetupTexture_TGA_RGB888_mipmap

  ====================
*/
GLuint R_SetupTexture_TGA_RGB888_mipmap( ca_tga_t *tga )
{
	int		i;
	GLuint		texobj;
	int		pixels;
	char            *tmp, *ptr;

	if ( !tga )
		__error( "null tga.\n" );

	if ( tga->bpp != 3 )
		__error( "no 24-bit-rgb tga.\n" );

	glGenTextures( 1, &texobj );
	glBindTexture( GL_TEXTURE_2D, texobj );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);           
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);           
                                                                                
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);       
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); 

	tmp = (char *) alloca( tga->width*tga->height*tga->bpp );
	ptr = tmp;                                                              
                               
	pixels = tga->width*tga->height;

        for ( i = 0; i < pixels; i++ )                               
        {                                                                       
                *ptr++ = tga->image[3*i];                                     
                *ptr++ = tga->image[3*i+1];                                   
                *ptr++ = tga->image[3*i+2];                                    
        }

	if ( gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGB, tga->width, tga->height, GL_RGB, GL_UNSIGNED_BYTE, tmp ) != 0 )
		__error( "opengl texture download failed.\n" );

	return texobj;
}


/*
  ====================
  R_SetupTexture_TGA_RGBA8888_mipmap

  ====================
*/
GLint R_SetupTexture_TGA_RGBA8888_mipmap( ca_tga_t *tga )
{
        int             i;                                                      
        GLuint          texobj;                                                 
	int		pixels;
	char            *tmp, *ptr;                                             


	if ( !tga )
		__error( "null tga.\n" );

	if ( tga->bpp != 4 )
		__error( "no 32-bit-argb tga.\n" );
	                                                    
        glGenTextures( 1, &texobj );                                            
        glBindTexture( GL_TEXTURE_2D, texobj );                                 
                                                                                
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);           
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);           
                                                                                
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);       
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); 
                                                                                
        tmp = (char *) alloca( tga->width*tga->height*tga->bpp );          
        ptr = tmp;                                                              
                               
	pixels = tga->width*tga->height;

        for ( i = 0; i < pixels; i++ )                               
        {                                                                       
                *ptr++ = tga->image[4*i+1];                                   
                *ptr++ = tga->image[4*i+2];                                    
                *ptr++ = tga->image[4*i+3];                                    
                *ptr++ = tga->image[4*i];                                     

        }                                                                       

        if ( gluBuild2DMipmaps( GL_TEXTURE_2D, GL_RGBA, tga->width, tga->height, GL_RGBA, GL_UNSIGNED_BYTE, tmp ) != 0 )
		__error( "opengl texture download failed.\n" );	

        return texobj; 
}

/*
  ====================
  R_SetupTexture_TGA_RGB888

  ====================
*/
GLint R_SetupTexture_TGA_RGB888( ca_tga_t *tga )
{
	int		i;
	GLuint		texobj;
	int		pixels;
	char            *tmp, *ptr;

	if ( !tga )
		__error( "null tga.\n" );

	if ( tga->bpp != 3 )
		__error( "no 24-bit-rgb tga.\n" );

	glGenTextures( 1, &texobj );
	glBindTexture( GL_TEXTURE_2D, texobj );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);           
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);           
                                                                                
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);       
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 

	tmp = (char *) alloca( tga->width*tga->height*tga->bpp );
	ptr = tmp;                                                              
                               
	pixels = tga->width*tga->height;

        for ( i = 0; i < pixels; i++ )                               
        {                                                                       
                *ptr++ = tga->image[3*i];                                     
                *ptr++ = tga->image[3*i+1];                                   
                *ptr++ = tga->image[3*i+2];                                    
        }

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, tga->width, tga->height, 0, GL_RGB, GL_UNSIGNED_BYTE, tmp );
	// __error( "opengl texture download failed.\n" );

	return texobj;	
}

/*
  ====================
  R_SetupTexture_TGA_RGBA8888

  ====================
*/
GLint R_SetupTexture_TGA_RGBA8888( ca_tga_t *tga )
{
        int             i;                                                      
        GLuint          texobj;                                                 
	int		pixels;
	char            *tmp, *ptr;                                             


	if ( !tga )
		__error( "null tga.\n" );

	if ( tga->bpp != 4 )
		__error( "no 32-bit-argb tga.\n" );
	                                                    
        glGenTextures( 1, &texobj );                                            
        glBindTexture( GL_TEXTURE_2D, texobj );                                 
                                                                                
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);           
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);           
                                                                                
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);       
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
                                                                                
        tmp = (char *) alloca( tga->width*tga->height*tga->bpp );          
        ptr = tmp;                                                              
                               
	pixels = tga->width*tga->height;

        for ( i = 0; i < pixels; i++ )                               
        {                                                                       
                *ptr++ = tga->image[4*i+1];                                   
                *ptr++ = tga->image[4*i+2];                                    
                *ptr++ = tga->image[4*i+3];                                    
                *ptr++ = tga->image[4*i];                                     

        }                                                                       

	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, tga->width, tga->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tmp ); 
	//__error( "opengl texture download failed.\n" );	

        return texobj; 	
}
