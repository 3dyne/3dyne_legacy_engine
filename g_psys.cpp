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



// g_psys.c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "lib_math.h"
#include "shock.h"
#include "ca_service.h"
#include "g_shared.h"

#include "lib_psys.h"
#include "g_psys.h"

/*
  =============================================================================
  generic particle systems

  =============================================================================
*/

/*
  ==================================================
  particle sources

  ==================================================
*/

/*
  ==============================
  G_ParticleSourceGenOrigin

  ==============================
*/
void G_ParticleSourceGenOrigin( particle_source_t *source, vec3d_t origin )
{
	Vec3dCopy( origin, source->origin );
	origin[0]+=-0.25+(rand()%10)/20.0;
//	origin[1]+=-0.25+(rand()%10)/20.0;
	origin[2]+=-0.25+(rand()%10)/20.0;
}


/*
  =============================================================================
  field particle system

  =============================================================================
*/

/*
  ==================================================
  field particles

  ==================================================
*/

/*
  ==============================
  G_CreateFieldParticleFromFactory

  ==============================
*/
field_particle_t * G_CreateFieldParticleFromFactory( field_particle_factory_t *factory )
{
	field_particle_t	*p;

	p = NEWTYPE( field_particle_t );

	p->msec_left = factory->msec_life;
	p->speed_scale = factory->speed_scale;

	return p;
}

/*
  ==============================
  G_DestroyFieldParticle

  ==============================
*/
void G_DestroyFieldParticle( field_particle_t *particle )
{
	FREE( particle );
}

/*
  ==================================================
  field space

  ==================================================
*/

/*
  ==============================
  G_FieldSpaceGetCell

  ==============================
*/
field_cell_t * G_FieldSpaceGetCell( field_space_t *space, int x, int y, int z )
{
	int		ofs;

	if ( x < 0 || y < 0 || z < 0 ||
	     x >= space->xsize || y >= space->ysize || z >= space->zsize )
		return NULL;

	ofs = x + y*space->xsize + z*space->xsize*space->ysize;
	return &space->cells[ofs];
}

/*
  ==================================================
  field particle system simulation

  ==================================================
*/

/*
  ==============================
  G_FieldParticleMoveInFieldSpace

  ==============================
*/
void G_FieldParticleMoveInFieldSpace( field_particle_t *p, field_space_t *space, msec_t time_run )
{
	vec3d_t		pos;
	field_cell_t	*cell;
	int	x, y, z;
	
	PSYS_FieldParticleGetOrigin( p, pos );
	x = (int)(rint(pos[0]));
	y = (int)(rint(pos[1]));
	z = (int)(rint(pos[2]));

	cell = G_FieldSpaceGetCell( space, x, y, z );

	if ( !cell )
	{
		// no field info for this position
		// fixme: stop particle, kill particle or keep old direction ?
//		return; // stop it
	}
	else
	{
		fp_t		t;

		t = time_run/1000.0;
		
		Vec3dScale( p->dir, p->speed_scale * t, cell->vec );
	}

	Vec3dAdd( p->pos, p->pos, p->dir );

	// don't get a negative unsigned ...
	if ( p->msec_left < time_run )
	{
		p->msec_left = 0;
	}
	else
	{
		p->msec_left -= time_run;	//
	}
}

/*
  ==============================
  G_FieldParticleSystemRun

  ==============================
*/
void G_FieldParticleSystemRun( field_particle_system_t *fpsys, msec_t time_run )
{
	field_particle_t	*p;
	field_particle_t	*next;
	field_particle_t	*head;
	
	int		active_num;
	int		spawn_num;
	int		allow_num;

	int		i;

	// clip time
	if ( time_run > 100 )
	{
		time_run = 100;
	}
	if ( time_run < 10 )
	{
		time_run = 10;
	}

//	__named_message( "psys_run for %d msec\n", time_run );

	//
	// remove dead and count active particles
	//
	active_num = 0;
	head = NULL;
	for ( p = fpsys->particle_head; p ; p = next )
	{
		next = p->next;

		if ( p->msec_left == 0 )
		{
			// particle is dead
			G_DestroyFieldParticle( p );
		}
		else
		{
			// keep particle
			p->next = head;
			head = p;
			active_num++;
		}
	}

	//
	// spawn particles
	//

	allow_num = fpsys->psys.max_particle_num - active_num;
	
	if ( allow_num >= fpsys->psys.spawn_rate )
	{
		spawn_num = fpsys->psys.spawn_rate;
	}
	else
	{
		spawn_num = allow_num;
	}

//	printf( "spawn_num %d, %f time, ", spawn_num, (time_run/1000.0) );
	{
		fp_t	frac;
		frac = spawn_num;
		frac *= (time_run/1000.0);	

		spawn_num = ceil( frac );
	}
//	printf( " = %d spawn\n", spawn_num );

//	printf( "allow_num %d, active_num %d, spawn_num %d\n", allow_num, active_num, spawn_num );

	for ( i = 0; i < spawn_num; i++ )
	{		
		vec3d_t		origin;

		p = G_CreateFieldParticleFromFactory( &fpsys->factory );
		G_ParticleSourceGenOrigin( &fpsys->source, origin );
		PSYS_FieldParticleSetOrigin( p, origin );
		
		p->next = head;
		head = p;
	}

	fpsys->particle_head = head;

	//
	// run each particle
	//

	for ( p = fpsys->particle_head; p ; p = p->next )
	{
		G_FieldParticleMoveInFieldSpace( p, fpsys->fieldspace, time_run );
	}	
}
