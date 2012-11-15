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



// at_psys.c

#include "l_locals.h"

#include "lib_psys.h"

/*
  ==============================
  Delete

  ==============================
*/
static void Delete( at_psys_t *self )
{
	L_ChangeToDead( &self->ati );
}

/*
  ==============================
  Destroy

  ==============================
*/
static void Destroy( at_psys_t *self )
{
	li.MM_Free( self );
}

/*
  ==============================
  GetOrigin
  
  ==============================
*/
static void GetOrigin( at_psys_t *self, vec3d_t vec )
{
	Vec3dCopy( vec, self->origin );
}

/*
  ==============================
  SetOrigin
  
  ==============================
*/
static void SetOrigin( at_psys_t *self, vec3d_t vec )
{
	Vec3dCopy( self->origin, vec );
}

/*
  ==============================
  Com

  ==============================
*/
static void Com( at_psys_t *self, char *in, char *out )
{
	li.SH_Printf( "got com string\n" );	
}

/*
  ==============================
  Think1

  ==============================
*/
static void Think1( at_psys_t *self )
{
//	__named_message( "psys\n" );


	if( ((sh_var_t*)li.SH_GetVar( "fpsys_hack" )) ->ivalue )
	{	
		if ( !self->target_space && self->wf_num_till_dead == 0 )
		{
			vec3d_t		mvec = { 0, 0.0, 2.0 };
			
			Vec3dInit( mvec, -0.5+(rand()%1000)/1000.0,0.0*(rand()%1000)/1000.0, -0.5+(rand()%1000)/1000.0 );
			Vec3dUnify( mvec );
			Vec3dScale( mvec, 3.0, mvec );

			// create field space manipulation
			self->wf_num_till_dead = 30+rand()%70;	// 3 secs

			if( (rand()%1000)/1000.0 > 0.5 ) 
				li.SND_AddJob( li.GI_ResourceSearch( "sound.env.squall1" ), SND_TYPE_NORMAL, 1, self->origin );
			else
				li.SND_AddJob( li.GI_ResourceSearch( "sound.env.squall2" ), SND_TYPE_NORMAL, 1, self->origin );

			// copy orginal field for use as target during settlement
			self->target_space = PSYS_CreateFieldSpaceFromFieldSpace( self->fpsys->fieldspace );
			
			// manipulate original field
			PSYS_FieldSpaceManipulateVec( self->fpsys->fieldspace, mvec );
		}
		else if ( self->target_space )
		{
			if ( self->wf_num_till_dead == 0 )
			{
				// end of manipulation
				PSYS_DestroyFieldSpace( self->fpsys->fieldspace );
				self->fpsys->fieldspace = self->target_space;
				self->target_space = NULL;
			}
			else
			{
				// run manipulation
				PSYS_FieldSpaceSettlement( self->fpsys->fieldspace, self->target_space, 0.2 );
				
				self->wf_num_till_dead--;
			}
		}

		Vec3dInit( self->wf_fpsys.min, -32, -32, -32 );
		Vec3dInit( self->wf_fpsys.max, 32, 32, 32 );
		     
		Vec3dAdd( self->wf_fpsys.min, self->origin, self->rel_min );
		Vec3dAdd( self->wf_fpsys.max, self->origin, self->rel_max );
		
		self->wf_fpsys.fpsys = self->fpsys;
		li.GI_RegisterWFInfo( WFInfoType_fpsys, &self->wf_fpsys );
	}
}

/* 
  ==================================================
  particle system special style init

  ==================================================
*/
#define AT_PSYS_FIRE1_MSEC_LIFE		( 1500 )

