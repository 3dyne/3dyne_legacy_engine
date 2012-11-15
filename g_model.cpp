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



// g_model.c

#include "interfaces.h"
#include "r_private.h"

/*
  ==============================
  G_SetupModel

  ==============================
*/
void G_SetupModelRecursive( md_coordsys_t *cs, vec3d_t ref_origin, vec3d_t ref_axis[3] )
{
	fp_t		mv_lat;
	fp_t		mv_lon;
	fp_t		mv_yaw;
	vec3d_t		mv_origin;
	u_list_iter_t	iter;
	md_coordsys_t	*child_cs;
	vec3d_t		tmp_axis[3];

	unsigned int	step;

	if ( cs->current_move )
	{
		step = MD_CalcStep( cs->current_move, ms_rfbegin );

		mv_origin[0] = *MD_MoveAccess( cs->current_move, step, 0 );
		mv_origin[1] = *MD_MoveAccess( cs->current_move, step, 1 );
		mv_origin[2] = *MD_MoveAccess( cs->current_move, step, 2 );
		mv_lat = *MD_MoveAccess( cs->current_move, step, 3 );
		mv_lon = *MD_MoveAccess( cs->current_move, step, 4 );
		mv_yaw = *MD_MoveAccess( cs->current_move, step, 5 );
	}
	else
	{
		mv_origin[0] = 0.0;
		mv_origin[1] = 0.0;
		mv_origin[2] = 0.0;
		mv_lat = 0.0;
		mv_lon = 0.0;
		mv_yaw = 0.0;
	}

	Vec3dScale( mv_origin, 64.0, mv_origin );

	MD_CalcAxisPoint( cs->origin, mv_origin, ref_axis );
	Vec3dAdd( cs->origin, cs->origin, ref_origin );
	
	Matrix3SetupRotate( tmp_axis, mv_lat*D2R, mv_yaw*D2R, mv_lon*D2R );
	Matrix3Concat( cs->axis, tmp_axis, ref_axis );

	if ( cs->child_list )
	{
		U_ListIterInit( &iter, cs->child_list );
		for( ; ( child_cs = (md_coordsys_t *)U_ListIterNext( &iter ) ) ; )
		{
			G_SetupModelRecursive( child_cs, cs->origin, cs->axis );
		}
	}
}

void G_SetupModel( md_coordsys_t *root, vec3d_t ref_origin, vec3d_t ref_axis[3] )
{
	G_SetupModelRecursive( root, ref_origin, ref_axis );
}


/*
  ==============================
  G_DrawSubmodel

  ==============================
*/
void G_DrawSubmodel( md_submodel_t *submodel, vec3d_t origin, vec3d_t axis[3] )
{
	vec3d_t		submodel_axis[3];
	vec3d_t		verts[1024];

	res_submd_cache_t	*cache;
	
	cache = (res_submd_cache_t *)(submodel->res_submd->res_cache);

	U_VACopy( verts, (vec3d_t * )&cache->data[cache->vertex_ofs], cache->vertexnum );

	U_VANormalize( verts, cache->vertexnum );
	U_VAScale( verts, cache->vertexnum, submodel->scale );
	Matrix3SetupRotate( submodel_axis, submodel->lat*D2R, submodel->yaw*D2R, submodel->lon*D2R );
	U_VARotateAxis( verts, cache->vertexnum, submodel_axis );
	U_VATranslate( verts, cache->vertexnum, submodel->origin );
	
	U_VARotateAxis( verts, cache->vertexnum, axis );
	U_VATranslate( verts, cache->vertexnum, origin );

//	R_BE_SetVertexArray( submd->vertexnum, verts );
	R_BE_SetVertexOfs( 0 );
	R_BE_AppendVertexArray( cache->vertexnum, verts );
	R_BE_SetVertexOfs( 0 );

	R_BE_LockVertexArray();

	R_BE_ResetLocalBuffers();
	R_BE_SetCMDBuffer( cache->cmdnum, (int *)&cache->data[cache->cmd_ofs] );
	R_BE_SetVrefArray( cache->vrefnum, (int *)&cache->data[cache->vref_ofs] );
	R_BE_SetTexcoordArray( cache->tcnum, (vec2d_t *)&cache->data[cache->tc_ofs] );

	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, ((res_gltex_cache_t *)(submodel->res_gltex->res_cache))->texobj );
	glColor3f( 1.0, 1.0, 1.0 );
	R_BE_ProcessCMDBuffer();
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	R_BE_UnlockVertexArray();	
}

/*
  ==============================
  G_DrawModel

  ==============================
*/
void G_DrawModelRecursive( md_coordsys_t *cs )
{
	u_list_iter_t		iter;
	md_coordsys_t		*child_cs;
	md_submodel_t		*submodel;
	
	if ( cs->submodel_list )
	{
		U_ListIterInit( &iter, cs->submodel_list );
		for( ; ( submodel = (md_submodel_t *)U_ListIterNext( &iter ) ) ; )
		{
			G_DrawSubmodel( submodel, cs->origin, cs->axis );
		}
	}

	if ( cs->child_list )
	{
		U_ListIterInit( &iter, cs->child_list );
		for ( ; ( child_cs = (md_coordsys_t *)U_ListIterNext( &iter ) ) ; )
		{
			G_DrawModelRecursive( child_cs );
		}
	}

}

void G_DrawModel( md_coordsys_t *root )
{
	G_DrawModelRecursive( root );
}
