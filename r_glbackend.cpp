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



// r_glbackend.c

#include "interfaces.h"
#include "defs.h"
#include "r_glbackenddefs.h"

#include "r_private.h"

#include "r_count.h"

static int	be_vofs;

static matrix3_t	be_matrix;
static vec3d_t		be_origin;

static bool_t	be_va_locked = false;
static vec4d_t	be_va[R_BE_VERTEX_ARRAY_SIZE];

static int	be_cmdnum;
static int	*be_cmds;

static int	be_vrefnum;
static int	*be_vrefs;

static int	be_colornum;
static vec3d_t	*be_colors;

static int	be_texcoordnum;
static vec2d_t	*be_texcoords;

#define FIX_UNITS	( 1.0/16.0 )

// extern declarations in r_count.h
unsigned int count_rf_current_mapnode;
unsigned int count_rf_num_visportal;
unsigned int count_rf_num_vis_mapnode;       // num mapnode from pvs
unsigned int count_rf_num_passed_mapnode;        // num mapnode passed frustum check
unsigned int count_rf_num_vis_shape;

unsigned int count_rf_num_run_job;
unsigned int count_rf_num_destroy_job;
unsigned int count_rf_num_vis_job;

unsigned int count_rf_num_shader_bindtex;
unsigned int count_rf_num_shader_blendfunc;
unsigned int count_rf_num_shader_layer;

unsigned int count_rf_num_fpsys;     // num fpsys registerd for rf
unsigned int count_rf_num_fpsys_visible; // num fpsys registerd passed vis
unsigned int count_rf_num_fpsys_bindtex;
unsigned int count_rf_num_fpsys_blendfunc;
unsigned int count_rf_num_fpsys_particle;    // num of rendered particles

unsigned int count_be_num_transform;
unsigned int count_be_num_begin;
unsigned int count_be_num_flush;
unsigned int count_be_num_tri;


/*
  ==============================
  R_BE_GetVertexOfs

  ==============================
*/
int R_BE_GetVertexOfs( void )
{
	return be_vofs;
}

/*
  ==============================
  R_BE_SetVertexOfs

  ==============================
*/
void R_BE_SetVertexOfs( int ofs )
{
	if ( ofs >= R_BE_VERTEX_ARRAY_SIZE )
	{
		__error( "reached R_BE_VERTEX_ARRAY_SIZE\n" );
	}

	be_vofs = ofs;
}

/*
  ==============================
  R_BE_AppendVertexArray

  ==============================
*/
void R_BE_AppendVertexArray( int vertexnum, vec3d_t va[] )
{
	int		i;

	if ( be_vofs + vertexnum >= R_BE_VERTEX_ARRAY_SIZE )
	{
		__error( "reached R_BE_VERTEX_ARRAY_SIZE\n" );
	}

	for ( i = 0; i < vertexnum; i++ )
	{
		vec3d_t		tmp;
		
		tmp[0] = (va[i][0]*FIX_UNITS) - be_origin[0];
		tmp[1] = (va[i][1]*FIX_UNITS) - be_origin[1];
		tmp[2] = (va[i][2]*FIX_UNITS) - be_origin[2];
		
		Matrix3Vec3dRotate( tmp, tmp, be_matrix );
		
		be_va[be_vofs][0] = tmp[0];
		be_va[be_vofs][1] = tmp[1] * 1.33;
		be_va[be_vofs][2] = 1.0;
		be_va[be_vofs][3] = 1.0 + tmp[2];
		
		be_vofs++;
		COUNT_INCR( count_be_num_transform );
	}
	
}



/*
  ==============================
  R_BE_SetVertexMatrix

  ==============================
*/
void R_BE_SetVertexMatrix( matrix3_t *mat )
{
	memcpy( be_matrix, mat, sizeof( matrix3_t ) );
}



/*
  ==============================
  R_BE_SetVertexOrigin

  ==============================
*/
void R_BE_SetVertexOrigin( vec3d_t origin )
{    
//	Vec3dCopy( be_origin, origin );
	Vec3dScale( be_origin, FIX_UNITS, origin );
}



/*
  ==============================
  R_BE_LockVertexArray

  ==============================
*/
void R_BE_LockVertexArray( void )
{
	be_va_locked = true;

//	glVertexPointer( 4, GL_FLOAT, 0, be_va );
//	glEnableClientState( GL_VERTEX_ARRAY );
}


/*
  ==============================
  R_BE_UnlockVertexArray

  ==============================
*/
void R_BE_UnlockVertexArray( void )
{
	be_va_locked = false;
//	glDisableClientState( GL_VERTEX_ARRAY );
}

/*
  ==============================
  R_BE_ResetLocalBuffers

  ==============================
*/
void R_BE_ResetLocalBuffers( void )
{
	be_cmds = NULL;
	be_vrefs = NULL;
	be_colors = NULL;
	be_texcoords = NULL;
}