void PSYS_InitStyle_fire1( at_psys_t *self )
{       
	vec3d_t		source_origin={ 1.5, 0.5, 1.5 };
	vec3d_t		scale = { 3, 4, 3 };

	vec3d_t		color_start1 = { 0, 0, 0 };
	vec3d_t		color_stop1 = { 1, 1, 1 };
	vec3d_t		color_start2 = { 1, 1, 1 };
	vec3d_t		color_stop2 = { 1, 1, 1 };
	vec3d_t		color_start3 = { 1, 1, 1 };
	vec3d_t		color_stop3 = { 0, 0, 0 };

	vec3d_t		vec_tmp;
	vec3d_t		vec_ofs = { -4.5, -4.66, -4.5 };

//	__named_message( "\n" );

	self->fpsys = NEWTYPE( field_particle_system_t );

	// 
	// setup field particle system
	//
	PSYS_InitFieldParticleSystem( self->fpsys );

	// setup generic particle system
	PSYS_InitParticleSystem( &self->fpsys->psys );
	PSYS_ParticleSystemSetMaxParticleNum( &self->fpsys->psys, 48 );
	PSYS_ParticleSystemSetSpawnRate( &self->fpsys->psys, 2 );

	//
	// setup particle face
	//
	PSYS_InitParticleFace( &self->fpsys->face );
	PSYS_ParticleFaceSetBlendFunc( &self->fpsys->face, particleFaceBlendFunc_addition );
	PSYS_ParticleFaceSetResource( &self->fpsys->face, li.GI_ResourceSearch( "gltex.fx.fire1a" ) );
	// gradient 1: 1500..1200
	PSYS_ParticleFaceAddGradient( &self->fpsys->face, 1500, 1000, color_start1, color_stop1, 0.3, 0.3 );
	// gradient 2: 1200..300
//	PSYS_ParticleFaceAddGradient( &self->fpsys->face, 1000, 700, color_start2, color_stop2, 0.3, 0.2 );
	// gradient 3: 300..0
	PSYS_ParticleFaceAddGradient( &self->fpsys->face, 1000, 0, color_start3, color_stop3, 0.3, 0.1 );

	//
	// setup field space
	//
	self->fpsys->fieldspace = PSYS_CreateFieldSpaceFromResource( li.GI_ResourceSearch( "lump.field.flame1" ) );

	//
	// setup field particle factory
	// 
	PSYS_InitFieldParticleFactory( &self->fpsys->factory );
	PSYS_FieldParticleFactorySetLife( &self->fpsys->factory, AT_PSYS_FIRE1_MSEC_LIFE );
	PSYS_FieldParticleFactorySetSpeedScale( &self->fpsys->factory, 1.0/4.0 );

	//
	// setup field particle source
	//
	PSYS_InitParticleSource( &self->fpsys->source );
	PSYS_ParticleSourceSetType( &self->fpsys->source, ParticleSource_point );
	PSYS_ParticleSourceSetOrigin( &self->fpsys->source, source_origin );

	//
	// setup field space to world transform
	//
	PSYS_FieldSpaceToWorldTransformSetScale( &self->fpsys->trans, scale );
	Vec3dAdd( vec_tmp, vec_ofs, self->origin );
	PSYS_FieldSpaceToWorldTransformSetShift( &self->fpsys->trans, vec_tmp );
}

