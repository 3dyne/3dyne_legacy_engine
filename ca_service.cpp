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



// ca_service.c
/*
  ============================================================================
  
                                                    Memory Manager
 
                                                       '99 by S. Berger

  ============================================================================
*/

#include "interfaces.h"



// static char mm_zonenames[][64] = {
// 	"cache",
// 	"life",
// 	"static",
// 	"dynamic"
// };

typedef struct {
	char		name[56];
	u_int32_t	loaded;
	size_t		size;
	void*		data;
}	ca_entry_t;



#if defined ( win32_x86 )
static HANDLE	heap;
#endif

// static ca_entry_t	entries[CA_LISTNUM][CA_ENTRYNUM];
// static int		llast[CA_LISTNUM];

// static int	ca_started = 0;
// static int	mm_curzone;

int		ca_lastaction;



// new mm style
//
// zoneflags
#define ZF_INUSE	0x1 // zone is used
#define ZF_MALLOCED	0x2 // zone is malloced

typedef struct {
	char		*start;
	char		*ptr;
	unsigned int	size;
	unsigned int	zflags;
} mm_zone_t;

static mm_zone_t	zones[MM_ZONENUM];

static int	mm_started = 0;
int	mm_malloced, mm_freed, mm_nummalloc, mm_numfree;

// static FILE	*logfile, *ptrfile;

void MM_StartUp()
{
// 	int	i;

	if( mm_started )
		__warning( "trying to restart MM without shutdown.\n" );

	mm_started = 1;

	mm_malloced = mm_freed = mm_nummalloc = mm_numfree = 0;

#ifdef log_malloc
	__named_message( "\tloging malloc / free to mem.log.\n" );
	logfile = fopen( "mem.log", "w" );
	ptrfile = fopen( "ptr.log", "w" );
#endif


#if defined( win32_x86 )
//	heap = HeapCreate( 0, 0, 0 );
//	heap = GetProcessHeap();

#endif

}

void  MM_ShutDown()
{
	int	i;

	if( !mm_started )
		__warning( "trying to shutdown MM without startup.\n" );

//	__named_message( "\n" );
	for( i = 0; i < MM_ZONENUM; i++ )
	{
		if( ( zones[i].zflags&ZF_INUSE ) && ( zones[i].zflags&ZF_MALLOCED ))
			free( zones[i].start );
#if 0
		else
			printf( "zone %d unused\n", i );
#endif
	}
	
	for( i = 0; i < MM_ZONENUM; i++ )
	{
		memset( &zones[i], 0, sizeof( mm_zone_t ));
	}

	mm_started = 0;

	if( mm_nummalloc - mm_numfree )
		__warning( "unfreed memory! check for leaks. ( %d, %.2f KB )\n", mm_nummalloc - mm_numfree, (mm_malloced - mm_freed)/1024.0 );


#ifdef log_malloc
	fprintf( logfile, "end\n" );
	
	fclose( logfile );
	fclose( ptrfile );
#endif

#if defined( win32_x86 )
//	HeapDestroy( heap );
#endif
}

/*
  ====================
  _MM_Malloc

  non loging malloc warper. 
  use the MM_Malloc macro
  ====================
*/

void *_MM_Malloc( int size )
{
	void	*ptr;

#ifdef mm_checksize
	unsigned int	*t;

	size += sizeof( unsigned int );

	
#endif // mm_checksize


#if defined ( linux_i386 )
	ptr = malloc( size );

#elif defined ( win32_x86 )
//	ptr = ( void * ) HeapAlloc( heap, 0, size );
	ptr = malloc( size );
#endif
	
#ifdef mm_checksize
	t = ptr;
	ptr = (( unsigned char * )ptr + sizeof( unsigned int ));
	*t = size;
#endif
	

	if( !ptr )
		__error( "malloc failed\n" );

	mm_nummalloc++;
	mm_malloced += size;
	

	return ptr;

}



