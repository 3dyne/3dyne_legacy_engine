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



// unitex.c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "lib_math.h"
#include "shock.h"
#include "g_shared.h"
#include "ca_service.h"

#include "unitex_defs.h"
#include "imgcache.h"

// hack:
#include "g_defs.h"
extern g_map_t	*a_map;

/*
  ==============================
  UniTex_NewFrom_gltexres

  ==============================
*/
unitex_t * UniTex_NewFrom_gltexres( g_resource_t *gltex )
{
	unitex_t	*uni;	

	uni = NEWTYPE( unitex_t );
	uni->flags = UNITEX_FLAGS_SOURCE_GLTEXRES;
	
	uni->real_source = gltex;

	return uni;
}

/*
  ==============================
  UniTex_NewFrom_subimage

  ==============================
*/
unitex_t * UniTex_NewFrom_subimage( ic_subimage_t *sub )
{
	unitex_t	*uni;	

	uni = NEWTYPE( unitex_t );
	uni->flags = UNITEX_FLAGS_SOURCE_SUBIMAGE;
	
	uni->real_source = sub;

	return uni;	
}

/*
  ==============================
  UniTex_IsReal_gltexres

  ==============================
*/
g_resource_t * UniTex_IsReal_gltexres( unitex_t *uni )
{
	if ( uni->flags & UNITEX_FLAGS_SOURCE_GLTEXRES )
	{
		return (g_resource_t *)uni->real_source;
	}
	return NULL;
}

/*
  ==============================
  UniTex_IsReal_subimage

  ==============================
*/
ic_subimage_t * UniTex_IsReal_subimage( unitex_t *uni )
{
	if ( uni->flags & UNITEX_FLAGS_SOURCE_SUBIMAGE )
	{
		return (ic_subimage_t*)uni->real_source;
	}
	return NULL;	
}


/*
  ==============================
  UniTex_IsValid

  ==============================
*/
bool_t UniTex_IsValid( unitex_t *uni )
{

	if ( uni->flags & UNITEX_FLAGS_SOURCE_GLTEXRES )
	{
		if ( uni->flags & UNITEX_FLAGS_TEXOBJ_CACHED )
			return true;
	}
	else if ( uni->flags & UNITEX_FLAGS_SOURCE_SUBIMAGE )
	{
		if ( uni->flags & UNITEX_FLAGS_TEXOBJ_CACHED )
			return true;
	}

	return false;
}


/*
  ==============================
  UniTex_TryToValidate

  ==============================
*/
void UniTex_TryToValidate( unitex_t *uni )
{
	if ( uni->flags & UNITEX_FLAGS_SOURCE_GLTEXRES )
	{
		g_resource_t		*gltex;
		res_gltex_cache_t	*cache;
		
		gltex = ( g_resource_t * ) uni->real_source;
		cache = ( res_gltex_cache_t * ) gltex->res_cache;

		if ( !cache )
			__error( "fixme: no gltex resource cache in from unitex till now\n" );

		uni->cached_texobj = cache->texobj;
		uni->flags |= UNITEX_FLAGS_TEXOBJ_CACHED;		
	}
	else if ( uni->flags & UNITEX_FLAGS_SOURCE_SUBIMAGE ) 
	{
		ic_subimage_t		*sub;

		sub = ( ic_subimage_t * ) uni->real_source;

		if ( !IC_SubImageIsCached( sub ) )
		{
			if ( IC_MgrCacheSubImage( a_map->imgcache, sub ) == ICMgrCacheRes_ok )
			{
				uni->cached_texobj = IC_SubImageGetImageGLtexobj( sub );
				uni->flags |= UNITEX_FLAGS_TEXOBJ_CACHED;		
			}
		}
	}
	else
	{
		__error( "invalid source in unitex\n");	
	}
}

