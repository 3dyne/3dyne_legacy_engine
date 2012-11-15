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



// shader.c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "lib_math.h"
#include "lib_container.h"
#include "shock.h"
#include "g_shared.h"
#include "ca_service.h"
#include "r_private.h"
#include "r_glbackend.h"
#include "shader.h"
#include "layers.h"
#include "unitex.h"
#include "mesh.h"

#include "r_count.h"
#include "sh_parser.h"

/*
  =============================================================================
  instance stuff

  =============================================================================
*/

/*
  ==================================================
  Shader_Mgr

  ==================================================
*/

/*
  ==============================
  Shader_NewMgr

  ==============================
*/
shader_mgr_t * Shader_NewMgr( void )
{
	shader_mgr_t	*mgr;

	mgr = NEWTYPE( shader_mgr_t );
	mgr->shader_list = U_NewList();

	return mgr;
}

/*
  ==============================
  Shader_FreeMgr

  ==============================
*/
void Shader_FreeMgr( shader_mgr_t *mgr )
{
	U_FreeList( mgr->shader_list );
	FREE( mgr );
}

/*
  ==============================
  Shader_MgrCleanUp
  
  ==============================
*/
void Shader_MgrCleanUp( shader_mgr_t *mgr )
{
	__error( "not impl\n" );
}

/*
  ==============================
  Shader_MgrRegisterShader

  ==============================
*/
void Shader_MgrRegisterShader( shader_mgr_t *mgr, shader_t *shd )
{
	// fixme: should duplicated shaders be checked here ?

	U_ListInsertAtHead( mgr->shader_list, shd );
}

/*
  ==============================
  Shader_MgrSearchShader

  ==============================
*/
shader_t * Shader_MgrSearchShader( shader_mgr_t *mgr, layers_characteristic_t *ca )
{
	u_list_iter_t		iter;
	shader_t		*shd;

	U_ListIterInit( &iter, mgr->shader_list );
	for ( ; ( shd = (shader_t *)U_ListIterNext( &iter ) ) ; )
	{		
		if ( Layers_CompareCharacteristics( shd->ca, ca ) )
			return shd;
	}
	
	return NULL;
}

/*
  ==============================
  Shader_MgrDump
  
  ==============================
*/
void Shader_MgrDump( shader_mgr_t *mgr )
{
	u_list_iter_t		iter;
	shader_t		*shd;

	printf( "Shader_MgrDump:\n" );
	
	printf( " %d shaders\n", U_ListLength( mgr->shader_list ) );

	U_ListIterInit( &iter, mgr->shader_list );
	for ( ; ( shd = (shader_t *)U_ListIterNext( &iter ) ) ; )
	{
		Shader_Dump( shd );
	}
}


/*
  ==================================================
  Shader

  ==================================================
*/

/*
  ==============================
  Shader_New

  ==============================
*/
shader_t * Shader_New( layers_characteristic_t *ca )
{
	shader_t	*shd;

	shd = NEWTYPE( shader_t );
	shd->ca = ca;

	return shd;
}

/*
  ==============================
  Shader_CleanUp

  ==============================
*/
void Shader_CleanUp( shader_t *shd )
{
	__error( "not impl\n" );
}

/*
  ==============================
  Shader_Free

  ==============================
*/
void Shader_Free( shader_t *shd )
{
	FREE( shd->ca );
	FREE( shd );
}

/*
  ==============================
  Shader_GetLayersCharacteristic

  ==============================
*/
layers_characteristic_t * Shader_GetLayersCharacteristic( shader_t *shd )
{
	return shd->ca;
}

/*
  ==============================
  Shader_Dump

  ==============================
*/
void Shader_Dump( shader_t *shd )
{
	int		i;

	printf( "Shader_Dump:\n" );

	printf( " %d layers\n", shd->ca->layernum );

	for ( i = 0; i < shd->ca->layernum; i++ )
	{
		printf( " %d", shd->ca->layers[i].ordinal );
	}
	printf( "\n" );
}


/*
  =============================================================================
  frame suff

  =============================================================================
*/

/*
  ==================================================
  Shader_Mgr

  ==================================================
*/