/*
  ====================
  _MM_Free
  ====================
*/
void _MM_Free( void *ptr )
{
	
#ifdef mm_checksize
	unsigned int	*t;
#endif
	

	if( !ptr )
		return;


#ifdef mm_checksize
	t = ( unsigned int * )((unsigned char* )ptr - sizeof( unsigned int ));

	mm_freed += *t;
	ptr = t;

#endif

	
	
#if defined ( linux_i386 )
	free( ptr );
	mm_numfree++;
// todo: add mm_freed
#elif defined ( win32_x86 )
	mm_numfree++;

//HeapFree( heap, 0, ptr );
	free( ptr );
#endif


}

#ifdef log_malloc

void *_MM_LogMalloc( int size, char *file, int line )
{
	void	*ptr;
	char	funchist[1024];

	ptr = _MM_Malloc( size );


#if defined trace_functions
	TF_FuncHistory( funchist, 1024 );
	fprintf( logfile, "malloc: %p %s %d\n", ptr, file, line );
	fprintf( logfile, "  function: %s\n", funchist );
#else
	fprintf( logfile, "malloc: %p %s %d %d\n", ptr, file, line, size );
#endif
	fprintf( ptrfile, "%p\n", ptr );
		
	return ptr;
}

void _MM_LogFree( void *ptr, char* file, int line )
{
	char funchist[1024];
#if defined trace_functions
	TF_FuncHistory( funchist, 1024 );
	fprintf( logfile, "free: %p %s\n", ptr, file, line );
	fprintf( logfile, "  function: %s\n", funchist );
#else
	fprintf( logfile, "free: %p %s %d\n", ptr, file, line );
#endif
	fprintf( ptrfile, "%p\n", ptr );
	_MM_Free( ptr );
}

#endif


void MM_ShowInfo()
{
	SHV_Printf( "====================\n" );
	SHV_Printf( "ever malloced: %d\n", mm_malloced );
	SHV_Printf( "mallocs: %d\n", mm_nummalloc );
	SHV_Printf( "frees: %d\n", mm_numfree );
	SHV_Printf( "malloced: %d\n", mm_malloced - mm_freed );
}

#if 0
void CA_StartUp( )
{
	int	i, i2;

	if( ca_started )
		__warning( "trying to restart CA without shutdown.\n" );

	__named_message( "\tinit %d cache lumps\n", CA_ENTRYNUM );

	for( i2 = 0; i2 < CA_LISTNUM; i2++ )
	{
		for( i = 0; i < CA_ENTRYNUM; i++ )
		{
			memset( entries[i2][i].name, 0, 56 );
			entries[i2][i].loaded = 0;
			entries[i2][i].data = NULL;
		}
		llast[i2] = 0;
	}
	ca_started = 1;
}

void CA_ShutDown()
{
#if 0
	int	i, i2;
	int	freecount = 0;

	if( !ca_started )
	{
		__warning( "trying to shutdown CA without startup\n" );
		return;
	}	

	for( i2 = 0; i2 < CA_LISTNUM; i2++ )

	{	
		for( i = 0; i < CA_ENTRYNUM; i++ )
		{
			if( entries[i2][i].loaded )
			{
				freecount++;
				MM_Free( entries[i2][i].data );
			}
		}

	}

	if( freecount )
		__named_message( "\t%d unfreed junks\n", freecount );
	else
		__named_message( "\n" );

	ca_started = 0;
#endif
}

void CA_BeginCaching()
{
	//	MM_FreeZone( ZONE_CACHE ); 
	ca_lastaction = gc_renderframe;
}


