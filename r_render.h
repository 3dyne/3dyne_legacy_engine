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



// r_render.h

/*
  =============================================================================
  external render API

  =============================================================================
*/
  
#ifndef __r_render
#define __r_render

#include "defs.h"
#include "interfaces.h"

/*
  ==================================================
  glx, wgl specific stuff

  ==================================================
*/
void R_StartUp( void );
void R_ShutDown( void );

void R_SwapBuffer( void );
void R_Hint( int );

/*
  ==================================================
  draw2d stuff

  ==================================================
*/

int R_Draw2dRegisterTga( ca_tga_t *texture, int type );

void R_BeginDraw2d( void );
void R_EndDraw2d( void );
void R_Draw2dSetWindow( fp_t x0, fp_t y0, fp_t x1, fp_t y1 );
void R_Draw2dSetGrid( int xgrid, int ygrid );
void R_Draw2dSetTexture( int tex );
void R_Draw2dSetColor3f( fp_t red, fp_t green, fp_t blue );
void R_Draw2dSetDrawMode( int mode );
void R_Draw2dTexRect( int x, int y, int width, int height, fp_t tx, fp_t ty, fp_t twidth, fp_t theight );
void R_Draw2dRect( int x, int y, int width, int height );

#endif 
