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



#include "l_locals.h"


static hobj_t	*ats_hobj;


void SP_Thing( arche_t* at )
{
	
}


sp_field_t sp_fields[] = {
	{ "testobj", NEW_testobj },
	{ "pos", NEW_pos },
	{ "sound", NEW_sound },
	{ "waypoint", NEW_waypoint },
	{ "dlight", NEW_dlight },
	{ "psys", NEW_psys },
	{ "switch", NEW_switch },
	{ "light", NEW_light },
	{ "spotlight", NEW_light },
	{ "facelight", NEW_light },
	{ "item", NEW_item },
	{ NULL, NULL }
};

at_field_t at_fields[] = {
	{ "origin", SP_ATMOFS( origin ), F_VECTOR },
	{ NULL, 0, F_INT }
};

unsigned char *AT_StrCopyToNew( unsigned char* buf )
{
	unsigned char	*ptr;
	int	l;
	
	l = strlen( buf ) + 1;

	ptr = ( unsigned char* ) li.MM_Malloc( l );
	strcpy( ptr, buf );
//	li.SH_Printf( "s1: %s", ptr );
	return	ptr;
}

void AT_ParseField( unsigned char *key, unsigned char *value, arche_t *at )
{
	int	i;
	at_field_t	*field;
	unsigned char	*at_b;
	vec3d_t		vec;

	void		*ptr;


	for( i = 0; i < 255; i++ )
	{
		if( !at_fields[i].key )
			break;

		if( !strcmp( key, at_fields[i].key ))
		{
			field = &at_fields[i];
			at_b = ( unsigned char * ) at;
			//li.SH_Printf( "found field: %s", field->key );
//			ptr = ( at_b + *(int*)(at_b+(at->ags[field->at_ofs]))+field->ag_ofs);
//			ptr = ( at_b + at->ags[field->at_ofs] + field->ag_ofs);
			
			ptr = at_b + at_fields[i].at_ofs;
				
			switch( field->ftype )
			{
			case F_INT:
				__named_message( "int\n" );
				*(( int * )ptr) = atoi( value );
				break;
		

#if 1	
			case F_VECTOR:
				sscanf( value, "%f %f %f", &vec[0], &vec[1], &vec[2] );
				__named_message( "vector\n" );
				(( float * )ptr)[0] = vec[0];
				(( float * )ptr)[1] = vec[1];
				(( float * )ptr)[2] = vec[2];
			
				break;
				
			case F_FLOAT:
				*(( float * )ptr) = atof( value );
				break;
				
			case F_STRING:
				*(( char ** )ptr) = AT_StrCopyToNew( value );
				//at->type = AT_StrCopyToNew( value );
//				li.SH_Printf( "%d string: %s %s", i, *( char ** )( at_b+field->ofs ), at->type );
//				li.SH_Printf( "at2: %p", at );
				break;

#endif
			default:
				__error( "unhandled field type\n" );
			}
			
			return;
		}
	}
	li.SH_Printf( "ParseAT: no such field: %s %s", key, value );
}


void AT_InsertPair( hpair_t *pair, arche_t *at )
{
	int	i, found = 0;;


	AT_ParseField( pair->key, pair->value, at );

#if 0
	for( i = 0; ; i++ )
	{
		if( !at_fields[i].key )
			break;


		if( !strcmp( at_fields[i].key, pair->key ))
		{
			li.SH_Printf( "found pair %s %s", pair->key, pair->value );
		}
	}
#endif
}

void SP_InitDefaults( arche_t *at, char *classname )
{
	hobj_t	*defs;
	hpair_search_iterator_t	iter;
	hpair_t	*pair;

	if( strlen( classname ) >= 31 )
		__error( "classname %s too long\n", classname );

	strcpy( at->classname, classname );

	defs = FindClass( at_prefs, classname );
	if( !defs )
		__error( "no at defaults for %s\n", classname );

	InitHPairSearchIterator( &iter, defs, "*" );

	for( ; ( pair = SearchGetNextHPair( &iter )); )
	{
//		li.SH_Printf( " def: %s", pair->key );
		AT_InsertPair( pair, at );
			
	}
}	     

#if 0
void SP_ByClass( hobj_t *cls )
{
	arche_t	*at;
	hpair_search_iterator_t	iter;
	hpair_t		*pair;

	int	found = 0;
	int	i;
	


	for( i = 0; ; i++ )
	{
		if( !sp_fields[i].type )
			break;

		if( !strcmp( cls->type, sp_fields[i].type ))
		{
			found = 1;
			break;
		}
	}

	if( !found )
	{
		li.SH_Printf( "no spawnfunc for %s", cls->type );
		return;
	}

	

	InitHPairSearchIterator( &iter, cls, "*" );
	li.SH_Printf( "spawning %s", cls->type );
	at = L_GetArcheT();

	SP_InitDefaults( at, cls->type );
	

	for( ; ( pair = SearchGetNextHPair( &iter ) ); )
	{
		AT_InsertPair( pair, at );
	}
	li.SH_Printf( "origin: %f %f %f", at->origin[0], at->origin[1], at->origin[2] );
}

#endif

void SP_TryByClass( hobj_t *cls )
{
	int	i;
	void *(*func)( void ) = NULL;
	ati_t	*ati;
	vec3d_t	vec;

	for( i = 0; ; i++ )
	{
		if( !sp_fields[i].type )
			break;

		if( !strcmp( cls->type, sp_fields[i].type ))
		{
			func = sp_fields[i].spawnfunc;
			break;
		}
	}

	if ( func )
	{
		ati = ( ati_t * )func();
		ati->ftbl->InitFromClass( ati, cls );
		ati->ftbl->GetOrigin( ati, vec );
//		L_InsertArcheT( ati );
//		L_RegisterNewArcheType( ati );
		L_ChangeToWakeUp( ati );
		printf( "[%s,%s] origin %f %f %f\n", cls->type, cls->name, vec[0], vec[1], vec[2] );
	}
	else
	{
//		__warning( "no spawnfunc for '%s'\n", class->type );
	}
}

void WorldSpring( char * name )
{
	char	tt[256];
	hobj_search_iterator_t	iter;
	hobj_t	*cls;
	hobj_t	*root;

	li.SH_Printf( "hack: load lightsource class" );
	sprintf( tt, "world/%s.d/fake/_light_source.hobj", name ); 
	root = li.GI_ReadClass( tt );
	if ( !root )
	{
		__error( "can't load lightsource class\n" );
	}
	l_lightsource_root = root;

	sprintf( tt, "world/%s.d/ats.hobj", name ); 
	li.SH_Printf( "spawning from %s", tt );
	ats_hobj = li.GI_ReadClass( tt );	

	InitClassSearchIterator( &iter, ats_hobj, "*" );

	for( ; ( cls = SearchGetNextClass( &iter ) ); )
	{
//		li.SH_Printf( "%s %s", cls->type, cls->name );
		SP_TryByClass( cls );
	}
}

void PlayerSpring( char *name )
{
	if ( at_num_player >= L_PLAYERNUM )
		Error( "reached L_PLAYERNUM\n" );

	local_player = at_num_player;		// fixme
	at_players[at_num_player] = NEW_player( name );      
	L_ChangeToWakeUp( (ati_t *) at_players[at_num_player] );

	at_num_player++;
}
