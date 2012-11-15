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



// ib_service.h

#ifndef ib_service_h
#define ib_service_h
#include "ib_defs.h"
//#include "defs.h"

extern int	ib_lastaction;
extern int	sc_readbytes;


void IB_StartUp();
void IB_ShutDown();

int IB_ProbeSource( char *name );
void IB_AddSource( const char *name, int type );

ib_file_t *IB_Open( const char *name );
ib_file_t *IB_OpenDiskFile( const char *name );
unsigned int IB_Read( void *buf, unsigned int size, unsigned int blocks, ib_file_t *file );
unsigned int IB_GetSize( ib_file_t *file );
void IB_SetPos( ib_file_t *file, unsigned int pos );
unsigned int IB_GetPos( ib_file_t *file );
void IB_Close( ib_file_t *file );
 
char *IB_GetLastname( );
int IB_GetChar( ib_file_t *file );
short IB_GetShort( ib_file_t *file );
int IB_GetInt( ib_file_t *file );
void IB_Skip( ib_file_t *file, int size );
void IB_DumpLastSar( const char *outdir );
#endif
