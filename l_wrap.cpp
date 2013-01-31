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



// l_wrap.c

#include "interfaces.h"
#include "r_fake/r_interface.h"
#include "r_fake/g_trace.h"
#include "defs.h"
#include "db_shape.h"
#include "db_lightmap.h"
#include "layer_defs.h"
#include "shape_defs.h"

extern g_resources_t	*g_rs;	// fixme
extern g_state_t	*g_st;
extern int		gc_localworldframe;

g_resource_t * GI_ResourceSearch( const char *name )
{
	return G_ResourceSearch( g_rs, name );
}

g_resource_t * GI_ResourceAttach( char *name, char *user_name,
				  int (*user_callback_func)(g_resource_t *r, int reason) )
{
	return G_ResourceAttach( g_rs, name, user_name, user_callback_func );
}

void GI_ResourceDetach( char *name, char *user_name )
{
	G_ResourceDetach( g_rs, name, user_name );
}

hobj_t * GI_ReadClass( char *path )
{
	hobj_t		*root;
	tokenstream_t	*ts;

	ts = BeginTokenStream( path );
	if ( !ts )
		__error( "can't open file '%s'\n", path );

	root = ReadClass( ts );
	EndTokenStream( ts );

	return root;
}



/*
  ==============================
  GI_RegisterWFInfo

  ==============================
*/
void GI_RegisterWFInfo( wfInfoType type, void *wfinfo )
{
	__chkptr( g_st );

#if 0
	switch( type )
	{

	case WFInfoType_model:
	{
		if ( gc_state->cl.wf_model_num >= G_CLIENT_MAX_MODELS )
			__error( "reached G_CLIENT_MAX_MODELS\n" );

		gc_state->cl.wf_models[gc_state->cl.wf_model_num++] = (wf_model_info_t *) wfinfo;			
	}
	break;
	
	case WFInfoType_dlight:
	{
		if ( gc_state->cl.wf_dlight_num >= G_CLIENT_MAX_DLIGHTS )
			__error( "reached G_CLIENT_MAX_DLIGHTS\n" );
		
		gc_state->cl.wf_dlights[gc_state->cl.wf_dlight_num++] = (wf_dlight_info_t *) wfinfo;	
	}
	break;

	case WFInfoType_sprite:
	{
		if ( gc_state->cl.wf_sprite_num >= G_CLIENT_MAX_SPRITES )
			__error( "reached G_CLIENT_MAX_SPRITES\n" );
		
		gc_state->cl.wf_sprites[gc_state->cl.wf_sprite_num++] = (wf_sprite_info_t *) wfinfo;			
	}
	break;

	case WFInfoType_fpsys:
	{
		if ( gc_state->cl.wf_fpsys_num >= G_CLIENT_MAX_FPSYSS )
			__error( "reached G_CLIENT_MAX_FPSYSS\n" );
		
		gc_state->cl.wf_fpsyss[gc_state->cl.wf_fpsys_num++] = (wf_fpsys_info_t *) wfinfo;			
	}
	break;
	
	case WFInfoType_light:
	{
#if 0
		//
		// hack
		//
		
		wf_light_info_t		*light;

		light = (wf_light_info_t *) wfinfo;
		LightmapDBU_ApplyLightInfo( g_st->map->lightmap_db, light );
#else
		if ( gc_state->cl.wf_light_num >= G_CLIENT_MAX_LIGHTS )
			__error( "reached G_CLIENT_MAX_LIGHTS\n" );
		
		gc_state->cl.wf_lights[gc_state->cl.wf_light_num++] = (wf_light_info_t *) wfinfo;			
#endif
	}
	break;

	case WFInfoType_shape:
	{
		if ( gc_state->cl.wf_shape_num >= G_CLIENT_MAX_SHAPES )
			__error( "reached G_CLIENT_MAX_SHAPES\n" );
		
		gc_state->cl.wf_shapes[gc_state->cl.wf_shape_num++] = (wf_shape_info_t *) wfinfo;
	}
	break;

	default:
		__error( "unknown type\n" );
	}

#endif
}