/*
  ==============================
  Shader_MgrFrameBegin
  ==============================
*/
void Shader_MgrFrameBegin( shader_mgr_t *mgr )
{
	u_list_iter_t	iter;
	shader_t	*shader;

	U_ListIterInit( &iter, mgr->shader_list );
	for ( ; ( shader = (shader_t *)U_ListIterNext( &iter ) ) ; )
	{
		Shader_FrameBegin( shader );
	}
}

/*
  ==============================
  Shader_MgrFrameEnd

  ==============================
*/
void	Shader_MgrFrameEnd( shader_mgr_t *mgr )
{
	u_list_iter_t	iter;
	shader_t	*shader;

	U_ListIterInit( &iter, mgr->shader_list );
	for ( ; ( shader = (shader_t *)U_ListIterNext( &iter ) ) ; )
	{
		Shader_FrameEnd( shader );
	}
}

/*
  ==============================
  Shader_MgrRender

  ==============================
*/
void	Shader_MgrRender( shader_mgr_t *mgr )
{
	int		pass;
	int		i, j;

	u_list_iter_t	iter;
	shader_t	*shader;
	layer_t		*layer;
	shape_t		*shp;
	int		texobj, last_texobj;
	
	int		*m_vrefs;
	int		*m_cmds;
	vec2d_t		*m_tcs;

	int		m_vrefnum;
	int		m_cmdnum;
	int		m_tcnum;

	bool_t		tex_enable = 0;
	bool_t		draw_lightmap;
	static sh_var_t	*var_drawtris = NULL;
	static sh_var_t	*var_drawlightmap = NULL;

//	R_BE_ResetCounters();

	COUNT_RESET( count_rf_num_shader_bindtex );
	COUNT_RESET( count_rf_num_shader_blendfunc );
	COUNT_RESET( count_rf_num_shader_layer );

	if ( !var_drawtris )
		var_drawtris = SHP_GetVar( "r_drawtris" );
	if ( !var_drawlightmap )
		var_drawlightmap = SHP_GetVar( "r_drawlightmap" );

	if ( var_drawtris->ivalue == 1 )
	{
		glDisable( GL_TEXTURE_2D );
		glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
	}
	else
	{
		glEnable( GL_TEXTURE_2D );
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	}

	if ( var_drawlightmap->ivalue == 1 )
	{
		draw_lightmap = true;
	}
	else
	{
		draw_lightmap = false;		
	}

	//
	// first pass: draw layers, not set LAYER_STATE_FLAGS_SHADE_LAST
	// second pass: draw layers, set LAYER_STATE_FLAGS_SHADE_LAST
	//

	for ( pass = 0; pass < 4; pass++ )
	{

		U_ListIterInit( &iter, mgr->shader_list );
		for ( ; ( shader = (shader_t *)U_ListIterNext( &iter ) ) ; )	
		{
			for ( i = 0; i < shader->ca->layernum; i++ )
			{	
				int	flags;

				flags = shader->ca->layers[i].glstate.flags;

				if ( pass == 0 && 
				     ( ( flags & LAYER_STATE_FLAGS_SHADE_P1) ||
				       ( flags & LAYER_STATE_FLAGS_SHADE_P2) ||
				       ( flags & LAYER_STATE_FLAGS_SHADE_P3) ) )
					continue;
				
				if ( pass == 1 &&
				     ( !( flags & LAYER_STATE_FLAGS_SHADE_P1 ) ||
				       ( flags & LAYER_STATE_FLAGS_SHADE_P2) ||
				       ( flags & LAYER_STATE_FLAGS_SHADE_P3) ) )
					continue;

				if ( pass == 2 &&
				     ( ( flags & LAYER_STATE_FLAGS_SHADE_P1 ) ||
				       !( flags & LAYER_STATE_FLAGS_SHADE_P2) ||
				       ( flags & LAYER_STATE_FLAGS_SHADE_P3) ) )
					continue;

				if ( pass == 3 &&
				     ( ( flags & LAYER_STATE_FLAGS_SHADE_P1 ) ||
				       ( flags & LAYER_STATE_FLAGS_SHADE_P2) ||
				       !( flags & LAYER_STATE_FLAGS_SHADE_P3) ) )
					continue;
				

				// speed up: copy texobj from unitex into shape
				for( j = 0; j < shader->shape_ref_num; j++ )
				{
					shp = shader->shape_refs[j];
					layer = shp->la->layers[i];	
					
					shp->texobj = UniTex_GetGLtexobj( layer->real_source ); 
				} 
				
				Shader_SortShapes( shader );
				
				glBlendFunc( shader->ca->layers[i].glstate.sfactor,
					     shader->ca->layers[i].glstate.dfactor );
				COUNT_INCR( count_rf_num_shader_blendfunc );

				glDepthMask( shader->ca->layers[i].glstate.flags & LAYER_STATE_FLAGS_DEPTH_MASK );
				
				last_texobj = -1;

				if ( draw_lightmap )
				{
					glEnable( GL_TEXTURE_2D );
					tex_enable = true;
				}
				
				for( j = 0; j < shader->shape_ref_num; j++ )
				{
					COUNT_INCR( count_rf_num_shader_layer );
					shp = shader->shape_refs[j];
					layer = shp->la->layers[i];							
					
					if ( draw_lightmap )
					{
						if ( layer->ordinal == 1 && tex_enable == true )
						{
							tex_enable = false;
							glDisable( GL_TEXTURE_2D );
						}
					}
					
					texobj = shp->texobj; //UniTex_GetGLtexobj( layer->real_source );
					if ( texobj != last_texobj )
					{
						glBindTexture( GL_TEXTURE_2D, texobj );
						COUNT_INCR( count_rf_num_shader_bindtex );
						last_texobj = texobj;
					}
					
//				printf( "%d ", texobj );
					
					Mesh_GetIntBase( shp->mesh, 1, &m_vrefnum, &m_vrefs );
					Mesh_GetIntBase( shp->mesh, 2, &m_cmdnum, &m_cmds );
					Mesh_GetVec2dBase( layer->tc, 0, &m_tcnum, &m_tcs );
					
					R_BE_SetVertexOfs( shp->vofs );		
					
					R_BE_SetCMDBuffer( m_cmdnum, m_cmds );
					R_BE_SetVrefArray( m_vrefnum, m_vrefs );
					R_BE_SetTexcoordArray( m_tcnum, m_tcs );
					
				// hack: ambient color
					glColor4ubv( layer->ambient_color );
					
					R_BE_ProcessCMDBuffer();	
				}
			}
		}
	}
}

