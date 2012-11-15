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



// r_videoscape.c

/*
  =============================================================================

  load videoScape objects from blender

  =============================================================================
*/

#include "r_interfaces.h"
#include "r_videoscape.h"

/*
  ==============================
  VSC_NewModel

  ==============================
*/

vsc_model_t*	VSC_NewModel( int pointnum )
{
	vsc_model_t	*m;
	size_t		size;

	size = (size_t)((vsc_model_t *)0)->p[pointnum];
	m = (vsc_model_t *) NEWBYTES( size );

	return m;
}


/*
  ==============================
  VSC_FreeModel

  ==============================
*/
void VSC_FreeModel( vsc_model_t *model )
{
	FREE( model );
}


/*
  ==============================
  VSC_NewFace

  ==============================
*/
vsc_face_t* VSC_NewFace( int pointnum )
{
	vsc_face_t	*f;
	size_t		size;

	size = (size_t)&((vsc_face_t *)0)->p[pointnum];
	f = (vsc_face_t *) NEWBYTES( size );

	return f;
}


/*
  ==============================
  VSC_FreeFace

  ==============================
*/
void VSC_FreeFace( vsc_face_t *face )
{
	FREE( face );
}

/*
  ==============================
  VSC_DumpModel

  ==============================
*/
void VSC_DumpModel( vsc_model_t *m )
{
	printf( "VSC_DumpModel:\n" );
	printf( " vertices: %d\n", m->pointnum );
	printf( " faces: %d\n", m->facenum );
}


/*
  ==============================
  VSC_LoadModel

  ==============================
*/
vsc_model_t* VSC_LoadModel( char *file )
{
	int		i;
	int		num;
	vsc_model_t	*m;
	vsc_face_t	*f;
	tokenstream_t	*ts;

	ts = BeginTokenStream( file );
	if ( !ts )
		__error( "can't open videoScape model.\n" );
	
	GetToken( ts );
	if ( strcmp( ts->token, "3DG1" ) )
		goto model_error;

	GetToken( ts );
	num = atoi( ts->token );
//	printf( "%d ", num );
	m = VSC_NewModel( num );
	m->pointnum = num;
	
	//
	// read vertices ...
	//
	for ( i = 0; i < num; i++ )
	{
		GetToken( ts );
		m->p[i][0] = atof( ts->token ) * 16.0;
		GetToken( ts );
		m->p[i][1] = atof( ts->token ) * 16.0;
		GetToken( ts );
		m->p[i][2] = atof( ts->token ) * 16.0;		
	}

	//
	// read faces ...
	//

	for(;;)
	{
//		if ( GetToken() == TOKEN_FAIL )
//			break;

		GetToken( ts );
		num = atoi( ts->token ); // facevertexnum

		if ( num == 0 )
			break;


//		printf( "%d ", num );
		f = VSC_NewFace( num );
		f->pointnum = num;
		for ( i = 0; i < num; i++ )
		{
			GetToken( ts );
			f->p[i] = atoi( ts->token );
		}

		GetToken( ts ); // color
		f->color = strtol( &ts->token[2], NULL, 16 );
		
		// link to list
		f->next = m->faces;
		m->faces = f;
		m->facenum++;
	}

	EndTokenStream( ts );

	printf( "model ok.\n" );
	return m;

model_error:
	printf( "parse error.\n" );
	exit(-1);
}
