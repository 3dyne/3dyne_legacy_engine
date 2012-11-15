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



// r_volume.c

/*
  =============================================================================
  volume light cluster map

  =============================================================================
*/

#include "r_interfaces.h"
#include "r_volume.h"
#include "render.h"

/*
  ==============================
  R_NewVolumeCache

  ==============================
*/
volume_cache_t * R_NewVolumeCache( ivec3d_t min, ivec3d_t max )
{
	int		pointnum;
	int		size;
	int		xs, ys, zs;
	volume_cache_t	*vc;

	xs = 1+max[0]-min[0];
	ys = 1+max[1]-min[1];
	zs = 1+max[2]-min[2];

	pointnum = xs*ys*zs;

	size = (int)((volume_cache_t *)0)->vecs[pointnum];
	vc = (volume_cache_t *) NEWBYTES( size );

	vc->xs = xs;
	vc->ys = ys;
	vc->zs = zs;

	IVec3dCopy( vc->min, min );
	IVec3dCopy( vc->max, max );

	IVec3dPrint( min );
	IVec3dPrint( max );

	return vc;
}

void R_FreeVolumeCache( volume_cache_t *vc )
{
	FREE( vc );
}

/*
  ==============================
  R_AccessVolumeCache

  ==============================
*/
fp_t * R_AccessVolumeCache( volume_cache_t *vc, ivec3d_t pos )
{
	int		i;
	ivec3d_t	cpos;
	fp_t		*addr;

//	printf( "*" ); IVec3dPrint( pos );

	for ( i = 0; i < 3; i++ )
	{
		if ( pos[i] < vc->min[i] || pos[i] > vc->max[i] )
			return NULL;
	}

	cpos[0] = pos[0]-vc->min[0];
	cpos[1] = pos[1]-vc->min[1];
	cpos[2] = pos[2]-vc->min[2];

	addr = vc->vecs[cpos[0]+cpos[1]*vc->xs+cpos[2]*vc->xs*vc->ys];
	return addr;
}

/*
  ==============================
  R_FillVolumeCache

  ==============================
*/

static int	access_count = 0;

void R_FillCClusterIntoVolumeCache( map3_cache_t *cache, ccluster_t *cc )
{
	ivec3d_t	cpos, pos;
	int		x, y, z;

//	printf( "#" ); IVec3dPrint( cc->pos );

	cpos[0] = cc->pos[0]*4;
	cpos[1] = cc->pos[1]*4;
	cpos[2] = cc->pos[2]*4;

	U_BeginExpand( &cc->cinfo, cc->cdata );
	
	for ( z = 0; z < 4; z++ )
	{
		for ( y = 0; y < 4; y++ )
		{
			for ( x = 0; x < 4; x++ )
			{
				void		*addr;
				vec3d_t		tmp;
				fp_t		*vec;
			      
				U_ExpandFP32( &tmp[0] );
				U_ExpandFP32( &tmp[1] );
				U_ExpandFP32( &tmp[2] );

				pos[0] = cpos[0] + x;
				pos[1] = cpos[1] + y;
			        pos[2] = cpos[2] + z;
				
				addr = G_AccessMap3Cache( cache, pos );
				if ( !addr )
					continue;
//				printf( "." );
				access_count++;
				vec = (fp_t *) addr;
				Vec3dCopy( vec, tmp );
			}
		}
	}
	U_EndExpand();	
}

/*
  ==============================
  R_FillVolumeCache

  ==============================
*/
void R_FillVolumeCache( map3_cache_t *cache, ccmap3_t *map, vec3d_t min, vec3d_t max )
{
	ivec3d_t	cmin, cmax;
	ivec3d_t		cpos;

	IVec3dRint( cmin, min );
	IVec3dRint( cmax, max );
	IVec3dUnitSnap( cmin, cmin, 64 );
	IVec3dUnitSnap( cmax, cmax, 64 );

	for ( cpos[0] = cmin[0]; cpos[0] <= cmax[0]; cpos[0]++ )
	{
		for ( cpos[1] = cmin[1]; cpos[1] <= cmax[1]; cpos[1]++ )
		{
			for ( cpos[2] = cmin[2]; cpos[2] <= cmax[2]; cpos[2]++ )
			{
				ccluster_t *cc;

				cc = G_CCMap3FindCCluster( map, cpos );
				if ( !cc )
					continue;

//				IVec3dPrint( cpos );
				R_FillCClusterIntoVolumeCache( cache, cc );
			}
		}
	}
}



