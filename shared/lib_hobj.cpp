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



// lib_hobj.c

#include "lib_hobj.h"
#include "lib_hmanager.h"
#include "shared.h"

#include "s_config.h"
#include "s_mem.h"

#include "shock.h"

#define printf  S_Printf                                                        
#define Error   __error
 
/*
  ==================================================
  hobject stuff
  
  ==================================================
*/



/*
  ====================
  NewClass
  
  ====================
*/
hobj_t* NewClass( const char *type, const char *name )
{
	hobj_t		*obj;

//	obj = ( hobj_t * ) S_Malloc( sizeof( hobj_t ) );
	obj = NEWTYPE( hobj_t );

	__chkptr( obj );

	memset( obj, 0, sizeof( hobj_t ) );

	strcpy( obj->type, type );
	strcpy( obj->name, name );

	obj->next = NULL;
	obj->hobjs = NULL;
	obj->pairs = NULL;
	obj->extra = NULL;
	return obj;
}


/*
  ====================
  FreelCass

  ====================
*/
void FreeClass( hobj_t *obj )
{
	FREE( obj );
}



/*
  ====================
  NewHPair

  ====================
*/
hpair_t* NewHPair( void )
{
	hpair_t		*pair;

//	pair = ( hpair_t * ) S_Malloc( sizeof( hpair_t ) );
	pair = NEWTYPE( hpair_t );
	__chkptr( pair );
	memset( pair, 0, sizeof( hpair_t ) );

//	pair->value = S_Malloc( HPAIR_VALUE_SIZE );
//	memset( pair->value, 0, HPAIR_VALUE_SIZE );
	pair->value = (char *)NEWBYTES( HPAIR_VALUE_SIZE );

	pair->next = NULL;

	return pair;
}

hpair_t* NewHPair2( const char *type, const char *key, const char *value )
{
	hpair_t		*pair;
	int		size;

//	pair = ( hpair_t * ) S_Malloc( sizeof( hpair_t ) );
//	__chkptr( pair );
//	memset( pair, 0, sizeof( hpair_t ) );
	pair = NEWTYPE( hpair_t );

	strcpy( pair->type, type );

	size = strlen( key ) + 1;
//	pair->key = NEWBYTES( size );
	strcpy( pair->key, key );

//	pair->value = S_Malloc( strlen( value ) + 1 );
//	__chkptr( pair->value );
	size = strlen( value ) + 1;
	pair->value = (char *)NEWBYTES( size );
	strcpy( pair->value, value );

	pair->next = NULL;

	return pair;	
}


/*
  ==============================
  CopyHPair

  ==============================
*/
hpair_t* CopyHPair( hpair_t *pair )
{
	return NewHPair2( pair->type, pair->key, pair->value );	
}

/*
  ====================
  FreeHPair

  ====================
*/
void FreeHPair( hpair_t *pair )
{
	if ( pair->value )
		FREE( pair->value );

	FREE( pair );
}


/*
  ==============================
  DestroyClass

  ==============================
*/
void DestroyClass( hobj_t *obj )
{
    __warning( "DestroyClass not impl\n" );
// #warning DestroyClass not impl
}

/*
  ==============================
  DeepDestroyClass

  ==============================
*/
void DeepDestroyClass( hobj_t *obj )
{
//#warning DeepDestroyClass not impl

	hobj_t		*o, *onext;
	hpair_t		*p, *pnext;

	for ( o = obj->hobjs; o ; o=onext )
	{
		onext = o->next;
		DeepDestroyClass( o );	
	}

	for ( p = obj->pairs; p ; p=pnext )
	{
		pnext = p->next;
		FreeHPair( p );
	}

	FreeClass( o );
}

/*
  ====================
  SetClassExtra

  ====================
*/
void SetClassExtra( hobj_t *self, void *ptr )
{
	self->extra = ptr;
}

/*
  ====================
  GetClassExtra

  ====================
*/
void * GetClassExtra( hobj_t *self )
{
	return self->extra;
}

/*
  ====================
  InsertHPair

  ====================
*/
void InsertHPair( hobj_t *self, hpair_t *pair )
{
	pair->next = self->pairs;
	self->pairs = pair;
}



