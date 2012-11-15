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



// lib_model.c

//#include "interfaces.h"
//#include "defs.h"

#include "lib_math.h"
#include "lib_container.h"
#include "lib_model.h"

/*
  ==============================
  MD_NewCoordsys

  ==============================
*/
md_coordsys_t * MD_NewCoordsys( void )
{
	md_coordsys_t	*cs;

	cs = NEWTYPE( md_coordsys_t );
	return cs;
}



/*
  ==============================
  MD_FreeCoordsys

  ==============================
*/
void MD_FreeCoordsys( md_coordsys_t *cs )
{
	FREE( cs );
}

/*
  ==============================
  MD_CalcStep
  
  ==============================
*/
int MD_CalcStep( md_move_t *mv, msec_t msec_local )
{
	int	step;

	step = (msec_local/mv->msecs_per_step)%mv->step_num;
	return step;
}

/*
  ==============================
  MD_MoveAccess

  ==============================
*/
fp_t * MD_MoveAccess( md_move_t *mv, unsigned int step, int param )
{
	fp_t	*ptr;

	ptr = (fp_t *)mv->steps;
	return &ptr[step*6+param];
}


/*
  ==============================
  MD_MoveGetStepnum

  ==============================
*/
unsigned int MD_MoveGetStepnum( md_move_t *mv )
{
	return mv->step_num;
}


/*
  ==============================
  MD_MoveDump

  ==============================
*/
void MD_MoveDump( md_move_t *mv )
{
	int	i, j;
	fp_t	*f;

	printf( "MD_MoveDump: \n" );

	for ( i = 0; i < mv->step_num; i++ )
	{
		printf( "step %d: ", i );
		for ( j = 0; j < 6; j++ )
		{
			f = MD_MoveAccess( mv, i, j );
			printf( "%f ", *f );
		}
		printf( "\n" );
	}
}


/*
  ==============================
  MD_CoordsysInsertCoordsys

  ==============================
*/
void MD_CoordsysInsertCoordsys( md_coordsys_t *parent, md_coordsys_t *child )
{
	if ( !parent->child_list )
	{
		parent->child_list = U_NewList();
	}

	U_ListInsertAtHead( parent->child_list, child );
}

/*
  ==============================
  MD_CoordsysInsertSubmodel

  ==============================
*/
void MD_CoordsysInsertSubmodel( md_coordsys_t *parent, md_submodel_t *submodel )
{
	if ( !parent->submodel_list )
	{
		parent->submodel_list = U_NewList();
	}

	U_ListInsertAtHead( parent->submodel_list, submodel );
}

/*
  ==============================
  MD_CoordsysSetMove

  ==============================
*/
void MD_CoordsysSetMove( md_coordsys_t *cs, md_move_t *mv )
{
	cs->current_move = mv;
}

/*
  ==============================
  MD_CalcAxisPoint

  ==============================
*/
void MD_CalcAxisPoint( vec3d_t out, vec3d_t in, vec3d_t axis[3] )
{
	out[0] = in[0]*axis[0][0] + in[1]*axis[1][0] + in[2]*axis[2][0];
	out[1] = in[0]*axis[0][1] + in[1]*axis[1][1] + in[2]*axis[2][1];
	out[2] = in[0]*axis[0][2] + in[1]*axis[1][2] + in[2]*axis[2][2];	
}

