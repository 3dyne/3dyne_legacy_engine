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



// main.h
#ifndef __MAIN_H_INCLUDED
#define __MAIN_H_INCLUDED
#include <math.h>

#include "defs.h"


#define MM_HEAPSIZE	( 2000000 )
//#define SARFILE0NAME	"/mnt/arch"
#define SARFILE0NAME	"/tmp/arch00.sar"


extern char		font[128][64];
extern int		m_counter;

extern char	padir[];
extern char	pgdir[];

extern g_resources_t	*g_rs;
extern g_state_t	*g_st;

extern l_export_t	le;
extern l_import_t	li;

//extern l_export_t	*(*L_GetAPI_ptr)( l_import_t* );

void StartUpGpi();
void ShutDownGpi();
void SaveConfig();
void Exit( void );

void ShockHandler();
int g_main( int, char** );

#endif
