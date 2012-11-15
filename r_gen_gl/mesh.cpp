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



// mesh.c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "lib_math.h"
#include "shock.h"
#include "g_shared.h"
#include "ca_service.h"

#include "mesh_defs.h"

static unique_t		m_count = 1;

static int	m_arraynum;
static mesh_array_t	m_arrays[16];

/*
  ==============================
  Mesh_BeginInstance

  ==============================
*/
void Mesh_BeginInstance( void )
{
	m_arraynum = 0;
}

/*
  ==============================
  Mesh_AddIntArray

  ==============================
*/
void Mesh_AddIntArray( int size )
{
	if ( m_arraynum >= MESH_MAX_ARRAYS )
		__error( "reached MESH_MAX_ARRAYS\n" );

	m_arrays[m_arraynum].size = size;
	m_arrays[m_arraynum].elmt_bytes = sizeof( int );
	m_arraynum++;
}

/*
  ==============================
  Mesh_AddFloatArray

  ==============================
*/
void Mesh_AddFloatArray( int size )
{
	if ( m_arraynum >= MESH_MAX_ARRAYS )
		__error( "reached MESH_MAX_ARRAYS\n" );

	m_arrays[m_arraynum].size = size;
	m_arrays[m_arraynum].elmt_bytes = sizeof( fp_t );
	m_arraynum++;	
}

/*
  ==============================
  Mesh_AddVec2dArray

  ==============================
*/
void Mesh_AddVec2dArray( int size )
{
	if ( m_arraynum >= MESH_MAX_ARRAYS )
		__error( "reached MESH_MAX_ARRAYS\n" );

	m_arrays[m_arraynum].size = size;
	m_arrays[m_arraynum].elmt_bytes = sizeof( vec2d_t );
	m_arraynum++;		
}

/*
  ==============================
  Mesh_AddVec3dArray

  ==============================
*/
void Mesh_AddVec3dArray( int size )
{
	if ( m_arraynum >= MESH_MAX_ARRAYS )
		__error( "reached MESH_MAX_ARRAYS\n" );

	m_arrays[m_arraynum].size = size;
	m_arrays[m_arraynum].elmt_bytes = sizeof( vec3d_t );
	m_arraynum++;	
}

/*
  ==============================
  Mesh_AddExternIntArray

  ==============================
*/
void Mesh_AddExternIntArray( int size, void *ptr )
{
	if ( m_arraynum >= MESH_MAX_ARRAYS )
		__error( "reached MESH_MAX_ARRAYS\n" );

	m_arrays[m_arraynum].size = size;
	m_arrays[m_arraynum].elmt_bytes = 0;
	m_arrays[m_arraynum].ptr = ptr;
	m_arraynum++;
}

/*
  ==============================
  Mesh_AddExternFloatArray

  ==============================
*/
void Mesh_AddExternFloatArray( int size, void *ptr )
{
	if ( m_arraynum >= MESH_MAX_ARRAYS )
		__error( "reached MESH_MAX_ARRAYS\n" );

	m_arrays[m_arraynum].size = size;
	m_arrays[m_arraynum].elmt_bytes = 0;
	m_arrays[m_arraynum].ptr = ptr;
	m_arraynum++;	
}

/*
  ==============================
  Mesh_AddExternVec2dArray

  ==============================
*/
void Mesh_AddExternVec2dArray( int size, void *ptr )
{
	if ( m_arraynum >= MESH_MAX_ARRAYS )
		__error( "reached MESH_MAX_ARRAYS\n" );

	m_arrays[m_arraynum].size = size;
	m_arrays[m_arraynum].elmt_bytes = 0;
	m_arrays[m_arraynum].ptr = ptr;
	m_arraynum++;		
}

/*
  ==============================
  Mesh_AddExternVec3dArray

  ==============================
*/
void Mesh_AddExternVec3dArray( int size, void *ptr )
{
	if ( m_arraynum >= MESH_MAX_ARRAYS )
		__error( "reached MESH_MAX_ARRAYS\n" );

	m_arrays[m_arraynum].size = size;
	m_arrays[m_arraynum].elmt_bytes = 0;
	m_arrays[m_arraynum].ptr = ptr;
	m_arraynum++;	
}

/*
  ==============================
  Mesh_EndInstance

  ==============================
*/
mesh_t * Mesh_EndInstance( void )
{
	int		i;
	mesh_t	*mesh;
	int		bytes;
	size_t		size;
    int total_bytes;
	unsigned char	*ptr;

	size = (size_t)&(((mesh_t *)0)->arrays[m_arraynum]);

	bytes = 0;       
	for ( i = 0; i < m_arraynum; i++ )
	{
		if ( m_arrays[i].elmt_bytes == 0 )
			continue;

		// internal memory
		bytes += m_arrays[i].size * m_arrays[i].elmt_bytes;
	}
	
	total_bytes = size + bytes;
	mesh = (mesh_t *)NEWBYTES( total_bytes );
	mesh->name = m_count++;

	mesh->arraynum = m_arraynum;
	
	ptr = ((unsigned char*)mesh) + size;
	for ( i = 0; i < m_arraynum; i++ )
	{
		if (  m_arrays[i].elmt_bytes == 0 )
		{
			mesh->arrays[i].ptr = m_arrays[i].ptr;
			mesh->arrays[i].size = m_arrays[i].size;
			mesh->arrays[i].elmt_bytes = 0;
		}
		else
		{
			mesh->arrays[i].ptr = (void *)ptr;
			mesh->arrays[i].size = m_arrays[i].size;
			mesh->arrays[i].elmt_bytes = m_arrays[i].elmt_bytes;
			ptr += ( m_arrays[i].size * m_arrays[i].elmt_bytes );
		}
	}
	
	return mesh;
}

/*
  ==============================
  Mesh_GetName

  ==============================
*/
unique_t Mesh_GetName( mesh_t *mesh )
{
	return mesh->name;
}

/*
  ==============================
  Mesh_CheckName

  ==============================
*/
bool_t Mesh_CheckName( mesh_t *mesh, unique_t name )
{
	if ( mesh->name == name )
		return true;

	return false;
}

/*
  ==============================
  Mesh_Free

  ==============================
*/
void Mesh_Free( mesh_t *mesh )
{
	FREE( mesh );
}

/*
  ==============================
  Mesh_GetIntBase

  ==============================
*/
void Mesh_GetIntBase( mesh_t *mesh, int array, int *size, int **ptr )
{
	*size = mesh->arrays[array].size;
	*ptr = (int *)(mesh->arrays[array].ptr);
}

/*
  ==============================
  Mesh_GetFloatBase

  ==============================
*/
void Mesh_GetFloatBase( mesh_t *mesh, int array, int *size, fp_t **ptr )
{
	*size = mesh->arrays[array].size;
	*ptr = (fp_t *)(mesh->arrays[array].ptr);
}

/*
  ==============================
  Mesh_GetVec2dBase
  
  ==============================
*/
void Mesh_GetVec2dBase( mesh_t *mesh, int array, int *size, vec2d_t **ptr )
{
	*size = mesh->arrays[array].size;
	*ptr = (vec2d_t *)(mesh->arrays[array].ptr);
}       

/*
  ==============================
  Mesh_GetVec3dBase

  ==============================
*/
void Mesh_GetVec3dBase( mesh_t *mesh, int array, int *size, vec3d_t **ptr )
{
	*size = mesh->arrays[array].size;
	*ptr = (vec3d_t *)(mesh->arrays[array].ptr);
}


