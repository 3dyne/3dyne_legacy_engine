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

#include "interfaces.h"

#include "lib_hobjdefs.h"
#include "lib_hobj.h"
#include "u_hobj.h"

#if 0
/*
  ========================================
  hobj file io

  ========================================
*/
static int		cur_deep;
static FILE		*cur_h;

void WriteIndent( void )
{
	int		i;
	for ( i = 0; i < cur_deep; i++ )
		fprintf( cur_h, "\t" );
}

void WriteClassRecursive( hobj_t *self, int ignore_deep )
{
	hobj_t		*o;
	hpair_t		*p;
	int		len;
	char		*ptr;

	if ( cur_deep != ignore_deep )
	{
		WriteIndent();
		fprintf( cur_h, "class \"%s\" \"%s\"\n", self->type, self->name );
		WriteIndent();
		fprintf( cur_h, "{\n" );
	}
		
	cur_deep++;


	for ( o = self->hobjs; o ; o=o->next )
	{
		WriteClassRecursive( o, ignore_deep );
	}
	
	for ( p = self->pairs; p ; p=p->next )
	{
		WriteIndent();
		len = strlen( p->value );
		if ( len <= HPAIR_VALUE_FRAG )
		{				
			fprintf( cur_h, "\"%s\" \"%s\" \"%s\"\n", p->type, p->key, p->value );
		}
		else
		{
			cur_deep++;
			fprintf( cur_h, "\"%s\" \"%s\"\n", p->type, p->key );
			WriteIndent();
			fprintf( cur_h, "\"%.*s\"\n", HPAIR_VALUE_FRAG, p->value );
			ptr = p->value+HPAIR_VALUE_FRAG;
			len-=HPAIR_VALUE_FRAG;
			for ( ; len > HPAIR_VALUE_FRAG; len-=HPAIR_VALUE_FRAG, ptr+=HPAIR_VALUE_FRAG )
			{
				WriteIndent();			
				fprintf( cur_h, "\"~%.*s\"\n", HPAIR_VALUE_FRAG, ptr );	
			}
			if ( len )
			{
				WriteIndent();
				fprintf( cur_h, "\"~%s\"\n", ptr );	
			}
			cur_deep--;
		}
	}

	cur_deep--;

	if ( cur_deep != ignore_deep )
	{
		WriteIndent();
		fprintf( cur_h, "}\n" );
	}
}

void WriteClass( hobj_t *self, FILE *h )
{
	cur_deep = 0;
	cur_h = h;
	WriteClassRecursive( self, -1 );
}

void WriteClassWithoutRoot( hobj_t *self, FILE *h )
{
	cur_deep = 0;
	cur_h = h;
	WriteClassRecursive( self, 0 );
	fprintf( h, "end" );
}

#endif
/*
  ========================================
  ReadClass

  ========================================
*/

//
// this version uses a tmp buffer
// 

hobj_t* ReadClassRecursive( tokenstream_t *ts )
{
	char		type[HOBJ_TYPE_SIZE];
	char		name[HOBJ_NAME_SIZE];

	hobj_t		*self;
	hobj_t		*hobj;
	hpair_t		*pair;

	int		len;
//	char		*ptr;

	char		tmpbuf[0x10000];

	// begin new class
	
	if( GetToken( ts ) == TOKEN_FAIL )
	{
		__error( "TOKEN FAIL. unexpected end of class\n" );
	}

	if ( strcmp( ts->token, "obj" ) && strcmp( ts->token, "class" ))
		__error( "ReadClass: expected 'class' token.\n" );
//	printf( "%s\n", ts->token ); getchar();

	// get type

	if( GetToken( ts ) == TOKEN_FAIL )
	{
		__error( "TOKEN FAIL. unexpected end of class\n" );
	}
//	printf( "%s\n", ts->token ); getchar();
	strcpy( type, ts->token );
	
	// get name

	if( GetToken( ts ) == TOKEN_FAIL )
	{
		__error( "TOKEN FAIL. unexpected end of class\n" );
	}
//	printf( "%s\n", ts->token ); getchar();
	strcpy( name, ts->token );

	self = NewClass( type, name );
	
	// expect '{'

	if( GetToken( ts ) == TOKEN_FAIL )
	{
		__error( "TOKEN FAIL. unexpected end of class\n" );
	}
//	printf( "%s\n", ts->token ); getchar();
	if ( ts->token[0] != '{' )
		__error( "ReadClassRecursive: expected '{' of class.\n" );

	for(;;)
	{
		if( GetToken( ts ) == TOKEN_FAIL )
		{
			__error( "TOKEN FAIL. unexpected end of class\n" );
		}
		


		if ( ts->token[0] == '}' )
			break; // finish class

		if ( !strcmp( ts->token, "obj" ) || !strcmp( ts->token, "class" ) )
		{			
			KeepToken( ts );
			hobj = ReadClassRecursive( ts );
			InsertClass( self, hobj );
			continue;
		}



		// it's a pair
		pair = NewHPair();

		// type
		strcpy( pair->type, ts->token );
		// get key
		if( GetToken( ts ) == TOKEN_FAIL )
		{
			__error( "TOKEN FAIL. unexpected end of class\n" );
		}

		strcpy( pair->key, ts->token );
		// get value

		if( GetToken( ts ) == TOKEN_FAIL )
		{
			__error( "TOKEN FAIL. unexpected end of class\n" );
		}
		__chkptr( pair->value );

		MM_Free( pair->value );

		strcpy( tmpbuf, ts->token );

		// is it a mulit token value ?
		for (;;)
		{
			if( GetToken( ts ) == TOKEN_FAIL )
			{
				__error( "TOKEN FAIL. unexpected end of class\n" );
			}
			
			if ( ts->token[0] == '~' )
			{
				// yes
				len = strlen( tmpbuf );
//				ptr = realloc( pair->value, len + strlen( ts->token ) + 2 );
//				__chkptr( ptr );
				strcpy( tmpbuf+len, &ts->token[1] );
//				pair->value = ptr;
			}
			else
			{
				KeepToken( ts );
				break;
			}
		}

		pair->value = (char *)MM_Malloc( strlen( tmpbuf ) + 2 );
		__chkptr( pair->value );
		strcpy( pair->value, tmpbuf );

		InsertHPair( self, pair );
//		printf( "pair: %s %s %s\n", pair->type, pair->key, pair->value );
		
	}

	return self;
}

hobj_t* ReadClass( tokenstream_t *ts )
{
	hobj_t	*root;

	if( GetToken( ts ) == TOKEN_FAIL )
	{
		__error( "TOKEN FAIL. unexpected end of class\n" );
	}
	printf( "token: %s\n", ts->token );
	if ( strcmp( ts->token, "obj" ) && strcmp( ts->token, "class" ))
	{
		printf( "ReadClass: expected 'class' token.\n" );
		return NULL;
	}

	KeepToken( ts );
	root = ReadClassRecursive( ts );
	
	return root;
}

