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



// cl_atpsys.c

#include "cl_state.h"
#include "cl_head.h"

#include "s_mem.h"
//#include "lib_hobj.h"
#include "lib_psys.h"
#include "shock.h"

#include "events.h"

typedef struct cl_at_psys_s
{
	cl_ati_t	atif;

	// 
	// changed by update event
	//
	
	bool_t		u_pos_mask;
	vec3d_t		u_pos;
	
	bool_t		u_style_mask;
	psysStyle	u_style;

	// 
	// current
	//

	psysStyle	style;
	vec3d_t		origin;

	vec3d_t		rel_min;
	vec3d_t		rel_max;

	// manipulation stuff
	bool_t		in_manipulation;     
	field_space_t	*initial_fieldspace;	// the initial field 
	field_space_t	*manipulation_fieldspace;	// the initial field 
	
	msec_t		time_last_manip;
	int		count;

	msec_t		time_last_shock;
	vec3d_t		shock_dir;
	fp_t		settlement;

	// render info
	ri_psys_t	ri_psys;

} cl_at_psys_t;

/*
  ==============================
  UnpackEventPsys

  ==============================
*/
static void UnpackEventPsys( cl_ati_t *atif, unique_t id, eventType type, byte_iter_t *bi )
{
	cl_at_psys_t	*psys;

	psys = ADDR_OF_BASE( cl_at_psys_t, atif, atif );

	if ( type == eventType_client_obj_update_pos )
	{
		ByteIter_Unpack3fv( bi, psys->u_pos );
		psys->u_pos_mask = true;
	}
	else if ( type == eventType_client_obj_update_style )
	{
		psys->u_style = (psysStyle)ByteIter_Unpacki( bi );
		psys->u_style_mask = true;
	}
	else
	{
		__warning( "unknown event for client object '#%u'\n", id );
	}
}


/*
  ==============================
  RunPsys

  ==============================
*/
static void PsysStopStyle( cl_at_psys_t *psys )
{
	if ( psys->style != psysStyle_none )
	{
		__warning( "PSYS_CleanUpFieldParticleSystem not impl\n" );
//		PSYS_CleanUpFieldParticleSystem( &psys->ri_psys.fpsys );
		psys->style = psysStyle_none;
	}
}

#define AT_PSYS_FIRE1_MSEC_LIFE		( 1000 )