void DrawMap3Cache_as_vector( map3_cache_t *map )
{
	ivec3d_t		pos;

	glDisable( GL_TEXTURE_2D );

	for ( pos[0] = map->min[0]; pos[0] <= map->max[0]; pos[0]++ )
	{
		for ( pos[1] = map->min[1]; pos[1] <= map->max[1]; pos[1]++ )
		{
			for ( pos[2] = map->min[2]; pos[2] <= map->max[2]; pos[2]++ )
			{
				void		*addr;
				fp_t		*norm;
				vec3d_t		tmp;
				vec3d_t		tmp2;
				vec4d_t		w;

				addr = G_AccessMap3Cache( map, pos );
				norm = (fp_t *) addr;
				if ( !norm )
					continue;
				
				tmp[0] = (fp_t)(pos[0]*16.0+8.0)/16.0;
				tmp[1] = (fp_t)(pos[1]*16.0+8.0)/16.0;
				tmp[2] = (fp_t)(pos[2]*16.0+8.0)/16.0;

				Vec3dMA( tmp2, 0.5, norm, tmp );

				glBegin( GL_LINES );
				glColor3f( 1.0, 0, 0 );
				CalcVertex( w, tmp );
				glVertex4fv( w );
				glColor3f( 0, 0, 1.0 );
				CalcVertex( w, tmp2 );
				glVertex4fv( w );
				glEnd();
			}
		}
	}
}

void DrawMap3Cache_as_color( map3_cache_t *map )
{
	ivec3d_t		pos;

	glDisable( GL_TEXTURE_2D );

	for ( pos[0] = map->min[0]; pos[0] <= map->max[0]; pos[0]++ )
	{
		for ( pos[1] = map->min[1]; pos[1] <= map->max[1]; pos[1]++ )
		{
			for ( pos[2] = map->min[2]; pos[2] <= map->max[2]; pos[2]++ )
			{
				void		*addr;
				fp_t		*norm;
				vec3d_t		tmp;
				vec3d_t		tmp2;
				vec4d_t		w;

				addr = G_AccessMap3Cache( map, pos );
				norm = (fp_t *) addr;
				if ( !norm )
					continue;
				
				tmp[0] = (fp_t)(pos[0]*16.0+8.0)/16.0;
				tmp[1] = (fp_t)(pos[1]*16.0+8.0)/16.0;
				tmp[2] = (fp_t)(pos[2]*16.0+8.0)/16.0;

				glBegin( GL_POINTS );
				glColor3f( norm[0], norm[1], norm[2] );
				CalcVertex( w, tmp );
				glVertex4fv( w );
//				glColor3f( 0, 0, 1.0 );
//				CalcVertex( w, tmp2 );
//				glVertex4fv( w );
				glEnd();
			}
		}
	}
}


/*
  ==================================================
  test stuff

  ==================================================
*/

typedef struct pcell_s
{
	vec3d_t		norm;
	int		pnum;	// particles in cell
} pcell_t;


static vec3d_t r_porigin;

map3_cache_t	*r_volume = NULL;

void R_VolumeCacheTest( vec3d_t pos, bool_t update )
{
	static map3_cache_t	*cache = NULL;
	static map3_cache_t	*cache2 = NULL;
	vec3d_t		min;
	vec3d_t		max;

	if ( update )
	{
		min[0] = pos[0] - 128.0;
		min[1] = pos[1] - 128.0;
		min[2] = pos[2] - 128.0;
		max[0] = pos[0] + 127.0;
		max[1] = pos[1] + 255.0;
		max[2] = pos[2] + 127.0;
		Vec3dCopy( r_porigin, pos );
		r_porigin[1] -= 32.0;
	}

//	vec3d_t		min = {4352,-192,-320};
//	vec3d_t		max = {4608,192,0};

	if ( update )
	{
		ivec3d_t	umin, umax;
		IVec3dRint( umin, min );
		IVec3dRint( umax, max );
		IVec3dUnitSnap( umin, umin, 16 );
		IVec3dUnitSnap( umax, umax, 16 );
	
		IVec3dPrint( umin );
		IVec3dPrint( umax );
	
		access_count = 0;
		if ( cache )
			G_FreeMap3Cache( cache );
		if ( cache2 )
			G_FreeMap3Cache( cache2 );


		r_volume = cache = G_NewMap3Cache( umin, umax, sizeof( vec3d_t ) );
		cache2 = G_NewMap3Cache( umin, umax, sizeof( pcell_t ) );

		R_FillVolumeCache( cache, g_st->map->volume_ccmap3, min, max );
		R_FillVolumeCache( cache2, g_st->map->field_ccmap3, min, max );

//		printf( " %d cells of %d filled (%dx%dx%d)\n", access_count, vc->xs*vc->ys*vc->zs, vc->xs, vc->ys, vc->zs );
//		getchar();
	}

	if ( cache && cache2 )
	{
//		DrawMap3Cache_as_color( cache );
		RunTestSmoke( cache, cache2 );
	}
}