/*
  ====================
  RemoveHPair

  ====================
*/
bool_t RemoveHPair( hobj_t *self, hpair_t *pair )
{
	hpair_t		*p, *next, *head;
	bool_t		success;

	head = NULL;
	success = false;
	for( p = self->pairs; p ; p=next )
	{
		next = p->next;
		if ( p == pair )
		{
			success = true;
			continue;
		}
		p->next = head;
		head = p;
	}
	self->pairs = head;
	return success;
}

/*
  ====================
  RemoveAndDestroyAllHPairsOfKey

  ====================
*/
void RemoveAndDestroyAllHPairsOfKey( hobj_t *self, char *key )
{
	hpair_t	*pair;
	for(;;)
	{
		pair = FindHPair( self, key );
		if ( !pair )
			break;
		RemoveHPair( self, pair );
	}
}

/*
  ====================
  InsertClass

  ====================
*/
void InsertClass( hobj_t *self, hobj_t *obj )
{
	obj->next = self->hobjs;
	self->hobjs = obj;
	
	obj->parent = self;
}



/*
  ====================
  RemoveClass

  ====================
*/
bool_t RemoveClass( hobj_t *self, hobj_t *obj )
{
	hobj_t		*o, *next, *head;
	bool_t		success;

	head = NULL;
	success = false;
	for( o = self->hobjs; o ; o=next )
	{
		next = o->next;
		if ( o == obj )
		{
			success = true;
			obj->parent = NULL;
			continue;
		}
		o->next = head;
		head = o;
	}
	self->hobjs = head;
	return success;
}

/*
  ====================
  RemoveClass2

  new function, cause objects now now 
  their parent
  ====================
*/
bool_t RemoveClass2( hobj_t *obj )
{
	hobj_t		*parent;
	hobj_t		*o, *next, *head;	
	bool_t		success;
	
	if ( obj->parent == NULL )
		// can't remove root class from anywhere
		return true;

	head = NULL;
	success = false;
	parent = obj->parent;
	for( o = parent->hobjs; o ; o=next )
	{
		next = o->next;
		if ( o == obj )
		{
			success = true;
			obj->parent = NULL;
			continue;
		}
		o->next = head;
		head = o;
	}
	parent->hobjs = head;
	return success;
}


/*
  ====================
  FindHPair

  ====================
*/
hpair_t* FindHPair( hobj_t* self, const char* key )
{
	hpair_t		*p;

	for ( p = self->pairs; p ; p=p->next )
	{
		if ( !strcmp( key, p->key ) )
			return p;
	}
	return NULL;
}


/*
  ====================
  FindClass

  ====================
*/
hobj_t* FindClass( hobj_t* self, const char* name )
{
	hobj_t		*o;

	for ( o = self->hobjs; o ; o=o->next )
	{
		if ( !strcmp( name, o->name ) )
			return o;
	}
	return NULL;
}

/*
  ====================
  FindClassType

  ====================
*/
hobj_t* FindClassType( hobj_t* self, const char* type )
{
	hobj_t		*o;

	for ( o = self->hobjs; o ; o=o->next )
	{
		if ( !strcmp( type, o->type ) )
			return o;
	}
	return NULL;
}

/*
  ====================
  InitClassSearchIterator

  ===================
*/
void InitClassSearchIterator( hobj_search_iterator_t* iter, hobj_t* obj, const char* type )
{
	iter->hobj = obj;
	iter->current = obj->hobjs;
	strcpy( iter->search_type, type );
}

/*
  ====================
  InitHPairSearchIterator

  ===================
*/
void InitHPairSearchIterator( hpair_search_iterator_t* iter, hobj_t* obj, const char* key )
{
	iter->hobj = obj;
	iter->current = obj->pairs;
	strcpy( iter->search_key, key );
}


/*
  ====================
  SearchGetNextClass

  ====================
*/
hobj_t* SearchGetNextClass( hobj_search_iterator_t *iter )
{
	hobj_t	*cur;

	cur = NULL;

	if ( !iter->current )
		return NULL;

	// search hobj with search_type
	for ( ; iter->current; iter->current=iter->current->next )
	{
		if ( !strcmp( iter->search_type, iter->current->type ) ||
		     iter->search_type[0] == '*' ) 
		{
			cur = iter->current;
			break;
		}
	}
	if ( !iter->current )
		return NULL;
	
	iter->current = iter->current->next;

	return cur;
}

