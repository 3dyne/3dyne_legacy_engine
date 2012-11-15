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



// lib_parse.c

#include <ctype.h>

#include "lib_parse.h"

#include "shock.h"

/*
  ==============================
  Parse_GetToken

  ==============================
*/
void Parse_GetToken( const char **buf_ptr, char *tkn, int max_tkn_size )
{
	int	c;
	int	lquota = 0;
	int	pos = 0;
	const char	*buf;
	

	//u_token = NULL;
	buf = *buf_ptr;
	
	c = *buf;

	if ( c == 0 )
	{
		tkn[0] = 0;
	}

	for( ;; )
	{
		c = *buf;
		buf++;

		if( c == '\"' )
		{
			lquota = 1;
			c = *(buf++);
			break;
		}
		
		if( !isspace( c ) )
		{
			break;
		}

#if 0
		if( c == '#' )
		{
			tkn[0] = 0;
			return;
		}
#endif
	}
	
	for( ;; )
	{
		if( c == '\"' && lquota )
			break;

		if( c == 0 )
		{
//			__named_message( "0x0" );
			buf--;
			break; 
		}

		if( isspace( c )  && !lquota )
		{
			buf--;
			break;
		}

		if( pos >= max_tkn_size )
			__error( "token too long\n" );

#if 0		
		if( c == '#' )
		{
			tkn[0] = 0;
			return;
		}
#endif	
	
		tkn[pos] = (char)c;
		
		pos++;
		c = *(buf++);	
	}
	if( pos > max_tkn_size )  // double save
		__error( "token too long\n" );	
	
	tkn[pos] = 0;
	*buf_ptr = buf;
	return;	
}
