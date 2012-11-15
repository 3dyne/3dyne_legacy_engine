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



// r_field.c

#include "r_interfaces.h"
#include "render.h"


/*
  =============================================================================
  field node

  =============================================================================
*/

typedef struct fieldnode_s
{
	hobj_t		*self;
	cplane_t	*pl;

	// node
	int		child[2];

	// leaf
	vec3d_t		fieldvec;
} fieldnode_t;

#define MAX_FIELDNODES	( 32000 )

int		r_fieldnodenum;
fieldnode_t	r_fieldnodes[MAX_FIELDNODES];

void SetupField( char *plane_name, char *node_name )
{
	hmanager_t		*planehm;
	hmanager_t		*nodehm;
	
	int		i;
	int		index;
	hobj_t		*node;
	hobj_t		*plane;
	hobj_t		*child;
	hpair_t		*pair;
	hmanager_type_iterator_t	iters[3];

	int		field_leafs = 0;
	int		empty_leafs = 0;

	planehm = ReadPlaneClass( plane_name );
	if ( !planehm )
		__error( "can't load field plane class.\n" );

	nodehm = NewHManagerLoadClass( node_name );
	if ( !nodehm )
		__error( "can't load field node class.\n" );
	
	r_fieldnodenum = 0;
	
	HManagerIndexClassesOfType( nodehm, &r_fieldnodenum, "fieldnodes" );
	HManagerIndexClassesOfType( nodehm, &r_fieldnodenum, "fieldnode_front" );
	HManagerIndexClassesOfType( nodehm, &r_fieldnodenum, "fieldnode_back" );

	printf( "r_fieldnodenum: %d\n", r_fieldnodenum );

	if ( r_fieldnodenum >= MAX_FIELDNODES )
		__error( "reached MAX_FIELDNODES\n" );

	HManagerInitTypeSearchIterator( &iters[0], nodehm, "fieldnodes" );
	HManagerInitTypeSearchIterator( &iters[1], nodehm, "fieldnode_front" );
	HManagerInitTypeSearchIterator( &iters[2], nodehm, "fieldnode_back" );

	for ( i = 0; i < 3; i++ )
	{
		for ( ; ( node = HManagerGetNextClass( &iters[i] ) ); )
		{
			pair = FindHPair( node, "index" );
			if ( !pair )
				__error( "missing 'index'\n" );
			HPairCastToInt_safe( &index, pair );

			r_fieldnodes[index].self = node;
			
			pair = FindHPair( node, "plane" );
			if ( pair )
			{
				// it's a node
				plane = HManagerSearchClassName( planehm, pair->value );
				if ( !plane )
					__error( "fieldnode '%s' can't find plane '%s'.\n", node->name, pair->value );
				r_fieldnodes[index].pl = GetClassExtra( plane );

				child = FindClassType( node, "fieldnode_front" );
				if ( !child )
					__error( "missing 'fieldnode_front' in fieldnode '%s'.\n", node->name );
				pair = FindHPair( child, "index" );
				if ( !pair )
					__error( "fieldnode '%s' missing 'index' of child '%s'.\n", node->name, child->name );
				HPairCastToInt( &r_fieldnodes[index].child[0], pair );

				child = FindClassType( node, "fieldnode_back" );
				if ( !child )
					__error( "missing 'fieldnode_back' in fieldnode '%s'.\n", node->name );
				pair = FindHPair( child, "index" );
				if ( !pair )
					__error( "fieldnode '%s' missing 'index' of child '%s'.\n", node->name, child->name );
				HPairCastToInt( &r_fieldnodes[index].child[1], pair );
			}
			else
			{
				// it's a field leaf
				pair = FindHPair( node, "fieldvector" );
				if ( pair )
				{
					HPairCastToVec3d_safe( r_fieldnodes[index].fieldvec, pair );
					field_leafs++;
				}
				else
				{
					Vec3dInit( r_fieldnodes[index].fieldvec, 0, 0, 0 );
					empty_leafs++;
				}
			}
		}
	}
     
	printf( " %d field leafs, %d empty leafs\n", field_leafs, empty_leafs );
}


/*
  =============================================================================
  field map stuff

  =============================================================================
*/
#define MAX_FIELDCELLS	( 400000 )

