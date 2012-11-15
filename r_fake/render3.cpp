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



// render3.c

#include "r_interfaces.h"
#include "render.h"

#include "r_hvis.h"

#include "sys_dep.h"
#include "u_va.h"

#include "lib_math.h"
#include "lib_container.h"

#include "db_lightmap.h"
#include "imgcache.h"
#include "mesh.h"
#include "tess.h"
#include "shape_defs.h"
#include "layer.h"
#include "unitex.h"
#include "layers.h"
#include "shader.h"

#include "r_sprite.h"

#include "r_shpctrljobs.h"

#include "r_count.h"
#include "g_resources.h"
#include "sh_parser.h"
#include "la_service.h"
#include "r_interface.h"

g_map_t		*a_map;	// set by R_InitData

static sh_var_t	*r_lod = NULL;

fp_t	r_curlod = 0.0;


// counts render frames forever 
int		r_frame_count = 1;

// something like r_frame_count ...
// to avoid identical shaperefs collected inside different render stages
int		r_local_count = 1;		

//
// 'interactive' stuff
//

int			ri_local_light_num;	// set by R_SetLocalLightInfos
ri_local_light_t	**ri_local_lights;

int			ri_sprite_num;	// set by R_SetSpriteInfos
ri_sprite_t	**ri_sprite_infos;

int			ri_psys_num;	// set by R_SetPsysInfos
ri_psys_t	**ri_psys_infos;

int			ri_halo_num;
ri_halo_t	**ri_halo_infos;

//
// Shape Control Job List
//

u_list_t	*r_job_list = NULL;

//
// image cache upload util
//

icu_upload_frame_t	r_upload_frame;

/*
  ==============================
  J_ObjWarning

  ==============================
*/
void J_ObjWarning( hobj_t *obj, const char *text )
{
	
	__message( "\n===== Shape Control Job Warning =====\n" );
	__named_message( "job in init state said:\n" );
	__named_message( "%s\n", text );
	
	ClassTraceToRoot( obj );
	__message( "=======================================\n" );
}


/*
  =============================================================================
  additional render data setup

  =============================================================================
*/

void SetupNodeBoundsRecursive( int node, fp_t *min, fp_t *max )
{
	mapnode_t		*n;

	n = &r_mapnodes[node];
	
	if ( !n->pl )
	{
		// it's a leaf
	    
		if ( n->visinfo )
		{
			// it's a visleaf, add bound box 
			Vec3dAddToBB( min, max, n->min );
			Vec3dAddToBB( min, max, n->max );			
		}
	}
	else
	{
		// it's a node
		Vec3dInitBB( n->min, n->max, 999999.9 );
		SetupNodeBoundsRecursive( n->child[0], n->min, n->max );
		SetupNodeBoundsRecursive( n->child[1], n->min, n->max );

		Vec3dAddToBB( min, max, n->min );
		Vec3dAddToBB( min, max, n->max );

//		Vec3dPrint( n->min );
//		Vec3dPrint( n->max );
	}
}

void SetupNodeBounds( void )
{
  vec3d_t		min = {0,0,0}, max = {0,0,0};

	SetupNodeBoundsRecursive( 0, min, max );
}

void SetupNodeLeafbitsRecursive( int node, int *firstbit, int *lastbit )
{
	mapnode_t		*n;

	n = &r_mapnodes[node];

	if ( !n->pl )
	{
		// it's a leaf

		int		bitpos;
		hpair_t		*pair;

		pair = FindHPair( n->self, "bitpos" );
		if ( pair )
		{
			// it's a visleaf			
			HPairCastToInt_safe( &bitpos, pair );
			*firstbit = *lastbit = bitpos;
		}
		else
		{
			*firstbit = *lastbit = -1;
		}
	}
	else
	{
		// it's a node
		int	f1, l1;
		int	f2, l2;

		SetupNodeLeafbitsRecursive( n->child[0], &f1, &l1 );
		SetupNodeLeafbitsRecursive( n->child[1], &f2, &l2 );

		if ( f1 != -1 && f2 != -1 )
		{
			n->firstbit = f1 < f2 ? f1 : f2;
			n->lastbit = l1 > l2 ? l1 : l2;
		}
		else if ( f1 != -1 && f2 == -1 )
		{
			n->firstbit = f1;
			n->lastbit = l1;
		}
		else if ( f1 == -1 && f2 != -1 )
		{
			n->firstbit = f2;
			n->lastbit = l2;
		}
		else
		{
			n->firstbit = -1;
			n->lastbit = -1;
		}

		*firstbit = n->firstbit;
		*lastbit = n->lastbit;

//		printf( "%d %d\n", n->firstbit, n->lastbit );
	}
}

void SetupNodeLeafbits( void )
{
	int		first, last;
	SetupNodeLeafbitsRecursive( 0, &first, &last );		
}



/*
  ====================
  ReadPlaneClass

  ====================
*/
hmanager_t * ReadPlaneClass( char *name )
{
	tokenstream_t	*ts;
	hobj_t		*planecls;
	hmanager_t	*hm;
	hobj_search_iterator_t	iter;
	hobj_t		*plane;
	hobj_t		*flipplane;
	cplane_t		*pl;
	int		num;
	hpair_t		*pair;

	ts = BeginTokenStream( name );
	planecls = ReadClass( ts );
	EndTokenStream( ts );

	hm = NewHManager();
	HManagerSetRootClass( hm, planecls );
	HManagerRebuildHash( hm );

	//
	// create compiled planes
	//

	fprintf( stderr, "load plane class and compile ...\n" );

	InitClassSearchIterator( &iter, planecls, "plane" );

	for ( num = 0; ( plane = SearchGetNextClass( &iter ) ); num++ )
	{
		pl = NEWTYPE( cplane_t );

		// plane norm
		pair = FindHPair( plane, "norm" );
		if ( !pair )
			__error( "missing plane normal.\n" );
		HPairCastToVec3d_safe( pl->norm, pair );

		// plane dist
		pair = FindHPair( plane, "dist" );
		if ( !pair )
			__error( "missing plane distance.\n" );
		HPairCastToFloat_safe( &pl->dist, pair );
		
		// plane type
		pair = FindHPair( plane, "type" );
		if ( !pair )
			__error( "missing plane type.\n" );
		HPairCastToInt_safe( &pl->type, pair );

		pl->self = plane;
		SetClassExtra( plane, pl );
		
	}

	//
	// resolve clsref_flipplane
	//
	InitClassSearchIterator( &iter, planecls, "plane" );

	for ( num = 0; ( plane = SearchGetNextClass( &iter ) ); num++ )
	{
		// plane flipplane clsref
		pair = FindHPair( plane, "flipplane" );
		if ( !pair )
			__error( "missinig clsref flipplane" );

		flipplane = HManagerSearchClassName( hm, pair->value );
		if ( !flipplane )
			__error( "can't resolve clsref flipplane.\n" );

		pl = (cplane_t *)GetClassExtra( plane );
		pl->flipplane = (cplane_t *)GetClassExtra( flipplane );
	}

	printf( " %d planes\n", num );

	return hm;
}


