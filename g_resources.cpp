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



// g_resources.c

#include "interfaces.h"
#include "defs.h"
#include "g_shared.h"


const void * GetKey_resource( const void *obj )
{
	g_resource_t	*r;

	r = (g_resource_t *) obj;
	
	return (void*)(&r->text[(unsigned int)r->name_ofs]);
}

const void * GetKey_res_type( const void *restype )
{
	return (void*)(((g_res_type_t*)(restype))->name);
}

int KeyCompare_c_string( const void *res1_key, const void *res2_key )
{
	return strcmp( (char*)(res1_key), (char*)(res2_key) );
}

/*
  ==============================
  G_NewResources

  ==============================
*/
g_resources_t * G_NewResources( void )
{
	g_resources_t		*res;

	res = NEWTYPE( g_resources_t );

	U_InitMap( res->res_map = NEWTYPE( u_map_t ), map_default, KeyCompare_c_string, GetKey_resource );
	U_InitMap( res->type_map = NEWTYPE( u_map_t ), map_default, KeyCompare_c_string, GetKey_res_type );

	return res;
}

/*
  ==============================
  G_FreeResources

  ==============================
*/
void G_FreeResources( g_resources_t *rp )
{
	printf( "FIX ME: G_FreeResources - not yet\n" );
}


/*
  ==============================
  G_DumpResources

  ==============================
*/
void G_ResourceTypeDump( g_res_type_t *rt )
{
	if ( !rt )
		return;
	printf( " type '%s' registered, funcs : ", rt->name );
	if ( rt->register_func )
		printf( "[register] ");
	if ( rt->unregister_func )
		printf( "[unregister] ");
	if ( rt->cache_func )
		printf( "[cache] ");
	if ( rt->uncache_func )
		printf( "[uncache] ");

	printf( "\n" );
}

void G_ResourceTypeDump_void( void *rt )
{
	G_ResourceTypeDump( (g_res_type_t *)(rt) );
}

static int g_res_count;
void G_ResourceCount( void *obj )
{
	g_res_count++;
}

void G_ResourcesDump( g_resources_t *rp )
{
	printf( "G_DumpResources:\n" );

	printf( " registerd types:\n" );
	U_MapForEach( rp->type_map, G_ResourceTypeDump_void );

	printf( " registerd resources:\n" );
	g_res_count = 0;
	U_MapForEach( rp->res_map, G_ResourceCount );
	printf( "  %d\n", g_res_count );
	
	printf( " cached resources:\n" );
}

/*
  ==================================================
  resource stuff

  ==================================================
*/

/*
  ==============================
  G_NewResource

  ==============================
*/
g_resource_t * G_NewResource( char *res_name, char *res_type )
{
	g_resource_t	*r;
	size_t		size;
	int		name_size;
	int		type_size;

	name_size = strlen( res_name ) + 1; 
	type_size = strlen( res_type ) + 1;

	size = (size_t)(&(((g_resource_t*)0)->text[name_size+type_size]));
	r = (g_resource_t*)NEWBYTES( size );

	r->type_ofs = 0;
	r->name_ofs = type_size;
	
	strcpy( &r->text[r->type_ofs], res_type );
	strcpy( &r->text[r->name_ofs], res_name );
	r->state = G_RESOURCE_STATE_NONE;
	
	r->user_list = U_NewList();
       
	return r;
}


