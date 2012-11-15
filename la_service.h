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



//layouter.h
#ifndef __LAYOUTER_H_INCLUDED
#define __LAYOUTER_H_INCLUDED
#include "defs.h"

void LA_RegisterFont( const char * );
void LA_StartUp();
void LA_ShutDown();
void LA_BeginDraw();
void LA_EndDraw();
void LA_DrawString( const char *, int, int );
void LA_DrawStringCRT( const char *, int, int );
void LA_DrawString80( const char *, int, int );
void LA_DrawString80CRT( const char *, int, int );
void LA_DrawChar( char, int, int );
void LA_DrawCharCRT( char, int, int );


//int LA_TgaIsCached( char * );
void LA_DrawGltexByGltex( res_gltex_cache_t *, int, int, int, int, int );
void LA_DrawGltexByName( const char *, int, int, int, int, int );

hudc_text_t* LA_HUD_CTextInit( int cols, int rows, void (*drawFunc) ( const char*, int, int ) );
void LA_HUD_CTextFlush( hudc_text_t *, int, int );

void LA_HUDPutChar( hudc_text_t *, char );
void LA_HUDPutLine( hudc_text_t* context, char* textline );
void LA_HUDPutString( hudc_text_t* context, char* string );
void LA_FreeGLResources();
void LA_InitGLResources();
void LA_InsertVarview( const char *varname, void (* callback )( int flag, char *string ));
void LA_ShowVarviews();

// implemented in la_varviews.c :
void LA_InitVarviews();

#endif