/*
  =============================================================================
  bsp stuff

  =============================================================================
*/

mapnode_t * FindLeafForPoint( vec3d_t point )
{
	int		node;
	mapnode_t	*n;
	fp_t		d;
	
	node = 0;

	for(;;)
	{
		n = &r_mapnodes[node];
		if ( !n->pl )
		{
			return n;
		}

		d = Vec3dDotProduct( n->pl->norm, point ) - n->pl->dist;
		if ( d >= 0 )
		{
			// go to front child
			node = n->child[0];
		}
		else
		{
			// go to back child
			node = n->child[1];
		}
	}
}

/*
  ==============================
  BSP_VisitLeafsInBoundBox

  ==============================
*/

void BSP_VisitLeafsInBoundBox_r( mapnode_t *node, vec3d_t min, vec3d_t max, void (*action_func)(mapnode_t *leaf) )
{
	if ( node->pl )
	{
		plane_t		tmp;
		planeTestState	side;
		
		Vec3dCopy( tmp.norm, node->pl->norm );
		tmp.dist = node->pl->dist;

		side = PlaneTestBB( &tmp, min, max );

		if ( side == PlaneTest_front || side == PlaneTest_split )
		{
			BSP_VisitLeafsInBoundBox_r( &r_mapnodes[node->child[0]], min, max, action_func );
		}
		if ( side == PlaneTest_back || side == PlaneTest_split )
		{
			BSP_VisitLeafsInBoundBox_r( &r_mapnodes[node->child[1]], min, max, action_func );
		}
	}
	else
	{
		if ( node->visinfo )
		{
			action_func( node );
		}
	}
}

void BSP_VisitLeafsInBoundBox( vec3d_t min, vec3d_t max, void (*action_func)(mapnode_t *leaf) )
{
	BSP_VisitLeafsInBoundBox_r( &r_mapnodes[0], min, max, action_func );
}

/*
  ==============================
  InsertShapeRefIntoMapnodes

  ==============================
*/

static int	stat_shaperefnum;

void InsertShapeRefIntoMapnodesRecursive( mapnode_t *node, shape_t *shp )
{
	if ( node->pl )
	{
		plane_t		tmp;
		planeTestState	side;

		Vec3dCopy( tmp.norm, node->pl->norm );
		tmp.dist = node->pl->dist;

		side = PlaneTestBB( &tmp, shp->min, shp->max );

		if ( side == PlaneTest_front || side == PlaneTest_split )
		{
			InsertShapeRefIntoMapnodesRecursive( &r_mapnodes[node->child[0]], shp );
		}
		if ( side == PlaneTest_back || side == PlaneTest_split )
		{
			InsertShapeRefIntoMapnodesRecursive( &r_mapnodes[node->child[1]], shp );
		}

	}
	else
	{
		if ( node->visinfo )
		{
			shape_ref_t	*sr;

			sr = NEWTYPE( shape_ref_t );
			sr->shape = shp;
			sr->next = node->shaperefs;
			node->shaperefs = sr;
			stat_shaperefnum++;
		}
	}
}

void InsertShapeRefIntoMapnodes( shape_t *shp )
{
	if ( !shp->mesh )
		__error( "(null) mesh in shape\n" );

	InsertShapeRefIntoMapnodesRecursive( &r_mapnodes[0], shp );	
}

// ========================================
// var view callbacks


static void VV_Vis( int flag, char *string )
{
	sprintf( string, "vis:%d mnodes (%d passed) w %d shapes", count_rf_num_vis_mapnode, count_rf_num_passed_mapnode, count_rf_num_vis_shape );
}

static void VV_Rjobs( int flag, char *string )
{
	sprintf( string, "rjobs: %d jobs (%d passed), %d destroied", count_rf_num_run_job, count_rf_num_vis_job, count_rf_num_destroy_job );
}

void R_Init()
{
	LA_InsertVarview( "vv_vis", VV_Vis );
	LA_InsertVarview( "vv_rjobs", VV_Rjobs );
}

void R_SetupShapeRefs( void )
{
	int		i;

	printf( "R_SetupShapeRefs:\n" );

	//
	// insert simple face shapes
	//
	stat_shaperefnum = 0;
	for ( i = 0; i < r_sfaceshapenum; i++ )
	{
		shape_t		*shp;

		shp = r_sfaceshapes[i];

		if ( !shp->mesh )
		{
			Tess_CreateMesh( shp );
		}
		InsertShapeRefIntoMapnodes( shp );	
		Mesh_Free( shp->mesh );
		shp->mesh = NULL;
	}
	printf( " %d simple face shapes with %d shaperefs\n", r_sfaceshapenum, stat_shaperefnum );

	//
	// insert curved surface shapes
	//
	stat_shaperefnum = 0;
	for ( i = 0; i < r_csurfshapenum; i++ )
	{
		shape_t		*shp;

		shp = r_csurfshapes[i];

		if ( !shp->mesh )
		{
			Tess_CreateMesh( shp );
		}
		InsertShapeRefIntoMapnodes( shp );			
		Mesh_Free( shp->mesh );
		shp->mesh = NULL;
	}
	printf( " %d curved surface shapes with %d shaperefs\n", r_csurfshapenum, stat_shaperefnum );

	//
	// insert curved face shapes
	//
	stat_shaperefnum = 0;
	for ( i = 0; i < r_cfaceshapenum; i++ )
	{
		shape_t		*shp;

		shp = r_cfaceshapes[i];

		if ( !shp->mesh )
		{
			Tess_CreateMesh( shp );
		}
		InsertShapeRefIntoMapnodes( shp );
		Mesh_Free( shp->mesh );
		shp->mesh = NULL;
	}
	printf( " %d curved face shapes with %d shaperefs\n", r_cfaceshapenum, stat_shaperefnum );

	//
	// insert meshtile shapes
	//
	stat_shaperefnum = 0;
	for ( i = 0; i < r_meshtileshapenum; i++ )
	{
		shape_t		*shp;

		shp = r_meshtileshapes[i];

		if ( !shp->mesh )
		{
			Tess_CreateMesh( shp );
		}
		InsertShapeRefIntoMapnodes( shp );
		Mesh_Free( shp->mesh );
		shp->mesh = NULL;
	}
	printf( " %d meshtile shapes with %d shaperefs\n", r_meshtileshapenum, stat_shaperefnum );
}

/*
  ==============================
  R_CleanUpAllShapeRefs

  ==============================
*/
void R_CleanUpAllShapeRefs( void )
{
	int		i;
	shape_ref_t	*sr, *next;

	for ( i = 0; i < r_mapnodenum; i++ )
	{
		for ( sr = r_mapnodes[i].shaperefs; sr ; sr=next )
		{
			next = sr->next;
			FREE( sr );
		}
	}
}