/*
  ====================
  SearchGetNextHPair

  ====================
*/
hpair_t* SearchGetNextHPair( hpair_search_iterator_t *iter )
{
	hpair_t	*cur;

	cur = NULL;

	if ( !iter->current )
		return NULL;

	// search hpair with search_type
	for ( ; iter->current; iter->current=iter->current->next )
	{
		if ( !strcmp( iter->search_key, iter->current->key ) || iter->search_key[0] == '*' )
		{
			cur = iter->current;
			break;
		}
	}
	if ( !iter->current )
		return NULL;
	
	iter->current = iter->current->next;

	return cur;
}

/*
  ====================
  DumpClass
  
  ====================
*/
void DumpClass( hobj_t *self )
{
	hobj_t		*o;
	hpair_t		*p;
	int		num;

	printf( "DumpClass:\n" );
	printf( " type %s, name %s\n", self->type, self->name );
	for ( num = 0, o = self->hobjs; o ; o=o->next, num++ )
	{ }
	printf( " %d hobjs", num );
	for ( num = 0, p = self->pairs; p ; p=p->next, num++ )
	{ }
	printf( " %d pairs\n", num );
}

/*
  ====================
  DumpHPair

  ====================
*/
void DumpHPair( hpair_t *self )
{
	printf( "DumpHPair:\n" );
}



/*
  ====================
  DeepDumpClass

  ====================
*/
static int	cur_objnum;
static int	cur_pairnum;
static int	cur_maxdeep;
static int	cur_deep;
static int	cur_bytes;

void DeepDumpClassRecursive( hobj_t *self )
{
	hpair_t		*p;
	hobj_t		*o;

	cur_bytes += sizeof( hobj_t );

	if ( cur_deep > cur_maxdeep )
		cur_maxdeep = cur_deep;

	for ( p = self->pairs; p ; p=p->next, cur_pairnum++ )
	{
		cur_bytes += sizeof( hpair_t );
		if ( p->value )
			cur_bytes += strlen( p->value );
	}
	for ( o = self->hobjs; o ; o=o->next, cur_objnum++ )
	{
		cur_deep++;
		DeepDumpClassRecursive( o );
		cur_deep--;
	}
}

void DeepDumpClass( hobj_t *self )
{
	printf( "DeepDumpClass:\n" );

	cur_objnum = 0;
	cur_pairnum = 0;
	cur_maxdeep = 0;
	cur_deep = 0;
	cur_bytes = 0;
	DeepDumpClassRecursive( self );

	printf( " total classes %d, total pairs %d, max deep %d\n", cur_objnum, cur_pairnum, cur_maxdeep );
	printf( " %d bytes\n", cur_bytes );
}


/*
  ==============================
  ClassTraceToRoot

  ==============================
*/
void ClassTraceToRoot( hobj_t *obj )
{
	__named_message( "tracing for '%s' of type '%s' ...\n", obj->name, obj->type );

	for( obj = obj->parent; obj ; obj=obj->parent )
	{
		__named_message( "parent is '%s' of type '%s'\n", obj->name, obj->type );
	}
}

/*
  ==================================================
  hpair cast stuff

  ==================================================
*/
  
void HPairCastToVec3d( vec3d_t v, hpair_t *pair )
{
	char            *ptr;
	ptr = pair->value;
	v[0] = (fp_t) strtod( ptr, &ptr );
	v[1] = (fp_t) strtod( ptr, &ptr );
	v[2] = (fp_t) strtod( ptr, &ptr );
}

void HPairCastToVec2d( vec2d_t v, hpair_t *pair )
{
	char            *ptr;
	ptr = pair->value;
	v[0] = (fp_t) strtod( ptr, &ptr );
	v[1] = (fp_t) strtod( ptr, &ptr );
}

void HPairCastToFloat( fp_t *f, hpair_t *pair )
{
	*f = atof( pair->value );
}

