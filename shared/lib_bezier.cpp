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



// lib_bezier.c

#include "lib_bezier.h"
#include <string.h>


/*
  ==================================================
  internal bezier stuff
  
  ==================================================
*/


static	int	inv_tab_init = 0;
float		inv_tab[16];

void init_eval( void )
{
	int		i;

	if ( inv_tab_init == 0)
		inv_tab_init = 1;
	else
		return;

	for ( i = 1; i < 16; i++ )
		inv_tab[i] = 1.0 / i;
}


/*
  ====================
  EvalBezierCurve3d

  calc point <out> at coord <t> 
  from ctrlpoints <cp>
  ====================
*/
  
void EvalBezierCurveVec3d( fp_t *cp, vec3d_t out, float t, int order )
{
	float		s, powert;
	int		i, k, bincoeff;

	init_eval();	// fixme

	if ( order >= 2 )
	{
		bincoeff = order - 1;
		s = 1.0 - t;

		for ( k = 0; k < 3; k++ )
			out[k] = s*cp[k] + bincoeff*t*cp[3+k];

		for ( i = 2, cp+=2*3, powert=t*t; i < order; 
		      i++, powert*=t, cp+=3 )
		{
			bincoeff *= order-i;
			bincoeff *= inv_tab[i];

			for( k = 0; k<3; k++ )
				out[k] = s*out[k] + bincoeff*powert*cp[k];
		}
	}
	else
	{
		for ( k = 0; k < 3; k++ )
			out[k] = cp[k];
	}
}

void EvalBezierSurfaceVec3d( fp_t *cn, vec3d_t out, fp_t u, fp_t v, int uorder, int vorder )
{
	float		*cp;
	int		i, uinc;

	init_eval(); // fixme

	cp = cn+uorder*vorder*3;
	uinc = vorder*3;

	if ( vorder > uorder )
	{
		if ( uorder >= 2 )
		{
			float		s, poweru;
			int		j, k, bincoeff;

			for ( j = 0; j < vorder; j++ )
			{
				float	*ucp;

				ucp = &cn[j*3];
				bincoeff = uorder - 1;
				s = 1.0 - u;
				
				for ( k = 0; k < 3; k++ )
					cp[j*3+k] = s*ucp[k] + bincoeff*u*ucp[uinc+k];

				for ( i = 2, ucp += 2*uinc, poweru = u*u; i < uorder;
				      i++, poweru *= u, ucp += uinc )
				{
					bincoeff *= uorder-i;
					bincoeff *= inv_tab[i];
					
					for ( k = 0; k < 3; k++ )
						cp[j*3+k] = s*cp[j*3+k] + 
							bincoeff*poweru*ucp[k];
				}
			}
			EvalBezierCurveVec3d( cp, out, v, vorder );
		}
		else
		{
			EvalBezierCurveVec3d( cn, out, v, vorder );
		}
	}
	else
	{
		if ( vorder > 1 )
		{
			int		i;

			for ( i = 0; i < uorder; i++, cn += uinc )
			{
				EvalBezierCurveVec3d( cn, &cp[i*3], v, vorder );
			}

			EvalBezierCurveVec3d( cp, out, u, uorder );
		}
		else
		{
			EvalBezierCurveVec3d( cn, out, u, uorder );
		}
	}
}


/*
  ==================================================
  lib_bezier interface

  ==================================================
*/



/*
  ========================================
  malloc and free

  ========================================
*/


/*
  ====================
  NewBezierCurve

  ====================
*/

curve_ctrl_t* NewBezierCurve( int order )
{
	curve_ctrl_t	*c;
	size_t		size;

	size = (size_t)((curve_ctrl_t *)0)->cp[order];
	c = (curve_ctrl_t *) malloc( size );
	memset( c, 0, size );

	c->order = order;
	
	return c;
}



/*
  ====================
  FreeBezierCurve

  ====================
*/
void FreeBezierCurve( curve_ctrl_t *c )
{
	free( c );
}
  


/*
  ====================
  NewBezierSurface

  ====================
*/
surface_ctrl_t* NewBezierSurface( int uorder, int vorder )
{
	surface_ctrl_t		*c;
	size_t			size;
	// fixme
	size = (size_t)((surface_ctrl_t *)0)->cp[uorder*vorder+(uorder>vorder?uorder:vorder)];
	c = (surface_ctrl_t *) malloc( size );
	memset( c, 0, size );

	c->uorder = uorder;
	c->vorder = vorder;

	return c;
}



/*
  ====================
  FreeBezierSurface

  ====================
*/
void FreeBezierSurface( surface_ctrl_t *sc )
{
	free( sc );
}


/*
  ========================================
  ControlPoints

  ========================================
*/



/*
  ====================
  SetCurveCtrlPoint

  ====================
*/
void SetCurveCtrlPoint( curve_ctrl_t *c, int u, vec3d_t pos )
{       
	// fixme: range check ?
	Vec3dCopy( c->cp[u], pos );
}