typedef struct particle_s
{
	vec3d_t		pos;
	vec3d_t		dir;

	vec3d_t		color1;
	vec3d_t		color2;

	fp_t		speed;
	int		count;

	pcell_t		*cellref;
} particle_t;

#define MAX_PARTICLES	( 50 )

static particle_t	r_particles[MAX_PARTICLES];

void RunTestSmoke( map3_cache_t *cache, map3_cache_t *cache2, bool_t reset )
{
	vec3d_t		vec;
	int		i;
	vec3d_t		v3;
	vec4d_t		v;
	static int	count = 0;
	static GLuint	texobj;
	
	static fp_t	rotate[32][4][2];
	fp_t		trans;

	if ( reset )
	{
		for ( i = 0; i < MAX_PARTICLES; i++ )
		{
			r_particles[i].cellref = NULL;
		}
	}

	if ( count == 0 )
	{

//		texobj = Misc_GenTexture_TGA_8888( ART_PATH "/fx/smoke2.tga" );
//		texobj = Misc_GenTexture_TGA_8888( "/home/mcb/art/fx/smoke3.tga" );

		{
			fp_t	pts[4][2]={ {1.0, -1.0}, {-1.0,-1.0}, { -1.0, 1.0 }, { 1.0, 1.0 } };
			int	j, k;
			fp_t	s, c, angle;
			for ( j = 0; j < 32; j++ )
			{
				angle = (j/32.0) * M_PI*2.0; //(j*(360.0/16.0)) / (180.0*M_PI);
				s = sin( angle );
				c = cos( angle );
				for ( k = 0; k < 4; k++ )
				{
					rotate[j][k][0] = c*pts[k][0] - s*pts[k][1];
					rotate[j][k][1] = s*pts[k][0] + c*pts[k][1];
					rotate[j][k][0]*=2.0;
					rotate[j][k][1]*=2.0;
				}
			}
		}
	}

	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glBindTexture( GL_TEXTURE_2D, texobj );
	glEnable( GL_TEXTURE_2D );
	glEnable ( GL_BLEND );                                          
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
//	glBlendFunc( GL_ONE, GL_ONE );
	glDepthMask(GL_FALSE);
//	glDisable( GL_DEPTH_TEST );

	if ( count == 0 )
	{
		for ( i = 0; i < MAX_PARTICLES; i++ )
		{
			r_particles[i].count = i;
			r_particles[i].speed = 1.0;
			Vec3dInit( r_particles[i].pos, -192, 0, -384 );
			Vec3dInit( r_particles[i].dir, 0, 1, 0 );
		}
	}



	// debug draw cached field cells      
//	DrawCachedFieldCells();

//	glColor4f( 1.0, 1.0, 1.0, 1.0 );


	for ( i = 0; i < MAX_PARTICLES; i++ )
	{
		if ( r_particles[i].count <= 0 )
		{
			// die
			if ( r_particles[i].cellref )
			{
				r_particles[i].cellref->pnum--;	
			}

			// spawn
			r_particles[i].count = MAX_PARTICLES;
			r_particles[i].speed = 4.0;
			Vec3dInit( r_particles[i].color1, 0, 0, 0 );
			Vec3dInit( r_particles[i].pos, r_porigin[0], r_porigin[1], r_porigin[2]  );
			Vec3dInit( r_particles[i].dir, -0.25+(_Random()%1000)/2000.0, 1, -0.25+(_Random()%1000)/2000.0 );
		}

		if ( ! (r_particles[i].count & 31 ) )
		{
			vec3d_t		rnd;
			Vec3dInit( rnd, -0.25+(_Random()%1000)/2000.0, 0/*-0.1+(_Random()%1000)/1000.0*/, -0.25+(_Random()%1000)/2000.0 );
			Vec3dAdd( r_particles[i].dir, r_particles[i].dir, rnd );
		}

		if ( count )
		{
			pcell_t		*oldcell;
			oldcell = r_particles[i].cellref;
			if ( oldcell )
			{
				r_particles[i].cellref->pnum--;
				r_particles[i].cellref = NULL;
			}

			Vec3dMA( r_particles[i].pos, r_particles[i].speed, r_particles[i].dir, r_particles[i].pos );

			{
				void	*addr;
				ivec3d_t	upos;
				pcell_t		*cell;

				IVec3dRint( upos, r_particles[i].pos );
				IVec3dUnitSnap( upos, upos, 16 );
				addr = G_AccessMap3Cache( cache2, upos );
				cell = (pcell_t *) addr;
#if 0
				if ( cell )
				{
//					Vec3dPrint( norm );
					if ( cell->pnum >= 100 )
					{
						vec3d_t		rnd;
						Vec3dMA( r_particles[i].pos, r_particles[i].speed, r_particles[i].dir, r_particles[i].pos );	

						Vec3dInit( rnd, -0.25+(_Random()%1000)/2000.0, 0/*-0.1+(_Random()%1000)/1000.0*/, -0.25+(_Random()%1000)/2000.0 );
						Vec3dAdd( r_particles[i].dir, r_particles[i].dir, rnd );
						Vec3dUnify( r_particles[i].dir );
						if ( oldcell )
						{
							r_particles[i].cellref = oldcell;
							r_particles[i].cellref->pnum++;
						}
					}
					else
					{
						r_particles[i].cellref = cell;
						r_particles[i].cellref->pnum++;

//						Vec3dMA( r_particles[i].pos, r_particles[i].speed, cell->norm, r_particles[i].pos );
					}
				}
#else
				if ( cell )
				{
					Vec3dMA( r_particles[i].pos, r_particles[i].speed, cell->norm, r_particles[i].pos );
				}
#endif
			}
			
			r_particles[i].count--;
		}

		if ( r_particles[i].count >= 30 )
		{
			trans = 1.0-(r_particles[i].count-30) / 20.0;
		}

		else if ( r_particles[i].count <= 20 )
		{
			trans = r_particles[i].count / 20.0;
		}
		else
			trans = 0.75;

//		trans *= 0.5;
		{
			vec3d_t		delta;
			ivec3d_t	upos;
			void		*addr;
			fp_t		*color;
			IVec3dRint( upos, r_particles[i].pos );
			IVec3dUnitSnap( upos, upos, 16 );
			addr = G_AccessMap3Cache( cache, upos );
//			printf( "%p ", addr );
			color = (fp_t *) addr;
			if ( !color )
			{
				Vec3dInit( r_particles[i].color2, 0, 0, 0 );
//				glColor3f( 0, 0, 0 );
			}
			else
			{
				Vec3dCopy( r_particles[i].color2, color );
//				glColor3f( color[0]*trans, color[1]*trans, color[2]*trans );
			}

			Vec3dSub( delta, r_particles[i].color2, r_particles[i].color1 );
			Vec3dMA( r_particles[i].color1, 0.2, delta, r_particles[i].color1 );

//			glColor3f( r_particles[i].color1[0]*trans, r_particles[i].color1[1]*trans, r_particles[i].color1[2]*trans );
			glColor4f( r_particles[i].color1[0], r_particles[i].color1[1], r_particles[i].color1[2], trans );
		}		

		Vec3dScale( v3, 1/16.0, r_particles[i].pos );
		CalcVertex( v, v3 );		

		{
			int	frame;

			frame = r_particles[i].count & 31;
			if ( i&1 )
				frame = 31-frame;

//			frame = 0;

			glBegin( GL_TRIANGLE_FAN );
			glTexCoord2f( 0,0 );
			glVertex3f( (v[0]+rotate[frame][0][0])/v[3], (v[1]+rotate[frame][0][1])/v[3], 1.0/v[3] );
			glTexCoord2f( 1,0 );
			glVertex3f( (v[0]+rotate[frame][1][0])/v[3], (v[1]+rotate[frame][1][1])/v[3], 1.0/v[3] );
			glTexCoord2f( 1,1 );
			glVertex3f( (v[0]+rotate[frame][2][0])/v[3], (v[1]+rotate[frame][2][1])/v[3], 1.0/v[3] );
			glTexCoord2f( 0,1 );
			glVertex3f( (v[0]+rotate[frame][3][0])/v[3], (v[1]+rotate[frame][3][1])/v[3], 1.0/v[3] );
//		Vec3dPrint( v );
//		Vec3dPrint( v[1] );
			
			glEnd();
		}
//		r_particles[i].speed *= 1.05;
//		printf( "%d ", r_particles[i].count );
		

	}
//	glEnd();


//	glBegin( GL_TRIANGLES );


//	glVertex3fv( r_particle[0]-8, r_particle[1]-8, r_particle[2] );
//	glVertex3fv( r_particle[0]+8, r_particle[1]-8, r_particle[2] );

	glDepthMask(GL_TRUE);
	count++;
}
