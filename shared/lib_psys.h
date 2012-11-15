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



// lib_psys.h

#ifndef __lib_psys
#define __lib_psys

#include "lib_psysdefs.h"

/*
  =============================================================================
  generic particle system stuff

  =============================================================================
*/

void		PSYS_InitParticleSystem( particle_system_t *psys );
void		PSYS_ParticleSystemSetMaxParticleNum( particle_system_t *psys, int max_particle_num );
void		PSYS_ParticleSystemSetSpawnRate( particle_system_t *psys, int spawn_rate );

void		PSYS_InitParticleFace( particle_face_t *face );
void		PSYS_ParticleFaceSetBlendFunc( particle_face_t *face, particleFaceBlendFunc blend_func );
void		PSYS_ParticleFaceSetResource( particle_face_t *face, g_resource_t *gltex_res );
void		PSYS_ParticleFaceAddGradient( particle_face_t *face,
					      msec_t msec_start, msec_t msec_stop,
					      vec3d_t color_start, vec3d_t color_stop,
					      fp_t size_start, fp_t size_stop );

// source
void		PSYS_InitParticleSource( particle_source_t *source );	// init object internals
//void		PSYS_CleanUpFieldParticleSource( particle_source_t *source );	// cleanup object internals
void		PSYS_ParticleSourceSetType( particle_source_t *source, particleSource type );
void		PSYS_ParticleSourceSetOrigin( particle_source_t *source, vec3d_t origin );

/*
  =============================================================================
  field particle system

  =============================================================================
*/

// system
void		PSYS_InitFieldParticleSystem( field_particle_system_t *fpsys );		// init object internals
//void		PSYS_CleanUpFieldParticleSystem( field_particle_system_t *fpsys );	// cleanup object internals

// field space
field_space_t *	PSYS_CreateFieldSpaceFromResource( g_resource_t *res );			// create object and init internals
field_space_t * PSYS_CreateFieldSpaceFromFieldSpace( field_space_t *space );		// dto
void		PSYS_DestroyFieldSpace( field_space_t *space );				// cleanup internals and destroy object
void		PSYS_FieldSpaceManipulateVec( field_space_t *space, vec3d_t vec );
void		PSYS_FieldSpaceSettlement( field_space_t *is, field_space_t *target, fp_t p_const );

// factory
void		PSYS_InitFieldParticleFactory( field_particle_factory_t *factory );	// init object internals
//void		PSYS_CleanUpFieldParticleFactory( field_particle_factory_t *factory );	// cleanup object internals
void		PSYS_FieldParticleFactorySetLife( field_particle_factory_t *factory, msec_t msec_life );
void		PSYS_FieldParticleFactorySetSpeedScale( field_particle_factory_t *factory, fp_t speed_scale );

// field particle
void PSYS_FieldParticleSetOrigin( field_particle_t *particle, vec3d_t origin );
void PSYS_FieldParticleGetOrigin( field_particle_t *particle, vec3d_t origin );

// field space to world transform
void		PSYS_InitFieldSpaceToWorldTransform( field_space_to_world_transform_t *trans );	// init object internals
void		PSYS_FieldSpaceToWorldTransformSetScale( field_space_to_world_transform_t *trans, vec3d_t scale );
void		PSYS_FieldSpaceToWorldTransformSetShift( field_space_to_world_transform_t *trans, vec3d_t shift );

#endif