/*
  ==============================
  R_SelectShapesInBoundBox

  ==============================
*/
static void (*select_shapes_in_bound_box_func)(unique_t shape_id ) = NULL;
static fp_t *select_shapes_in_bound_box_min;
static fp_t *select_shapes_in_bound_box_max;

static void LeafFunc_SelectShapesInBoundBox( mapnode_t *n )
{
	shape_ref_t	*shp_ref;
	shape_t		*shp;


	for ( shp_ref = n->shaperefs; shp_ref ; shp_ref=shp_ref->next )
	{
		shp = shp_ref->shape;

		if ( shp->last_local == r_local_count )
		{
			continue;
		}
		shp->last_local = r_local_count;

		if ( Shape_BBDoIntersect( shp, select_shapes_in_bound_box_min, select_shapes_in_bound_box_max ) )
		{
			select_shapes_in_bound_box_func( shp->self_id );
		}
	}
}

void R_SelectShapesInBoundBox( vec3d_t min, vec3d_t max, void (*action_func)(unique_t shape_id ) )
{
	select_shapes_in_bound_box_min = min;
	select_shapes_in_bound_box_max = max;
	__chkptr( action_func );
	select_shapes_in_bound_box_func = action_func;
	
	BSP_VisitLeafsInBoundBox( min, max, LeafFunc_SelectShapesInBoundBox );
	r_local_count++;
}

/*
  ==============================
  R_IsBoundBoxVisible

  ==============================
*/

static bool_t is_bound_box_visible;

static void LeafFunc_IsBoundBoxVisible( mapnode_t *n )
{
	if ( n->is_visible )
		is_bound_box_visible = true;	
}

bool_t R_IsBoundBoxVisible( vec3d_t min, vec3d_t max )
{
	is_bound_box_visible = false;
	BSP_VisitLeafsInBoundBox( min, max, LeafFunc_IsBoundBoxVisible );	
	return is_bound_box_visible;
}

/*
  =============================================================================
  get visible SHAPES from PVS

  =============================================================================
*/

vec3d_t	r_flatcolors[256];

bool_t	debug_disable_specular = false;
bool_t	debug_disable_diffuse = false;
bool_t  debug_enable_trans = false;

bool_t	debug_wire_only = false;
bool_t	debug_visleaf_allow = false;
bool_t	debug_draw_visleaf = false;
bool_t	debug_draw_fieldcache = false;

vec3d_t		r_origin;
matrix3_t	r_matrix;
matrix3_t	r_invmatrix;
frustum_t	r_frustum;

int	r_visible_shape_num;
shape_t	*r_visible_shapes[10240];

void CacheVisibleShapes( unsigned char *vis )
{
	int		i;
	mapnode_t	*n;

	shape_t		*shp;
	shape_ref_t	*sr;
	
	r_visible_shape_num = 0;

	for ( i = 0; i < r_visleafnum; i++ )
	{
		if ( vis[i>>3] & (1<<(i&7)) )
		{
// 			frustumClipMask		clip;		// stored by CullVisleafs
			n = &r_mapnodes[r_leafbitpos[i]];
			n->is_visible = true;
// 			clip = n->clip;
			
			if ( !n->shaperefs )
				continue;
			
			for ( sr = n->shaperefs; sr ; sr=sr->next )
			{
				shp = sr->shape;
				__chkptr( shp );
				if ( shp->last_rf == r_frame_count )
				{
					continue;
				}
				else
				{
					shp->last_rf = r_frame_count;

					if ( shp->tess_name == ShapeTesselation_simple_face )
					{					
						if ( Vec3dDotProduct( shp->norm, r_origin ) - shp->dist < -16.0 )
							continue;
						
//						r_cached_sface_shapes[r_cached_sface_shape_num++] = shp;
					}
					else if ( shp->tess_name == ShapeTesselation_curved_face )
					{
						if ( Vec3dDotProduct( shp->norm, r_origin ) - shp->dist < -16.0 )
							continue;
//						r_cached_cface_shapes[r_cached_cface_shape_num++] = shp;
					}
					else if ( shp->tess_name == ShapeTesselation_glmesh )
					{
						if ( Vec3dDotProduct( shp->norm, r_origin ) - shp->dist < -16.0 )
							continue;
//						r_cached_cface_shapes[r_cached_cface_shape_num++] = shp;
					}
					else if ( shp->tess_name == ShapeTesselation_curved_surface )
					{
//						r_cached_csurf_shapes[r_cached_csurf_shape_num++] = shp;
					}
					else
					{
						__error( "unknown tess_name in shape\n" );
					}
					
					shp->visible_rf = r_frame_count;
					r_visible_shapes[r_visible_shape_num++] = shp;
					
					COUNT_INCR( count_rf_num_vis_shape );

					// prepare for dlight
					shp->falloff_nodes = NULL;
				}
			}
		}
		else
		{
			n = &r_mapnodes[r_leafbitpos[i]];
			n->is_visible = false;
		}
	}
}

void CullVisleafs( unsigned char *vis )
{
	int		i;
	mapnode_t	*n;
	int		pv_leafs = 0;
	int		culled_leafs = 0;
	frustumClipMask		clip;
	int		clip_num = 0;
	int		noclip_num = 0;

	for ( i = 0; i < r_visleafnum; i++ )
	{
		if ( vis[i>>3] & (1<<(i&7)) )
		{
			COUNT_INCR( count_rf_num_vis_mapnode );
			n = &r_mapnodes[r_leafbitpos[i]];
			//	printf( "r_frustum: %p\n", &r_frustum );
			clip = R_FrustumTestBB( &r_frustum, n->min, n->max);
			n->clip = clip;		// can be reused
			if ( clip == Frustum_call )
			{
				culled_leafs++;
				vis[i>>3] &= ~(1<<(i&7));
			}
			else 
			{
				COUNT_INCR( count_rf_num_passed_mapnode );
				
				if ( clip == Frustum_cnone )
				{
					noclip_num++;
				}
				else
				{
					clip_num++;
				}
			}
			pv_leafs++;
		}
	}
	
//	printf( " %d leafs culled of %d potential visible of %d visleafs (%d, %d)\n", culled_leafs, pv_leafs, r_visleafnum, noclip_num, clip_num );                                            
}

void DumpVisleaf( int visleaf )
{
	int		i;
	visleaf_t	*vl;
// 	portal_t	*pt;

	if ( visleaf == -1 )
	{
		printf( "no visleaf ?\n" );
		return;
	}

	vl = &r_visleafs[visleaf];

	for ( i = 0; i < vl->portalnum; i++ )
	{
// 		pt = &r_portals[i+vl->startportal];
//		printf( "c%d-t%d ", pt->complex_num, pt->through_num );
	}
	printf( "\n" );
}

