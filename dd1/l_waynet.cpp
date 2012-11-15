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



// l_waynet.c
#include "l_locals.h"

u_list_t	*waynet;

void L_InitWayNet()
{
	waynet = U_NewList();
}


void L_InsertWaypoint( at_waypoint_t *at )
{
	U_ListInsertAtHead( waynet, at );
}

void *WayNetFindBest( vec3d_t origin )
{
	ati_t	*best;
	ati_t	*ati;
	u_list_iter_t	iter;

	fp_t		bestlen;
	
	U_ListIterInit( &iter, waynet );


	best = NULL;
	
	for( ;( ati = ( ati_t * )U_ListIterNext( &iter )); )
	{
		vec3d_t	lenvec1, vec;
		fp_t	len;
		if( !best )
		{
			best = ati;
			ati->ftbl->GetOrigin( ati, vec );
			Vec3dSub( lenvec1, origin, vec );
			bestlen = Vec3dLen( lenvec1 );
			continue;
		}
		ati->ftbl->GetOrigin( ati, vec );
		Vec3dSub( lenvec1, origin, vec );
		len = Vec3dLen( lenvec1 );
		
		if( len <= bestlen )
		{
			best = ati;
			bestlen = len;
		}
	}

	return ( void * )best;
}
		
		