/*
  ==============================
  G_ResourceRegisterType

  ==============================
*/
void G_ResourceTypeRegister( g_resources_t *rp, const char *type_name,
			     g_resource_t * (*register_func)(hobj_t *resobj),
			     void (*unregister_func)(g_resource_t *res),
			     void (*cache_func)(g_resource_t *res),
			     void (*uncache_func)(g_resource_t *res) )
{
	g_res_type_t	*rt;

	if ( U_MapSearch( rp->type_map, type_name ) )
		return;		// type already registerd

	rt = NEWTYPE( g_res_type_t );
	
	if ( strlen( type_name ) > 60 )
		__error( "G_ResourceRegisterType: type name too long\n" );

	if ( !register_func )
		printf( "Warning: G_ResourceRegisterType, register_func is null\n" );
	if ( !unregister_func )
		printf( "Warning: G_ResourceRegisterType, unregister_func is null\n" );
	if ( !cache_func ) 
		printf( "Warning: G_ResourceRegisterType, cache_func is null\n" );
	if ( !uncache_func )
		printf( "Warning: G_ResourceRegisterType, uncache_func is null\n" );

	strcpy( rt->name, type_name );
	rt->register_func = register_func;
	rt->unregister_func = unregister_func;
	rt->cache_func = cache_func;
	rt->uncache_func = uncache_func;

	if ( !U_MapInsert( rp->type_map, rt ) )
		__error( "G_ResourceRegisterType: odd, type register failed\n" );

}



/*
  ==============================
  G_ResourceFromResObj

  ==============================
*/
void G_ResourceFromResObj( g_resources_t *res, hobj_t *cls )
{
	g_res_type_t	*rt;
	g_resource_t	*r;
	hpair_t		*name;

	if ( !cls || !res )
	{
		__named_message( "!cls || !res\n" );
		return;		
	}


//	printf( "res: %s ", cls->name );
	
	if ( strcmp( cls->type, "resource" ) )
	{
		printf( "Warning: G_ResourceFromClass, class is not of type 'resource', ignore\n" );
		return;
	}

	rt = (g_res_type_t*) U_MapSearch( res->type_map, cls->name );

	if ( !rt )
	{
		printf( "Warning: G_ResourceFromClass, type '%s' of resource is not registered, ignore\n", cls->name );
		return;
	}

	name = FindHPair( cls, "name" );
	if ( !name )
	{
		printf( "Warning: G_ResourceFromClass, missing key 'name', ignore\n" );
		return;
    	}
//	printf( "%s\n", name->value );
	
	if ( U_MapSearch( res->res_map, name->value ) )
	{
		printf( "Warning: G_ResourceFromClass, there is already a resource with the same name, ignore\n" );
		return;
	}

	if ( !rt->register_func )
	{
		printf( "Warning: G_ResourceFromClass, type '%s' has no register_func, ignore\n", cls->name );
		return;
	}

	r = rt->register_func( cls );

	if ( !r )
	{
		printf( "Warning: G_ResourceFromClass, register_func failed, ignore\n" );
		return;
	}

	if ( !U_MapInsert( res->res_map, r ) )
	{
		printf( "Warning: G_ResourceFromClass, failed\n" );
	}

}


/*
  ==============================
  G_ResourceFromClass

  ==============================
*/
void G_ResourceFromClass( g_resources_t *res, const char *name )
{
	tokenstream_t	*ts;
	hobj_t		*root;
	hobj_t		*resobj;
	hobj_search_iterator_t	iter;
	
	printf("G_ResourceFromClass: %s\n", name);

	ts = BeginTokenStream( name );
	if ( !ts )
		__error( "can't open resource class '%s'\n", name );

	root = ReadClass( ts );
	EndTokenStream( ts );

	InitClassSearchIterator( &iter, root, "resource" );
	for( ; ( resobj = ( SearchGetNextClass( &iter ) )) ; )
	{
		G_ResourceFromResObj( res, resobj );
	}

//	DeepDestroyClass( root );
	__named_message( "\n" );
}


/*
  ==============================
  G_ResourceCheck

  ==============================
*/
bool_t G_ResourceCheck( g_resources_t *res, char *name )
{
	g_resource_t	*r;

	r = (g_resource_t*) U_MapSearch( res->res_map, name );

	if ( !r )
	{
		return false;
	}

	return true;
}