typedef struct fcell_s
{
	ivec3d_t	pos;
	vec3d_t		vec;
} fcell_t;

int		r_fcellnum = 0;
fcell_t		r_fcells[MAX_FIELDCELLS];

void SetupFieldMap( char *name )
{
	int		i;
	FILE		*h;
	h = fopen( name, "r" );
	if ( !h )
		__error( "can't open fieldmap binary.\n" );

	fread( &r_fcellnum, 4, 1, h );
	if ( r_fcellnum > MAX_FIELDCELLS )
		__error( "reached MAX_FIELDCELLS\n" );

	for ( i = 0; i < r_fcellnum; i++ )
	{
		short		s;
		fread( &s, 2, 1, h );
		r_fcells[i].pos[0] = (int) s;
		fread( &s, 2, 1, h );
		r_fcells[i].pos[1] = (int) s;
		fread( &s, 2, 1, h );
		r_fcells[i].pos[2] = (int) s;

		fread( &r_fcells[i].vec[0], 4, 1, h );
		fread( &r_fcells[i].vec[1], 4, 1, h );
		fread( &r_fcells[i].vec[2], 4, 1, h );
//		Vec3dUnify( r_fcells[i].vec );
	}

	printf( " %d fieldcells\n", r_fcellnum );
}

#define MAX_CACHEDFC		( 8192 )
int	xs, ys, zs;
ivec3d_t	r_fmin;
ivec3d_t	r_fmax;
int		r_cachedfcnum;
vec3d_t		r_cachedfc[MAX_CACHEDFC];
vec3d_t		r_cachedfp[MAX_CACHEDFC];

void CacheFieldCellsOfBB( ivec3d_t min, ivec3d_t max )
{
	int		i, j;
	int		num;

	for ( i = 0; i < 3; i++ )
	{
		r_fmin[i] = min[i];
		r_fmax[i] = max[i];
	}

	xs = max[0] - min[0];
	ys = max[1] - min[1];
	zs = max[2] - min[2];

	r_cachedfcnum = xs*ys*zs;

	if ( r_cachedfcnum > MAX_CACHEDFC )
		__error( "reached MAX_CACHEDFC\n" );

	memset( r_cachedfc, 0, xs*ys*zs*12 );
	
	num = 0;
	for ( i = 0; i < r_fcellnum; i++ )
	{
		for ( j = 0; j < 3; j++ )
		{
			if ( r_fcells[i].pos[j] < min[j] || r_fcells[i].pos[j] > max[j] )
			{
				break;
			}			
		}
		if ( j != 3 )
			continue;

		{
			ivec3d_t	v;
			v[0] = r_fcells[i].pos[0] - min[0];
			v[1] = r_fcells[i].pos[1] - min[1];
			v[2] = r_fcells[i].pos[2] - min[2];
			Vec3dCopy( r_cachedfc[v[0]+v[1]*xs+v[2]*xs*ys], r_fcells[i].vec );
			Vec3dInit( r_cachedfp[v[0]+v[1]*xs+v[2]*xs*ys], r_fcells[i].pos[0]*1.0, r_fcells[i].pos[1]*1.0, r_fcells[i].pos[2]*1.0 );
		}
		num++;
	}
	printf( " %d fieldcells cached\n", num );
}

void DrawCachedFieldCells( void )
{
	int		i;

	glDisable( GL_TEXTURE_2D );
	glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );


	for( i = 0; i < r_cachedfcnum; i++ )
	{
		vec3d_t		v1, v2;
		vec4d_t		w;

		Vec3dCopy( v1, r_cachedfp[i] );
		Vec3dMA( v2, 0.5, r_cachedfc[i], v1 );

		glBegin( GL_LINES );
		glColor3f( 1.0, 0, 0 );
		CalcVertex( w, v1 );
		glVertex4fv( w );
		glColor3f( 0, 0, 1.0 );
		CalcVertex( w, v2 );
		glVertex4fv( w );
		glEnd();
	}
}

