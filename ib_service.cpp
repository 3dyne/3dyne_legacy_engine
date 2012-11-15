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



// ib_service.c




#include "interfaces.h"
#include <bzlib.h>

#define IB_SRC_NUM	(16)
#define SAR_ID_SIZE	( 4 )
#define SAR_TYPE_SIZE	( 4 )
#define SAR_HEADER_SIZE ( SAR_ID_SIZE + SAR_TYPE_SIZE + 12 )
#define SAR_FILENAME_SIZE ( 56 )
#define SAR_DIRENT_SIZE	( SAR_FILENAME_SIZE + 8 )

#define IB_GETC_BUFSIZE	( 4096 * 10/*256 * 255*/ )


#ifndef BZ_MAX_UNUSED
#define BZ_MAX_UNUSED ( 5000 )
#endif

typedef struct sar_dir_entry_s {
	char	name[56];
	int	pos;
	int	size;
} sar_dir_entry_t;

typedef struct ib_source_s {
	ib_file_t *(*OpenFile)( char *name, struct ib_source_s *self );
	void (*Delete)( struct ib_source_s *self );




	union ib_source_private_u {
		struct ib_source_sar_private_s {
			char	name[256];
			u_map_t	*name_map;
		} sar;

		struct ib_source_disk_private_s {
			char	dir[256];
		} disk;
	} private_;
} ib_source_t;

typedef struct ib_bzdata_s
{
	int	filesize;
	char	buf[BZ_MAX_UNUSED];
	int	bufN;

	bz_stream	strm;
} ib_bzdata_t;

static char** ib_lastsarnames = 0;
static int ib_lastsarsize = 0;

static ib_source_t	*ib_src[IB_SRC_NUM];
static int		ib_srclast;

static char		ib_lastname[256];

int ib_lastaction;

int sc_readbytes;

// ========================================
// ib_file_t disk members

int DISK_Read( void *buf, unsigned int size, ib_file_t *self )
{
	int	res;
	char	*c;

	
	__chkptr( buf );
	__chkptr( self );

	// test hack
	if( size == 1 )
	{
		res = fgetc( self->private_.disk.h );
		if( res == EOF )
		{
//			__error( "EOF\n" );
			return 0;
		}

		c = (char *)buf;
		*c = ( char )res;
		return 1;
	} else
	{
#if 0/*defined ( win32_x86 )*/
#warning win32 getc hack!
		int	i, res;
		char	*c;

		c = buf;
		for( i = 0; i < size; i++ )
		{
			res = fgetc( self->private_.disk.h );
			if ( res == EOF )
			{
				//__error( "EOF\n" );
				c[i] = 255;
				continue;
			}
			c[i] = (char) res;
		}
		
#else
		res = fread( buf, 1, size, self->private_.disk.h );
#endif
	}
	return res;
}

unsigned int DISK_GetSize( ib_file_t *self )
{
	unsigned int pos, end;

	__chkptr( self );
	pos = ftell( self->private_.disk.h );
	fseek( self->private_.disk.h, 0, SEEK_END );
	end = ftell( self->private_.disk.h );
	fseek( self->private_.disk.h, pos, SEEK_SET );

	return end;
}

void DISK_SetPos( ib_file_t *self, unsigned int pos )
{
	__chkptr( self );
	fseek( self->private_.disk.h, pos, SEEK_SET );
}

unsigned int DISK_GetPos( ib_file_t *self )
{
	unsigned int pos;

	__chkptr( self );

	pos = ftell( self->private_.disk.h );
	return pos;
}

void DISK_Close( ib_file_t *self )
{
	__chkptr( self );
	
	if( !self->private_.disk.h )
	{
		__warning( "file not open: '%s'\n", self->filename );
		return;
	}

	fclose( self->private_.disk.h );
	MM_Free( self );
}

// ========================================
// ib_source_t disk members

void DISK_Delete( ib_source_t *self )
{
	MM_Free( self );
	// what can we do?
}

