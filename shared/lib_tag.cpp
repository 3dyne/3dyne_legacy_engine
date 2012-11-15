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



// lib_tag.c

#include "lib_tag.h"
#include "g_shared.h"

/*
  ==============================
  U_NewTag

  ==============================
*/
u_tag_t * U_NewTag( char *tag_name, int objsize )
{
	int	textsize;
	size_t		realsize;
	u_tag_t	*tag;

	textsize = strlen( tag_name ) + 1 + objsize;		
	realsize = (size_t)&(((u_tag_t *)0)->text[textsize]);
	tag = (u_tag_t *)NEWBYTES( realsize );

	tag->data_size = objsize;
	strcpy( &tag->text[objsize], tag_name );

	return tag;
}

/*
  ==============================
  U_TagGetObjPtr

  ==============================
*/
void * U_TagGetObjPtr( u_tag_t *tag )
{
	return (void *)(&tag->text[0]);
}

/*
  ==============================
  U_TagGetObjSize

  ==============================
*/
int U_TagGetObjSize( u_tag_t *tag )
{
	return tag->data_size;
}

/*
  ==============================
  U_TagGetKey

  ==============================
*/
void * U_TagGetKey( const void *tag )
{
	u_tag_t		*t;
	char		*c;

	t = (u_tag_t *) tag;
	c = &t->text[t->data_size];

//	printf( "U_TagGetKey: %s\n", c );

	return (void *)(c);
}

/*
  ==============================
  U_TagKeyCompare

  ==============================
*/
int U_TagKeyCompare( const void *k1, const void *k2 )
{
	return strcmp( (char *)(k1), (char *)(k2) );
}