void FindFieldVectorForPoint( vec3d_t point, vec3d_t vec )
{
	int		i;
	ivec3d_t		v;

	v[0] = (int)_Rint(point[0]/16.0);
	v[1] = (int)_Rint(point[1]/16.0);
	v[2] = (int)_Rint(point[2]/16.0);

	for ( i = 0; i < 3; i++ )
	{
		if ( v[i] < r_fmin[i] || v[i] > r_fmax[i] )
			break;
	}
	if ( i != 3 )
	{
		// point out of cached area
		printf( "*" );
		Vec3dInit( vec, 0, 0, 0 );
		return;		
	}

	v[0]-=r_fmin[0];
	v[1]-=r_fmin[1];
	v[2]-=r_fmin[2];

	Vec3dCopy( vec, r_cachedfc[v[0]+v[1]*xs+v[2]*xs*ys] );
}

/*
  =============================================================================
  particle test

  =============================================================================
*/

static int	r_count = 0;

typedef struct particle_s
{
	vec3d_t		pos;
	vec3d_t		dir;
	fp_t		speed;
	int		count;
} particle_t;

#define MAX_PARTICLES	( 25 )

static particle_t	r_particles[MAX_PARTICLES];

void FindFieldVectorForPoint_( vec3d_t point, vec3d_t vec )
{
	int		node;
	fieldnode_t	*n;
	fp_t		d;

	node = 0;
	
	for(;;)
	{
		n = &r_fieldnodes[node];
		if ( !n->pl )
		{
			Vec3dCopy( vec, n->fieldvec );
			return;
		}

		d = Vec3dDotProduct( n->pl->norm, point ) - n->pl->dist;
		if ( d >= 0 )
		{
			node = n->child[0];
		}
		else
		{
			node = n->child[1];
		}
	}
}

void FieldTest( vec3d_t pos )
{
	vec3d_t		vec;

	FindFieldVectorForPoint( pos, vec );
	Vec3dPrint( vec );
}



void RunParticle( void )
{
	vec3d_t		vec;
	int		i;
	vec3d_t		v3;
	vec4d_t		v;

	static GLuint	texobj;
	
	static fp_t	rotate[32][4][2];


	if ( r_count == 0 )
	{
		ivec3d_t	min = { 64/16-6, 224/16-6, 928/16-6 };
		ivec3d_t	max = { 64/16+6, 224/16+6, 928/16+6 };

//		texobj = Misc_GenTexture_TGA_8888( "/home/mcb/art/fx/smoke2.tga" );
//		texobj = Misc_GenTexture_TGA_8888( ART_PATH "fx/blip2.tga" );

		CacheFieldCellsOfBB( min, max );
//		getchar();
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
				}
			}
		}
	}

	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glBindTexture( GL_TEXTURE_2D, texobj );
	glEnable( GL_TEXTURE_2D );
	glEnable ( GL_BLEND );                                          
//	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glBlendFunc( GL_ONE, GL_ONE );
	glDepthMask(GL_FALSE);
//	glDisable( GL_DEPTH_TEST );

	if ( r_count == 0 )
	{
		for ( i = 0; i < MAX_PARTICLES; i++ )
		{
			r_particles[i].count = i;
			r_particles[i].speed = 1.0;
			Vec3dInit( r_particles[i].pos, -480.0, 96.0, -784.0 );
			Vec3dInit( r_particles[i].dir, 0, 1, 0 );


		}
	}



	// debug draw cached field cells      