void SetCurveCtrlPoint3f( curve_ctrl_t *c, int u, fp_t x, fp_t y, fp_t z )
{
	Vec3dInit( c->cp[u], x, y, z );
}



/*
  ====================
  SetSurfaceCtrlPoint

  ====================
*/
void SetSurfaceCtrlPoint( surface_ctrl_t *sc, int u, int v, vec3d_t pos )
{
	Vec3dCopy( sc->cp[v*sc->uorder+u], pos );
}

void SetSurfaceCtrlPoint3f( surface_ctrl_t *sc, int u, int v, fp_t x, fp_t y, fp_t z )
{
	Vec3dInit( sc->cp[v*sc->uorder+u], x, y, z );
}

void GetSurfaceCtrlPoint( surface_ctrl_t *sc, int u, int v, vec3d_t pos )
{
	Vec3dCopy( pos, sc->cp[v*sc->uorder+u] );	
}

/*
  ========================================
  eval

  ========================================
*/



/*
  ====================
  EvalCurvePoint

  ====================
*/
void EvalCurvePoint( curve_ctrl_t *c, fp_t u, vec3d_t out )
{
	EvalBezierCurveVec3d( c->cp[0], out, u, c->order );
}



/*
  ====================
  EvalCurvePoints

  ====================
*/
curve_points_t* EvalCurvePoints( curve_ctrl_t *cc, int unum )
{
	curve_points_t		*cp;
	size_t			size;
	fp_t			u;
	int			s;

	size = (size_t)((curve_points_t *)0)->p[unum];
	cp = (curve_points_t *) malloc( size );

	cp->pointnum = unum;
	
	unum--;
	for ( s = 0; s <= unum; s++ )
	{
		u = (fp_t)s / (fp_t)unum;
		EvalBezierCurveVec3d( cc->cp[0], cp->p[s], u, cc->order );
	}

	return cp;
}


/*
  ====================
  EvalSurfacePoint

  ====================
*/
void EvalSurfacePoint( surface_ctrl_t *sc, fp_t u, fp_t v, vec3d_t out )
{
	EvalBezierSurfaceVec3d( sc->cp[0], out, u, v, sc->uorder, sc->vorder );
}

/*
  ====================
  EvalSurfacePoints

  ====================
*/
surface_points_t* EvalSurfacePoints( surface_ctrl_t *sc, int unum, int vnum )
{
	surface_points_t	*sp;
	size_t			size;
	fp_t			u, v;
	int			s, t;

	fp_t			tmp[1024];
	int			tsize;

	if ( sc->uorder > sc->vorder )
	{
		tsize = sizeof(vec3d_t)*sc->uorder*sc->vorder+sizeof(vec3d_t)*sc->uorder;
	}
	else
	{
		tsize = sizeof(vec3d_t)*sc->uorder*sc->vorder+sizeof(vec3d_t)*sc->vorder;
	}

	size = (size_t)((surface_points_t *)0)->p[unum*vnum];
	sp = (surface_points_t *) malloc( size );

	sp->upointnum = unum;
	sp->vpointnum = vnum;

	unum--;
	vnum--;
	for ( s = 0; s <= unum; s++ )
	{
		u = (fp_t)s / (fp_t)unum;
		for ( t = 0; t <= vnum; t++ )
		{
			v = (fp_t)t / (fp_t)vnum;

			memcpy( tmp, sc->cp[0], tsize );
			EvalBezierSurfaceVec3d( tmp, sp->p[t*(unum+1)+s], u, v, sc->uorder, sc->vorder );
		}
	}
	
	return sp;
}



/*
  ====================
  GetSurfacePoint

  ====================
*/
void GetSurfacePoint( surface_points_t *mesh, int u, int v, vec3d_t pos )
{
	Vec3dCopy( pos, mesh->p[u+v*mesh->upointnum] );
}



/*
  ====================
  SetSurfacePoint

  ====================
*/
void SetSurfacePoint( surface_points_t *mesh, int u, int v, vec3d_t pos )
{
	Vec3dCopy( mesh->p[u+v*mesh->upointnum], pos ); 
}



/*
  ====================
  GetCurvePoint

  ====================
*/
void GetCurvePoint( curve_points_t *curve, int u, vec3d_t pos )
{
	Vec3dCopy( pos, curve->p[u] );
}


/*
  ====================
  FreeCurvePoints

  ====================
*/
void FreeCurvePoints( curve_points_t *c )
{
	free( c );
}

/*
  ====================
  FreeSurfacePoints
  
  ====================
*/
void FreeSurfacePoints( surface_points_t *sp )
{
	free( sp );
}


