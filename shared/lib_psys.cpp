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



// lib_psys.c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "s_config.h"
#include "s_mem.h"

#include "shock.h"

#include "lib_math.h"
#include "lib_pack.h"
#include "lib_psys.h"

#include "res_lumpdefs.h"

/*
  =============================================================================
  generic particle system stuff

  =============================================================================
*/

/*
  ==================================================
  generic particle system

  ==================================================
*/

/*
  ==============================
  PSYS_InitParticleSystem

  ==============================
*/
void PSYS_InitParticleSystem( particle_system_t *psys )
{

}

/*
  ==============================
  PSYS_ParticleSystemSetMaxParticleNum

  ==============================
*/
void PSYS_ParticleSystemSetMaxParticleNum( particle_system_t *psys, int max_particle_num )
{
	psys->max_particle_num = max_particle_num;
}

/*
  ==============================
  PSYS_ParticleSystemSetSpawnRate
  
  ==============================
*/
void PSYS_ParticleSystemSetSpawnRate( particle_system_t *psys, int spawn_rate )
{
	psys->spawn_rate = spawn_rate;
}

/*
  ==================================================
  particle face

  ==================================================
*/

/*
  ==============================
  PSYS_InitParticleFace

  ==============================
*/
void PSYS_InitParticleFace( particle_face_t *face )
{
	face->gradient_num = 0;
	face->gltex_res = NULL;
}

/*
  ==============================
  PSYS_ParticleFaceSetBlendFunc

  ==============================
*/
void PSYS_ParticleFaceSetBlendFunc( particle_face_t *face, particleFaceBlendFunc blend_func )
{
	face->blend_func = blend_func;
}

/*
  ==============================
  PSYS_ParticleFaceSetResource

  ==============================
*/
void PSYS_ParticleFaceSetResource( particle_face_t *face, g_resource_t *gltex_res )
{
	face->gltex_res = gltex_res;
}

/*
  ==============================
  PSYS_ParticleFaceAddGradient

  ==============================
*/
void		PSYS_ParticleFaceAddGradient( particle_face_t *face,
					      msec_t msec_start, msec_t msec_stop,
					      vec3d_t color_start, vec3d_t color_stop,
					      fp_t size_start, fp_t size_stop )
{
	if ( face->gradient_num >= PARTICLE_FACE_MAX_GRADIENTS )
	{
		__error( "reached PARTICLE_FACE_MAX_GRADIENTS\n " );
	}

	face->msec_start[face->gradient_num] = msec_start;
	face->msec_stop[face->gradient_num] = msec_stop;
	face->size_start[face->gradient_num] = size_start;
	face->size_stop[face->gradient_num] = size_stop;
	Vec3dCopy( face->color_start[face->gradient_num], color_start );
	Vec3dCopy( face->color_stop[face->gradient_num], color_stop );
	
	face->gradient_num++;
}


/*
  ==================================================
  particle source

  ==================================================
*/

/*
  ==============================
  PSYS_InitParticleSource

  ==============================
*/
void PSYS_InitParticleSource( particle_source_t *source )
{
	
}

/*
  ==============================
  PSYS_ParticleSourceSetType

  ==============================
*/
void PSYS_ParticleSourceSetType( particle_source_t *source, particleSource type )
{
	source->type = type;
}

/*
  ==============================
  PSYS_ParticleSourceSetOrigin

  ==============================
*/
void PSYS_ParticleSourceSetOrigin( particle_source_t *source, vec3d_t origin )
{
	Vec3dCopy( source->origin, origin );
}


/*
  =============================================================================
  field particle system

  =============================================================================
*/

/*
  ==============================
  PSYS_InitFieldParticleSystem

  ==============================
*/
void PSYS_InitFieldParticleSystem( field_particle_system_t *psys )
{

}


/*
  ==============================
  PSYS_CreateFieldSpaceFromResource

  ==============================
*/

/*
  ------------------------------
  binary field lump layout

  int	cell_num
  int	xmin, ymin, zmin
  int	xmax, ymax, zmax
  
  for ( i = 0; i < cell_num; i++ )
  {
	short	xpos, ypos, zpos
	vec3d_t	vec
	
  } 
  ------------------------------
*/

