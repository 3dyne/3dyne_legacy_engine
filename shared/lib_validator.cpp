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



// lib_validator.c

#include "lib_validator.h"

/*
  ==============================
  Validator_Init

  ==============================
*/
void Validator_Init( validator_t *self )
{
	self->is_valid = false;
	self->upward = NULL;
}

/*
  ==============================
  Validator_IsValid

  ==============================
*/
bool_t Validator_IsValid( validator_t *self )
{
	return self->is_valid;
}

/*
  ==============================
  Validator_SetValid

  ==============================
*/
bool_t Validator_SetValid( validator_t *self )
{
	self->is_valid = true;
}

/*
  ==============================
  Validator_InvalidateUpward
  
  ==============================
*/
void Validator_InvalidateUpward( validator_t *self )
{
	for ( ; self ; self=self->upward )
	{
		self->is_valid = false;
	}
}

/*
  ==============================
  Validator_SetUpward

  ==============================
*/
void Validator_LinkUpward( validator_t *self, validator_t *upward )
{
	slef->upward = upward;
}