/*
  ==============================
  EvalUVPoints

  ==============================
*/
uv_points_t * EvalUVPoints( surface_ctrl_t *sc, int unum, int vnum )
{
	uv_points_t	*sp;
	size_t			size;
	fp_t			u, v;
	int			s, t;

	size = (size_t)((uv_points_t *)0)->p[unum*vnum];
	sp = (uv_points_t *) malloc( size );

	sp->upointnum = unum;
	sp->vpointnum = vnum;

	unum--;
	vnum--;
	for ( s = 0; s <= unum; s++ )
	{
		u = (fp_t)s / (fp_t)unum;
		for ( t = 0; t <= vnum; t++ )
		{
			v = (fp_t)t / (fp_t)vnum;

//			EvalBezierSurfaceVec3d( tmp, sp->p[t*(unum+1)+s], u, v, sc->uorder, sc->vorder );
			sp->p[t*(unum+1)+s][0] = u;
			sp->p[t*(unum+1)+s][1] = v;
		}
	}
	
	return sp;	
}

void FreeUVPoints( uv_points_t *pts )
{
	free( pts );
}

void GetUVPoint( uv_points_t *mesh, int u, int v, vec2d_t pos )
{
	Vec2dCopy( pos, mesh->p[u+v*mesh->upointnum] );
}


/*
  ====================
  NewSurfacePoints

  ====================
*/
surface_points_t* NewSurfacePoints( int unum, int vnum )
{
	surface_points_t	*sp;
	size_t			size;

	size = (size_t)((surface_points_t *)0)->p[unum*vnum];
	sp = (surface_points_t *) malloc( size );
	memset( sp, 0, size );

	sp->upointnum = unum;
	sp->vpointnum = vnum;

	return sp;
}


/*
  ====================
  SurfaceNormals

  ====================
*/
bool_t IsPointValid( int u, int v, int unum, int vnum )
{
	if ( u < 0 || v < 0 || u >= unum || v >= vnum )
		return false;
	return true;
}

surface_points_t* EvalSurfaceNormals( surface_points_t *in )
{
	vec3d_t		un, vn;
	vec3d_t		center;
	vec3d_t		tmp;
	int		unum, vnum;
	int		u, v;
	vec3d_t		norm;


	surface_points_t *nmesh;

	unum = in->upointnum;
	vnum = in->vpointnum;

	nmesh = NewSurfacePoints( unum, vnum );

	for ( u = 0; u < unum; u++ )
		for ( v = 0; v < vnum; v++ )
		{
			GetSurfacePoint( in, u, v, center );
			
			Vec3dInit( vn, 0, 0, 0 );
			Vec3dInit( un, 0, 0, 0 );

			if ( IsPointValid( u-1, v, unum, vnum ) )
			{
				GetSurfacePoint( in, u-1, v, tmp );
				Vec3dSub( tmp, center, tmp );
				Vec3dAdd( un, tmp, un );
			}
			if ( IsPointValid( u+1, v, unum, vnum ) )
			{
				GetSurfacePoint( in, u+1, v, tmp );
				Vec3dSub( tmp, tmp, center );
				Vec3dAdd( un, tmp, un );
			}
			if ( IsPointValid( u, v-1, unum, vnum ) )
			{
				GetSurfacePoint( in, u, v-1, tmp );
				Vec3dSub( tmp, center, tmp );
				Vec3dAdd( vn, tmp, vn );
			}
			if ( IsPointValid( u, v+1, unum, vnum ) )
			{
				GetSurfacePoint( in, u, v+1, tmp );
				Vec3dSub( tmp, tmp, center );
				Vec3dAdd( vn, tmp, vn );
			}

			Vec3dCrossProduct( norm, un, vn );
			Vec3dUnify( norm );
			
			SetSurfacePoint( nmesh, u, v, norm );
		}

	return nmesh;
}

#if 0
int main()
{
	int		i;

	curve_ctrl_t	*cc;
	curve_points_t	*p;
	surface_ctrl_t	*sc;
	surface_points_t	*sp;
	surface_points_t	*nmesh;

	init_eval();

#if 0
	cc = NewBezierCurve( 3 );
	SetCurveCtrlPoint3f( cc, 0, 3,3,0 );
	SetCurveCtrlPoint3f( cc, 1, 0,3,0 );
	SetCurveCtrlPoint3f( cc, 2, 0,0,0 );

	p = EvalCurvePoints( cc, 10 );

	for ( i = 0; i < p->pointnum; i++ )
	{
		printf( "%f %f %f\n", p->p[i][0], p->p[i][1], p->p[i][2] );
	}
	
	FreeCurvePoints( p );
	FreeBezierCurve( cc );
#else
	sc = NewBezierSurface( 2, 2 );
	SetSurfaceCtrlPoint3f( sc, 0,0, 0,0,0 );
	SetSurfaceCtrlPoint3f( sc, 1,0, 1,0,0 );
	SetSurfaceCtrlPoint3f( sc, 1,1, 1,1,0 );
	SetSurfaceCtrlPoint3f( sc, 0,1, 0,1,0 );

	sp = EvalSurfacePoints( sc, 10, 10 );
	
	nmesh = EvalSurfaceNormals( sp );

	for ( i = 0; i < 100; i++ )
	{
		printf( "%f %f %f\n", nmesh->p[i][0], nmesh->p[i][1], nmesh->p[i][2] );
	}
	
	FreeSurfacePoints( sp );
	FreeBezierSurface( sc );
#endif

}

#endif