ib_file_t *DISK_Open( char *name, ib_source_t *self )
{
	FILE	*h;
	char	filename[256];
	ib_file_t	*tmp;

	sprintf( filename, "%s/%s", self->private_.disk.dir, name );
//	__named_message( " trying %s\n", filename );

	h = fopen( filename, FOPEN_READ );
	if( !h )
		return NULL;
	
	//	tmp = MM_Malloc( sizeof( ib_file_t ));
	tmp = NEWTYPE( ib_file_t );
//	printf( "open: ptr: %p\n", tmp );

	tmp->Read = DISK_Read;
	tmp->GetSize = DISK_GetSize;
	tmp->SetPos = DISK_SetPos;
	tmp->GetPos = DISK_GetPos;
	tmp->Close = DISK_Close;
	
	tmp->private_.disk.h = h;
	return tmp;
}

ib_source_t *DISK_AddSource( const char *name )
{
	ib_source_t	*tmp;
	
	__message( "\t\tadded \"%s\" [dir]\n", name );
	
	tmp = NEWTYPE(ib_source_t );
	__chkptr( tmp );

	tmp->OpenFile = DISK_Open;
	tmp->Delete = DISK_Delete;
	
	if( strlen( name ) >= 255 )
	{
		__warning( "path too long. ignored.\n" );
		return NULL;
	}
	strcpy( tmp->private_.disk.dir, name );
	
	return tmp;
}

// ========================================
// ib_file_t sar members
int SAR_Read( void *buf, unsigned int size, ib_file_t *self )
{
// 	int	res;
// 	char	*c;
	int	n;

	if( self->private_.sar.pointer >= self->private_.sar.size )
		return 0;

	if( ( self->private_.sar.pointer + size ) > self->private_.sar.size )
	{
		n = self->private_.sar.size - self->private_.sar.pointer;
//		printf( "read to much: %d\n", n );
	} else
		n = size;

	self->private_.sar.pointer += n;
	
#if 0 // single byte read speed up fake. now replaced by IB_GetChar
	if( size == 1 )
	{
		res = getc( self->private_.sar.h );
		if( res == EOF )
			__error( "EOF in sar. shame!\n" );
		
		c = buf;
		*c = ( char )res;
		return 1;
	} else
#endif
		fread( buf, n, 1, self->private_.sar.h );

	return n;
}

unsigned int SAR_GetSize( ib_file_t *self )
{
//	__named_message( "%d\n", self->private_.sar.size );
	return self->private_.sar.size;
}

void SAR_SetPos( ib_file_t *self, unsigned int pos )
{
//q	__named_message( "\n" );
	self->private_.sar.pointer = pos;

	fseek( self->private_.sar.h, self->private_.sar.pos + pos, SEEK_SET );
}

unsigned int SAR_GetPos( ib_file_t *self )
{
	return self->private_.sar.pointer;
}

void SAR_Close( ib_file_t *self )
{
	fclose( self->private_.sar.h );
	
	MM_Free( self );
} 

// ========================================
// ib_source_t sar members

const void *SAR_GetKey( const void *obj )
{
	sar_dir_entry_t	*ent;
	
	ent = ( sar_dir_entry_t * ) obj;

	return ent->name;
}

int SAR_CompareKey( const void *k1, const void *k2 )
{
	return strcmp( ( char * ) k1, ( char * ) k2 );
}

void SAR_SourceFree( void *ptr )
{
	MM_Free( ptr );
}

void SAR_Delete( ib_source_t *self )
{
//	U_MapDestroy( self->private_.sar.name_map, SAR_SourceFree );
	U_CleanUpMap( self->private_.sar.name_map, SAR_SourceFree );
	FREE( self->private_.sar.name_map );

	MM_Free( self );
}

ib_file_t *SAR_Open( char *name, ib_source_t *self )
{
	sar_dir_entry_t	*ent;
	ib_file_t	*tmp;
	FILE	*h;

//	__named_message( "trying: %s\n", self->private_.sar.name );

	__chkptr( self->private_.sar.name_map );
	ent = ( sar_dir_entry_t	* )U_MapSearch( self->private_.sar.name_map, ( void * )name );
	if( !ent )
		return NULL;

	h = fopen( self->private_.sar.name, FOPEN_READ );
	if( !h )
	{
		__warning( "sarfile disapeared! %s\n", name );
		return NULL;
	}
    
	//	tmp = MM_Malloc( sizeof( ib_file_t ));
	tmp = NEWTYPE( ib_file_t );
	__chkptr( tmp );

	tmp->Read = SAR_Read;
	tmp->GetSize = SAR_GetSize;
	tmp->SetPos = SAR_SetPos;
	tmp->GetPos = SAR_GetPos;
	tmp->Close = SAR_Close;
	
	tmp->private_.sar.h = h;
	tmp->private_.sar.pos = ent->pos;
	tmp->private_.sar.size = ent->size;
	tmp->private_.sar.pointer = 0;

	fseek( h, ent->pos, SEEK_SET );
	return tmp;
}

