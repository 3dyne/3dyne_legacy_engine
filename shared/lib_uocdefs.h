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



// lib_uocdefs.h

#ifndef __lib_uocdefs
#define __lib_uocdefs

#include "u_defs.h"

#define UOC_MESSAGE_MAX_SIZE		( 256 )

typedef struct uoc_message_s
{
	char		text[UOC_MESSAGE_MAX_SIZE];
} uoc_message_t;

typedef enum
{
	UOCNodePriority_normal,
	UOCNodePriority_lowest,
	UOCNodePriority_low,
	UOCNodePriority_high,
	UOCNodePriority_highest,

	UOCNodePriority_num
} uocNodePriority;

typedef struct uoc_node_s
{
	unique_t		self_id;	// primary key
	void			*owner_obj;
	uocNodePriority		level;
	struct uoc_ftbl_s	*ftbl;
} uoc_node_t;

typedef struct uoc_ftbl_s
{
	void	(*Comm)(uoc_node_t *called_obj, uoc_message_t *send, uoc_message_t *recv );
} uoc_ftbl_t;




#endif