static void PsysStartStyle_fire1( cl_at_psys_t *psys )
{

	vec3d_t		source_origin={ 1.5, 0.5, 1.5 };
	vec3d_t		scale = { 8, 16, 8 };

	vec3d_t		color_start1 = { 0, 0, 0 };
	vec3d_t		color_stop1 = { 1, 1, 1 };
	vec3d_t		color_start3 = { 1, 1, 1 };
	vec3d_t		color_stop3 = { 0, 0, 0 };

	vec3d_t		vec_tmp;
	vec3d_t		vec_ofs = { -12.0, -18.66, -12.0 };


//	__named_message( "\n" );

//	self->fpsys = NEWTYPE( field_particle_system_t );

	// 
	// setup field particle system
	//
	PSYS_InitFieldParticleSystem( &psys->ri_psys.fpsys );

	// setup generic particle system
	PSYS_InitParticleSystem( &psys->ri_psys.fpsys.psys );
	PSYS_ParticleSystemSetMaxParticleNum( &psys->ri_psys.fpsys.psys, 100 );
	PSYS_ParticleSystemSetSpawnRate( &psys->ri_psys.fpsys.psys, 50 );

	//
	// setup particle face
	//
	PSYS_InitParticleFace( &psys->ri_psys.fpsys.face );
	PSYS_ParticleFaceSetBlendFunc( &psys->ri_psys.fpsys.face, particleFaceBlendFunc_addition );
	PSYS_ParticleFaceSetResource( &psys->ri_psys.fpsys.face, g_api.GI_ResourceSearch( "gltex.fx.fire2" ) );
	// gradient 1: 1500..1200
	PSYS_ParticleFaceAddGradient( &psys->ri_psys.fpsys.face, 1000, 700, color_start1, color_stop1, 1.0, 1.0 );
	// gradient 2: 1200..300
//	PSYS_ParticleFaceAddGradient( &self->fpsys->face, 1000, 700, color_start2, color_stop2, 0.3, 0.2 );
	// gradient 3: 300..0
	PSYS_ParticleFaceAddGradient( &psys->ri_psys.fpsys.face, 700, 0, color_start3, color_stop3, 1.0, 0.3 );

	//
	// setup field space
	//
	psys->ri_psys.fpsys.fieldspace = PSYS_CreateFieldSpaceFromResource( g_api.GI_ResourceSearch( "lump.field.flame1" ) );
	// keep the original field
	psys->initial_fieldspace = PSYS_CreateFieldSpaceFromFieldSpace( psys->ri_psys.fpsys.fieldspace ); 
//	psys->manipulation_fieldspace = PSYS_CreateFieldSpaceFromFieldSpace( psys->ri_psys.fpsys.fieldspace ); 	
	//
	// setup field particle factory
	// 
	PSYS_InitFieldParticleFactory( &psys->ri_psys.fpsys.factory );
	PSYS_FieldParticleFactorySetLife( &psys->ri_psys.fpsys.factory, AT_PSYS_FIRE1_MSEC_LIFE );
	PSYS_FieldParticleFactorySetSpeedScale( &psys->ri_psys.fpsys.factory, 3.5 );

	//
	// setup field particle source
	// 
	PSYS_InitParticleSource( &psys->ri_psys.fpsys.source );
	PSYS_ParticleSourceSetType( &psys->ri_psys.fpsys.source, ParticleSource_point );
	PSYS_ParticleSourceSetOrigin( &psys->ri_psys.fpsys.source, source_origin );

	//
	// setup field space to world transform
	//
	PSYS_FieldSpaceToWorldTransformSetScale( &psys->ri_psys.fpsys.trans, scale );
	Vec3dAdd( vec_tmp, vec_ofs, psys->origin );
	PSYS_FieldSpaceToWorldTransformSetShift( &psys->ri_psys.fpsys.trans, vec_tmp );	
}


static void PsysStartStyle_fire2( cl_at_psys_t *psys )
{

	vec3d_t		source_origin={ 1.5, 0.5, 1.5 };
	vec3d_t		scale = { 8, 16, 8 };

	vec3d_t		color_start1 = { 0, 0, 0 };
	vec3d_t		color_stop1 = { 0.5, 0.5, 0.5 };
	vec3d_t		color_start3 = { 0.5, 0.5, 0.5 };
	vec3d_t		color_stop3 = { 0, 0, 0 };

	vec3d_t		vec_tmp;
	vec3d_t		vec_ofs = { -12.0, -18.66, -12.0 };


//	__named_message( "\n" );

//	self->fpsys = NEWTYPE( field_particle_system_t );

	// 
	// setup field particle system
	//
	PSYS_InitFieldParticleSystem( &psys->ri_psys.fpsys );

	// setup generic particle system
	PSYS_InitParticleSystem( &psys->ri_psys.fpsys.psys );
	PSYS_ParticleSystemSetMaxParticleNum( &psys->ri_psys.fpsys.psys, 45 );
	PSYS_ParticleSystemSetSpawnRate( &psys->ri_psys.fpsys.psys, 50 );

	//
	// setup particle face
	//
	PSYS_InitParticleFace( &psys->ri_psys.fpsys.face );
	PSYS_ParticleFaceSetBlendFunc( &psys->ri_psys.fpsys.face, particleFaceBlendFunc_addition );
	PSYS_ParticleFaceSetResource( &psys->ri_psys.fpsys.face, g_api.GI_ResourceSearch( "gltex.fx.fire2" ) );
	// gradient 1: 1500..1200
	PSYS_ParticleFaceAddGradient( &psys->ri_psys.fpsys.face, 1000, 700, color_start1, color_stop1, 1.0, 1.0 );
	// gradient 2: 1200..300
//	PSYS_ParticleFaceAddGradient( &self->fpsys->face, 1000, 700, color_start2, color_stop2, 0.3, 0.2 );
	// gradient 3: 300..0
	PSYS_ParticleFaceAddGradient( &psys->ri_psys.fpsys.face, 700, 0, color_start3, color_stop3, 1.0, 0.3 );

	//
	// setup field space
	//
	psys->ri_psys.fpsys.fieldspace = PSYS_CreateFieldSpaceFromResource( g_api.GI_ResourceSearch( "lump.field.flame1" ) );
	// keep the original field
	psys->initial_fieldspace = PSYS_CreateFieldSpaceFromFieldSpace( psys->ri_psys.fpsys.fieldspace ); 
//	psys->manipulation_fieldspace = PSYS_CreateFieldSpaceFromFieldSpace( psys->ri_psys.fpsys.fieldspace ); 	
	//
	// setup field particle factory
	// 
	PSYS_InitFieldParticleFactory( &psys->ri_psys.fpsys.factory );
	PSYS_FieldParticleFactorySetLife( &psys->ri_psys.fpsys.factory, AT_PSYS_FIRE1_MSEC_LIFE );
	PSYS_FieldParticleFactorySetSpeedScale( &psys->ri_psys.fpsys.factory, 3.5 );

	//
	// setup field particle source
	// 
	PSYS_InitParticleSource( &psys->ri_psys.fpsys.source );
	PSYS_ParticleSourceSetType( &psys->ri_psys.fpsys.source, ParticleSource_point );
	PSYS_ParticleSourceSetOrigin( &psys->ri_psys.fpsys.source, source_origin );

	//
	// setup field space to world transform
	//
	PSYS_FieldSpaceToWorldTransformSetScale( &psys->ri_psys.fpsys.trans, scale );
	Vec3dAdd( vec_tmp, vec_ofs, psys->origin );
	PSYS_FieldSpaceToWorldTransformSetShift( &psys->ri_psys.fpsys.trans, vec_tmp );	
}