ib_source_t *SAR_AddSource( const char *name )
{
	int	i, dirofs, filenum, checksum;

	char	head_buf[SAR_HEADER_SIZE];
	char	ent_buf[SAR_DIRENT_SIZE];

	FILE	*h;
	ib_source_t	*tmp;
	sar_dir_entry_t	*ent;
	
	__chkptr( name );
	
	h = fopen( name, FOPEN_READ );
	if( !h )
	{
		__warning( "cannot open '%s'\n", name );
		return NULL;
	}
	
	if( ib_lastsarnames != 0 ) {
		for( i = 0; i < ib_lastsarsize; i++ ) {
			free( ib_lastsarnames[i] );
		}
		
		free( ib_lastsarnames );
		ib_lastsarnames = 0;
	}



	fread( head_buf, SAR_HEADER_SIZE, 1, h );
	{
		char	id[4], type[4];

		U_BeginUnpack( U_PACKED_BIN, head_buf, SAR_HEADER_SIZE );
		
		U_UnpackString( id, SAR_ID_SIZE );
		U_UnpackString( type, SAR_TYPE_SIZE );

		if( memcmp( id, "CSAR", SAR_ID_SIZE ))
		{
			__warning( "wrong id in sarfile. failed\n" );
			fclose( h );
			return NULL;
		}

		if( memcmp( type, "2new", SAR_TYPE_SIZE ))
		{
			__warning( "wrong sar type. failed\n" );
			fclose( h );
			return NULL;
		}
	}
		
	U_Unpacks32( &dirofs );
	U_Unpacks32( &filenum );
	U_Unpacks32( &checksum );

	U_EndUnpack();	    

	ib_lastsarnames = (char **) malloc( filenum * sizeof( char *) );
	memset( ib_lastsarnames, 0, filenum * sizeof( char * ));
	ib_lastsarsize = filenum;
	
	fseek( h, dirofs, SEEK_SET );
	
	tmp = NEWTYPE(ib_source_t);
	__chkptr( tmp );

	tmp->OpenFile = SAR_Open;
	tmp->Delete = SAR_Delete;

	strcpy( tmp->private_.sar.name, name );

	tmp->private_.sar.name_map = NEWTYPE( u_map_t );
	__chkptr( tmp->private_.sar.name_map );
	U_InitMap( tmp->private_.sar.name_map, map_default,  SAR_CompareKey, SAR_GetKey );
	
	for( i = 0; i < filenum; i++ )
	{
//		char	filename[SAR_FILENAME_SIZE];
		
		ent = NEWTYPE( sar_dir_entry_t );
		
		__chkptr( ent );
		
		fread( ent_buf, SAR_DIRENT_SIZE, 1, h );
		
		U_BeginUnpack( U_PACKED_BIN, ent_buf, SAR_DIRENT_SIZE );
		U_UnpackString( ent->name, SAR_FILENAME_SIZE );
		ent->name[SAR_FILENAME_SIZE-1] = '\0';
		
		U_Unpacks32( &( ent->pos ));
		U_Unpacks32( &( ent->size ));
		U_EndUnpack();

		U_MapInsert( tmp->private_.sar.name_map, ( void * )ent );
		
		ib_lastsarnames[i] = strdup( ent->name );
	}
	fclose( h );

	__message( "\t\tadded \"%s\" [%d]\n", name, filenum );
	return tmp;
}

// ========================================
// regular IB interface

void IB_StartUp()
{
	int	i;
	__named_message( "\n" );
	ib_srclast = 0;
	for( i = 0; i < IB_SRC_NUM; i++ )
	{
		ib_src[i] = NULL;
	}
	memset( ib_lastname, 0, 255 );
}

void IB_ShutDown()
{
	int	i;
	
	for( i = 0; i < IB_SRC_NUM; i++ )
	{
		if( !ib_src[i] )
			continue;

		if( ib_src[i]->Delete )
		{
			ib_src[i]->Delete( ib_src[i] );
			ib_src[i] = NULL;
		}
		else
			__warning( "no delete function in src %d\n", i );
	}
	__named_message( "\n" );
}