/*
  ==============================
  GI_TraceBB

  ==============================
*/
g_trace_t * GI_TraceBB( vec3d_t min, vec3d_t max, vec3d_t trymin, vec3d_t trymax )
{
	return G_TraceBoundBox( gc_state->map.blockmap, min, max, trymin, trymax );
}

void GI_MapTraceBB( g_trace_t *tr, vec3d_t min, vec3d_t max, vec3d_t trymin, vec3d_t trymax )
{
	g_trace_t	*tmp;
	tmp = G_TraceBoundBox( gc_state->map.blockmap, min, max, trymin, trymax );     
	memcpy( tr, tmp, sizeof( g_trace_t ) );

//	printf( "wrap: scale %f\n", tr->scale );
}

void GI_MapTraceLine( g_trace_t *tr, vec3d_t from, vec3d_t to )
{
	G_TraceLine( gc_state->map.blockmap, tr, from, to );
}


/*
  ==============================
  GI_SelectShapesInBoundBox

  ==============================
*/
void GI_SelectShapesInBoundBox( vec3d_t min, vec3d_t max, void (*action_func)(unique_t shape_id ) )
{
	if ( g_st ) {
        
		R_SelectShapesInBoundBox( min, max, action_func );
    }
}


/*
  ==============================
  GI_SelectLightSource

  ==============================
*/
lightsource_t * GI_SelectLightSourceWhereAnyLightID( unique_t id_any_light )
{
	return gc_state->map.lightmap_db->SelectLightsourceWhereAnyLightID( id_any_light );
}


/*
  ==============================
  GI_LightSourceUpdate

  ==============================
*/
void GI_LightSourceUpdate( lightsource_t *lsrc )
{
	lightmap_t	*lmap;
	int		num;

	__chkptr( lsrc );

	num = 0;

	//
	// visit all lightmaps of this source 
	//
	for ( lmap = lsrc->head; lmap ; lmap=lmap->source_link )
	{
		if ( !lmap->layer_ref )
		{
//			__warning( "source got a lightmap that ain't got a layer_ref\n" );
			continue;
		}

		//
		// force layer to recombine lightmaps
		//

		lmap->layer_ref->flags &= ~LAYER_FLAGS_VALID_LIGHTMAP_COMBINE;
		lmap->layer_ref->shape->flags |= SHAPE_FLAGS_LAYERS_NEED_CHECK;

		num++;
	}

//	__named_message( " %d lightmaps changed\n", num );
	
}

/*
  ==============================
  GI_GetUniqueID

  ==============================
*/
unique_t GI_GetUniqueID( void )
{
	return GC_GetUniqueID( gc_state );
}


/*
  ==============================
  GI_MapShapeIDSetFlatColor

  ==============================
*/
void GI_MapShapeIDSetFlatColor( unique_t id_shape, vec3d_t flat_color )
{
	
	shape_t				*shape;
	layer_t				*layer;
	
	u_list_iter_t		iter;
	
	shape = ShapeDB_SelectShapeWhere_selfid( gc_state->map.shape_db, id_shape );
	
	if ( shape )
	{
		U_ListIterInit( &iter, shape->layer_list );
		for ( ; ( layer = (layer_t *)U_ListIterNext( &iter ) ) ; )
		{
			if ( layer->ordinal == 1 )	// the primary texture layer
			{	
				layer->ambient_color[C_R] = (unsigned char)(flat_color[0] * 255.0);
				layer->ambient_color[C_G] = (unsigned char)(flat_color[1] * 255.0);
				layer->ambient_color[C_B] = (unsigned char)(flat_color[2] * 255.0);
				layer->ambient_color[C_A] = 255;
				break;
			}
		}
	}
}	



