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



// cbspbrush.h

#ifndef __cbspbrush
#define __cbspbrush

#include "r_defs2.h"


#ifndef HAVE_OWN_CPLANE_TYPE
typedef struct cplane_s
{
	hobj_t		*self;

	vec3d_t		norm;
	fp_t		dist;
	int		type;

	struct cplane_s	*flipplane;

	// internal
	int		count;	// for bsp quick test

} cplane_t;
#endif

typedef struct ctexdef_s
{
	hobj_t		*self;
} ctexdef_t;

typedef struct csurface_s
{
	unsigned int		contents;
	unsigned int		state;
	cplane_t		*pl;
	ctexdef_t		*td;

	// internal
	polygon_t		*p;
} csurface_t;

typedef struct cbspbrush_s
{
	unsigned int		contents;
	hobj_t			*original;

	// internal
	vec3d_t			min, max;
	struct cbspbrush_s	*next;

	int			surfacenum;
	struct csurface_s	surfaces[16];	// variable
} cbspbrush_t;

ctexdef_t * NewCTexdef( void );
cplane_t * NewCPlane( void );

cbspbrush_t * NewBrush( int surfnum );
void FreeBrush( cbspbrush_t *b );
void FreeBrushList( cbspbrush_t *list );
cbspbrush_t * CopyBrush( cbspbrush_t *b );
int BrushListLength( cbspbrush_t *list );
void CreateBrushPolygons( cbspbrush_t *b );
void CalcBrushBounds( cbspbrush_t *b );
void CalcBrushListBounds( cbspbrush_t *list, vec3d_t min, vec3d_t max );
fp_t CalcBrushVolume( cbspbrush_t *b );

#define BRUSH_BACK_ON   ( 0 )
#define BRUSH_FRONT_ON  ( 1 )
#define BRUSH_BACK      ( 2 )
#define BRUSH_FRONT     ( 3 )
#define BRUSH_SPLIT     ( 4 )

#define BRUSH_NONE	( 5 )	// no test result


int CSG_IsExactOnPlane( cbspbrush_t *in, cplane_t *plane );
void CSG_SplitBrush_new( cbspbrush_t *in, cplane_t *plane, cbspbrush_t **front, cbspbrush_t **back );

cbspbrush_t * CompileCBspbrushClass( hobj_t *brush, hmanager_t *planehm, hmanager_t *texdefhm );
hobj_t * BuildCBpsbrushClass( cbspbrush_t *b, hmanager_t *planehm, hmanager_t *texdefhm );

#endif
