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



// ib_defs.h

#ifndef ib_defs_h
#define ib_defs_h

#define SOURCE_SAR	( 0 )
#define SOURCE_DISK	( 1 )

struct ib_bzdata_s;

typedef struct ib_file_s {
	int (*Read)( void *buf, unsigned int size, struct ib_file_s *self );
	unsigned int (*GetSize)( struct ib_file_s *self );
	void (*SetPos)( struct ib_file_s *self, unsigned int pos );
	unsigned int (*GetPos)( struct ib_file_s *self );
	void (*Close)( struct ib_file_s *self );

	char	filename[256]; // this may be usefull for debuging ...

	struct ib_bzdata_s	*bz;

	unsigned char	*getcbuf;
	int	getcpos;
	int	getcdata;

	union private_u {
		struct ib_file_sar_private_s {
			FILE *h;
			unsigned int	pos;
			unsigned int	size;
			unsigned int	pointer;
		} sar;
		
		struct ib_file_disk_private_s {
			FILE *h;
		} disk;
	} private_;
} ib_file_t;

typedef ib_file_t ibfile_t;

#endif
