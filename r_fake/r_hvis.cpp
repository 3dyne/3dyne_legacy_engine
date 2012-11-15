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



// r_hvis.c

/*
  =============================================================================
  HeuristicVIS :
  --------------

  checks the quality of a visleaf PVS and decides whether some visclips
  reduce the polygon count

  =============================================================================
*/

#include "render.h"
#include "r_defs.h"
#include "r_state.h"
#include "r_hvis.h"

/*
  ==============================
  R_HVIS_CullVisleafs

  ==============================
*/

void R_HVIS_CullVisleafs( int visleaf, unsigned char *see, frustum_t *f )
{
	visleaf_t		*vl;
	portal_t		*pt;
	int		i, j;
	int		through_num = 0;
	int		see_num;
	int		test_num;
	int		diff_num;

	int		better_num;
	unsigned char	better_see[4096];


//	for ( i = 0; i < r_visleafnum/8+1; i++ )
//		printf( "%x ", see[i] );

	vl = &r_visleafs[visleaf];

	memset( better_see, 0, 4096 );
	better_num = 0;

	for ( i = 0; i < vl->portalnum; i++ )
	{
		pt = &r_portals[i+vl->startportal];
		
		if ( pt->see_through )
		{
#if 1
			if ( R_HVIS_PortalInFrontOfNear( f, pt ) )
			{
				through_num++;
				R_HVIS_SetupVisclip( f, pt, true );

				// check all pvs leafs with visclip
				see_num = 0;
				test_num = 0;
				for ( j = 0; j < r_visleafnum; j++ )
				{
					if ( (pt->see_through[j>>3] & 1U<<(j&7)) && (see[j>>3] & 1U<<(j&7)) )
					{
						mapnode_t	*n;
						n = &r_mapnodes[r_leafbitpos[j]];	
						
						if ( R_HVIS_VisclipTestBB( n->min, n->max ) )
						{
							better_see[j>>3] |= 1U<<(j&7);
							see_num++;
						}
						test_num++;
					}
				}
//				printf( "%d of %d, ", see_num, test_num );
			}
#else
			for ( j = 0; j < r_visleafnum; j++ )
			{				
				if ( (pt->see_through[j>>3] & 1U<<(j&7)) && (see[j>>3] & 1U<<(j&7)) )
					better_see[j>>3] |= 1U<<(j&7);
			}
#endif

		}
	}
//	printf( "\n %d\n", vl->bitpos );
	better_see[vl->bitpos>>3] |= (1U<<(vl->bitpos&7));
//	better_see[1] |= 2U;

//	for ( i = 0; i < r_visleafnum/8+1; i++ )
//		printf( "%x ", better_see[i] );


	see_num = 0;
	diff_num = 0;
	for ( i = 0; i < r_visleafnum; i++ )
	{
		if ( see[i>>3] & (1U<<(i&7)) )
			see_num++;
		if ( better_see[i>>3] & (1U<<(i&7)) )
			better_num++;

		if ( (see[i>>3] & (1U<<(i&7)) ) ^ (better_see[i>>3] & (1U<<(i&7)) ) )
		{
			diff_num++;
//			printf( "%d:%d != %d, ", i, see[i>>3], better_see[i>>3] );
		}
	}


	memcpy( see, better_see, 4096 );

//	printf( "\n test %d of %d portals: see %d, better_see %d, diff_num %d\n", through_num, vl->portalnum, see_num, better_num, diff_num );
}
