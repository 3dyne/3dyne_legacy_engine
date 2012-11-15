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



// u_hmanagerdefs.h

#ifndef __u_hmanagerdefs
#define __u_hmanagerdefs

#include "lib_hobjdefs.h"

/*
  ==================================================
  hmanager stuff

  ==================================================
*/



#define HMANAGER_HASH_SIZE	( 256 )

typedef struct hmanager_listnode_s
{
	struct 	hmanager_listnode_s	*next;
	hobj_t		*obj;
} hmanager_listnode_t;

typedef struct 
{
	hobj_t		*root;

	hmanager_listnode_t	*hash[HMANAGER_HASH_SIZE];
} hmanager_t;

typedef struct
{
	int		hash;
	char		type[HOBJ_TYPE_SIZE];
	hmanager_t	*hm;
	struct hmanager_listnode_s	*current;
} hmanager_type_iterator_t;


#endif