void PVS_OrSeeThrough( int visleaf, unsigned char *see )
{
	visleaf_t		*vl;
	portal_t		*pt;
	int		i, j;
	int		see_bytes;

	see_bytes = ( r_visleafnum / 8 ) + 1;

	memset( see, 0, see_bytes );

	vl = &r_visleafs[visleaf];
	
	for ( i = 0; i < vl->portalnum; i++ )
	{
		pt = &r_portals[i+vl->startportal];
		
		if ( pt->see_through )
		{
			for ( j = 0; j < see_bytes; j++ )
			{
				see[j] |= pt->see_through[j];
			}
			COUNT_INCR( count_rf_num_visportal );
		}
	}

	// don't forget the visleaf itself
	see[vl->bitpos >> 3] |= 1U<<(vl->bitpos&7);
}

bool_t PVS_UpdateCache( vec3d_t origin )
{
	mapnode_t	*leaf;
	unsigned char	see[4096];

	COUNT_RESET( count_rf_current_mapnode );
	COUNT_RESET( count_rf_num_visportal );
	COUNT_RESET( count_rf_num_vis_mapnode );
	COUNT_RESET( count_rf_num_passed_mapnode );
	COUNT_RESET( count_rf_num_vis_shape );

	leaf = FindLeafForPoint( origin );

	if ( leaf->visleaf != -1 )
	{

//		printf( "visleaf: %s\n", r_visleafs[leaf->visleaf].self->name );
#if 0
		memcpy( see, leaf->can_see, r_visleafnum/8+1 );
		CullVisleafs( see );

		R_HVIS_CullVisleafs( leaf->visleaf, see, &r_frustum );
#else
		PVS_OrSeeThrough( leaf->visleaf, see );
		CullVisleafs( see );
//		R_HVIS_CullVisleafs( leaf->visleaf, see, &r_frustum );
#endif
		CacheVisibleShapes( see );

		return true;
	}
	return false;
}

/*
  =============================================================================
  sprite stuff

  =============================================================================
*/
void R_RenderSprites( void )
{
	int			i;
	ri_sprite_t	*sprite;

	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glEnable( GL_TEXTURE_2D );
	glEnable( GL_BLEND );                                          
//	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );	
	glBlendFunc( GL_ONE, GL_ONE );	
	glDepthMask(GL_FALSE);

	for ( i = 0; i < ri_sprite_num; i++ )
	{
		sprite = ri_sprite_infos[i];
		glColor4ubv( sprite->rgba );

		if ( ((res_gltex_cache_t*)(sprite->gltex_res->res_cache))->comp == resGltexComponents_rgb )
		{
			glBlendFunc( GL_ONE, GL_ONE );	
		}
		else
		{
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		}

		R_DrawSpriteWithRotation( sprite->gltex_res, sprite->origin, sprite->width, sprite->height, sprite->rotate );
	}
}

/*
  =============================================================================
  fpsys stuff

  =============================================================================
*/
void R_RenderFPsyss( void )
{
	int			i, j;
	ri_psys_t		*info;
	field_particle_system_t	*fpsys;
	field_particle_t	*p;

	res_gltex_cache_t	*cache;
	int		texobj;	

	vec3d_t		pos;

	COUNT_RESET( count_rf_num_fpsys );
	COUNT_RESET( count_rf_num_fpsys_visible );
	COUNT_RESET( count_rf_num_fpsys_bindtex );
	COUNT_RESET( count_rf_num_fpsys_bindtex );
	COUNT_RESET( count_rf_num_fpsys_blendfunc );
	COUNT_RESET( count_rf_num_fpsys_particle );
	
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glEnable( GL_TEXTURE_2D );
	glEnable( GL_BLEND );                                          
	glDepthMask(GL_FALSE);

//	glColor3f( 0.33, 0.33, 0.33 );	
	glColor3f( 0.20, 0.20, 0.20 );	
//	glColor3f( 1.0, 1.0, 1.0 );	

	for ( i = 0; i < ri_psys_num; i++ )
	{
		info = ri_psys_infos[i];
		fpsys = &info->fpsys;

		COUNT_INCR( count_rf_num_fpsys );

		if ( !R_IsBoundBoxVisible( info->min, info->max ) )
			continue;

		COUNT_INCR( count_rf_num_fpsys_visible );

		if ( fpsys->face.blend_func == particleFaceBlendFunc_addition )
		{
			glBlendFunc( GL_ONE, GL_ONE );	
			COUNT_INCR( count_rf_num_fpsys_blendfunc );
		}
		else if ( fpsys->face.blend_func == particleFaceBlendFunc_multiply )
		{
			glBlendFunc( GL_ONE, GL_ZERO );
			COUNT_INCR( count_rf_num_fpsys_blendfunc );
		}
		else if ( fpsys->face.blend_func == particleFaceBlendFunc_transparency )
		{
			glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );		
			COUNT_INCR( count_rf_num_fpsys_blendfunc );
		}

		cache = ( res_gltex_cache_t * ) fpsys->face.gltex_res->res_cache;
		texobj = cache->texobj;		
		glBindTexture( GL_TEXTURE_2D, texobj );
		COUNT_INCR( count_rf_num_fpsys_bindtex );

		for ( p = fpsys->particle_head; p ; p = p->next )
		{
			//
			// transform to world space
			//
			pos[0] = fpsys->trans.scale[0] * p->pos[0];
			pos[1] = fpsys->trans.scale[1] * p->pos[1];
			pos[2] = fpsys->trans.scale[2] * p->pos[2];
			Vec3dAdd( pos, pos, fpsys->trans.shift );

			// get the gradient for particle time
			for ( j = 0; j < fpsys->face.gradient_num; j++ )
			{
				if ( p->msec_left < fpsys->face.msec_start[j] &&
				     p->msec_left >= fpsys->face.msec_stop[j] )
				{
					fp_t	scale;
					fp_t		size;
					fp_t		intens;	// hack: use only red as intensity

					scale = 1.0 - (p->msec_left - fpsys->face.msec_stop[j])/(fp_t)(fpsys->face.msec_start[j]-fpsys->face.msec_stop[j] );

					size = fpsys->face.size_start[j] + scale * ( fpsys->face.size_stop[j] - fpsys->face.size_start[j] );

					intens = fpsys->face.color_start[j][0] + scale * ( fpsys->face.color_stop[j][0] - fpsys->face.color_start[j][0] );
					
					glColor3f( intens, intens, intens );

					R_DrawTexImage2DWithRotation( pos, size, size, ((p->msec_left/2+ms_rfbegin/2))*((((size_t)(p))&16)?-1.0:1.0) );	
//					printf( "%d ", (((int)(p))&16)?-1:1 );
					COUNT_INCR( count_rf_num_fpsys_particle );
					break;
				}
			}
		}
	}
	
	glDepthMask(GL_TRUE);
}

/*
  =============================================================================
  halo stuff

  =============================================================================
*/