static void PsysStartStyle( cl_at_psys_t *psys )
{
	if ( psys->style == psysStyle_fire1 )
	{
		PsysStartStyle_fire1( psys );
	}
	else if ( psys->style == psysStyle_fire2 )
	{
		PsysStartStyle_fire2( psys );
	}
	else
	{
		__error( "unkown psys style\n" );
	}	
}

static void PsysUpdatePos( cl_at_psys_t *psys )
{
	if ( psys->style == psysStyle_fire1 )
	{
		vec3d_t		vec_tmp;
		vec3d_t		vec_ofs = { -12.0, -18.66, -12.0 };
		vec3d_t		scale = { 8, 16, 8 };
		//
		// setup field space to world transform
		//
		
		PSYS_FieldSpaceToWorldTransformSetScale( &psys->ri_psys.fpsys.trans, scale );
		Vec3dAdd( vec_tmp, vec_ofs, psys->origin );
		PSYS_FieldSpaceToWorldTransformSetShift( &psys->ri_psys.fpsys.trans, vec_tmp );		
	}
	else if ( psys->style == psysStyle_fire2 )
	{
		vec3d_t		vec_tmp;
		vec3d_t		vec_ofs = { -12.0, -18.66, -12.0 };
		vec3d_t		scale = { 8, 16, 8 };
		//
		// setup field space to world transform
		//
		
		PSYS_FieldSpaceToWorldTransformSetScale( &psys->ri_psys.fpsys.trans, scale );
		Vec3dAdd( vec_tmp, vec_ofs, psys->origin );
		PSYS_FieldSpaceToWorldTransformSetShift( &psys->ri_psys.fpsys.trans, vec_tmp );		
	}
}

void PsysRunManipulation_fire2( cl_at_psys_t *psys )
{
	vec3d_t		mvec;
	
	if ( (rand()%1000) < 50 )
	{
		psys->in_manipulation ^= 1;

		if ( psys->in_manipulation )
		{
			Vec3dInit( mvec, -0.5+(rand()%1000)/1000.0,0.0*(rand()%1000)/1000.0, -0.5+(rand()%1000)/1000.0 );
			Vec3dUnify( mvec );
			
			if ( (rand()%1000) < 200 && 0 )
			{
				Vec3dScale( mvec, 3.0, mvec );
			}
			else
			{
				Vec3dScale( mvec, 1.0, mvec );
			}
			
			if ( psys->manipulation_fieldspace )
			{
				PSYS_DestroyFieldSpace( psys->manipulation_fieldspace );
			}
			psys->manipulation_fieldspace = PSYS_CreateFieldSpaceFromFieldSpace( psys->initial_fieldspace );
			PSYS_FieldSpaceManipulateVec( psys->manipulation_fieldspace, mvec );	
		}
	}


	if ( psys->in_manipulation )
	{
		if ( psys->manipulation_fieldspace )
		{
			PSYS_FieldSpaceSettlement( psys->ri_psys.fpsys.fieldspace, psys->manipulation_fieldspace, 0.1 ); 	
		}
	}
	else
	{
		if ( psys->initial_fieldspace )
		{
			PSYS_FieldSpaceSettlement( psys->ri_psys.fpsys.fieldspace, psys->initial_fieldspace, 0.1 ); 
		}
	}
}

