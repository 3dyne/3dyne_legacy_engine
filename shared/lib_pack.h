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



 // u_packed.h

#ifndef __u_packed
#define __u_packed
#include "u_defs.h"

void U_BeginPack( int mode, void *ptr, int maxsize );
int U_EndPack( void );	// returns actually packed bytes
void U_BeginUnpack( int mode, void *ptr, int size );
int U_EndUnpack( void ); // returns actually unpacked bytes

void U_Packs8( char );
void U_Packs16( short );
void U_Packs32( int );
void U_Packfp32( float );
void U_PackntString( char * );
void U_PackString( char *, int size );

void U_Unpacks8( char * );
void U_Unpacks16( short * );
void U_Unpacks32( int * );
void U_Unpackfp32( float * );
void U_UnpackntString( char *, int maxsize );
void U_UnpackString( char *, int size );
void U_UnpackSkip( int );

#endif