void R_TestHalo( void )
{
	vec3d_t		pos={16.0,16.0,16.0};
	vec3d_t		tmp;
	vec4d_t		v;

	Vec3dScale( tmp, 1.0/16.0, pos );
	CalcVertex( v, tmp );	
	if ( v[3] <= 0.0 )
		return;

	v[3] = 1.0/v[3];

	{
		int		x, y;
		fp_t		xx, yy;

		x = ( (v[0]*v[3])*320.0+320.0 );
		y = ( (v[1]*v[3])*240.0+240.0 );
		
		xx = v[0]*v[3];
		yy = v[1]*v[3];

		if ( x>=0 && x<640 && y>=0 && y<480 )
		{
			fp_t		z1, z2;
			
			// read current z1
			glReadBuffer( GL_BACK );
			glReadPixels( x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z1 );
			
			// write halo z
			glBegin( GL_POINTS );
			glVertex3f( xx, yy, v[3] );
			glEnd();
			
			// read current z2
			glReadBuffer( GL_BACK );
			glReadPixels( x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z2 );			
			

			if ( z1 != z2 )
			{
				printf( "halo active\n" );
				//	R_DrawSprite( G_ResourceSearch( g_rs, "gltex.fx.halo7" ), pos, 4*1.0, 4*1.33 );
			}
			else
			{
				printf( "no halo\n" );
			}
//			printf( "(%d, %d, %.6f, %.6f)\n", x, y, z1, z2 );
		}
	}	
}

void R_DrawHaloSprite( g_resource_t *gltex, vec3d_t pos, fp_t width, fp_t height );

void R_RenderHalos( void )
{
	int		i;
	ri_halo_t	*halo;

	g_resource_t	*halo_res;

	halo_res = G_ResourceSearch( g_rs, "gltex.fx.halo1" );
	
	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glEnable( GL_TEXTURE_2D );
	glEnable( GL_BLEND );                                          
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );	

	//
	// check visibility of all halos
	//
	glReadBuffer( GL_BACK );

	for ( i = 0; i < ri_halo_num; i++ )
	{
		vec3d_t		tmp;
		vec4d_t		v;

		fp_t		xx, yy;
		int		x, y;
		
		fp_t		z1, z2;

		halo = ri_halo_infos[i];
		
		Vec3dScale( tmp, 1.0/16.0, halo->origin );
		CalcVertex( v, tmp );	

		if ( v[3] <= 0.0 )
		{
			halo->was_visible = false;
			continue;
		}

		v[3] = 1.0/v[3];
		
		// fixme: screen size
		xx = v[0]*v[3];
		yy = v[1]*v[3];
		x = ( xx*320.0+320.0 );
		y = ( yy*240.0+240.0 );

		if ( x<0 || x>=640 || y<0 || y>=480 )
		{
			halo->was_visible = false;
			continue;
		}
		
		// read current z1 at halo origin
		glReadPixels( x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z1 );

		// draw halo origin
		glBegin( GL_POINTS );
		glVertex3f( xx, yy, v[3] );
		glEnd();

		// read current z2 at halo origin
		glReadPixels( x, y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &z2 );	

//		printf( "z1 %f, z2 %f\n", z1, z2 );

		if ( z1 != z2 )
		{
//			printf( "halo active\n" );
			halo->was_visible = true;
		}
		else
		{
//			printf( "no halo\n" );
			halo->was_visible = false;
		}
	}

//	glDepthMask(GL_TRUE);
	glDisable(GL_DEPTH_TEST);
//	glDepthFunc( GL_GEQUAL );

	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	glEnable( GL_TEXTURE_2D );
	glEnable ( GL_BLEND );                                          
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );	

	for ( i = 0; i < ri_halo_num; i++ )
	{
		halo = ri_halo_infos[i];

		if ( halo->was_visible )
		{
			res_gltex_cache_t	*cache;
			int		texobj;
			
			cache = ( res_gltex_cache_t * ) halo_res->res_cache;
			texobj = cache->texobj;			
			glBindTexture( GL_TEXTURE_2D, texobj );
			
			glColor4ubv( halo->rgba );
			R_DrawHaloSprite( halo_res, halo->origin, halo->width, halo->height );	
		}
	}
}

/*
  =============================================================================
  shape pipeline

  =============================================================================
*/

void R_InvalidateAllUnitex( void )
{
	int		i;
	shape_t		*shp;

	for ( i = 0; i < r_sfaceshapenum; i++ )	
	{
		u_list_iter_t		iter;
		layer_t			*layer;

		shp = r_sfaceshapes[i];		

		U_ListIterInit( &iter, shp->layer_list );
		for ( ; ( layer = (layer_t *)U_ListIterNext( &iter ) ) ; )
		{
//			if ( !UniTex_IsReal_subimage( layer->real_source ) )
//			{
//				// unitex is not a subimage
//				continue;
//			}

			UniTex_TryToInvalidate( layer->real_source );
			Layer_FreeTexCoords( layer );
		}		
		
//		shp->valid = 0;
		shp->flags &= ~SHAPE_FLAGS_IS_VALID;
		
	}

	for ( i = 0; i < r_cfaceshapenum; i++ )	
	{
		u_list_iter_t		iter;
		layer_t			*layer;

		shp = r_cfaceshapes[i];

		U_ListIterInit( &iter, shp->layer_list );
		for ( ; ( layer = (layer_t *)U_ListIterNext( &iter ) ) ; )
		{
//			if ( !UniTex_IsReal_subimage( layer->real_source ) )
//			{
//				// unitex is not a subimage
//				continue;
//			}

			UniTex_TryToInvalidate( layer->real_source );
			Layer_FreeTexCoords( layer );
		}		

//		shp->valid = 0;
		shp->flags &= ~SHAPE_FLAGS_IS_VALID;

	}

	for ( i = 0; i < r_csurfshapenum; i++ )	
	{
		u_list_iter_t		iter;
		layer_t			*layer;

		shp = r_csurfshapes[i];

		U_ListIterInit( &iter, shp->layer_list );
		for ( ; ( layer = (layer_t *)U_ListIterNext( &iter ) ) ; )
		{
//			if ( !UniTex_IsReal_subimage( layer->real_source ) )
//			{
//				// unitex is not a subimage
//				continue;
//			}

			UniTex_TryToInvalidate( layer->real_source );
			Layer_FreeTexCoords( layer );
		}		

//		shp->valid = 0;
		shp->flags &= ~SHAPE_FLAGS_IS_VALID;
	}

	for ( i = 0; i < r_meshtileshapenum; i++ )	
	{
		u_list_iter_t		iter;
		layer_t			*layer;

		shp = r_meshtileshapes[i];

		U_ListIterInit( &iter, shp->layer_list );
		for ( ; ( layer = (layer_t *)U_ListIterNext( &iter ) ) ; )
		{
			UniTex_TryToInvalidate( layer->real_source );
			Layer_FreeTexCoords( layer );
		}		

		shp->flags &= ~SHAPE_FLAGS_IS_VALID;
	}
}