/*
  ==============================
  Shader_MgrFrameRegisterShape
  
  ==============================
*/
void	Shader_MgrFrameRegisterShape( shader_mgr_t *mgr, shape_t *shp )
{
	Shader_FrameRegisterShape( shp->shd, shp );
}


/*
  ==================================================
  Shader

  ==================================================
*/

/*
  ==============================
  Shader_FrameBegin

  ==============================
*/
void Shader_FrameBegin( shader_t *shd )
{
	shd->shape_ref_num = 0;
}

/*
  ==============================
  Shader_FrameEnd

  ==============================
*/
void Shader_FrameEnd( shader_t *shd )
{
	
}

/*
  ==============================
  Shader_FrameRegisterShape

  ==============================
*/
void Shader_FrameRegisterShape( shader_t *shd, shape_t *shp )
{
	// fixme: should not be checked here
	if ( shp->shd != shd )
		__error( "wrong shader in shape\n" );

	if ( shd->shape_ref_num >= SHADER_MAX_SHAPES )
		__error( "reached SHADER_MAX_SHAPES\n" );

	shd->shape_refs[shd->shape_ref_num] = shp;
	shd->shape_ref_num++;
}

/*
  ==============================
  Shader_SortShapes

  ==============================
*/

int QSort_ShapeCompareFunc( const void *l1, const void *l2 )
{
	shape_t		*s1;
	shape_t		*s2;

	int		texobj1;
	int		texobj2;

	s1 = (*(shape_t**)(l1));
	s2 = (*(shape_t**)(l2));

	texobj1 = s1->texobj; //UniTex_GetGLtexobj( s1->la->layers[shader_sort_layer]->real_source );
	texobj2 = s2->texobj; //UniTex_GetGLtexobj( s2->la->layers[shader_sort_layer]->real_source );
	
	if ( texobj1 > texobj2 )
		return 1;
	else if ( texobj1 < texobj2 )
		return -1;
	return 0;
}

void Shader_SortShapes( shader_t *shd )
{
	qsort( (void *)&shd->shape_refs[0], shd->shape_ref_num, sizeof( shape_t * ), QSort_ShapeCompareFunc );
}
  

