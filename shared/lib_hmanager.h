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



#include "lib_hobjdefs.h"
#include "lib_hmanagerdefs.h"
#include "lib_math.h"

#ifndef __lib_fuckyou
#define __lib_fuckyou
/*
  ==================================================
  hmanager stuff

  ==================================================
*/

hmanager_t * NewHManager( void );
hmanager_t * NewHManagerFromClass( hobj_t *root  );
hmanager_t * NewFlatHManagerFromClass( hobj_t *root );

void FreeHManager( hmanager_t *hm );
void DumpHManager( hmanager_t *hm, bool_t verbose );
hobj_t* HManagerGetRootClass( hmanager_t *hm );
void HManagerSetRootClass( hmanager_t *hm, hobj_t *obj );
int HManagerCalcHashkey( char *name );
void HManagerRebuildHash( hmanager_t *hm );
void HManagerHashClass( hmanager_t *hm, hobj_t *obj );
void HManagerUnhashClass( hmanager_t *hm, hobj_t *obj );

void HManagerIndexClassesOfType( hmanager_t *hm, int *start, const char *type );

hobj_t * HManagerSearchClassName( hmanager_t *hm, char *name );
hobj_t * HManagerSearchClassName_linear( hmanager_t *hm, char *name );

void HManagerInsertClass( hmanager_t *hm, hobj_t *parent, hobj_t *obj );
void HManagerRemoveClass( hmanager_t *hm, hobj_t *obj );
void HManagerDeepDestroyClass( hmanager_t *hm, hobj_t *obj );
void HManagerRemoveAndDestroyAllClassesOfType( hmanager_t *hm, hobj_t *obj, char *type );

bool_t HManagerCheckClassConsistancy( hmanager_t *hm );

void HManagerInitTypeSearchIterator( hmanager_type_iterator_t *iter, hmanager_t *hm, const char *type );
hobj_t * HManagerGetNextClass( hmanager_type_iterator_t *iter );


#endif