void R_ValidateShapeMesh( void )
{
	int		i;
	shape_t		*shp;
	int		tess_num;		// number of tesselated shapes

	tess_num = 0;
	
	for ( i = 0; i < r_visible_shape_num; i++ )
	{
		shp = r_visible_shapes[i];

		if ( /*shp->valid*/ shp->flags & SHAPE_FLAGS_IS_VALID )
			continue;
		
		if ( !shp->mesh )
		{
			u_list_iter_t		iter;
			layer_t			*layer;				
		       	
			// the shape is not tesselated
			Tess_CreateMesh( shp );								       	      		

			// clean up layers too
			U_ListIterInit( &iter, shp->layer_list );
			for ( ; ( layer = (layer_t *)U_ListIterNext( &iter ) ); )
			{
				Layer_FreeTexCoords( layer );	
			}

			tess_num++;
		}		
	}

	if ( tess_num > 0 )
		printf( "created shape tesselation meshes: %d\n", tess_num );
}

void R_ValidateShapeUniTex( void )
{
	int		i;
	shape_t		*shp;
	int		try_num;
	int		unitex_num;

	unitex_num = 0;
	for ( try_num = 0; try_num < 2; try_num++ )
	{
		for ( i = 0; i < r_visible_shape_num; i++ )
		{
			u_list_iter_t		iter;
			layer_t			*layer;				
			
			shp = r_visible_shapes[i];
			
			if ( /*shp->valid*/ shp->flags & SHAPE_FLAGS_IS_VALID )
				continue;
			
			U_ListIterInit( &iter, shp->layer_list );
			for ( ; ( layer = (layer_t *)U_ListIterNext( &iter ) ); )
			{
				if ( UniTex_IsValid( layer->real_source ) )
					continue;

				unitex_num++;
				
				UniTex_TryToValidate( layer->real_source );
				if ( !UniTex_IsValid( layer->real_source ) )
				{
					printf( "invalidate imgcache\n" );
					
					IC_MgrCleanUp( a_map->imgcache );						
					R_InvalidateAllUnitex();
					goto try_again;
				}			
			}
		}
		break;

	try_again:		
		{}
	}	

	if ( try_num == 2 )
	{
		__error( "second try failed\n" );
	}

	if ( unitex_num > 0 )
		printf( "validated unitex number: %d\n", unitex_num );
}

void R_ValidateShapeTexCoords( void )
{
	int		i;
	shape_t		*shp;
	int		texcoord_num;

	texcoord_num = 0;
	for ( i = 0; i < r_visible_shape_num; i++ )
	{
		u_list_iter_t		iter;
		layer_t			*layer;				

		shp = r_visible_shapes[i];
		
		if ( /*shp->valid*/ shp->flags & SHAPE_FLAGS_IS_VALID )
			continue;
		       	
		U_ListIterInit( &iter, shp->layer_list );
		for ( ; ( layer = (layer_t *)U_ListIterNext( &iter ) ); )
		{
			if ( !layer->tc )
			{
				Layer_CreateTexCoords( layer, shp->mesh );
				texcoord_num++;
			}
		}
	}

	if ( texcoord_num > 0 )
		printf( "created shape texcoord meshes: %d\n", texcoord_num );
}


void R_ShapesIntoShaders( void )
{
	int		i;
	shape_t		*shp;
	int		shape_num;	// number of shapes, that get a new shader_refs
	int		shader_num;	// number of create shaders

	shader_num = 0;
	shape_num = 0;

	Shader_MgrFrameBegin( a_map->shader_mgr );

	for ( i = 0; i < r_visible_shape_num; i++ )
	{
		shp = r_visible_shapes[i];

		if ( !shp->shd )
		{
			// no layer_array
			layers_characteristic_t		*ca;
			shader_t		*shd;

			shape_num++;
			LayerList_BuildArrays( shp->layer_list, &ca, &shp->la );

			// try to find a shader for this layer
			shd = Shader_MgrSearchShader( a_map->shader_mgr, ca );
			if ( shd )
			{
				// ok, there is already a shader for this kind of layer characteristic
				FREE( ca );
			}
			else
			{
				// create a new shader for this layer characteristic
				shader_num++;
				shd = Shader_New( ca );		// shader is responsible for 'ca'
				Shader_MgrRegisterShader( a_map->shader_mgr, shd );
			}

			shp->shd = shd;
//			shp->valid = 1;
			shp->flags |= SHAPE_FLAGS_IS_VALID;
		}

		Shader_MgrFrameRegisterShape( a_map->shader_mgr, shp );
	}

	Shader_MgrFrameEnd( a_map->shader_mgr );

	if ( shader_num > 0 )
		printf( "created shaders: %d\n", shader_num );
	if ( shape_num > 0 )
		printf( "shapes got a shader ref: %d\n", shape_num );
}

void R_RenderShaders( void )
{
	int		i;
	shape_t		*shp;

	vec3d_t		*m_verts;
	int		m_vertexnum;

	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );   
	glEnable( GL_BLEND );
	//
	// setup vertex arrays
	//

//	R_BE_ResetCounters();

	R_BE_SetVertexOfs( 0 );
	for ( i = 0; i < r_visible_shape_num; i++ )
	{
		shp = r_visible_shapes[i];
		if ( !shp->mesh )
			continue;

		shp->vofs = R_BE_GetVertexOfs();
		Mesh_GetVec3dBase( shp->mesh, 0, &m_vertexnum, &m_verts );
		R_BE_AppendVertexArray( m_vertexnum, m_verts );
	}

	R_BE_LockVertexArray();
		
	Shader_MgrRender( a_map->shader_mgr );

	R_BE_UnlockVertexArray();

//	R_BE_DumpCounters();
}



void R_SetupShapeLod_old()
{
	int		i;

	if( !r_lod )
		r_lod = SHP_GetVar( "r_lod" );
			
	if ( r_lod->fvalue != r_curlod )
	{
		r_curlod = r_lod->fvalue;
		for ( i = 0; i < r_cfaceshapenum; i++ )
		{
			r_cfaceshapes[i]->u_lod = ( int )( r_curlod * 20.0 + 3.0 );
			if ( r_cfaceshapes[i]->mesh )
			{
				Mesh_Free( r_cfaceshapes[i]->mesh );
				r_cfaceshapes[i]->mesh = NULL;

//				r_cfaceshapes[i]->valid = 0;
				r_cfaceshapes[i]->flags &= ~SHAPE_FLAGS_IS_VALID;

				// hack:  layers are deleted by R_ValidateShapeMesh
			}
		}

		for ( i = 0; i < r_csurfshapenum; i++ )
		{
			if ( r_csurfshapes[i]->u_lod >= 3 )
			{
				r_csurfshapes[i]->u_lod = ( int )( r_curlod * 20.0 + 3.0 );	
			}
			if ( r_csurfshapes[i]->v_lod >= 3 )
			{
				r_csurfshapes[i]->v_lod = ( int )( r_curlod * 20.0 + 3.0 );
			}
			if ( r_csurfshapes[i]->mesh )
			{
				Mesh_Free( r_csurfshapes[i]->mesh );
				r_csurfshapes[i]->mesh = NULL;

//				r_csurfshapes[i]->valid = 0;
				r_csurfshapes[i]->flags &= ~SHAPE_FLAGS_IS_VALID;

				// hack:  layers are deleted by R_ValidateShapeMesh
			}
		}		
	}	
}


