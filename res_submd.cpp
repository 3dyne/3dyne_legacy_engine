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



// res_submd.c

#include "interfaces.h"
#include "g_shared.h"

#include "r_glbackenddefs.h"

#include "res_submd.h"
#include "res_submddefs.h"

res_submd_cache_t * Res_CacheInSUBMD( res_submd_register_t *reg )
{
	tokenstream_t		*ts;
	int	i, num, bytes;
    size_t size;
	int	vertexnum;
	int	vrefnum;
	int	cmdnum;

	int	tcnum;

	hpair_t	*pair;

	vec3d_t	vertices[1024];
	int	vrefs[2048];
	int	cmds[512];
	vec2d_t	tcs[2048];

	res_submd_cache_t		*submd;

	submd = NULL;

	//
	// load vsc
	// 

	pair = FindHPair( reg->resobj, "path_vsc" );
	if ( !pair )
		__error( "missing key 'path_vsc' in 'submd' resource '%s'\n", reg->resobj->name );

       
	ts = BeginTokenStream( pair->value );
	if ( !ts )
		__error( "load of '%s' failed\n", pair->value );

	// get header
	GetToken( ts );
	if ( strcmp( ts->token, "3DG1" ) )
		__error( "missing submd header\n" );

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

	EndTokenStream( ts );


	//
	// load tc
	//
	
	pair = FindHPair( reg->resobj, "path_tc" );
	if ( !pair )
		__error( "missing key 'path_tc' in 'submd' resource '%s'\n", reg->resobj->name );

	ts = BeginTokenStream( pair->value );
	if ( !ts )
		__error( "load of '%s' failed\n", pair->value );
		
	
	tcnum = 0;
	for(;;)
	{
		GetToken( ts );

		if ( !strcmp( "end", ts->token ) )
			break;

		num = atoi( ts->token );

		for ( i = 0; i < num; i++ )
		{
			GetToken( ts );
			tcs[tcnum][0] = atof( ts->token );
			GetToken( ts );
			tcs[tcnum][1] = 1.0-atof( ts->token );
			tcnum++;
		}
	}

	EndTokenStream( ts );

	// terminate cmd 
	cmds[cmdnum++] = BE_CMD_END;
	cmds[cmdnum++] = 0;

	// calc total size
	bytes = vertexnum*3*4 + vrefnum*4 + cmdnum*4 + tcnum*2*4;
	size = (size_t)(&((res_submd_cache_t *)0)->data[bytes]);

	submd = (res_submd_cache_t *)NEWBYTES( size );

	submd->vertexnum = vertexnum;
	submd->vrefnum = vrefnum;
	submd->tcnum = tcnum;
	submd->cmdnum = cmdnum;

	submd->vertex_ofs = 0;
	submd->vref_ofs = vertexnum*3*4;
	submd->tc_ofs = vertexnum*3*4 + vrefnum*4;
	submd->cmd_ofs = vertexnum*3*4 + vrefnum*4 + tcnum*2*4;
	
	memcpy( &submd->data[submd->vertex_ofs], vertices, vertexnum*3*4 );
	memcpy( &submd->data[submd->vref_ofs], vrefs, vrefnum*4 );
	memcpy( &submd->data[submd->tc_ofs], tcs, tcnum*2*4 );
	memcpy( &submd->data[submd->cmd_ofs], cmds, cmdnum*4 );
	
	return submd;
}


void Res_DumpSUBMD( res_submd_cache_t *submd )
{
	printf( "Res_DumpSUBMD:\n" );
	printf( " %d vertices\n", submd->vertexnum );
	printf( " %d vertex refs\n", submd->vrefnum );
	printf( " %d backend commands\n", submd->cmdnum );
}

/*
  ==================================================
  internal resource interface

  ==================================================
*/

g_resource_t * Res_RegisterSUBMD( hobj_t *resobj )
{
	res_submd_register_t	*reg;
	g_resource_t	*r;
	hpair_t		*name;

	name = FindHPair( resobj, "name" );
	if ( !name )
	{
		__error( "missing key 'name'\n" );
		return NULL;
	}

	r = G_NewResource( name->value, resobj->name );

	reg = NEWTYPE( res_submd_register_t );
	reg->resobj = resobj;
		      	
	r->res_register = reg;
	r->state = G_RESOURCE_STATE_REGISTERED;

	return r;
}

void Res_UnregisterSUBMD( g_resource_t *res )
{
	printf( "Res_UnregisterSUBMD: do nothing\n" );
}

void Res_CacheSUBMD( g_resource_t *res )
{
	res_submd_register_t	*res_register;

	if ( res->state != G_RESOURCE_STATE_REGISTERED )
		return;

	res_register = (res_submd_register_t *) res->res_register;

	res->res_cache = Res_CacheInSUBMD( res_register );
	res->state = G_RESOURCE_STATE_CACHED;
}

void Res_UncacheSUBMD( g_resource_t *res )
{
	printf( "Res_UncacheSUBMD: do nothing\n" );
}