static void PsysRunManipulation_fire1( cl_at_psys_t *psys )
{
	vec3d_t		mvec;
	
	if ( cl_time - psys->time_last_manip > 500 )
	{

//		psys->in_manipulation ^= 1;
		
		if ( psys->count == 0 )
		{	
			Vec3dInit( mvec, 0, -1, 0 );
			
			if ( psys->manipulation_fieldspace )
			{
				PSYS_DestroyFieldSpace( psys->manipulation_fieldspace );
			}
			psys->manipulation_fieldspace = PSYS_CreateFieldSpaceFromFieldSpace( psys->initial_fieldspace );
			PSYS_FieldSpaceManipulateVec( psys->manipulation_fieldspace, mvec );	
		}
		if ( psys->count == 1 )
		{	
			Vec3dInit( mvec, 0, 1, 0 );
			
			if ( psys->manipulation_fieldspace )
			{
				PSYS_DestroyFieldSpace( psys->manipulation_fieldspace );
			}
			psys->manipulation_fieldspace = PSYS_CreateFieldSpaceFromFieldSpace( psys->initial_fieldspace );
			PSYS_FieldSpaceManipulateVec( psys->manipulation_fieldspace, mvec );	
		}		
		if ( psys->count == 2 )
		{

		}

		psys->time_last_manip = cl_time;
		psys->count++;
		if ( psys->count > 5 )
			psys->count = 0;
	}


	if ( psys->count == 0 || psys->count == 1 )
	{
		if ( psys->manipulation_fieldspace )
		{
			PSYS_FieldSpaceSettlement( psys->ri_psys.fpsys.fieldspace, psys->manipulation_fieldspace, 1.0 ); 	
		}
	}

	if ( psys->count >= 2 )
	{
		if ( psys->initial_fieldspace )
		{
			PSYS_FieldSpaceSettlement( psys->ri_psys.fpsys.fieldspace, psys->initial_fieldspace, 1.0 ); 
		}
	}
}

