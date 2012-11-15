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



// r_defs.h

#ifndef r_defs_h
#define r_defs_h


/*
  ==================================================
  draw2d stuff

  ==================================================
*/
// R_Draw2dRegisterTga: arg type
#define		DRAW2D_REGISTER_TGA_MIPMAP	( 1 )
#define		DRAW2D_REGISTER_TGA_RGB		( 2 )
#define		DRAW2D_REGISTER_TGA_ARGB	( 4 )

// R_Draw2dSetDrawMode: arg mode
#define		DRAW2D_DRAWMODE_ARGB		( 0 )
#define		DRAW2D_DRAWMODE_ADD_RGB		( 1 )
#define		DRAW2D_DRAWMODE_REPLACE		( 2 )

// R_Hint: tell your wishes to the render
#define		R_HINT_GRAB_MOUSE	( 1 )
#define		R_HINT_UNGRAB_MOUSE	( 2 )
#endif