int CA_EndCaching()
{
	int	i, i2;
	int	freed = 0;
	TFUNC_ENTER;


	for( i2 = 0; i2 < CA_LISTNUM; i2++ )
	{
		for( i = 0; i < CA_ENTRYNUM * CA_LISTNUM; i++ )
		{
			if( entries[i2][i].loaded )
			{
				memset( entries[i2][i].name, 0, 56 );
				entries[i2][i].loaded = 0;
				MM_Free( entries[i2][i].data );
				freed++;
			}
		}
		llast[i2] = 0;
	}
	//MM_SetDataEnd( dynamicstart );
	TFUNC_LEAVE;
	return freed;
}


// ========================================
// load _filename_ with _loadfunc( char *data )_
// to buffer in zone _zid_ and return buffer
// this does not manipulate the cache, so it
// can be used from outside this module without
// any limitations.
// ========================================

void *CA_Load( char *filename, void *(*loadfunc)( char *, int ) )
{
	ib_file_t	*h;
	int		filesize;
	char		*buf;
	char		*data;

	TFUNC_ENTER;	
	h = IB_Open( filename );
	if( !h )
		__error( "while loading: %s. cannot open\n", filename );

	
	filesize = IB_GetSize( h );


	buf = alloca( filesize );
	
	if( !buf )
		__error( "while loading: %s. alloca failed\n", filename );
	

	IB_Read( buf, filesize, 1, h );

	IB_Close( h );
	
//	MM_SetCurrentZone( zid );

	if( !loadfunc )
		__error( "while loading: %s. shame! no loadfunc\n", filename );

	data = loadfunc( buf, filesize );
	if( !data )
		__error( "while loading: %s. loadfunc returned NULL\n", filename );

	TFUNC_LEAVE;
	return data;
}

// ============================================================================

// ========================================
//  intern lowlevel caching functions
//  this is the place for some optimization
// ========================================
	
// is _filename_ cached in list _listid_ ?

int CA_IsCached( char *filename, int listid )
{
	int	i;
	TFUNC_ENTER;
//	__named_message( "%s\n", filename );

	if( listid >= CA_LISTNUM )
		__error( "no list for datatype %d\n", listid );

	if( llast[listid] >= CA_ENTRYNUM )
		__error( "list %d out of space\n", listid );

	for( i = 0; i <= llast[listid]; i++ )
	{
		if( !strncmp( filename, entries[listid][i].name, 56 ))
		{		
			//__named_message( "found cached: %s at %d\n", filename, i );
			return	i;
		}
	}
	TFUNC_LEAVE;
	return	-1;
}



// put data _ptr_ to cachelist _listid_ with the identification name _filename_

int CA_ToCache( char *filename, void *ptr, int listid )
{
	TFUNC_ENTER;
	if( llast[listid] >= CA_ENTRYNUM )
		__error( "list %d out of space\n", listid ); 

	strncpy( entries[listid][llast[listid]].name, filename, 56 );
	entries[listid][llast[listid]].data = ptr;
	entries[listid][llast[listid]].loaded = 1;
//	__named_message( "%s to %d\n", filename, llast[listid] );
	TFUNC_LEAVE;
	return llast[listid]++;
//	llast[listid]++;
	
}
	
// ========================================
//  search the cachelist for _filename_ or
//  cache filename with loadfunc( char *buf )
//  to cachelist id 
// ========================================

int CA_CacheFile( char *filename, void *(*loadfunc)( char *, int ), int listid )
{
	void	*data;
	int	tmp, i;

	TFUNC_ENTER;
	tmp = CA_IsCached( filename, listid );
	if( tmp >= 0 )
	{
		TFUNC_LEAVE;
		return tmp;
	}


	data = CA_Load( filename, loadfunc );

	i = CA_ToCache( filename, data, listid );

	tmp = i + ( 0x01000000 ) * listid;
	
	TFUNC_LEAVE;
	return tmp;
}

// get data of __id_ from list _listid_

void *CA_GetPtr( int _id )
{
	int	listid, id;
	listid = _id >> 24;
	id = _id & 0x00ffffff;

//	__named_message( "%d %d %p\n", listid, id, entries[listid][id].data );
	return entries[listid][id].data;

}

#endif
