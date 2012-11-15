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



// sh_parser.h

#ifndef sh_parser_h
#define sh_parser_h

#include "defs.h"

extern char		shargs[16][256];
extern sh_command_t	shcommands[];

void SHP_StartUp( void );
void SHP_ShutDown( void );

void SHP_ParseFile( ibfile_t* handle );
void SHP_ParseBuf( const char* );
void SHP_ParseLine( char* );
void SHP_ParseToken( char **buf_ptr );
sh_var_t *SHP_GetVar( const char * );
sh_var_t *SHP_SetVar( const char *, const char *, unsigned int );

void SHP_ParseToken2( unsigned char **buf_ptr, unsigned char *tkn, int max_tkn_size );

#endif