void HPairCastToInt( int *i, hpair_t *pair )
{
	*i = atoi( pair->value );
}

void HPairCastToString( char *t, hpair_t *pair )
{
	strcpy( t, pair->value );
}

// type safe casts

void HPairCastToVec3d_safe( vec3d_t v, hpair_t *pair )
{
	char            *ptr;

	if ( dstrcasecmp( "vec3d", pair->type ) )
	{
		printf( "Warning: HPairCastToVec3d_safe: safe cast failed for \"%s\" of type \"%s\".\n",
			pair->key, pair->type );
		Vec3dInit( v, 0, 0, 0 );
		return;
	}
	
	ptr = pair->value;
	v[0] = (fp_t) strtod( ptr, &ptr );
	v[1] = (fp_t) strtod( ptr, &ptr );
	v[2] = (fp_t) strtod( ptr, &ptr );
}

void HPairCastToVec2d_safe( vec2d_t v, hpair_t *pair )
{
	char            *ptr;

	if ( dstrcasecmp( "vec2d", pair->type ) )
	{
		printf( "Warning: HPairCastToVec2d_safe: safe cast failed for \"%s\" of type \"%s\".\n",
			pair->key, pair->type );
		Vec2dInit( v, 0, 0 );
		return;
	}	

	ptr = pair->value;
	v[0] = (fp_t) strtod( ptr, &ptr );
	v[1] = (fp_t) strtod( ptr, &ptr );
}

void HPairCastToFloat_safe( fp_t *f, hpair_t *pair )
{
	if ( dstrcasecmp( "float", pair->type ) )
	{
		printf( "Warning: HPairCastToFloat_safe: safe cast failed for \"%s\" of type \"%s\".\n",
			pair->key, pair->type );
		*f = 0.0;
		return;
	}
	
	*f = atof( pair->value );
}

void HPairCastToInt_safe( int *i, hpair_t *pair )
{
	if ( dstrcasecmp( "int", pair->type ) )
	{
		printf( "Warning: HPairCastToInt_safe: safe cast failed for \"%s\" of type \"%s\".\n",
			pair->key, pair->type );
		*i = 0;
		return;
	}
	
	*i = atoi( pair->value );
}

void HPairCastToString_safe( char *t, hpair_t *pair )
{

	if ( dstrcasecmp( "string", pair->type ) )
	{
		printf( "Warning: HPairCastToString_safe: safe cast failed for \"%s\" of type \"%s\".\n",
			pair->key, pair->type );
		*t = 0;
		return;
	}	

	strcpy( t, pair->value );
}

void HPairCastToClsref_safe( char *t, hpair_t *pair )
{

	if ( dstrcasecmp( "ref", pair->type ) )
	{
		printf( "Warning: HPairCastToClsref_safe: safe cast failed for \"%s\" of type \"%s\".\n",
			pair->key, pair->type );
		*t = 0;
		return;
	}	

	strcpy( t, pair->value );
}

int HexCharToInt( unsigned char c )
{
	if ( c >= 48 && c <= 57 )
		return c-48;
	if ( c >= 65 && c <= 106 )
		return (c-65)+10;
	return 0;
}

unsigned char IntToHexChar( int i )
{
	if ( i >= 0 && i <= 9 )
		return 48+i;
	if ( i >= 10 && i <= 16 )
		return 65+(i-10);
	return 48;
}

void HPairCastToBstring_safe( void *ptr, int *max_buf_size, hpair_t *pair )
{
	int		i, len;
	unsigned char		*c;

	c = (unsigned char *) ptr;

	if ( dstrcasecmp( "bstring", pair->type ) )
	{
		printf( "Warning: HPairCastToBstring_safe: safe cast failed for \"%s\" of type \"%s\".\n",
			pair->key, pair->type );
		*c = 0;
		*max_buf_size = 0;
		return;
	}	

	len = strlen( pair->value );

	if ( len&1 )
	{
		printf( "Warning: HPairCastToBstring_safe: value length is odd in \"%s\" of type \"%s\".\n",
			pair->key, pair->type );		
		*c = 0;
		*max_buf_size = 0;
	}

	if ( len / 2 > (*max_buf_size) )
	{
		printf( "Warning: HPairCastToBstring_safe: destination buffer to small for \"%s\" of type \"%s\".\n",
			pair->key, pair->type );		
		*c = 0;
		*max_buf_size = 0;
	}

	*max_buf_size = 0;
	for ( i = 0; i < len; i+=2 )
	{
		*c = (unsigned char)( HexCharToInt(pair->value[i]) * 16 + HexCharToInt(pair->value[i+1]));
		c++;
		(*max_buf_size)++;
	}       
}