/*
  ==============================
  R_BE_SetCMDBuffer
 
  ==============================
*/
void R_BE_SetCMDBuffer( int cmdnum, int cmd_buf[] )
{
	be_cmdnum = cmdnum;
	be_cmds = cmd_buf;
}


/*
  ==============================
  R_BE_SetVrefArray

  ==============================
*/
void R_BE_SetVrefArray( int vrefnum, int vref_array[] )
{
	be_vrefnum = vrefnum;
	be_vrefs = vref_array;
}


/*
  ==============================
  R_BE_SetColorArray
 
  ==============================
*/
void R_BE_SetColorArray( int colornum, vec3d_t color_array[] )
{
	be_colornum = colornum;
	be_colors = color_array;
}


/*
  ==============================
  R_BE_SetTexcoordArray
 
  ==============================
*/
void R_BE_SetTexcoordArray( int texcoordnum, vec2d_t texcoord_array[] )
{
	be_texcoordnum = texcoordnum;
	be_texcoords = texcoord_array;
}


/*
  ==============================
  R_BE_ProcessCMDBuffer
 
  ==============================
*/
GLenum	glcmd_lut[] = { (GLenum)-1, GL_TRIANGLES, GL_TRIANGLE_FAN, GL_TRIANGLE_STRIP, GL_POLYGON };

void R_BE_ProcessCMDBuffer( void )
{
	int		i, j;
	int	num;
	int		cmd, *ptr, *end;

	vec4d_t		*va;      

	static int	glflush_hack = 0;
#if 0
	if ( be_va_locked == false )
	{
		__warning( "vertex array not locked\n" );
		return;
	}
		
	if ( !be_cmds )
	{
		__warning( "null command buffer\n" );
		return;
	}

	if ( be_cmdnum == 0 )
	{
		__warning( "empty command buffer\n" );
		return;
	}

	if ( !be_vrefs )
	{
		__warning( "null vertex reference buffer\n" );
		return;
	}

	if ( be_vrefnum == 0 )
	{
		__warning( "empty vertex reference buffer\n" );
		return;
	}

	if ( be_colors && ( be_colornum != be_vrefnum ) )
	{
		__warning( "colornum != vrefnum\n" );
		return;
	}

	if ( be_texcoords && ( be_texcoordnum != be_vrefnum ) )
	{
		__warning( "texcoordnum != vrefnum\n" );
		return;
	}

//	if ( be_vertexnum > 0 )
	{
		glVertexPointer( 4, GL_FLOAT, 0, be_va );
//		glEnableClientState( GL_VERTEX_ARRAY );
	}
//	else
//	{
//		return;
//	}

	if ( be_colors )
	{
		glColorPointer( 3, GL_FLOAT, 0, be_colors );
//		glEnableClientState( GL_COLOR_ARRAY );
	}

	if ( be_texcoords )
	{
		glTexCoordPointer( 2, GL_FLOAT, 0, be_texcoords );
//		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
	}
#endif


	va = &be_va[be_vofs];


	i = 0;
	ptr = be_cmds;
	end = &be_cmds[be_cmdnum];
	for(;;)
	{
		if ( ptr >= end )
			__error( "reached command buffer end\n" );

		cmd = *ptr++;
		

#if 1
		if ( cmd != BE_CMD_END )
		{
			num = *ptr++;	
			if ( glcmd_lut[cmd] == GL_TRIANGLE_FAN )
			{
				COUNT_ADD( count_be_num_tri, num-2 );
			}
			else if ( glcmd_lut[cmd] == GL_TRIANGLE_STRIP )
			{
				COUNT_ADD( count_be_num_tri, num-2 );
			}
			else if ( glcmd_lut[cmd] == GL_TRIANGLES )
			{
				COUNT_ADD( count_be_num_tri, num/3 );
			}
			else if ( glcmd_lut[cmd] == GL_POLYGON )
			{
				COUNT_ADD( count_be_num_tri, num-2 );
			}
			
			glBegin( glcmd_lut[cmd] );
			COUNT_INCR( count_be_num_begin );
			for ( j = 0; j < num; j++, i++ )
			{
				if ( be_colors )
					glColor3fv( be_colors[i] );
				if ( be_texcoords )
					glTexCoord2fv( be_texcoords[i] );
				glVertex4fv( va[be_vrefs[i]] );				
//				glArrayElement( i+be_vofs );
			}
			glEnd();			

			if ( glflush_hack >= r_glinfo->flushinterval )
			{
//				printf( "%d\n", r_glinfo->flushinterval );
				glFlush();
				COUNT_INCR( count_be_num_flush );
				glflush_hack = 0;
			}
			else
				glflush_hack++;

		}
		else
		{
			break;
		}

#else

#endif

	}

#if 0
	if ( glflush_hack >= r_glinfo->flushinterval )
	{
		glFlush();
		glflush_hack = 0;
	}
	else
		glflush_hack++;
#endif

//	glDisableClientState( GL_VERTEX_ARRAY );
//	glDisableClientState( GL_COLOR_ARRAY );
//	glDisableClientState( GL_TEXTURE_COORD_ARRAY );
}