static void RunPsys( cl_ati_t *atif )
{
	cl_at_psys_t	*psys;

	psys = ADDR_OF_BASE( cl_at_psys_t, atif, atif );

	if ( psys->u_style_mask )
	{
		if ( psys->u_style != psys->style )
		{
			//
			// change psys style
			//
			
			// drop old style
			PsysStopStyle( psys );
			
			psys->style = psys->u_style;
			
			PsysStartStyle( psys );
		}
		
		psys->u_style_mask = false;
	}

	if ( psys->u_pos_mask )
	{
		Vec3dCopy( psys->origin, psys->u_pos );

		PsysUpdatePos( psys );

		psys->u_pos_mask = false;
	}

	if ( psys->style == psysStyle_fire2 )
	{
		vec3d_t		kill_center;

		if ( CL_ProxyKillBoxMapTestPoint( psys->origin, kill_center ) )
		{

			Vec3dSub( psys->shock_dir, psys->origin, kill_center );
			Vec3dUnify( psys->shock_dir );
			Vec3dScale( psys->shock_dir, 3.0, psys->shock_dir );
			psys->time_last_shock = cl_time;
			psys->count = 0;
		}

		if ( cl_time - psys->time_last_shock >= 0 )
		{
			if ( psys->count == 0 )
			{
				if ( psys->manipulation_fieldspace )
				{
					PSYS_DestroyFieldSpace( psys->manipulation_fieldspace );
				}	
				psys->manipulation_fieldspace = PSYS_CreateFieldSpaceFromFieldSpace( psys->initial_fieldspace );
				PSYS_FieldSpaceManipulateVec( psys->manipulation_fieldspace, psys->shock_dir );	
				psys->in_manipulation = true;
				psys->count++;
			}
			psys->settlement = 0.9;
		}
		if ( cl_time - psys->time_last_shock > 70 )
		{
			if ( psys->count == 1 )
			{
//				Vec3dScale( psys->shock_dir, -2.0, psys->shock_dir );
				
				if ( psys->manipulation_fieldspace )
				{
//					PSYS_DestroyFieldSpace( psys->manipulation_fieldspace );
				}	
//				psys->manipulation_fieldspace = PSYS_CreateFieldSpaceFromFieldSpace( psys->initial_fieldspace );
//				PSYS_FieldSpaceManipulateVec( psys->manipulation_fieldspace, psys->shock_dir );	
				psys->in_manipulation = true;
				psys->count++;			
			}
			psys->settlement = 0.9;
		}
		if (  cl_time - psys->time_last_shock > 100 )
		{
			if ( psys->count == 2 )
			{			
//				psys->in_manipulation = false;
			}
		}
		if ( cl_time - psys->time_last_shock > 500 )
		{
			if ( cl_time&511 )
			{
				vec3d_t		mvec;
				Vec3dInit( mvec, -0.5+(rand()%1000)/1000.0,0.0*(rand()%1000)/1000.0, -0.5+(rand()%1000)/1000.0 );
				Vec3dUnify( mvec );	

				if ( psys->manipulation_fieldspace )
				{
					PSYS_DestroyFieldSpace( psys->manipulation_fieldspace );
				}	
				psys->manipulation_fieldspace = PSYS_CreateFieldSpaceFromFieldSpace( psys->initial_fieldspace );
				PSYS_FieldSpaceManipulateVec( psys->manipulation_fieldspace, mvec );	
				psys->in_manipulation = true;
				psys->settlement = 0.1;
			}
		}


		if ( psys->in_manipulation )
		{
			PSYS_FieldSpaceSettlement( psys->ri_psys.fpsys.fieldspace, psys->manipulation_fieldspace, psys->settlement );	
//			psys->in_manipulation = false;
		}
		else
		{
			PSYS_FieldSpaceSettlement( psys->ri_psys.fpsys.fieldspace, psys->initial_fieldspace, 0.7 ); 	
		}
//		PsysRunManipulation_fire2( psys );

		Vec3dAdd( psys->ri_psys.min, psys->origin, psys->rel_min );
		Vec3dAdd( psys->ri_psys.max, psys->origin, psys->rel_max );
		g_api.Render_Psys( &psys->ri_psys );
	}
	else if ( psys->style == psysStyle_fire1 )
	{
		PsysRunManipulation_fire1( psys );

		Vec3dAdd( psys->ri_psys.min, psys->origin, psys->rel_min );
		Vec3dAdd( psys->ri_psys.max, psys->origin, psys->rel_max );
		g_api.Render_Psys( &psys->ri_psys );		
	}
}
/*
  ==============================
  DestroyPsys

  ==============================
*/
static void DestroyPsys( cl_ati_t *atif )
{
	cl_at_psys_t	*psys;
	
	psys = ADDR_OF_BASE( cl_at_psys_t, atif, atif );
	FREE( psys );
}

/*
  ==================================================
  create client psys

  ==================================================
*/
static bool_t		ftbl_valid = false;
static cl_ati_ftbl_t	ftbl;

cl_ati_t * CL_CreatePsys( unique_t id, byte_iter_t *bi )
{
	cl_at_psys_t	*psys;
	
	if ( ftbl_valid == false )
	{
		ftbl_valid = true;
		INITTYPE( &ftbl, cl_ati_ftbl_t );

		ftbl.Destroy = DestroyPsys;
		ftbl.UnpackEvent = UnpackEventPsys;
		ftbl.Run = RunPsys;
		
	}

	psys = NEWTYPE( cl_at_psys_t );
	psys->atif.ftbl = &ftbl;
	psys->atif.id = id;
	
//	psys->atif.frame_count = 0;
	psys->style = psysStyle_none;

	return &psys->atif;
}