int IB_ProbeSource( char *name )
{
	int	stype;

	//test for extension
	if( !memcmp( name+( strlen( name ) - 4 ), ".sar", 4 ))
	{
		// is sar
		stype = SOURCE_SAR;
		return stype;
	}
	// is a directory
	return SOURCE_DISK;
}

void IB_AddSource( const char *name, int stype )
{
	ib_source_t	*src;
	
	if( !ib_srclast >= IB_SRC_NUM - 1 )
	{
		__warning( "one source too much! not added: '%d'\n", ib_srclast );
		return;
	}

	switch( stype )
	{
	case SOURCE_DISK:
		src = DISK_AddSource( name );
		if( !src )
		{
			__warning( "DISK_AddSource failed.\n" );
			return;
		}
		ib_src[ib_srclast] = src;
		ib_srclast++;
		break;

	case SOURCE_SAR:
		src = SAR_AddSource( name );
		if( !src )
		{
			__warning( "SAR_AddSource failed.\n" );
			return;
		}
		ib_src[ib_srclast] = src;
		ib_srclast++;
		break;

	default:
		__error( "type not implemented: %d\n", stype );
		break;
	}
}

static void *bzalloc( void *bla, int n, int m )
{
//	__named_message( "%d\n", n * m );

	return MM_Malloc( n * m );
}

static void bzfree( void *bla, void *p )
{
//	__named_message( "\n" );
	MM_Free( p );
}


ib_file_t *IB_Open( const char *_name )
{
	ib_file_t	*tmp = NULL;
	int	i;
// 	char	*ext;
	char	name[256];
	int	try_ = 0;



	if( !ib_srclast )
	{
		__warning( "no sources to search in\n" );
		return NULL;
	}


	if( _name[0] == '/' )
	{
		__warning( "path starting with / : '%s'. cutting!\n", _name );
		strcpy( name, _name+1 );
	} else
		strcpy( name, _name );

//	__named_message( "%s\n", name );


		

	for( ;; try_++)
	{
		if( (try_ == 1) )
		{
		       
			strcat( name, ".sbz" );

			continue;
		}
//		__named_message( "trying %s\n", name );
		for( i = ib_srclast - 1; i >= 0; i-- )
		{
			__chkptr( ib_src[i] );
			__chkptr( ib_src[i]->OpenFile );
			
			tmp = ib_src[i]->OpenFile( name, ib_src[i] );
			if( tmp )
			{
				strncpy( ib_lastname, name, 254 );
				break;
			}
		}
	


		if( ( !tmp ) && ( try_ > 1 ))
		{
			__warning( "file not found: '%s'\n", name );
			return NULL;
		}
		if( tmp )
			break;

	}

	ib_lastaction = gc_renderframe; 
	tmp->getcbuf = NULL;
	tmp->getcpos = tmp->getcdata = 0;
	tmp->bz = NULL;
	

	if( try_ ) // .sbz found
	{
		int	ret, itmp;
		char	id[4];
		

//		__named_message( "file is bz2 compressed\n" );
		tmp->bz = NEWTYPE( ib_bzdata_t );
		__chkptr( tmp->bz );

//		printf( "sizeof( ib_bzdata_t ) == %d\n", sizeof( ib_bzdata_t ));

		tmp->bz->bufN = 0;
		tmp->bz->strm.bzalloc = /*NULL;*/ bzalloc;
		tmp->bz->strm.bzfree = /*NULL;*/ bzfree;
		tmp->bz->strm.opaque = NULL;
		
			ret = BZ2_bzDecompressInit( &(tmp->bz->strm), 4, 0 );
		
		if( ret != BZ_OK )
		{
			__warning( "cannot initialize Decompressor\n" );
			MM_Free( tmp );
			tmp = NULL;
		}
		
		tmp->bz->strm.avail_in = tmp->bz->bufN;
		tmp->bz->strm.next_in = tmp->bz->buf;

		tmp->Read( id, 4, tmp );
//		printf( "id is '%s'\n", id );
		if( memcmp( id, "SBZ2", 4 ))
		{
			__warning( "wrong id in sbz file\n" );
			return NULL;
		}
		
	
		tmp->Read( &itmp, 4, tmp );

		tmp->bz->filesize = INT( itmp );
//		printf( "filesize: %d %d\n", tmp->bz->filesize, itmp );
		
		

	}

	strncpy( tmp->filename, name, 255 );


	return tmp;
}