//	DrawCachedFieldCells();

	glColor4f( 1.0, 1.0, 1.0, 1.0 );


	for ( i = 0; i < MAX_PARTICLES; i++ )
	{
		if ( r_particles[i].count <= 0 )
		{
			r_particles[i].count = MAX_PARTICLES;
			r_particles[i].speed = 2.0;
			Vec3dInit( r_particles[i].pos, 64, 224, 928 );
			Vec3dInit( r_particles[i].dir, -0.5+(_Random()%1000)/1000.0, 1, -0.5+(_Random()%1000)/1000.0 );
		}

		if ( ! (r_particles[i].count & 31 ) )
		{
			vec3d_t		rnd;
			Vec3dInit( rnd, -0.05+(_Random()%1000)/10000.0, -0.01+(_Random()%1000)/10000.0, -0.05+(_Random()%1000)/10000.0 );
			Vec3dAdd( r_particles[i].dir, r_particles[i].dir, rnd );
		}

		if ( r_count )
		{
			Vec3dMA( r_particles[i].pos, r_particles[i].speed, r_particles[i].dir, r_particles[i].pos );
			FindFieldVectorForPoint( r_particles[i].pos, vec );
			Vec3dMA( r_particles[i].pos, r_particles[i].speed, vec, r_particles[i].pos );
			r_particles[i].count--;
		}
//		Vec3dAdd( r_particles[i].dir, r_particles[i].dir, vec );

		Vec3dScale( v3, 1/16.0, r_particles[i].pos );
		CalcVertex( v, v3 );
//		glVertex3f( r_particles[i].pos[0]/16.0, -r_particles[i].pos[1]/16.0, r_particles[i].pos[2]/16.0 );

//		glBegin( GL_POINTS );

//		glColor3f( 1.0, 0.0, 0.0 );
//		glColor3f( 1.0, 1.0, 1.0 );

#if 1
		if ( r_particles[i].count > 20 )
			glColor4f( 1.0, 1.0, 1.0, 1.0 );
		else
			glColor4f( r_particles[i].count/20.0, r_particles[i].count/32.0, r_particles[i].count/32.0, r_particles[i].count/32.0 );
#endif
		{
			int	frame;

			frame = r_particles[i].count & 31;
			if ( i&1 )
				frame = 31-frame;

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
	r_count++;
}


void VolumetricDrawTest( ccluster_t *cc, vec3d_t pos )
{
	static GLuint	texobj = 0;
	vec3d_t		volume[4][4][4];
	ivec3d_t		cpos;
	ivec3d_t		pos2;
	int		x, y, z;
	fp_t	pts[4][2]={ {1.0, -1.0}, {-1.0,-1.0}, { -1.0, 1.0 }, { 1.0, 1.0 } };
	vec4d_t		v={0,0,0,2};
	if ( texobj == 0 )
	{
//		texobj = Misc_GenTexture_TGA_8888( ART_PATH "/fx/smoke2.tga" );	
	}

	//
	// expand compressed cluster 
	//

	U_BeginExpand( &cc->cinfo, cc->cdata );

	for ( z = 0; z < 4; z++ )
	{
		for ( y = 0; y < 4; y++ )
		{
			for ( x = 0; x < 4; x++ )
			{
				U_ExpandFP32( &volume[x][y][z][0] );
				U_ExpandFP32( &volume[x][y][z][1] );
				U_ExpandFP32( &volume[x][y][z][2] );
			}
		}
	}

	U_EndExpand();

	IVec3dRint( cpos, pos );
	IVec3dUnitSnap( cpos, cpos, 16 );

	pos2[0] = cc->pos[0]*4;
	pos2[1] = cc->pos[1]*4;
	pos2[2] = cc->pos[2]*4;

	cpos[0] -= pos2[0];
	cpos[1] -= pos2[1];
	cpos[2] -= pos2[2];

//	IVec3dPrint( cpos );
	Vec3dPrint( volume[cpos[0]][cpos[1]][cpos[2]] );

	glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
	glBindTexture( GL_TEXTURE_2D, texobj );
	glEnable( GL_TEXTURE_2D );
	glEnable ( GL_BLEND );                                          
//	glBlendFunc( GL_ONE, GL_ONE );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
	glDepthMask(GL_FALSE);
	glColor3fv( volume[cpos[0]][cpos[1]][cpos[2]] );

	glBegin( GL_TRIANGLE_FAN );
	glTexCoord2f( 0,0 );
	glVertex3f( (v[0]+pts[0][0])/v[3], (v[1]+pts[0][1])/v[3], 1.0/v[3] );
	glTexCoord2f( 1,0 );
	glVertex3f( (v[0]+pts[1][0])/v[3], (v[1]+pts[1][1])/v[3], 1.0/v[3] );
	glTexCoord2f( 1,1 );
	glVertex3f( (v[0]+pts[2][0])/v[3], (v[1]+pts[2][1])/v[3], 1.0/v[3] );
	glTexCoord2f( 0,1 );
	glVertex3f( (v[0]+pts[3][0])/v[3], (v[1]+pts[3][1])/v[3], 1.0/v[3] );
	glEnd();
	glDepthMask(GL_TRUE);
}