void PSYS_InitStyle_fire2( at_psys_t *self )
{       
	vec3d_t		source_origin={ 1.5, 0.5, 1.5 };
	vec3d_t		scale = { 8, 16, 8 };

	vec3d_t		color_start1 = { 0, 0, 0 };
	vec3d_t		color_stop1 = { 1, 1, 1 };
	vec3d_t		color_start2 = { 1, 1, 1 };
	vec3d_t		color_stop2 = { 1, 1, 1 };
	vec3d_t		color_start3 = { 1, 1, 1 };
	vec3d_t		color_stop3 = { 0, 0, 0 };

	vec3d_t		vec_tmp;
	vec3d_t		vec_ofs = { -12.0, -18.66, -12.0 };

//	__named_message( "\n" );

	self->fpsys = NEWTYPE( field_particle_system_t );

	// 
	// setup field particle system
	//
	PSYS_InitFieldParticleSystem( self->fpsys );

	// setup generic particle system
	PSYS_InitParticleSystem( &self->fpsys->psys );
	PSYS_ParticleSystemSetMaxParticleNum( &self->fpsys->psys, 45 );
	PSYS_ParticleSystemSetSpawnRate( &self->fpsys->psys, 2 );

	//
	// setup particle face
	//
	PSYS_InitParticleFace( &self->fpsys->face );
	PSYS_ParticleFaceSetBlendFunc( &self->fpsys->face, particleFaceBlendFunc_addition );
	PSYS_ParticleFaceSetResource( &self->fpsys->face, li.GI_ResourceSearch( "gltex.fx.fire2" ) );
	// gradient 1: 1500..1200
	PSYS_ParticleFaceAddGradient( &self->fpsys->face, 1500, 1000, color_start1, color_stop1, 1.0, 1.0 );
	// gradient 2: 1200..300
//	PSYS_ParticleFaceAddGradient( &self->fpsys->face, 1000, 700, color_start2, color_stop2, 0.3, 0.2 );
	// gradient 3: 300..0
	PSYS_ParticleFaceAddGradient( &self->fpsys->face, 1000, 0, color_start3, color_stop3, 1.0, 0.2 );

	//
	// setup field space
	//
	self->fpsys->fieldspace = PSYS_CreateFieldSpaceFromResource( li.GI_ResourceSearch( "lump.field.flame1" ) );

	//
	// setup field particle factory
	// 
	PSYS_InitFieldParticleFactory( &self->fpsys->factory );
	PSYS_FieldParticleFactorySetLife( &self->fpsys->factory, AT_PSYS_FIRE1_MSEC_LIFE );
	PSYS_FieldParticleFactorySetSpeedScale( &self->fpsys->factory, 1.0/4.0 );

	//
	// setup field particle source
	//
	PSYS_InitParticleSource( &self->fpsys->source );
	PSYS_ParticleSourceSetType( &self->fpsys->source, ParticleSource_point );
	PSYS_ParticleSourceSetOrigin( &self->fpsys->source, source_origin );

	//
	// setup field space to world transform
	//
	PSYS_FieldSpaceToWorldTransformSetScale( &self->fpsys->trans, scale );
	Vec3dAdd( vec_tmp, vec_ofs, self->origin );
	PSYS_FieldSpaceToWorldTransformSetShift( &self->fpsys->trans, vec_tmp );
}

void PSYS_InitStyle_fire3( at_psys_t *self )
{       
	vec3d_t		source_origin={ 1.5, 0.5, 1.5 };
	vec3d_t		scale = { 64, 128, 64 };

	vec3d_t		color_start1 = { 0, 0, 0 };
	vec3d_t		color_stop1 = { 1, 1, 1 };
	vec3d_t		color_start2 = { 1, 1, 1 };
	vec3d_t		color_stop2 = { 1, 1, 1 };
	vec3d_t		color_start3 = { 1, 1, 1 };
	vec3d_t		color_stop3 = { 0, 0, 0 };

	vec3d_t		vec_tmp;
	vec3d_t		vec_ofs = { -96.0, -149.33, -96.0 };

//	__named_message( "\n" );

	self->fpsys = NEWTYPE( field_particle_system_t );

	// 
	// setup field particle system
	//
	PSYS_InitFieldParticleSystem( self->fpsys );

	// setup generic particle system
	PSYS_InitParticleSystem( &self->fpsys->psys );
	PSYS_ParticleSystemSetMaxParticleNum( &self->fpsys->psys, 90 );
	PSYS_ParticleSystemSetSpawnRate( &self->fpsys->psys, 2 );

	//
	// setup particle face
	//
	PSYS_InitParticleFace( &self->fpsys->face );
	PSYS_ParticleFaceSetBlendFunc( &self->fpsys->face, particleFaceBlendFunc_addition );
	PSYS_ParticleFaceSetResource( &self->fpsys->face, li.GI_ResourceSearch( "gltex.fx.fire2" ) );
	// gradient 1: 1500..1200
	PSYS_ParticleFaceAddGradient( &self->fpsys->face, 1500, 1000, color_start1, color_stop1, 8.0, 8.0 );
	// gradient 2: 1200..300
//	PSYS_ParticleFaceAddGradient( &self->fpsys->face, 1000, 700, color_start2, color_stop2, 0.3, 0.2 );
	// gradient 3: 300..0
	PSYS_ParticleFaceAddGradient( &self->fpsys->face, 1000, 0, color_start3, color_stop3, 8.0, 0.5 );

	//
	// setup field space
	//
	self->fpsys->fieldspace = PSYS_CreateFieldSpaceFromResource( li.GI_ResourceSearch( "lump.field.flame1" ) );

	//
	// setup field particle factory
	// 
	PSYS_InitFieldParticleFactory( &self->fpsys->factory );
	PSYS_FieldParticleFactorySetLife( &self->fpsys->factory, AT_PSYS_FIRE1_MSEC_LIFE );
	PSYS_FieldParticleFactorySetSpeedScale( &self->fpsys->factory, 1.0/4.0 );

	//
	// setup field particle source
	//
	PSYS_InitParticleSource( &self->fpsys->source );
	PSYS_ParticleSourceSetType( &self->fpsys->source, ParticleSource_point );
	PSYS_ParticleSourceSetOrigin( &self->fpsys->source, source_origin );

	//
	// setup field space to world transform
	//
	PSYS_FieldSpaceToWorldTransformSetScale( &self->fpsys->trans, scale );
	Vec3dAdd( vec_tmp, vec_ofs, self->origin );
	PSYS_FieldSpaceToWorldTransformSetShift( &self->fpsys->trans, vec_tmp );
}


