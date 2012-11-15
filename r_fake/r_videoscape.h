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



// r_videoscape.h

#ifndef __r_videoscape
#define __r_videoscape

#include "shared.h"
#include "r_defs2.h"

typedef struct vsc_face_s
{
	struct vsc_face_s	*next;
	unsigned int	color;
	int		pointnum;
	int		p[8];		// variable sized
} vsc_face_t;

typedef struct vsc_model_s
{
	struct vsc_model_s	*next;
	int		facenum;
	vsc_face_t	*faces;
	int		pointnum;
	vec3d_t		p[8];		// variable sized
} vsc_model_t;

vsc_model_t*	VSC_NewModel( int pointnum );
void		VSC_FreeModel( vsc_model_t *model );
vsc_face_t*	VSC_NewFace( int pointnum );
void		VSC_FreeFace( vsc_face_t *face );

vsc_model_t*	VSC_LoadModel( char *file );


#endif