int HPairGetBstringLength( hpair_t *pair )
{
	int		len;

	len = strlen( pair->value );
	return len/2;
}

void BstringCastToHPair( void *ptr, int size, hpair_t *pair )
{
	int		i, j;
	unsigned char	*in;

	in = (unsigned char *) ptr;
	pair->value = (char *)realloc( pair->value, size * 2 + 1 );

	for ( i = 0, j = 0; i < size; i++, j+=2 )
	{
		pair->value[j] = IntToHexChar(in[i] / 16 );
		pair->value[j+1] = IntToHexChar(in[i] & 15 );
	}
	pair->value[j] = 0;	// arghh!!
}

void HPairCastFromVec3d( vec3d_t v, hpair_t *pair )
{
	char		tt[256];
	int		i;
	int		pos;

	pos = 0;
	for ( i = 0; i < 3; i++ )
	{
		if ( _Rint(v[i]) == v[i] )
			pos+=sprintf( &tt[pos], "%d ", (int)_Rint(v[i]) );
		else
			pos+=sprintf( &tt[pos], "%f ", v[i] );
	}
	
	pair->value = (char *)realloc( pair->value, pos+1 );
	strcpy( pair->value, tt );
}

/*
  ==================================================
  easy find and cast

  ==================================================
*/

/*
  ==============================
  EasyFindVec3d

  ==============================
*/
hpair_t * EasyFindVec3d( vec3d_t v, hobj_t *obj, const char *key )
{
	hpair_t		*pair;
	
	pair = FindHPair( obj, key );
	if ( !pair )
		Error( "missing key '%s' in name '%s' of type '%s'\n", key, obj->name, obj->type );
	
	HPairCastToVec3d_safe( v, pair );
	return pair;
}

/*
  ==============================
  EasyFindVec2d

  ==============================
*/
hpair_t * EasyFindVec2d( vec2d_t v, hobj_t *obj, const char *key )
{
	hpair_t		*pair;
	
	pair = FindHPair( obj, key );
	if ( !pair )
		Error( "missing key '%s' in name '%s' of type '%s'\n", key, obj->name, obj->type );
	
	HPairCastToVec2d_safe( v, pair );	
	return pair;
}

/*
  ==============================
  EasyFindFloat

  ==============================
*/
hpair_t * EasyFindFloat( fp_t *f, hobj_t *obj, const char *key )
{
	hpair_t		*pair;
	
	pair = FindHPair( obj, key );
	if ( !pair )
		Error( "missing key '%s' in name '%s' of type '%s'\n", key, obj->name, obj->type );
	
	HPairCastToFloat_safe( f, pair );
	return pair;
}

/*
  ==============================
  EasyFindInt

  ==============================
*/
hpair_t * EasyFindInt( int *i, hobj_t *obj, const char *key )
{
	hpair_t		*pair;
	
	pair = FindHPair( obj, key );
	if ( !pair )
		Error( "missing key '%s' in name '%s' of type '%s'\n", key, obj->name, obj->type );
	
	HPairCastToInt_safe( i, pair );
	return pair;
}

/*
  ==============================
  EasyFindString
  
  ==============================
*/
hpair_t * EasyFindString( char *t, hobj_t *obj, const char *key )
{
	hpair_t		*pair;
	
	pair = FindHPair( obj, key );
	if ( !pair )
		Error( "missing key '%s' in name '%s' of type '%s'\n", key, obj->name, obj->type );
	
	HPairCastToString_safe( t, pair );
	return pair;
}

