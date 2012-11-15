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



// r_initdata.c
#include "lib_container.h"
#include "r_interfaces.h"
#include "r_interface.h"
extern g_map_t	*a_map;

void CompilePlaneClass( hmanager_t *planehm );
void SetupNodeBounds( void );
void SetupNodeLeafbits();


void R_InitData( g_map_t *map )
{
	r_frame_count = 1;
	r_local_count = 1;
	r_curlod = 0.0;
	r_job_list = U_NewList();

	INITTYPE( &r_upload_frame, icu_upload_frame_t );
	
	// hack: global map for imgcache
	a_map = map;

	CompilePlaneClass( map->planehm );
	CompileMapnodeClass( map->mapnodehm, map->planehm );
	CompileVisleafClass( map->visleafhm, map->planehm );

	SetupNodeBounds();
	SetupNodeLeafbits();	

}

void R_CleanUpData( g_map_t *map )
{
	int			i;

	// todo: clean up ShapeCtrlJobs
	R_CleanUpAllShapeControlJobs();
	
	// todo: clean up Shapes/Layer/Shaders
	R_CleanUpAllShapes();
	R_CleanUpAllShapeRefs();

	// todo: clean up ImageCache

	// todo: clean up BSP

	// clean up visportal see
	for ( i = 0; i < r_portalnum; i++ )
	{
		FREE( r_portals[i].see_through );
	}
}

void R_InitGLResources( void )
{

}

void R_FreeGLResources( void )
{

}
