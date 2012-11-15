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



// ca_service.h
#ifndef __CA_SERVICE_H_INCLUDED
#define __CA_SERVICE_H_INCLUDED

#include "defs.h"

extern int mm_malloced, mm_freed, mm_nummalloc, mm_numfree;

void *_MM_Malloc( int );
void _MM_Free( void * );

#ifndef log_malloc


#define MM_Malloc( x )	_MM_Malloc( x )
#define MM_Free( x )		_MM_Free( x )

#else 
void *_MM_LogMalloc( int, char *, int );
void _MM_LogFree( void *, char *, int );

#define MM_Malloc( x )	_MM_LogMalloc( x , __FILE__, __LINE__ )
#define MM_Free( x )	_MM_LogFree( x , __FILE__, __LINE__ )
#endif 



extern int	sc_newmallocedbytes;


void  MM_StartUp();
void MM_ShutDown( void );

void MM_ShowInfo();

//void  MM_CoreDump();
//void MM_AddZone( unsigned int size, int zid );
//void MM_MallocZones();
//void MM_FreeZone( int zid );
//void *MM_ZoneMalloc( unsigned int size, int zid );

//void MM_DumpUsage( void );

//void MM_SetCurrentZone( int );

void  CA_StartUp();
void  CA_ShutDown();
void  CA_BeginCaching();
int   CA_EndCaching();
void  CA_CoreDump();
void  *CA_Load( char *, void *(*loadfunc)( char*, int ) );
int CA_CacheFile( char *, void *(*loadfunc)( char *, int ), int );	
void *CA_GetPtr( int );

#endif