/*
  ==============================
  EasyFindClsrefAsUnique

  ==============================
*/
hpair_t * EasyFindClsrefAsUnique( unique_t *uni, hobj_t *obj, const char *key )
{
	hpair_t		*pair;
	
	pair = FindHPair( obj, key );
	if ( !pair )
		Error( "missing key '%s' in name '%s' of type '%s'\n", key, obj->name, obj->type );
	
	if ( pair->value[0] != '#' )
		Error( "value '%s' is not a unique id style\n", pair->value );

	*uni = atoi( &pair->value[1] );       

	return pair;	
}

/*
  ==============================
  StringToUnique

  ==============================
*/
unique_t StringToUnique( char *text )
{
	if ( text[0] != '#' )
		return UNIQUE_INVALIDE;
//		Error( "string '%s' is not a unique id style\n", text );
	
	return atoi( &text[1] );
}

/*
  ==============================
  EasyLookupClsref

  ==============================
*/
hobj_t * EasyLookupClsref( hobj_t *obj, const char *key, hmanager_t *hm )
{
	hobj_t		*other;
	hpair_t		*pair;

	pair = FindHPair( obj, key );
	if ( !pair )
		__error( "missing key '%s' in class name '%s' of type '%s'\n", key, obj->name, obj->type );

	other = HManagerSearchClassName( hm, pair->value );
	if ( !other )
		__error( "can't find class name '%s' for clsref '%s' in class name '%s' of type '%s'\n",
			 pair->value, key, obj->name, obj->type );
	return other;
}


/*
  ==================================================
  easy new and insert

  ==================================================
*/

/*
  ==============================
  EasyNewVec3d

  ==============================
*/
void EasyNewVec3d( hobj_t *obj, const char *key, vec3d_t v )
{
	hpair_t	*pair;
	char	tt[256];

	sprintf( tt, "%f %f %f", v[0], v[1], v[2] );
	pair = NewHPair2( "vec3d", key, tt );
	InsertHPair( obj, pair );
}

/*
  ==============================
  EasyNewVec2d

  ==============================
*/
void EasyNewVec2d( hobj_t *obj, const char *key, vec2d_t v )
{
	hpair_t	*pair;
	char	tt[256];

	sprintf( tt, "%f %f", v[0], v[1] );
	pair = NewHPair2( "vec2d", key, tt );
	InsertHPair( obj, pair );	
}

/*
  ==============================
  EasyNewFloat

  ==============================
*/
void EasyNewFloat( hobj_t *obj, const char *key, fp_t f )
{
	hpair_t	*pair;
	char	tt[256];

	sprintf( tt, "%f", f );
	pair = NewHPair2( "float", key, tt );
	InsertHPair( obj, pair );		
}

/*
  ==============================
  EasyNewInt

  ==============================
*/
void EasyNewInt( hobj_t *obj, const char *key, int i )
{
	hpair_t	*pair;
	char	tt[256];

	sprintf( tt, "%d", i );
	pair = NewHPair2( "int", key, tt );
	InsertHPair( obj, pair );
}

/*
  ==============================
  EasyNewString

  ==============================
*/
void EasyNewString( hobj_t *obj, const char *key, const char *text )
{
	hpair_t		*pair;
	
	pair = NewHPair2( "string", key, text );
	InsertHPair( obj, pair );
}


/*
  ==============================
  EasyNewClsref

  ==============================
*/
void EasyNewClsref( hobj_t *obj, const char *key, hobj_t *referenced_obj )
{
	hpair_t		*pair;

	pair = NewHPair2( "ref", key, referenced_obj->name );
	InsertHPair( obj, pair );
}


/*
  ==============================
  EasyNewClassFromID

  ==============================
*/
hobj_t * EasyNewClassFromID( const char *type, unique_t id )
{
	hobj_t	*obj;
	char	str[256];

	sprintf( str, "#%u", id );
	
	obj = NewClass( type, str );
	return obj;
}


/*
  ==================================================
  easy compare

  ==================================================
*/

/*
  ==============================
  EasyCompareValue

  ==============================
*/
bool_t EasyCompareValue( hobj_t *obj, const char *key, const char *cmp_value )
{
	hpair_t		*pair;

	pair = FindHPair( obj, key );
	if ( !pair )
	{
		return false;
	}

	if ( !strcmp( pair->value, cmp_value ) )
	{
		return true;
	}

	return false;
}
