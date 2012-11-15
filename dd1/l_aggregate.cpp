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



// r_aggregate.c

#include "l_locals.h"

static save_field_t render_fields[] = {
	{ "model_num", SP_FOFS( ag_render_t, mindex ), F_INT },
	{ "origin", SP_FOFS( ag_render_t, origin ), F_VECTOR },
	{ "roll", SP_FOFS( ag_render_t, roll ), F_FLOAT },
	{ "pitch", SP_FOFS( ag_render_t, pitch ), F_FLOAT },
	{ "yaw", SP_FOFS( ag_render_t, yaw ), F_FLOAT },
	{ "frame", SP_FOFS( ag_render_t, frame ), F_INT },

	{ NULL, 0, 0 }
};

static save_field_t test_fields[] = {
	{ "a", SP_FOFS( ag_test_t, a ), F_INT },
	{ "b", SP_FOFS( ag_test_t, b ), F_INT },
	{ "c", SP_FOFS( ag_test_t, c ), F_INT },

	{ NULL, 0, 0 }
};

//
// experiment

int AG_TableChecksum( save_field_t *field )
{
	int	sum = 0;
	int	i;

	for( i = 0; field->key; field++, i++ )
	{
		sum += field->ofs;
		sum += field->ftype;
		sum *= i;
	}
	return sum;
}

int AG_Checksum()
{
	int sum = 0;

	sum += AG_TableChecksum( render_fields );
	sum += AG_TableChecksum( test_fields );
	
	return sum;
}


void AG_Save( void *ag, save_field_t *field )
{
	unsigned char	*ag_c;
	void		*ptr;

	ag_c = ( unsigned char * )ag;

	for( ; field->key; field++ )
	{

		ptr = ( void *) (ag_c + field->ofs );

		switch( field->ftype )
		{
		case F_INT:
			SAVE_PackInt( field->key, *(( int * )ptr) );
			__named_message( "%d\n", *( int * )ptr );
			break;

		case F_FLOAT:
			SAVE_PackFloat( field->key, *(( float * )ptr) );
			break;

		case F_VECTOR:
#if 0
			fprintf( h, "\"%f %f %f\"", (( float * )ptr)[0]
				 , (( float * )ptr)[1], (( float * )ptr)[2] );
#endif
			SAVE_PackVec3d( field->key, *(( vec3d_t * )ptr) );
			break;

		default:
			__error( "unknow field type\n" );
		}
	}
}

void AG_Load( void *ag, save_field_t *field )
{
	void	*ptr;
	char	*ag_c;
	vec3d_t	vec;

	ag_c = ( char* )ag;

	for( ;field->key; field++ )
	{
		ptr = ( void * ) ( ag_c + field->ofs );
		
		switch( field->ftype )
		{
		case F_INT:
			*( int * )ptr = LOAD_UnpackInt();
			break;

		case F_FLOAT:
			*( float *)ptr = LOAD_UnpackFloat();
#if 0
		case F_VECTOR:
			vec = LOAD_UnpackVec3d();
			
			*( float * )ptr = vec[0];
			*(( float * )ptr + 1 ) vec[1];
			*(( float * )ptr + 2 ) vec[2];
			break;
#endif
		default:
			__error( "unhandeled field type\n" );
		}
	}
}

void SP_AGRender( arche_t *at )
{
	ag_render_t	*ag;

	ag = AG_RENDER( at );
	ag -> roll = ag->pitch = ag->yaw = 111;

}

int AG_SizeofRender()
{
	return( sizeof( ag_render_t ));
}

void SAVE_AGRender( ag_render_t *ag )
{
	AG_Save( ag, render_fields );
}


void SP_AGTest( arche_t *at )
{
	ag_test_t	*ag;

	ag = AG_TEST( at );
	ag -> a = ag -> b = ag -> c = 111;

}

int AG_SizeofTest()
{
	return( sizeof( ag_test_t ));
}

void SAVE_AGTest( ag_test_t *ag )
{
	AG_Save( ag, test_fields );
}