/*
  ==================================================
  particle system init

  ==================================================
*/

/*
  ==============================
  InitFromClass

  ==============================
*/
static void InitFromClass( at_psys_t *self, hobj_t *cls )
{
	hobj_t		*pref;
	hpair_t		*pair;

//	__named_message( "\n" );
	
	strncpy( self->ati.type, cls->type, ATI_STRINGSIZE );
	strncpy( self->ati.name, cls->name, ATI_STRINGSIZE );
	self->ati.type[ATI_STRINGSIZE-1] = '\0';
	self->ati.name[ATI_STRINGSIZE-1] = '\0';

	pref = FindClass( at_prefs, cls->type );

	EasyFindVec3d( self->origin, cls, "origin" );

	pair = FindHPair( cls, "style" );
	if ( !pair )
		__error( "missing key 'style' in '%s'\n", cls->name );

	if ( !strcmp( pair->value, "fire1" ) )
	{
		PSYS_InitStyle_fire1( self );
	}
	else if ( !strcmp( pair->value, "fire2" ) )
	{
		PSYS_InitStyle_fire2( self );
	}
	else if ( !strcmp( pair->value, "fire3" ) )
	{
		PSYS_InitStyle_fire3( self );
	}
	else
	{
		__error( "unkown style for particle system\n" );
	}
}

/*
  ==============================
  NEW_psys

  ==============================
*/
static bool_t		ati_ftbl_psys_valid = false;
static ati_ftbl_t	ati_ftbl_psys;

at_psys_t *NEW_psys( void )
{
	at_psys_t	*obj;

	if ( ati_ftbl_psys_valid == false )
	{
		ati_ftbl_psys_valid = true;	

		memset( &ati_ftbl_psys, 0, sizeof( ati_ftbl_t ) );
		ati_ftbl_psys.GetOrigin = GetOrigin;
		ati_ftbl_psys.SetOrigin = SetOrigin;
		ati_ftbl_psys.Delete = Delete;
		ati_ftbl_psys.Destroy = Destroy;
		ati_ftbl_psys.Com = Com;
		ati_ftbl_psys.Think1 = Think1;
		ati_ftbl_psys.InitFromClass = InitFromClass;
	}

	obj = NEWTYPE( at_psys_t );

	obj->ati.state = ATIState_none;
	obj->ati.ftbl = &ati_ftbl_psys;

	return obj;
}
