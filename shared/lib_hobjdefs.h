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



// lib_hobjdefs.h

#ifndef __lib_hobjdefs
#define __lib_hobjdefs

/*
  ==================================================
  hobject/pairs stuff

  ==================================================
*/

#define HPAIR_TYPE_SIZE		( 8 )
#define HPAIR_KEY_SIZE		( 32 )
#define HPAIR_VALUE_SIZE	( 64 )
#define HOBJ_TYPE_SIZE		( 32 )
#define HOBJ_NAME_SIZE		( 32 )

#define HPAIR_VALUE_FRAG	( 60 )	// max token size of value frags

typedef struct hpair_s
{
	char		type[HPAIR_TYPE_SIZE];
	char		key[HPAIR_KEY_SIZE];
//	char		value[HPAIR_VALUE_SIZE];
	char		*value;

	// special type 'bstring'
//	char		*bytes;

	struct hpair_s	*next;
} hpair_t;

typedef struct hobj_s 
{
	char		type[HOBJ_TYPE_SIZE];
	char		name[HOBJ_NAME_SIZE];
	
	struct hpair_s	*pairs;		// list
	struct hobj_s	*hobjs;		// list

	struct hobj_s	*parent;
	struct hobj_s	*next;

	void		*extra;		// a pointer for user class rep
} hobj_t;

typedef struct hobj_search_iterator_s
{
	char		search_type[HOBJ_TYPE_SIZE];
	struct hobj_s	*hobj;
	struct hobj_s	*current;
} hobj_search_iterator_t;

typedef struct hpair_search_iterator_s
{
	char		search_key[HPAIR_KEY_SIZE];
	struct hobj_s	*hobj;
	struct hpair_s	*current;
} hpair_search_iterator_t;



#endif