/*
  ==============================
  G_ResourceSearch

  ==============================
*/
g_resource_t * G_ResourceSearch( g_resources_t *res, const char *name )
{
	g_resource_t	*r;
	g_res_type_t	*rt;

	r = (g_resource_t*)U_MapSearch( res->res_map, name );

	if ( !r )
	{
		__error( "can't find resource '%s'\n", name );
	}

	if ( r->state != G_RESOURCE_STATE_CACHED )
	{
		// have to be cached
		if ( r->state != G_RESOURCE_STATE_REGISTERED )
		{
			__error( "odd, resource '%s' exists, but is not registered\n", name );
		}
	
		// get resource type
		rt = (g_res_type_t*)U_MapSearch( res->type_map, &r->text[r->type_ofs] );
		if ( !rt )
		{
			__error( "odd, type of resource '%s' is not registered\n", name );
		}

		// cache for real
		rt->cache_func( r );

		if ( r->state != G_RESOURCE_STATE_CACHED )
			__error( "caching of resource '%s' failed\n", name );		
	}

	return r;
}

/*
  ==============================
  G_ResourceDump

  ==============================
*/
void G_ResourceDump( g_resource_t *r )
{
	u_list_iter_t	iter;
	g_res_user_t	*user;
	int		usernum;

	printf( "G_ResourceDump:\n" );
	printf( " res_name: %s\n", &r->text[r->name_ofs] );
	printf( " res_type: %s\n", &r->text[r->type_ofs] );
	printf( " state: %d ", r->state );
	if ( r->state == G_RESOURCE_STATE_REGISTERED )
		printf( "[registered]\n" );
	else if ( r->state == G_RESOURCE_STATE_CACHED )
		printf( "[cached]\n" );
	else
		printf( "[unknown]\n" );

	printf( " res_register: %p\n", r->res_register );
	printf( " res_cache: %p\n", r->res_cache );
	printf( " users:\n" );
	U_ListIterInit( &iter, r->user_list );
	usernum = 0;
	for ( ; ( user = (g_res_user_t*)U_ListIterNext( &iter ) ) ; usernum++ )
	{
		printf( "  [%s::%p]\n", user->user_name, user->user_callback_func );
	}

	if ( usernum != 0 )
	{
		printf( " ------------\n" );
		printf( " total users: %d\n", usernum );
	}
	else
	{
		printf( "  [no users]\n" );
	}
}

/*
  ==============================
  G_NewResourceUser

  ==============================
*/

g_res_user_t * G_NewResourceUser( char *user_name, int (*user_callback_func)(g_resource_t *r, int reason ) )
{
	g_res_user_t	*user;
	int		name_len;
	size_t		size;

	if ( user_name )
	{
		name_len = strlen( user_name ) + 1;
	}
	else
	{
		name_len = 1;
	}

	size = (size_t)(&((g_res_user_t *)0)->user_name[name_len]);
	user = (g_res_user_t*)NEWBYTES( size );

	if ( user_name )
	{
		strcpy( user->user_name, user_name );
	}
	else
	{
		user->user_name[0] = 0;
	}

	user->user_callback_func = user_callback_func;

	return user;
}

/*
  ==============================
  G_FreeResourceUser

  ==============================
*/
void G_FreeResourceUser( g_res_user_t *ru )
{
	free( ru );
}

/*
  ==============================
  G_ResourceAttache

  ==============================
*/

