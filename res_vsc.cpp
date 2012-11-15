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



// res_vsc.c

#include "interfaces.h"
#include "g_shared.h"

#include "r_glbackenddefs.h"

#include "res_vsc.h"
#include "res_vscdefs.h"

res_vsc_register_t * Res_SetupRegisterVSC( char *path )
{
	res_vsc_register_t	*reg;
	int		size;

	size = strlen( path ) + 1;

	reg = NEWBYTES( size );
	strcpy( reg->path, path );

	return reg;	
}


res_vsc_cache_t * Res_CacheInVSC( res_vsc_register_t *reg )
{
	tokenstream_t		*ts;
	int	i, num, bytes, size;
	int	vertexnum;
	int	vrefnum;
	int	cmdnum;

	vec3d_t	vertices[1024];
	int	vrefs[2048];
	int	cmds[512];

	res_vsc_cache_t		*vsc;

	ts = BeginTokenStream( reg->path );
	if ( !ts )
		__error( "load of '%s' failed\n", reg->path );

	// get header
	GetToken( ts );
	if ( strcmp( ts->token, "3DG1" ) )
		__error( "missing vsc header\n" );

	// get vertexnum
	GetToken( ts );
	vertexnum = atoi( ts->token );
	if ( vertexnum > 1024 )
		__error( "too much vertices\n" );
	
	// get vertices
	for ( i = 0; i < vertexnum; i++ )
	{
		GetToken( ts );
		vertices[i][0] = atof( ts->token );
		GetToken( ts );
		vertices[i][1] = atof( ts->token );
		GetToken( ts );
		vertices[i][2] = atof( ts->token );
	}

	// get faces
	vrefnum = 0;
	cmdnum = 0;
	for(;;)
	{
		// get face pointnum
		if ( GetToken( ts ) == TOKEN_FAIL )
			break;
	
		num = atoi( ts->token );
	
		if ( cmdnum + 2 >= 512 )
			__error( "too much commands\n" );

		cmds[cmdnum++] = BE_CMD_POLYGON;
		cmds[cmdnum++] = num;
		
		// get vertex refs
		for ( i = 0; i < num; i++ )
		{
			if ( vrefnum >= 2048 )
				__error( "too much vertex references\n" );
			GetToken( ts );		       
			vrefs[vrefnum++] = atoi( ts->token );
		}

		// get face color
		GetToken( ts );
	}

	// terminate cmd 
	cmds[cmdnum++] = BE_CMD_END;
	cmds[cmdnum++] = 0;

	// calc total size
	bytes = vertexnum * 3*4 + vrefnum * 4 + cmdnum * 4;
	size = (int)(&((res_vsc_cache_t *)0)->data[bytes]);

	vsc = NEWBYTES( size );

	vsc->vertexnum = vertexnum;
	vsc->vrefnum = vrefnum;
	vsc->cmdnum = cmdnum;

	vsc->vertex_ofs = 0;
	vsc->vref_ofs = vertexnum * 3*4;
	vsc->cmd_ofs = vertexnum * 3*4 + vrefnum * 4;

	memcpy( &vsc->data[vsc->vertex_ofs], vertices, vertexnum * 3*4 );
	memcpy( &vsc->data[vsc->vref_ofs], vrefs, vrefnum * 4 );
	memcpy( &vsc->data[vsc->cmd_ofs], cmds, cmdnum * 4 );
	
	return vsc;
}


void Res_DumpVSC( res_vsc_cache_t *vsc )
{
	printf( "Res_DumpVSC:\n" );
	printf( " %d vertices\n", vsc->vertexnum );
	printf( " %d vertex refs\n", vsc->vrefnum );
	printf( " %d backend commands\n", vsc->cmdnum );
}

/*
  ==================================================
  internal resource interface

  ==================================================
*/

g_resource_t * Res_RegisterVSC( hobj_t *resobj )
{
	g_resource_t	*r;
	hpair_t		*name;
	hpair_t		*path;

	name = FindHPair( resobj, "name" );
	if ( !name )
		return NULL;

	path = FindHPair( resobj, "path" );
	if ( !path )
		return NULL;

	r = G_NewResource( name->value, resobj->name );

	r->res_register = Res_SetupRegisterVSC( path->value );
	r->state = G_RESOURCE_STATE_REGISTERED;

	return r;
}

void Res_UnregisterVSC( g_resource_t *res )
{
	printf( "Res_UnregisterVSC: do nothing\n" );
}

void Res_CacheVSC( g_resource_t *res )
{
	res_vsc_register_t	*res_register;

	if ( res->state != G_RESOURCE_STATE_REGISTERED )
		return;

	res_register = (res_vsc_register_t *) res->res_register;

	res->res_cache = Res_CacheInVSC( res_register );
	res->state = G_RESOURCE_STATE_CACHED;
}

void Res_UncacheVSC( g_resource_t *res )
{
	printf( "Res_UncacheVSC: do nothing\n" );
}
