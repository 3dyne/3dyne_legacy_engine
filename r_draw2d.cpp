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



// r_draw2d.h

#include "r_private.h"
#include "interfaces.h"

// glu fake
void gluOrtho2D( GLdouble, GLdouble, GLdouble, GLdouble ); 

static int	r_xgrid;
static int	r_ygrid;
static fp_t	r_xgscl;
static fp_t	r_ygscl;

static fp_t	r_xorigin;
static fp_t	r_yorigin;
static fp_t	r_width;
static fp_t	r_height;

/*
  ====================
  R_Draw2dRegisterTga

  ====================
*/
int R_Draw2dRegisterTga( ca_tga_t *texture, int type )
{
	GLuint	texobj = 0;

	__chkptr( texture );

	if ( type == ( DRAW2D_REGISTER_TGA_RGB ) )
	{
		texobj = R_SetupTexture_TGA_RGB888( texture );
	}
	else if ( type == ( DRAW2D_REGISTER_TGA_RGB | DRAW2D_REGISTER_TGA_MIPMAP ) )
	{
		texobj = R_SetupTexture_TGA_RGB888_mipmap( texture );
	}
	else if ( type == ( DRAW2D_REGISTER_TGA_ARGB ) )
	{
		texobj = R_SetupTexture_TGA_RGBA8888( texture );
	}
	else if ( type == ( DRAW2D_REGISTER_TGA_ARGB | DRAW2D_REGISTER_TGA_MIPMAP ) )
	{
		texobj = R_SetupTexture_TGA_RGBA8888_mipmap( texture );
	}
	else if ( type == 0 )
	{
		if ( texture->bpp == 3 )
		{
			texobj = R_SetupTexture_TGA_RGB888( texture );
		}
		else if ( texture->bpp == 4 )
		{
			texobj = R_SetupTexture_TGA_RGBA8888( texture );
		}
		else
		{
			__error( "unknown tga type.\n" );
		}
	}
	else
	{
		__error( "unknown tga type.\n" );
	}

	return (int) texobj;
}


/*
  ====================
  R_BeginDraw2d

  ====================
*/
void R_BeginDraw2d( void )
{
	int	w, h;
	
	w = r_devicewidth->ivalue;
	h = r_deviceheight->ivalue;

	if ( w < 1 || w > 4096 || h < 1 || h > 2048 )
		__error( "odd device resolutin.\n" );

//	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode (GL_PROJECTION);                                                
	glLoadIdentity ();                                                           
	glOrtho (-2.0, 2.0, -2.0, 2.0, -1.0, 1.0 );
	glMatrixMode(GL_MODELVIEW);  
	glLoadIdentity ();         
	glDisable( GL_DEPTH_TEST );
}

/*
  ====================
  R_EndDraw2d

  ====================
*/
void R_EndDraw2d( void )
{
#if 0
	static fp_t	i = 0.0;
	glBegin( GL_POLYGON );

	glColor3f( i, i, i );
	i += 0.05;
	glVertex3f( -0.5, -0.5, 0 );
	glVertex3f( 0.5, -0.5, 0 );
	glVertex3f( 0.5, 0.5, 0 );
	glVertex3f( -0.5, 0.5, 0 );
	glEnd();
#endif
//	glFlush();
	glFlush();
}

/*
  ====================
  R_Draw2dSetWindow

  ====================
*/
void R_Draw2dSetWindow( fp_t x0, fp_t y0, fp_t x1, fp_t y1 )
{
	r_xorigin = x0;
	r_yorigin = y0;
	r_width = x1 - x0;
	r_height = y1 - y0;
}

/*
  ====================
  R_Draw2dSetGrid

  ====================
*/
void R_Draw2dSetGrid( int xgrid, int ygrid )
{
	r_xgrid = xgrid;
	r_ygrid = ygrid;

	r_xgscl = r_width / ((fp_t)(xgrid));
	r_ygscl = r_height / ((fp_t)(ygrid));
}

/*
  ====================
  R_Draw2dSetColor3f

  ====================
*/
void R_Draw2dSetColor3f( fp_t red, fp_t green, fp_t blue )
{
 	glColor3f( red, green, blue );
}

/*
  ====================
  R_Draw2dSetDrawMode

  ====================
*/
void R_Draw2dSetDrawMode( int mode )
{
	if ( mode == DRAW2D_DRAWMODE_ARGB )
	{
		glEnable ( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );		
	}
	else if ( mode == DRAW2D_DRAWMODE_ADD_RGB )
	{
		glEnable ( GL_BLEND );
		glBlendFunc( GL_ONE, GL_ONE );
	}
	else if ( mode == DRAW2D_DRAWMODE_REPLACE )
	{
		glEnable ( GL_BLEND );
		glBlendFunc( GL_ONE, GL_ZERO );
	}
	else
	{
		__error( "unknown mode.\n" );
	}
}

/*
  ====================
  R_Draw2dSetTexture

  ====================
*/
void R_Draw2dSetTexture( int texobj )
{
	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, (GLuint) texobj );
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
}

/*
  ====================
  R_Draw2dTexRect

  ====================
*/
void R_Draw2dTexRect( int x, int y, int width, int height, fp_t tx, fp_t ty, fp_t twidth, fp_t theight )
{
	fp_t	xx, yy;
	fp_t		w, h;
	
	xx = r_xorigin + ((fp_t)(x)) * r_xgscl;
	yy = r_yorigin + ((fp_t)(y)) * r_ygscl;
	
	w = ((fp_t)(width)) * r_xgscl;
	h = ((fp_t)(height)) * r_ygscl;
	
	glBegin( GL_POLYGON );
	glTexCoord2f( tx, ty );
	glVertex2f( xx, yy );

	glTexCoord2f( tx+twidth, ty );
	glVertex2f( xx+w, yy );

	glTexCoord2f( tx+twidth, ty+theight );
	glVertex2f( xx+w, yy+h );

	glTexCoord2f( tx, ty+theight );
	glVertex2f( xx, yy+h );
	glEnd();	
}

/*
  ====================
  R_Draw2dRect

  ====================
*/
void R_Draw2dRect( int x, int y, int width, int height )
{
	fp_t	xx, yy;
	fp_t		w, h;
	
	xx = r_xorigin + ((fp_t)(x)) * r_xgscl;
	yy = r_yorigin + ((fp_t)(y)) * r_ygscl;
	
	w = ((fp_t)(width)) * r_xgscl;
	h = ((fp_t)(height)) * r_ygscl;
	
	glBegin( GL_POLYGON );
	printf( "%f %f\n", xx, yy );
	glVertex2f( xx, yy );
	glVertex2f( xx+w, yy );
	glVertex2f( xx+w, yy+h );
	glVertex2f( xx, yy+h );
	glEnd();	
}