field_space_t * PSYS_CreateFieldSpaceFromResource( g_resource_t *res )
{
	field_space_t		*space;
	res_lump_cache_t	*lump;
	
	int		i;
	int		cell_num;
	int		min[3], max[3];
	int		total_cell_num;
	size_t		size;
	int		xsize, ysize, zsize;
	int		ofs;
	short		pos[3];

	lump = (res_lump_cache_t *) res->res_cache;
	__chkptr( lump );

	U_BeginUnpack( U_PACKED_BIN, lump->data, lump->size );
	U_Unpacks32( &cell_num );
	U_Unpacks32( &min[0] );
	U_Unpacks32( &min[1] );
	U_Unpacks32( &min[2] );
	U_Unpacks32( &max[0] );
	U_Unpacks32( &max[1] );
	U_Unpacks32( &max[2] );

	xsize = 1 + max[0]-min[0];
	ysize = 1 + max[1]-min[1];
	zsize = 1 + max[2]-min[2];

	total_cell_num = xsize*ysize*zsize;
	size = (size_t)&(((field_space_t *)0)->cells[total_cell_num]);

	space = (field_space_t *)NEWBYTES( size );

	space->xsize = xsize;
	space->ysize = ysize;
	space->zsize = zsize;

	for ( i = 0; i < cell_num; i++ )
	{
		U_Unpacks16( &pos[0] );
		U_Unpacks16( &pos[1] );
		U_Unpacks16( &pos[2] );

		ofs = pos[0] + pos[1]*xsize + pos[2]*xsize*ysize;
		
		U_Unpackfp32( &space->cells[ofs].vec[0] );
		U_Unpackfp32( &space->cells[ofs].vec[1] );
		U_Unpackfp32( &space->cells[ofs].vec[2] );
	}

	U_EndUnpack();
	
	return space;       
}

/*
  ==============================
  PSYS_CreateFieldSpaceFromFieldSpace

  ==============================
*/
field_space_t * PSYS_CreateFieldSpaceFromFieldSpace( field_space_t *in )
{
	field_space_t		*space;	
	size_t		size;
    int total_cell_num;

	total_cell_num = in->xsize*in->ysize*in->zsize;
	size = (size_t)&(((field_space_t *)0)->cells[total_cell_num]);
	space = (field_space_t *)NEWBYTES( size );
	memcpy( space, in, size );

	return space;
}

/*
  ==============================
  PSYS_DestroyFieldSpace

  ==============================
*/
void PSYS_DestroyFieldSpace( field_space_t *space )
{
	FREE( space );
}

/*
  ==============================
  PSYS_FieldSpaceManipulateVec

  ==============================
*/
void PSYS_FieldSpaceManipulateVec( field_space_t *space, vec3d_t vec )
{
	int		i;
	int		cell_num;

	cell_num = space->xsize*space->ysize*space->zsize;

	for ( i = 0; i < cell_num; i++ )
	{
		Vec3dAdd( space->cells[i].vec, space->cells[i].vec, vec );
	}	
}

/*
  ==============================
  PSYS_FieldSpaceSettlement

  ==============================
*/ 
void PSYS_FieldSpaceSettlement( field_space_t *is, field_space_t *target, fp_t p_const )
{
	int		i;
	int		cell_num;

	cell_num = is->xsize*is->ysize*is->zsize;

	for ( i = 0; i < cell_num; i++ )
	{
		vec3d_t		delta;

		Vec3dSub( delta, target->cells[i].vec, is->cells[i].vec );
		Vec3dScale( delta, p_const, delta );
		Vec3dAdd( is->cells[i].vec, delta, is->cells[i].vec );
	}
}

/*
  ==================================================
  field particle factory

  ==================================================
*/

/*
  ==============================
  PSYS_InitFieldParticleFactory

  ==============================
*/
void PSYS_InitFieldParticleFactory( field_particle_factory_t *factory )
{
	
}

/*
  ==============================
  PSYS_FieldParticleFactorySetParticleLife

  ==============================
*/
void PSYS_FieldParticleFactorySetLife( field_particle_factory_t *factory, msec_t msec_life )
{
	factory->msec_life = msec_life;
}

/*
  ==============================
  PSYS_FieldParticleFactorySetSpeedScale

  ==============================
*/
void PSYS_FieldParticleFactorySetSpeedScale( field_particle_factory_t *factory, fp_t speed_scale )
{
	factory->speed_scale = speed_scale;
}


/*
  ==================================================
  field particle

  ==================================================
*/

/*
  ==============================
  PSYS_FieldParticleSetOrigin

  ==============================
*/
void PSYS_FieldParticleSetOrigin( field_particle_t *particle, vec3d_t origin )
{
	Vec3dCopy( particle->pos, origin );
}

/*
  ==============================
  PSYS_FieldParticleGetOrigin

  ==============================
*/
void PSYS_FieldParticleGetOrigin( field_particle_t *particle, vec3d_t origin )
{
	Vec3dCopy( origin, particle->pos );
}


/*
  ==================================================
  field space to world transform

  ==================================================
*/

/*
  ==============================
  PSYS_InitFieldSpaceToWorldTransform

  ==============================
*/
void PSYS_InitFieldSpaceToWorldTransform( field_space_to_world_transform_t *trans )
{

}

/*
  ==============================
  PSYS_FieldSpaceToWorldTransformSetScale

  ==============================
*/
void PSYS_FieldSpaceToWorldTransformSetScale( field_space_to_world_transform_t *trans, vec3d_t scale )
{
	Vec3dCopy( trans->scale, scale );
}

/*
  ==============================
  PSYS_FieldSpaceToWorldTransformSetShift

  ==============================
*/
void PSYS_FieldSpaceToWorldTransformSetShift( field_space_to_world_transform_t *trans, vec3d_t shift )
{
	Vec3dCopy( trans->shift, shift );
}
