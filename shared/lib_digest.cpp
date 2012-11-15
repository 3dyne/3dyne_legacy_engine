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



// lib_digest.c

#include "lib_digest.h"

void InfoDigest_Begin( gen_info_digest_t *gen )
{
	gen->num = 0;
	gen->digest = 0;
}

void InfoDigest_Advance( gen_info_digest_t *gen, int num_byte, void *src )
{
	int		i;
	unsigned char		*c;

	info_digest_t	copy;

	static unsigned char subst[256] = {                                          
		41, 46, 67, 201, 162, 216, 124, 1, 61, 54, 84, 161, 236, 240, 6,              
		19, 98, 167, 5, 243, 192, 199, 115, 140, 152, 147, 43, 217, 188,              
		76, 130, 202, 30, 155, 87, 60, 253, 212, 224, 22, 103, 66, 111, 24,           
		138, 23, 229, 18, 190, 78, 196, 214, 218, 158, 222, 73, 160, 251,             
		245, 142, 187, 47, 238, 122, 169, 104, 121, 145, 21, 178, 7, 63,              
		148, 194, 16, 137, 11, 34, 95, 33, 128, 127, 93, 154, 90, 144, 50,            
		39, 53, 62, 204, 231, 191, 247, 151, 3, 255, 25, 48, 179, 72, 165,            
		181, 209, 215, 94, 146, 42, 172, 86, 170, 198, 79, 184, 56, 210,              
		150, 164, 125, 182, 118, 252, 107, 226, 156, 116, 4, 241, 69, 157,            
		112, 89, 100, 113, 135, 32, 134, 91, 207, 101, 230, 45, 168, 2, 27,           
		96, 37, 173, 174, 176, 185, 246, 28, 70, 97, 105, 52, 64, 126, 15,            
		85, 71, 163, 35, 221, 81, 175, 58, 195, 92, 249, 206, 186, 197,               
		234, 38, 44, 83, 13, 110, 133, 40, 132, 9, 211, 223, 205, 244, 65,            
		129, 77, 82, 106, 220, 55, 200, 108, 193, 171, 250, 36, 225, 123,             
		8, 12, 189, 177, 74, 120, 136, 149, 139, 227, 99, 232, 109, 233,              
		203, 213, 254, 59, 0, 29, 57, 242, 239, 183, 14, 102, 88, 208, 228,           
		166, 119, 114, 248, 235, 117, 75, 10, 49, 68, 80, 180, 143, 237,              
		31, 26, 219, 153, 141, 51, 159, 17, 131, 20                                   
	};

	c = (unsigned char *)src;
	copy = gen->digest;

	for ( i = 0; i < num_byte; i++ )
	{
		copy = (copy<<3) ^ ((copy&((1<<31)+(1<<30)+(1<<29)))>>29); 
		copy ^= (subst[((*c)+gen->num)&255]);

		gen->num++;
		
		c++;
	}

	gen->digest = copy;
}

info_digest_t InfoDigest_End( gen_info_digest_t *gen )
{
	return gen->digest;
}

unsigned int BufferChecksum32( int size, char *buf )
{
	gen_info_digest_t	gen;	

	InfoDigest_Begin( &gen );
	InfoDigest_Advance( &gen, size, (void *) buf );
	
	return InfoDigest_End( &gen );
}

unsigned short BufferChecksum16( int size, char *buf )
{
	gen_info_digest_t	gen;	

	unsigned int	d;

	InfoDigest_Begin( &gen );
	InfoDigest_Advance( &gen, size, (void *) buf );
	
	d =  InfoDigest_End( &gen );	

	return (unsigned short)((d&0xffff)^((d>>16)&0xffff));
}