/*
  ==============================
  GI_RenderAddSprite

  ==============================
*/
void GI_RenderAddSprite( ri_sprite_t *ri )
{
	if ( gc_state->cl.ri_sprite_num >= G_CLIENT_MAX_RI_SPRITES )
	{
		__error( "reached G_CLIENT_MAX_RI_SPRITES\n" );
	}

	gc_state->cl.ri_sprites[gc_state->cl.ri_sprite_num++] = ri;	
}

/*
  ==============================
  GI_RenderAddPsys

  ==============================
*/
void GI_RenderAddPsys( ri_psys_t *ri )
{
	if ( gc_state->cl.ri_psys_num >= G_CLIENT_MAX_RI_PSYSS )
	{
		__error( "reached G_CLIENT_MAX_RI_PSYSS\n" );
	}

	gc_state->cl.ri_psyss[gc_state->cl.ri_psys_num++] = ri;	
}

/*
  ==============================
  GI_RenderAddHalo

  ==============================
*/
void GI_RenderAddHalo( ri_halo_t *ri )
{
	if ( gc_state->cl.ri_halo_num >= G_CLIENT_MAX_RI_HALOS )
	{
		__error( "reached G_CLIENT_MAX_RI_HALOS\n" );
	}

	gc_state->cl.ri_halos[gc_state->cl.ri_halo_num++] = ri;
}

/*
  ==============================
  GI_RenderAddLocalLight

  ==============================
*/
void GI_RenderAddLocalLight( ri_local_light_t *ri )
{
	if ( gc_state->cl.ri_local_light_num >= G_CLIENT_MAX_RI_LOCAL_LIGHTS )
	{
		__error( "reached G_CLIENT_MAX_RI_LOCAL_LIGHTS\n" );
	}

	gc_state->cl.ri_local_lights[gc_state->cl.ri_local_light_num++] = ri;
}

/*
  ==============================
  GI_RenderAddModel

  ==============================
*/
void GI_RenderAddModel( ri_model_t *ri )
{
	if ( gc_state->cl.ri_model_num >= G_CLIENT_MAX_RI_MODELS )
	{
		__error( "reached G_CLIENT_MAX_RI_MODELS\n" );
	}

	gc_state->cl.ri_models[gc_state->cl.ri_model_num++] = ri;	
}


/*
  ==============================
  GI_RenderAddView

  ==============================
*/
void GI_RenderAddView( ri_view_t *ri )
{
	sh_var_t		*tmp;
	

	if ( ri->style == viewStyle_player )
	{
		tmp = SHP_GetVar( "gc_id_bind_view" );
		
		if ( ri->id == tmp->ivalue && ri->id != UNIQUE_INVALIDE )
		{
			// that's a hack that tells the object not to draw models and sprites
			ri->take_view = true;	// tell client that this view was rendered

			gc_state->cl.ri_view_player = ri;
		}
	}
	if ( ri->style == viewStyle_spectator )
	{
		gc_state->cl.ri_view_spectator = ri;	
	}
	
	if ( ri->style == viewStyle_sky )
	{
		gc_state->cl.ri_view_sky = ri;
	}
}

/*
  ==============================
  GI_DrawRect

  ==============================
*/
void GI_DrawRect( di_rect_t *di )
{
	if ( gc_state->cl.di_rect_num >= G_CLIENT_MAX_DI_RECTS )
	{
		__error( "reached G_CLIENT_MAX_DI_RECTS\n" );
	}

	gc_state->cl.di_rects[gc_state->cl.di_rect_num++] = di;	
}

/*
  ==============================
  GI_DrawText

  ==============================
*/
void GI_DrawText( di_text_t *di )
{
	if ( gc_state->cl.di_text_num >= G_CLIENT_MAX_DI_TEXTS )
	{
		__error( "reached G_CLIENT_MAX_DI_TEXTS\n" );
	}

	gc_state->cl.di_texts[gc_state->cl.di_text_num++] = di;	
}
