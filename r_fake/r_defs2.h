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



// r_defs.h

#ifndef __r_defs
#define __r_defs

#include "lib_math.h"
#include "u_hobj.h"
#include "lib_bezier.h"
#include "r_frustumdefs.h"

#include "g_defs.h"
#include "g_tracedefs.h"


#include "ca_defs.h"

#include "shape_defs.h"
#include "wf_info_defs.h"

//#include "render.h"
#ifndef HAVE_OWN_CPLANE_TYPE
typedef struct cplane_s
{
	hobj_t		*self;
	vec3d_t		norm;
	fp_t		dist;
	int		type;

//	cp_dword_t	id_flipplane;
	struct cplane_s	*flipplane;


	// internal
	int		count;	// for bsp quick test

} cplane_t;
#endif


typedef struct leafref_s
{
	int		mapnode;
} leafref_t;



typedef struct
{
	bool_t		have_ext_va;
	bool_t		have_ext_cva;
	bool_t		have_arb_multitexture;
}  gl_extensions_t;



//
// visleaf stuff
//

typedef struct portal_s
{	
//	int		complex_num;
//	int		through_num;

	unsigned char	*see_through;

	cplane_t	*pl;

	int		pointnum;
	int		startpoint;
} portal_t;

typedef struct visleaf_s
{
	hobj_t		*self;
	int		count;		// for drawing

	int		bitpos;
	int		portalnum;
	int		startportal;
} visleaf_t;

// mapnode

typedef struct shape_ref_s
{
	struct shape_ref_s	*next;
	shape_t		*shape;
} shape_ref_t;

typedef struct mapnode_s 
{
	hobj_t		*self;

	cplane_t	*pl;

	// node
	int		firstbit, lastbit;	// continues bitpos of all leafs in this node
	int		child[2];

	// node+leaf clipping
	vec3d_t		min;
	vec3d_t		max;
	frustumClipMask	clip;

	// empty leaf
	vec3d_t		center;

	int		is_visible;	// set by CacheVisibleShapes each rf
	int		visleaf;	// for debug draw, set by CompileVisleafClass

	bool_t		visinfo;
//	unsigned char	can_see[4096];
	int		leafrefnum;
	int		startleafref;

	int		brushrefnum;
	int		startbrushref;

	int		facenum;
	int		startface;

	// new shape stuff
	shape_ref_t	*shaperefs;		// list

} mapnode_t;


#endif