/*
  ==============================
  UniTex_TryToInvalidate

  ==============================
*/
void UniTex_TryToInvalidate( unitex_t *uni )
{
	if ( uni->flags & UNITEX_FLAGS_SOURCE_GLTEXRES )
	{
		// fixme: no invalidation by now
		uni->flags &= ~UNITEX_FLAGS_TEXOBJ_CACHED;
	}
	else if ( uni->flags & UNITEX_FLAGS_SOURCE_SUBIMAGE ) 
	{
		ic_subimage_t		*sub;

		sub = ( ic_subimage_t * ) uni->real_source;		
		IC_SubImageInvalidate( sub );
		
		uni->flags &= ~UNITEX_FLAGS_TEXOBJ_CACHED;	
	}	
	else
	{
		__error( "invalid source in unitex\n");
	}
}

/*
  ==============================
  UniTex_GetGLtexobj

  ==============================
*/
int UniTex_GetGLtexobj( unitex_t *uni )
{
	if ( ! (uni->flags & UNITEX_FLAGS_TEXOBJ_CACHED ) )
	{
		__error( "unitex ain't got a valid texobj\n" );
	}

	return uni->cached_texobj;
}

/*
  ==============================
  UniTex_RealTexCoord_01

  ==============================
*/
void UniTex_RealTexCoord_01( unitex_t *uni, vec2d_t out, vec2d_t in )
{
	if ( ! (uni->flags & UNITEX_FLAGS_TEXOBJ_CACHED ) )
	{
		__error( "unitex ain't got a valid texobj\n" );
	}	

	if ( uni->flags & UNITEX_FLAGS_SOURCE_GLTEXRES )
	{	     
		Vec2dCopy( out, in );
	}
	else if ( uni->flags & UNITEX_FLAGS_SOURCE_SUBIMAGE )
	{
		ic_subimage_t	*sub;
		vec2d_t		sub_size, img_size, sub_shift;
		
		sub = ( ic_subimage_t *) (uni->real_source);
		
#if 0
		if ( !IC_SubImageIsCached( sub ) )
		{
			IC_MgrCacheSubImage( a_map->imgcache, sub );
		}
#endif		
		IC_SubImageGetSubImageOffset( sub, sub_shift );
		IC_SubImageGetSubImageSize( sub, sub_size );
		IC_SubImageGetImageSize( sub, img_size );

		out[0] = sub_shift[0]/img_size[0] + (in[0]*sub_size[0])/img_size[0];
		out[1] = sub_shift[1]/img_size[1] + (in[1]*sub_size[1])/img_size[1];
	}
	else
	{
		__error( "invalid source in unitex\n");
	}
}

/*
  ==============================
  UniTex_RealTexCoord_0N

  ==============================
*/
void UniTex_RealTexCoord_0N( unitex_t *uni, vec2d_t out, vec2d_t in )
{
	if ( ! (uni->flags & UNITEX_FLAGS_TEXOBJ_CACHED ) )
	{
		__error( "unitex ain't got a valid texobj\n" );
	}

	if ( uni->flags & UNITEX_FLAGS_SOURCE_GLTEXRES )
	{
		g_resource_t		*glres;
		vec2d_t			size;

		glres = (g_resource_t *)uni->real_source;

#if 0
		if ( !(glres->state & G_RESOURCE_STATE_CACHED ) )
		{
			out[0]=out[1]=0.0;
			return;			
		}
#endif

		size[0] = (fp_t)(((res_gltex_cache_t *)glres->res_cache)->width);
		size[1] = (fp_t)(((res_gltex_cache_t *)glres->res_cache)->height);

		out[0] = in[0]/size[0];
		out[1] = in[1]/size[1];
	}
	else if ( uni->flags & UNITEX_FLAGS_SOURCE_SUBIMAGE )
	{
		ic_subimage_t	*sub;
		vec2d_t		img_size, sub_shift;
		
		sub = ( ic_subimage_t *) (uni->real_source);

#if 0		
		if ( !IC_SubImageIsCached( sub ) )
		{
			IC_MgrCacheSubImage( a_map->imgcache, sub );
		}
#endif	
	
		IC_SubImageGetSubImageOffset( sub, sub_shift );
		IC_SubImageGetImageSize( sub, img_size );

		out[0] = sub_shift[0]/img_size[0] + in[0]/img_size[0];
		out[1] = sub_shift[1]/img_size[1] + in[1]/img_size[1];
	
	}
	else
	{
		__error( "invalid source in unitex\n");
	}
	
}
