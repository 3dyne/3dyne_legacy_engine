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



// memlog.c

// anlyze mem.log files

#include <stdio.h>

typedef struct {
	int	ptr;
	int	line;
	int	havefc;

	char	file[32];
	char	funchist[512];
} ent_t;


static ent_t	frees[1024];
static ent_t	mallocs[1024];

int main()
{
	FILE *h;
	
	int	buf[256];

	int	fnum = 0, mnum = 0;

	int	i, i2;

	h = fopen( "mem.log", "r" );

	for( i = 0; i < 2000048; i++ )
	{
		fscanf( h, "%s", buf );

		if( !strcmp( buf, "end" ) )
			break;

		if( !strcmp( buf, "malloc:" ))
		{
			fscanf( h, "%i %s %d\n", &mallocs[mnum].ptr, mallocs[mnum].file, &mallocs[mnum].line );
			fscanf( h, "%s", buf );
			fscanf( h, "%s", &mallocs[mnum].funchist );
			mnum++;
			continue;
		}

		if( !strcmp( buf, "free:" ))
		{
			fscanf( h, "%i %s %d\n", &frees[fnum].ptr, frees[fnum].file, &frees[fnum].line );
			fscanf( h, "%s", buf );
			fscanf( h, "%s", &mallocs[mnum].funchist );
			fnum++;
			continue;
		}
	}

	printf( "mnum: %d fnum: %d unfreed: %d\n", mnum, fnum, mnum - fnum );

	fclose( h );

	for( i2 = 0; i2 < mnum; i2++ )
	{
//		printf( "checking: 0x%x\n", mallocs[i2] );
		for( i = 0; i < fnum; i++ )
		{
			if( mallocs[i2].ptr == frees[i].ptr )
			{
				mallocs[i2].ptr = 0;
//				printf( "found free\n" );
				break;
			}
		}
	}

	for( i = 0; i < mnum; i++ )
	{
		if( mallocs[i].ptr )
		{
			printf( "unfreed: 0x%x, %s, %d\n", mallocs[i].ptr, mallocs[i].file, mallocs[i].line );
			printf( "ftrace: %s\n", mallocs[i].funchist );
		}
	}
}