ib_file_t *IB_OpenDiskFile( const char *name )
{
	ib_file_t	*tmp;
	FILE	*h;

	//	tmp = MM_Malloc( sizeof( ib_file_t ));
	tmp = NEWTYPE( ib_file_t );
		//memset( tmp, 0, sizeof( ib_file_t ));

		//	tmp->bz = 0;

	__chkptr( tmp );

	h = fopen( name, FOPEN_READ );
	if( !h )

		return NULL;

	tmp->Read = DISK_Read;
	tmp->GetSize = DISK_GetSize;
	tmp->SetPos = DISK_SetPos;
	tmp->GetPos = DISK_GetPos;
	tmp->Close = DISK_Close;
	
	tmp->private_.disk.h = h;
	return tmp;
}
	
unsigned int IB_Read( void *buf, unsigned int size, unsigned int blocks, ib_file_t *file )
{
	unsigned int	res;
    int n = 0;
    int ret;
	
	__chkptr( file );
	__chkptr( buf );

	if( blocks != 1 )
		__error( "someone used blocks != 1\n" );

	if( file->bz )
	{
		file->bz->strm.avail_out = size;
		file->bz->strm.next_out = (char *)buf;
	
		for( ;; )
		{

			if( file->bz->strm.avail_in == 0 ) 
			{

				n = file->Read( file->bz->buf, BZ_MAX_UNUSED, file );
//			printf( "read: %d\n", n );
#if 0
				if( n )
				{
					FILE	*h;
					h = fopen( "bzlump", "w" );
					fwrite( file->bz->buf, n, 1, h );
					fclose( h );
					printf( "wrote bz data\n" );

				}

#endif
				if( !n )
					__warning( "n == 0. That _may_ cause problems\n" ); 
				file->bz->bufN = n;
				file->bz->strm.avail_in = n;
				file->bz->strm.next_in = file->bz->buf;
			}
		       	ret = BZ2_bzDecompress( &( file->bz->strm ) );

			if( ret != BZ_OK && ret != BZ_STREAM_END )
			{
				__error( "bz error: %d\n", ret );
			}

//			printf( "ret: %d\n", ret ); 

#if 0
			if( ret == BZ_OK )
				printf( "BZ_OK, avail_out: %d\n", file->bz->strm.avail_out );

#endif

#if 1

			if( ret == BZ_OK && n == 0 && file->bz->strm.avail_in == 0 && file->bz->strm.avail_out > 0 )
			{
				res =  size - file->bz->strm.avail_out;
				__warning( "unexpected end of bz2 file '%s'\n", file->filename );
				goto outside;
			}
#endif

			if( ret == BZ_STREAM_END )
			{
				res = size - file->bz->strm.avail_out;
				printf( "stream end\n" );
				goto outside;
			}

			if( file->bz->strm.avail_out == 0 )
			{
				res = size;
//				printf( "avail_out\n" );
				goto outside;
			}
			
		}
		
	} else
	{
		res = file->Read( buf, size, file );
	}

 outside:
	sc_readbytes += res;

	return res;
}

unsigned int IB_GetSize( ib_file_t *file )
{
	unsigned int	res;

	__chkptr( file );

	printf( "bz: %p\n", file->bz );

	if( file->bz )
	{
		res = file->bz->filesize;
	} else 
	{
		res = file->GetSize( file );
	}
	return res;
}

void IB_SetPos( ib_file_t *file, unsigned int pos )
{
	__chkptr( file );

	if( file->bz )
	{
		__error( "this doesn't work on bzipped file: '%s'\n", file->filename );
	}
	file->SetPos( file, pos );
}

unsigned int IB_GetPos( ib_file_t *file )
{
	unsigned int res;

	__chkptr( file );


	if( file->bz )
	{
		__error( "this doesn't work on bzipped file: '%s'\n", file->filename );
	}

	res = file->GetPos( file );
	return res;
}