/*
  =============================================================================
  shape pipeline

  REVISION
  =============================================================================
*/

void R_InvalidateShape( shape_t *shp )
{
//	shp->flags &= ~SHAPE_FLAGS_IS_VALID;
//	shp->flags &= ~SHAPE_FLAGS_LAYERS_ARE_VALID;
//	shp->flags &= ~SHAPE_FLAGS_SHADER_INFO_IS_VALID;	
	shp->flags = 0;
}

void R_SetupShapeLod()
{
	int		i;

	if( !r_lod )
		r_lod = SHP_GetVar( "r_lod" );
			
	if ( r_lod->fvalue != r_curlod )
	{
		printf( "########################################chk lod %f\n", r_lod->fvalue );

		r_curlod = r_lod->fvalue;
		for ( i = 0; i < r_cfaceshapenum; i++ )
		{
			R_InvalidateShape( r_cfaceshapes[i] );
			r_cfaceshapes[i]->u_lod = ( int )( r_curlod * 20.0 + 3.0 );
		}

		for ( i = 0; i < r_csurfshapenum; i++ )
		{
			R_InvalidateShape( r_csurfshapes[i] );
			if ( r_csurfshapes[i]->u_lod >= 3 )
			{
				r_csurfshapes[i]->u_lod = ( int )( r_curlod * 20.0 + 3.0 );	
			}
			if ( r_csurfshapes[i]->v_lod >= 3 )
			{
				r_csurfshapes[i]->v_lod = ( int )( r_curlod * 20.0 + 3.0 );
			}
		}		
	}	
}


void R_InvalidateAllShapes( void )
{
	int		i;
	shape_t		*shp;

	for ( i = 0; i < r_sfaceshapenum; i++ )	
	{
		shp = r_sfaceshapes[i];		

		// hack: keep the mesh for speed up
		shp->flags &= ~SHAPE_FLAGS_IS_VALID;
		shp->flags &= ~SHAPE_FLAGS_LAYERS_ARE_VALID;
		shp->flags &= ~SHAPE_FLAGS_SHADER_INFO_IS_VALID;		
	}

	for ( i = 0; i < r_cfaceshapenum; i++ )	
	{
		shp = r_cfaceshapes[i];
		shp->flags &= ~SHAPE_FLAGS_IS_VALID;
		shp->flags &= ~SHAPE_FLAGS_LAYERS_ARE_VALID;
		shp->flags &= ~SHAPE_FLAGS_SHADER_INFO_IS_VALID;
	}

	for ( i = 0; i < r_csurfshapenum; i++ )	
	{
		shp = r_csurfshapes[i];
		shp->flags &= ~SHAPE_FLAGS_IS_VALID;
		shp->flags &= ~SHAPE_FLAGS_LAYERS_ARE_VALID;
		shp->flags &= ~SHAPE_FLAGS_SHADER_INFO_IS_VALID;
	}
}

#if 0
bool_t Shape_TryValidation( shape_t *shp )
{
	if ( shp->flags & SHAPE_FLAGS_IS_VALID )
	{
		return;
	}

	//
	// shape is not valid
	//

	// check all layers
	U_ListIterInit( &iter, shp->layer_list );
	for ( ; ( layer= U_ListIterNext( &iter ) ) ; )
	{
//		res = Layer_TryValidation(
	}
}
#endif

void R_ValidateVisibleShapes( void )
{
	int		i;
	shape_t		*shp;
	shape_ctrl_job_t	*job;
	shapeCtrlJobRes		res;

	int		try_num;
	
	try_num = 0;

try_again:

	for ( i = 0; i < r_visible_shape_num; i++ )
	{
		shp = r_visible_shapes[i];
		__chkptr( shp );

		if ( (shp->flags & SHAPE_FLAGS_IS_VALID) && (shp->flags & SHAPE_FLAGS_LAYERS_NEED_CHECK ) )
		{		
			u_list_iter_t		iter;
			layer_t			*layer;
			// fixme:		       

			U_ListIterInit( &iter, shp->layer_list );
			for ( ; ( layer = (layer_t *)U_ListIterNext( &iter ) ) ; )
			{
				// 
				// need texcoord gen
				//

				if ( !layer->tc && shp->mesh )
				{
					Layer_CreateTexCoords( layer, shp->mesh );
				}

				//
				// need lightmap recombine
				//

				if ( !( layer->flags & LAYER_FLAGS_VALID_LIGHTMAP_COMBINE ) )
				{
					ic_subimage_t	*sub;

//					printf( "need_recombine\n" );

					layer->flags |= LAYER_FLAGS_VALID_LIGHTMAP_COMBINE;
					
					if ( layer->lightmap_head )
					{
						
						sub = UniTex_IsReal_subimage( layer->real_source );
						if ( sub )
						{
							char *pixels;
							
							pixels = LightmapDB_GetPixels( a_map->lightmap_db, layer->lightmap_head, sub->width*sub->height );
							memcpy( sub->pixels, pixels, sub->width*sub->height*4 );
//						FREE( pixels );
							
							ICU_UploadFrameAddSubImage( &r_upload_frame, sub );
						}
					}
				}
			}
			shp->flags &= ~SHAPE_FLAGS_LAYERS_NEED_CHECK;
		}

		if ( ! (shp->flags & SHAPE_FLAGS_IS_VALID ) )
		{
			job = ShapeCtrlJob_Create_ValidateShape( shp );
			res = job->ftbl->RunJob( job );
			job->ftbl->DestroyJob( job );
			
			if ( res != ShapeCtrlJobRes_success )
			{
				// surly the imgcache is full ...clean up ...and try once again
				try_num++;
				
				if ( try_num == 2 )
				{
					__error( "second try failed\n" );
				}
				printf( "invalidate imgcache\n" ); 

				IC_MgrCleanUp( a_map->imgcache );
				R_InvalidateAllShapes();

				goto try_again;
			}
		}
	}
}

void R_InsertVisibleShapesIntoShaders( void )
{
	int		i;
	shape_t		*shp;

	Shader_MgrFrameBegin( a_map->shader_mgr );

	for ( i = 0; i < r_visible_shape_num; i++ )
	{
		shp = r_visible_shapes[i];

		__chkptr( shp->shd );
		Shader_MgrFrameRegisterShape( a_map->shader_mgr, shp );		
	}
	
	Shader_MgrFrameEnd( a_map->shader_mgr );
}


/*
  =============================================================================
  renderer public interface

  =============================================================================
*/


int	r_argc;
char	**r_argv;

fp_t		r_speed = 0;
fp_t	r_yaw = 0.0;
fp_t	r_roll = 0.0;
fp_t	r_pitch = 0.0;

vec3d_t	r_delta;

bool_t	debug_lightpage = false;
int	debug_lightpagenum = 0;
bool_t	debug_time_test = false;
fp_t		r_frame_msec;