g_resource_t * G_ResourceAttach( g_resources_t *res, char *name, char *user_name, int (*user_callback_func)(g_resource_t *r, int reason ))
{
	g_resource_t	*r;
	g_res_type_t	*rt;
	g_res_user_t	*ru;
	u_list_iter_t	iter;

	r = (g_resource_t*)U_MapSearch( res->res_map, name );

	if ( !r )
	{
		// resource is not registered
		__error( "G_ResourceAttach: try to attach not registered resource name '%s'\n", name );
	}

	if ( r->state != G_RESOURCE_STATE_CACHED )
	{
		// have to be cached
		if ( r->state != G_RESOURCE_STATE_REGISTERED )
		{
			__error( "G_ResourceAttach: odd, resource '%s' exists, but is not registered\n", name );
		}
	
		// get resource type
		rt = (g_res_type_t*)U_MapSearch( res->type_map, &r->text[r->type_ofs] );
		if ( !rt )
		{
			__error( "G_ResourceAttach: odd, type of resource '%s' is not registered\n", name );
		}

		// cache for real
		rt->cache_func( r );

		if ( r->state != G_RESOURCE_STATE_CACHED )
			__error( "G_ResourceAttach: caching of resource '%s' failed\n", name );
	}       

	// 
	// register resource user 
	//

	U_ListIterInit( &iter, r->user_list );
	for ( ; ( ru = (g_res_user_t*)U_ListIterNext( &iter ) ) ; )
	{
		if ( !strcmp( ru->user_name, user_name ) )
		{
			// if the user is already user of the resource
			// the callbacks have to be the same.
			// because detach knows only the user_name
			if ( ru->user_callback_func != user_callback_func )
			{
				__error( "G_ResourceAttach: user '%s' already attached resource '%s' with a different callback\n", user_name, &r->text[r->name_ofs] );
			}
			
		}
	}
	
	ru = G_NewResourceUser( user_name, user_callback_func );

	U_ListInsertAtHead( r->user_list, ru );

	G_CallResourceUser( ru, r, G_RESOURCE_REASON_ATTACH_OK );
	G_CallResourceUser( ru, r, G_RESOURCE_REASON_GET_AVAILABLE );

	return r;
}


/*
  ==============================
  G_ResourceDetach

  ==============================
*/
void G_ResourceDetach( g_resources_t *res, char *name, char *user_name )
{
	g_resource_t	*r;
	g_res_user_t	*ru;
	u_list_iter_t	iter;

	r = (g_resource_t*)U_MapSearch( res->res_map, name );

	if ( !r )
	{
		__error( "G_ResourceDetach: try to detach not registered resource '%s'\n", name );
	}

	//
	// search user_name in user_list
	//
	U_ListIterInit( &iter, r->user_list );
	for ( ; ( ru = (g_res_user_t*)U_ListIterNext( &iter ) ) ; )
	{
		if ( !strcmp( ru->user_name, user_name ) )
		{
			U_ListIterRemoveGoNext( &iter );
			G_CallResourceUser( ru, r, G_RESOURCE_REASON_DETACH_OK );
			G_FreeResourceUser( ru );
			break;
		}
	}

	if ( !ru )
	{
		__error( "G_ResourceDetach: '%s' is not user of resource '%s'\n", user_name, &r->text[r->name_ofs] );
	}

}


/*
  ==============================
  G_CallResourceUser

  ==============================
*/
void G_CallResourceUser( g_res_user_t *ru, g_resource_t *r, int reason )
{
	if ( !ru->user_callback_func )
	{
		printf( "G_CallResourceUser: no callback for user '%s'\n", ru->user_name );
		return;
	}

	ru->user_callback_func( r, reason );
}


/*
  ==============================
  G_CallResourceUsers

  ==============================
*/
void G_CallResourceUsers( g_resource_t *r, int reason )
{
	u_list_iter_t	iter;
	g_res_user_t	*ru;

	U_ListIterInit( &iter, r->user_list );
	for ( ; ( ru = (g_res_user_t*)U_ListIterNext( &iter ) ) ; )
	{
		G_CallResourceUser( ru, r, reason );
	}
}


/*
  ==============================
  G_ResourcesForEach

  ==============================
*/
static void (*resources_for_each_func)(g_resource_t *r) = NULL;

void ResourcesForEachFunc( void *obj )
{
	resources_for_each_func( (g_resource_t *)(obj) );
}

void G_ResourcesForEach( g_resources_t *res, void (*action_func)(g_resource_t *r) )
{
	resources_for_each_func = action_func;
	U_MapForEach( res->res_map, ResourcesForEachFunc );
}
