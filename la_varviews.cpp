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



// la_varviews.c
// the prefered location to implement varview callbacks.

#include "interfaces.h"



static void mem_mallocs( int flag, char *string )
{
	sprintf( string, "bytes:\t%d, %d, %d", mm_malloced - mm_freed, mm_malloced, mm_freed );
}

static void mem_nums( int flag, char *string )
{
	sprintf( string, "num:\t%d, %d, %d", mm_nummalloc - mm_numfree, mm_nummalloc, mm_numfree );
}

static void frameinfo( int flag, char *string )
{
	if( flag == 1 )
		sprintf( string, "fps: %.1f", gc_fpfps );
	else if( flag == 2 )
		sprintf( string, "tpf: %d", gc_tpf );
	else if( flag == 3 )
		sprintf( string, "tps: %d", gc_tps );
		
}	
	


void LA_InitVarviews()
{
	LA_InsertVarview( "vv_frame", frameinfo );

	LA_InsertVarview( "vv_mem", mem_mallocs );
	LA_InsertVarview( "vv_mem", mem_nums );
}