int IB_GetChar( ib_file_t *file )
{
	unsigned char	c;
	int	i;

	__chkptr( file );
//	__named_message( "%p %p %d\n", file->getcbuf, file->bz, file->getcpos );


#if 0 //!defined (win32_x86)

	if( !file->getcbuf )
	{
//		__named_message( "mallocing getc buf %d\n", IB_GETC_BUFSIZE );
		file->getcbuf = MM_Malloc( IB_GETC_BUFSIZE );
//		printf( "ptr: %p\n", file->getcbuf );
		__chkptr( file->getcbuf );
		
		file->getcdata = IB_Read( file->getcbuf, IB_GETC_BUFSIZE, 1, file );
		file->getcpos = 0;
//		printf( "ready\n" );
	}

	if( file->getcpos >= IB_GETC_BUFSIZE )
	{
//		__named_message( "reading %d\n", IB_GETC_BUFSIZE );
		file->getcdata = IB_Read( file->getcbuf, IB_GETC_BUFSIZE, 1, file );
		file->getcpos = 0;
	}

	if( file->getcpos >= file->getcdata )
	{
		printf( "end. ( %s )\n", file->filename );
		return EOF;
	}
		
	c = file->getcbuf[file->getcpos];
	file->getcpos++;

	i = c;
#else
	int size = IB_Read( &c, 1, 1, file );

	if( size == 1 ) {
	  i = c;
	} else {
	  i = EOF;
	}
#endif

//	if( !strcmp( file->filename, "world/e1m2_bz.d/fake/_mapnode.hobj.sbz" ))
//		fprintf( stderr, "%c", i ); 


	return i;
	
}	

void IB_Skip( ib_file_t *file, int size )
{
//  	char c;
	int i;

	for( i = 0; i < size; i++ )
	{
		/*c = */IB_GetChar( file );
	}
}

short IB_GetShort( ib_file_t *file )
{
	short s;
	char *ptr;

	ptr = (char*)(&s);

	ptr[0] = IB_GetChar( file );
	ptr[1] = IB_GetChar( file );

	s = SHORT( s );
	return s;
}
	
int IB_GetInt( ib_file_t *file )
{
	int i;
	char *ptr;

	ptr = (char *)(&i);

	ptr[0] = IB_GetChar( file );
	ptr[1] = IB_GetChar( file );
	ptr[2] = IB_GetChar( file );
	ptr[3] = IB_GetChar( file );

	i = SHORT( i );
	return i;
}

void IB_Close( ib_file_t *file )
{
	__chkptr( file );

	
	if( file->getcbuf )
		MM_Free( file->getcbuf );

	if( file->bz ) // free data used by bzip2 compression
	{
		int	ret;
	      		ret = BZ2_bzDecompressEnd( &(file->bz->strm) );
		if( ret != BZ_OK )
		{
			__warning( "bzDecompressEnd returned %d\n", ret );
		}
		MM_Free( file->bz );
	}

	file->Close( file ); // close handles and _free_ file!


}


char *IB_GetLastname()
{
	return ib_lastname;
}


void IB_DumpLastSar( const char *outdir ) {
	return;
	int i, j;
	for( i = 0; i < ib_lastsarsize; i++ ) {
		char tmp[1024];
		char tmp2[1024];
		char buf[4096];
		
		char *name = ib_lastsarnames[i];
		memset( tmp, 0, 1024 );
		memset( tmp2, 0, 1024 );
		
		strncpy( tmp2, name, 1024 );
		
		
		if( strlen(tmp2) > 4 && memcmp( &tmp2[strlen(tmp2) - 4], ".sbz", 4 ) == 0 ) {
			tmp2[strlen(tmp2) - 4] = 0;
		}
		ib_file_t *h = IB_Open( tmp2 );
	
		
		
		
		for( j = 0; j < 1024 && tmp2[j] != 0; j++ ) {
			if( tmp2[j] == '/' ) {
				tmp2[j] = '@';
			}
		}
		
		strncat( tmp, outdir, 1024 );
		strncat( tmp, "/", 1024 );
		strncat( tmp, tmp2, 1024 );
		
		
		
		FILE *oh = fopen( tmp, "wb" );
		
		printf( "dump %s %p\n", tmp, oh );
		
// 		int n;
		
		// the sbz shit (some horrible last minute hack, IIRC) seems to be quite unstable when EOF is hit. We need
		// to keep track of the EOF ourselves.
		int size = IB_GetSize( h );
		while( size > 0 ) {
			int rs;
			if( size > 4096 ) {
				rs = 4096;
			} else {
				rs = size;
			}
			
			size -= rs;
			
			IB_Read( buf, rs, 1, h );
			fwrite( buf, rs, 1, oh );
		}
		
		fclose( oh );
		IB_Close( h );
	}
}
