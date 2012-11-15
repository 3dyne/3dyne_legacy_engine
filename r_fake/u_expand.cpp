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



// u_expand.c

#include "u_expand.h"

static char	*start;
static char	*pos;
static int	size;
static int	maxsize;

/*
  ==============================
  U_BeginExpand

  ==============================
*/
void U_BeginExpand( cinfo_t *cinfo, void *ptr )
{
	start = (char *) ptr;
	pos = start;
	maxsize = cinfo->size;
	size = 0;
}

/*
  ==============================
  U_EndExpand

  ==============================
*/
int U_EndExpand( void )
{
	return size;
}


/*
  ==============================
  U_ExpandS8

  ==============================
*/
void U_ExpandS8( char *s8 )
{
	*s8 = *pos;
	pos++;
	size++;
}



/*
  ==============================
  U_ExpandS16

  ==============================
*/
void U_ExpandS16( short *s16 )
{
	memcpy( s16, pos, 2 );
	pos+=2;
	size+=2;
}

/*
  ==============================
  U_ExpandS32

  ==============================
*/
void U_ExpandS32( int *s32 )
{
	memcpy( s32, pos, 4 );
	pos+=4;
	size+=4;
}


/*
  ==============================
  U_ExpandFP32

  ==============================
*/
void U_ExpandFP32( float *fp32 )
{
	memcpy( fp32, pos, 4 );
	pos+=4;
	size+=4;
}
