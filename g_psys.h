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



// g_psys.h

#ifndef __g_psys
#define __g_psys

#include "wf_info_defs.h"
#include "lib_psysdefs.h"

/*
  =============================================================================
  field particle system

  =============================================================================
*/

// field space
field_cell_t * G_FieldSpaceGetCell( field_space_t *space, int x, int y, int z );

// simulate
void G_FieldParticleMoveInFieldSpace( field_particle_t *particle, field_space_t *space, msec_t time_run );
void G_FieldParticleSystemRun( field_particle_system_t *fpsys, msec_t time_run );

// field particle
void G_DestroyFieldParticle( field_particle_t *particle );
field_particle_t * G_CreateFieldParticleFromFactory( field_particle_factory_t *factory );

// field particle source
void G_ParticleSourceGenOrigin( particle_source_t *source, vec3d_t origin );

#endif