bool_t	update_smoke_pos = false;

int	screen_dump_num = 0;
bool_t	do_screen_dump = false;

int	debug_lod = 7;

bool_t	do_draw_sky = true;

void R_SetLocalLightInfos( int info_num, ri_local_light_t *infos[] )
{
	ri_local_light_num = info_num;
	ri_local_lights = infos;
}

void R_SetSpriteInfos( int info_num, ri_sprite_t *infos[] )
{
	ri_sprite_num = info_num;
	ri_sprite_infos = infos;
}

void R_SetPsysInfos( int info_num, ri_psys_t *infos[] )
{
	ri_psys_num = info_num;
	ri_psys_infos = infos;
}

void R_SetHaloInfos( int info_num, ri_halo_t *infos[] )
{
	ri_halo_num = info_num;
	ri_halo_infos = infos;
}

/*
  ==============================
  R_SpawnShapeCtrlJob_LightmapStateWatch

  ==============================
*/
void R_SpawnShapeCtrlJob_LightmapStateWatch( shape_t *shp )
{
	shape_ctrl_job_t	*job;

	job = ShapeCtrlJob_Create_LightmapStateWatch( shp );

	//
	// keep the time continuity, new jobs are added at the end of the job list
	//

	U_ListInsertAtTail( r_job_list, job );
}

/*
  ==============================
  R_SpawnShapeCtrlJob_AmbientColor

  ==============================
*/
void R_SpawnShapeCtrlJob_AmbientColor( shape_t *shp, wf_shape_info_t *info )
{
	shape_ctrl_job_t	*job;

//	__warning( "GOT IT: #%u\n", info->shape_id );

	job = ShapeCtrlJob_Create_AmbientColor( shp, info );
	U_ListInsertAtTail( r_job_list, job );
}

 
/*
  ==============================
  R_RegisterShapeCtrlJob

  ==============================
*/
void R_RegisterShapeCtrlJob( shape_ctrl_job_t *job )
{
	U_ListInsertAtTail( r_job_list, job );
}

void R_HandleShapeControlJobs( void )
{
	u_list_iter_t		iter;
	shape_ctrl_job_t	*job;

	COUNT_RESET( count_rf_num_run_job );
	COUNT_RESET( count_rf_num_destroy_job );       
	COUNT_RESET( count_rf_num_vis_job );
	
	__chkptr( r_job_list );

	U_ListIterInit( &iter, r_job_list );
	for ( ; ( job = (shape_ctrl_job_t *)U_ListIterNext( &iter ) ) ; )
	{
		if ( job->state == ShapeCtrlJobState_run )
		{
			job->ftbl->RunJob( job );
			COUNT_INCR( count_rf_num_run_job );
		}
		if ( job->state == ShapeCtrlJobState_destroy )
		{
			U_ListIterRemoveGoPrev( &iter );
			job->ftbl->DestroyJob( job );
			COUNT_INCR( count_rf_num_destroy_job );
		}
	}

//	__named_message( " %d run, %d terminated\n", num_run, num_destroy );
}

void R_CleanUpAllShapeControlJobs( void )
{
	u_list_iter_t		iter;
	shape_ctrl_job_t	*job;

	COUNT_RESET( count_rf_num_run_job );
	COUNT_RESET( count_rf_num_destroy_job );       

	__chkptr( r_job_list );

	U_ListIterInit( &iter, r_job_list );
	for ( ; ( job = (shape_ctrl_job_t *)U_ListIterNext( &iter ) ) ; )
	{
		U_ListIterRemoveGoPrev( &iter );
		job->ftbl->DestroyJob( job );
		COUNT_INCR( count_rf_num_destroy_job );
	}	

	__named_message( " %d jobs destroyed\n", count_rf_num_destroy_job );
}
void R_InitGL_rendering();

void R_Prepare( void )
{
	R_InitGL_rendering();
	glColor4f( 1.0, 1.0, 1.0, 1.0 );

	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

 	glDepthMask(GL_TRUE);	
	glDepthFunc( GL_GEQUAL );
	glDepthRange( 0.0, 1.0 );	
	glEnable(GL_DEPTH_TEST);

	glDisable( GL_BLEND );	
       	glEnable( GL_TEXTURE_2D );

	ri_local_light_num = 0;
	ri_sprite_num = 0;
	ri_psys_num = 0;
}

void R_RenderView( void )
{
	//
	// get visible shapes from pvs
	//

	// stored in r_visible_shapes
	r_visible_shape_num = 0;
	PVS_UpdateCache( r_origin );

	R_SetupShapeLod();

	ICU_BeginSubImageUploadFrame( &r_upload_frame );

	R_HandleShapeControlJobs();
	R_ValidateVisibleShapes();
	R_InsertVisibleShapesIntoShaders();

	// upload all subimage changed 
	ICU_EndSubImageUploadFrame( &r_upload_frame );

	R_BeginDlightFrame_sw();

	// render scene 
	glClearDepth( 0.0 );
	glColor4f( 1.0, 1.0, 1.0, 1.0 );
//	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
//	glClear(GL_DEPTH_BUFFER_BIT);

	R_RenderSkyBox();

//	R_GetDepthBufferAccuracy();

	R_RenderShaders();

	R_RenderSprites();
        R_RenderFPsyss();

	R_RenderHalos();

	R_EndDlightFrame_sw();

//	R_TestHalo();

	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );

	glFlush();

	if ( do_screen_dump )
	{
		char		tt[256];

		sprintf( tt, "dump%d.tga", screen_dump_num++ );
		R_ScreenDump( tt );
		do_screen_dump = false;
	}


        glDisable(GL_CULL_FACE);                                                
        glEnable( GL_TEXTURE_2D );                                              
        glDisable(GL_BLEND);                                                    
//        glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
        glColor3f( 1.0, 1.0, 1.0 );                                             
	glDisable( GL_FOG );     


	r_frame_count++;
}

void R_ClearView()
{
	glClearDepth( 0.0 );
	glClearColor( 0.3, 0.3, 0.3, 0.0);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glFlush();
}

matrix3_t	sky_matrix;

void R_SetView( vec3d_t origin, fp_t latitude, fp_t longitude )
{

//	printf( "view: %f %f %f %f %f\n", origin[0], origin[1], origin[2], latitude, longitude );

	Vec3dCopy( r_origin, origin );

	Matrix3SetupRotate( r_matrix, 0*D2R, latitude*D2R, longitude*D2R );
	Matrix3Transpose( r_invmatrix, r_matrix );
	Matrix3SetupRotate( sky_matrix, 0*D2R, latitude*D2R, (longitude+180)*D2R );
	
	memset( &r_frustum, 0, sizeof( frustum_t ));

	R_InitBaseFrustum( &r_frustum, 64.0, 64.0, 32.0 );
	R_FrustumRotate( &r_frustum, r_invmatrix );
	R_FrustumTranslate( &r_frustum, r_origin );
	R_FrustumCalcPlanes( &r_frustum );
}


