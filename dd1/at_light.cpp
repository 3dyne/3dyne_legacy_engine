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



// at_light.c

#include "l_locals.h"

/*
  ==============================
  AT_LightDelete

  ==============================
*/
void AT_LightDelete( at_light_t *self )
{
	L_ChangeToDead( &self->ati );
}

/*
  ==============================
  AT_LightDestroy

  ==============================
*/
void AT_LightDestroy( at_light_t *self )
{
	li.MM_Free( self );
}

/*
  ==============================
  AT_LightGetOrigin

  ==============================
*/
void AT_LightGetOrigin( at_light_t *self, vec3d_t vec )
{
	Vec3dCopy( vec, self->origin );
}

/*
  ==============================
  AT_LightSetOrigin

  ==============================
*/
void AT_LightSetOrigin( at_light_t *self, vec3d_t vec )
{
	Vec3dCopy( self->origin, vec );
}

#define AT_LIGHT_STYLE_NORMAL			( 0 )
#define AT_LIGHT_STYLE_SIN_WAVE1		( 1 )
#define AT_LIGHT_STYLE_FADE_IN1			( 2 )
#define AT_LIGHT_STYLE_FADE_OUT1		( 3 )
#define AT_LIGHT_STYLE_FADE_IN_OUT1		( 4 )
#define AT_LIGHT_STYLE_NEON1			( 5 )
#define AT_LIGHT_STYLE_NEON_BROKEN1		( 6 )
#define AT_LIGHT_STYLE_ARC1			( 7 )

/*
  ==============================
  AT_LightCom

  ==============================
*/
void AT_LightCom( at_light_t *self, char *in, char *out )
{
//	printf( "COM: reciever [%s,%s] got '%s'\n", self->ati.type, self->ati.name, in );
	sprintf( out, "return: ok" );
	
	if ( self->ati.state == ATIState_sleep )
	{
		L_ChangeToWakeUp( &self->ati );	
	}

	__chkptr( in );
	self->state = atoi( in );

	self->wf_count = 0;
}


/*
  ==============================
  SearchLightSource

  ==============================
*/
static unique_t SearchLightSource( unique_t light_id )
{
	hobj_search_iterator_t	iter;
	hobj_t		*source;

	__chkptr( l_lightsource_root );

	InitClassSearchIterator( &iter, l_lightsource_root, "lightsource" );
	for ( ; ( source = SearchGetNextClass( &iter ) ) ; )
	{
		hpair_t		*pair;
		unique_t	other;

		pair = FindHPair( source, "light" );
		if ( !pair )
			continue;

		other = StringToUnique( pair->value );

		if ( other == light_id )
		{
			return StringToUnique( source->name );
		}
	}
	
	return UNIQUE_INVALIDE;		
}

static unique_t		select_source;
static unique_t		select_shape;
static void SelectShapeFunc( unique_t shape_id )
{
	unique_t		source_id;

	source_id = SearchLightSource(shape_id);
	__named_message( "shape: #%u, lightsource: #%u\n", shape_id, source_id );	
	if ( source_id != UNIQUE_INVALIDE )
	{
		select_source = source_id;
		select_shape = shape_id;
	}
}

/*
  ==============================
  AT_LightThink1

  ==============================
*/
void AT_LightThink1( at_light_t *self )
{
	
	// a light thinks only once in its life to register itself in the switchmap
//	__named_message( "wf_count %d, state %d\n", self->wf_count, self->state );

	if ( self->run_count == 0 )
	{
		if ( !strcmp( "facelight", self->ati.type ) )
		{
			__named_message( "facelight\n" );
			select_source = UNIQUE_INVALIDE;
			li.GI_SelectShapesInBoundBox( self->min, self->max, SelectShapeFunc );
			if ( select_source == UNIQUE_INVALIDE )
			{
				__warning( "light '%s' of type '%s' can't find its lightsource, die\n", self->ati.name, self->ati.type );	
				L_ChangeToDead( &self->ati );
				return;
			}
			self->lightsource_id = select_source;
			self->shape_id = select_shape;
			
			if ( self->switch_id != UNIQUE_INVALIDE )
			{
				L_SwitchMapInsertSwitchAble( (ati_t *) self, self->switch_id );
			}			
		}
		else 
		{
			//
			// look for a lightsource
			//
			self->lightsource_id = SearchLightSource( StringToUnique( self->ati.name ) );			
			if ( self->lightsource_id == UNIQUE_INVALIDE )
			{
				__warning( "light '%s' of type '%s' can't find its lightsource, die\n", self->ati.name, self->ati.type );
				L_ChangeToDead( &self->ati );
				return;
			}
			
			self->shape_id = UNIQUE_INVALIDE;	// ignore the facelight stuff

			//
			// insert into switchable map
			//			
			if ( self->switch_id != UNIQUE_INVALIDE )
			{
				L_SwitchMapInsertSwitchAble( (ati_t *) self, self->switch_id );
			}
		}
		self->wf_count = 0;
	}

	if ( self->style == AT_LIGHT_STYLE_NORMAL )
	{
		if ( self->wf_count == 0 )
		{
			if ( self->state == 1 )
			{
				// switch on stage		    			     
				self->wf_light.state = 1;
				self->wf_light.style_string = NULL;
				self->wf_light.lightsource_id = self->lightsource_id;
				li.GI_RegisterWFInfo( WFInfoType_light, &self->wf_light );				
				if ( self->shape_id != UNIQUE_INVALIDE )
				{
					Vec3dInit( self->wf_shape.color, 1.0, 1.0, 1.0 );
					self->wf_shape.style_string = NULL;
					self->wf_shape.shape_id = self->shape_id;
					li.GI_RegisterWFInfo( WFInfoType_shape, &self->wf_shape );
				}
			}
			else if ( self->state == 0 )
			{
				// switch off stage
				self->wf_light.state = 0;
				self->wf_light.style_string = NULL;
				self->wf_light.lightsource_id = self->lightsource_id;
				li.GI_RegisterWFInfo( WFInfoType_light, &self->wf_light );
				if ( self->shape_id != UNIQUE_INVALIDE )
				{
					Vec3dInit( self->wf_shape.color, 0.2, 0.2, 0.2 );
					self->wf_shape.style_string = NULL;
					self->wf_shape.shape_id = self->shape_id;
					li.GI_RegisterWFInfo( WFInfoType_shape, &self->wf_shape );
				}
			}
			L_ChangeToSleep( &self->ati );
		}
	}

	if ( self->style == AT_LIGHT_STYLE_SIN_WAVE1 )
	{
		if ( self->wf_count == 0 )
		{
			if ( self->state == 1 )
			{
				// switch on
				self->wf_light.state = 1; //|2;
				self->wf_light.style_string = "ABCDEFGHIJKLMNOPPONMLKJIHGFEDCBA";
				self->wf_light.lightsource_id = self->lightsource_id;
				li.GI_RegisterWFInfo( WFInfoType_light, &self->wf_light );	
			}
			else if ( self->state == 0 )
			{
				// switch off stage
				self->wf_light.state = 0;
				self->wf_light.style_string = NULL;
				self->wf_light.lightsource_id = self->lightsource_id;
				li.GI_RegisterWFInfo( WFInfoType_light, &self->wf_light );
			}
			L_ChangeToSleep( &self->ati );
		}
	}

	if ( self->style == AT_LIGHT_STYLE_FADE_IN1 )
	{
		if ( self->wf_count == 0 )
		{
			if ( self->state == 1 )
			{
				// switch on
				self->wf_light.state = 1|2;
				self->wf_light.style_string = "ABCDEFGHIJKLMNOPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP";
				self->wf_light.lightsource_id = self->lightsource_id;
				li.GI_RegisterWFInfo( WFInfoType_light, &self->wf_light );				
			}
			else if ( self->state == 0 )
			{
				// switch off stage
				self->wf_light.state = 0;
				self->wf_light.style_string = NULL;
				self->wf_light.lightsource_id = self->lightsource_id;
				li.GI_RegisterWFInfo( WFInfoType_light, &self->wf_light );	
				L_ChangeToSleep( &self->ati );			
			}
		}
		if ( self->wf_count == 10 )
		{
			if ( self->state == 1 )
			{
				// switch on
				self->wf_light.state = 1;
				self->wf_light.style_string = NULL;
				self->wf_light.lightsource_id = self->lightsource_id;
				li.GI_RegisterWFInfo( WFInfoType_light, &self->wf_light );				
				L_ChangeToSleep( &self->ati );
			}	
			else if ( self->state == 0 )
			{
				
			}
		}
	}

	if ( self->style == AT_LIGHT_STYLE_FADE_OUT1 )
	{
		if ( self->wf_count == 10 )
		{
			if ( self->state == 1 )
			{
			
			}
			else if ( self->state == 0 )
			{
				// stop switch off
				self->wf_light.state = 0;
				self->wf_light.style_string = NULL;
				self->wf_light.lightsource_id = self->lightsource_id;
				li.GI_RegisterWFInfo( WFInfoType_light, &self->wf_light );				
				L_ChangeToSleep( &self->ati );				
			}			
		}
		if ( self->wf_count == 0 )
		{
			if ( self->state == 1 )
			{
				// switch on
				self->wf_light.state = 1;
				self->wf_light.style_string = NULL;
				self->wf_light.lightsource_id = self->lightsource_id;
				li.GI_RegisterWFInfo( WFInfoType_light, &self->wf_light );				
				L_ChangeToSleep( &self->ati );
			}
			else if ( self->state == 0 )
			{
				// start switch off
				self->wf_light.state = 1|2;
				self->wf_light.style_string = "PONMLKJIHGFEDCBAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
				self->wf_light.lightsource_id = self->lightsource_id;
				li.GI_RegisterWFInfo( WFInfoType_light, &self->wf_light );				
			}	

		}
	}

	if ( self->style == AT_LIGHT_STYLE_FADE_IN_OUT1 )
	{
		if ( self->wf_count == 0 )
		{
			if ( self->state == 1 )
			{
				// start switch on
				self->wf_light.state = 1|2;
				self->wf_light.style_string = "ACEGIKMOPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPPP";
				self->wf_light.lightsource_id = self->lightsource_id;
				li.GI_RegisterWFInfo( WFInfoType_light, &self->wf_light );				
			}
			else if ( self->state == 0 )
			{
				// start switch off
				self->wf_light.state = 1|2;
				self->wf_light.style_string = "PONMLKJIHGFEDCBAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
				self->wf_light.lightsource_id = self->lightsource_id;
				li.GI_RegisterWFInfo( WFInfoType_light, &self->wf_light );				
			}			
		}
		if ( self->wf_count == 10 )
		{
			if ( self->state == 1 )
			{
				// stop switch on
				self->wf_light.state = 1;
				self->wf_light.style_string = NULL;
				self->wf_light.lightsource_id = self->lightsource_id;
				li.GI_RegisterWFInfo( WFInfoType_light, &self->wf_light );				
				L_ChangeToSleep( &self->ati );
			}	
			else if ( self->state == 0 )
			{
				// stop switch off
				self->wf_light.state = 0;
				self->wf_light.style_string = NULL;
				self->wf_light.lightsource_id = self->lightsource_id;
				li.GI_RegisterWFInfo( WFInfoType_light, &self->wf_light );	
				L_ChangeToSleep( &self->ati );	
			}
		}
	}

	if ( self->style == AT_LIGHT_STYLE_NEON_BROKEN1 )
	{
		if ( self->wf_count == 0 )
		{
			if ( self->state == 1 )
			{
				// switch on stage		    			     
				self->wf_light.state = 1;
				self->wf_light.style_string = "PAAPAPPAPAAAPAPPAAAAPAAPPPPAPAAAPAAPAAAAPAAPAAAAAAAAAPPPP";
				self->wf_light.lightsource_id = self->lightsource_id;
				li.GI_RegisterWFInfo( WFInfoType_light, &self->wf_light );

				// handel facelight shapes
				if ( self->shape_id != UNIQUE_INVALIDE )
				{
					Vec3dInit( self->wf_shape.color, 1.0, 1.0, 1.0 );
					self->wf_shape.style_string = self->wf_light.style_string;
					self->wf_shape.shape_id = self->shape_id;
					self->wf_shape.order_count++;	// old jobs dies
					li.GI_RegisterWFInfo( WFInfoType_shape, &self->wf_shape );
				}				
			}
			else if ( self->state == 0 )
			{
				// switch off stage
				self->wf_light.state = 0;
				self->wf_light.style_string = NULL;
				self->wf_light.lightsource_id = self->lightsource_id;
				li.GI_RegisterWFInfo( WFInfoType_light, &self->wf_light );

				// handel facelight shapes
				if ( self->shape_id != UNIQUE_INVALIDE )
				{
					Vec3dInit( self->wf_shape.color, 0.1, 0.1, 0.1 );
					self->wf_shape.style_string = self->wf_light.style_string;
					self->wf_shape.shape_id = self->shape_id;
					self->wf_shape.order_count++;	// old jobs dies
					li.GI_RegisterWFInfo( WFInfoType_shape, &self->wf_shape );
				}
			}
			L_ChangeToSleep( &self->ati );			
		}	       
	}

	if ( self->style == AT_LIGHT_STYLE_ARC1 )
	{
		if ( self->wf_count == 0 )
		{
			if ( self->state == 1 )
			{
				// switch on stage		    			     
				self->wf_light.state = 1;
				self->wf_light.style_string = "PAPAPAPAPAPAAAAAAAAAAAAAAAAAAAAAAAAPAPAPAPAAAAAAAAAAAAAPAPAAAAAAAAAAA";
				self->wf_light.lightsource_id = self->lightsource_id;
				li.GI_RegisterWFInfo( WFInfoType_light, &self->wf_light );				
			}
			else if ( self->state == 0 )
			{
				// switch off stage
				self->wf_light.state = 0;
				self->wf_light.style_string = NULL;
				self->wf_light.lightsource_id = self->lightsource_id;
				li.GI_RegisterWFInfo( WFInfoType_light, &self->wf_light );
			}
			L_ChangeToSleep( &self->ati );			
		}	       
	}


	if ( self->style == AT_LIGHT_STYLE_NEON1 )
	{
		if ( self->wf_count == 0 )
		{
			if ( self->state == 1 )
			{
				static char *neon_styles[8] = { 
					"PAPAPPAPAAAPAPPAAAAPAAPPAPAAAPAAPAAAAPAAPAAAPAAPAAAP",
					"APPPAPPAPPAAAAAPAPPAAAAAAAAAAPAAAAPAAAAPAAAAAPAAAAPA",
					"AAAAAAAPAAAAAPAAAPPPPPPPPPPAAAAAAAPPPPPAPPPPPPPAAAAA",
					"APAPPPAPAAAAAPPAAAPAPPAAPAPAAAPAPPPAAAPAPPPAPAPAAAPA",
					"PPPPAPPPPPAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAPPAPPPPAPPPP",
					"PPPPPPPPPAPPPPPPPPPAPAAAAPPPPPPPAAAAAAPPPPAAAAPPPPPP",
					"AAPPPAPPAPPPAPAAPPPAPAAPAPAPAPAPPAPPPAAAPAPPAAPPAPAA",
					"PPAPAAPPAPAPAAAPAPPPAAPPAPAPAPPAPPPAPPPPAAPPAPAPAPPA" };
				// switch on
				self->wf_light.state = 1;

				
				self->wf_light.style_string = neon_styles[random()&7];
				self->wf_light.lightsource_id = self->lightsource_id;
				li.GI_RegisterWFInfo( WFInfoType_light, &self->wf_light );
				
				// handel facelight shapes
				if ( self->shape_id != UNIQUE_INVALIDE )
				{
					Vec3dInit( self->wf_shape.color, 1.0, 1.0, 1.0 );
					self->wf_shape.style_string = self->wf_light.style_string;
					self->wf_shape.shape_id = self->shape_id;
					self->wf_shape.order_count++;	// old jobs dies
					li.GI_RegisterWFInfo( WFInfoType_shape, &self->wf_shape );
				}	
			}
			else if ( self->state == 0 )
			{
				// switch off stage
				self->wf_light.state = 0;
				self->wf_light.style_string = NULL;
				self->wf_light.lightsource_id = self->lightsource_id;
				li.GI_RegisterWFInfo( WFInfoType_light, &self->wf_light );	
				L_ChangeToSleep( &self->ati );	

				// handel facelight shapes
				if ( self->shape_id != UNIQUE_INVALIDE )
				{
					Vec3dInit( self->wf_shape.color, 0.1, 0.1, 0.1 );
					self->wf_shape.style_string = self->wf_light.style_string;
					self->wf_shape.shape_id = self->shape_id;
					self->wf_shape.order_count++;	// old jobs dies
					li.GI_RegisterWFInfo( WFInfoType_shape, &self->wf_shape );
				}				
			}
		}
		if ( self->wf_count == 10 )
		{
			if ( self->state == 1 )
			{
				// switch on
				self->wf_light.state = 1;
				self->wf_light.style_string = NULL;
				self->wf_light.lightsource_id = self->lightsource_id;
				li.GI_RegisterWFInfo( WFInfoType_light, &self->wf_light );				
				L_ChangeToSleep( &self->ati );
				
				// handel facelight shapes
				if ( self->shape_id != UNIQUE_INVALIDE )
				{
					Vec3dInit( self->wf_shape.color, 1.0, 1.0, 1.0 );
					self->wf_shape.style_string = self->wf_light.style_string;
					self->wf_shape.shape_id = self->shape_id;
					self->wf_shape.order_count++;	// old jobs dies
					li.GI_RegisterWFInfo( WFInfoType_shape, &self->wf_shape );
				}				
			}	
			else if ( self->state == 0 )
			{
				
			}
		}
	}


	self->wf_count++;
	self->run_count++;
}

/*
  ==============================
  AT_LightInitFromClass

  ==============================
*/

void AT_LightInitFromClass( at_light_t *self, hobj_t *cls )
{
	hpair_t		*pair;
	fp_t		dist;	// calc bound box from dist

	strncpy( self->ati.type, cls->type, ATI_STRINGSIZE );
	strncpy( self->ati.name, cls->name, ATI_STRINGSIZE );
	self->ati.type[ATI_STRINGSIZE-1] = '\0';
	self->ati.name[ATI_STRINGSIZE-1] = '\0';

	pair = FindHPair( cls, "lightsource" );
	if ( pair )
	{
		self->lightsource_id = StringToUnique( pair->value );
	}
	else
	{
		self->lightsource_id = UNIQUE_INVALIDE;
	}
	
	pair = FindHPair( cls, "switch" );
	if ( pair )
	{
		self->switch_id = StringToUnique( pair->value );
	}
	else
	{
		self->switch_id = UNIQUE_INVALIDE;
	}

	pair = FindHPair( cls, "style" );
	if ( pair )
	{
		if ( !strcmp( pair->value, "default" ) )
		{
			self->style = AT_LIGHT_STYLE_NORMAL;
		}
		else
		{
			self->style = atoi( pair->value );
		}
	}
	else
	{
		self->style = AT_LIGHT_STYLE_NORMAL;
	}

	EasyFindVec3d( self->origin, cls, "origin" );

	// facelight's search bound box
	pair = FindHPair( cls, "dist" );
	if ( !pair )
	{
		dist = 24.0;
	}
	else
	{
		HPairCastToFloat( &dist, pair );
	}
	{
		vec3d_t		min = { -1, -1, -1 };
		vec3d_t		max = { 1, 1, 1 };

		Vec3dScale( self->min, dist, min );
		Vec3dScale( self->max, dist, max );
		Vec3dAdd( self->min, self->min, self->origin );
		Vec3dAdd( self->max, self->max, self->origin );
	}
	
	self->state = 1;
}


/*
  ==============================
  NEW_light

  ==============================
*/
static bool_t		ati_ftbl_light_valid = false;
static ati_ftbl_t	ati_ftbl_light;

at_light_t * NEW_light( void )
{
	at_light_t		*obj;

	if ( ati_ftbl_light_valid == false )
	{
		ati_ftbl_light_valid = true;

		memset( &ati_ftbl_light, 0, sizeof( ati_ftbl_t ) );
		ati_ftbl_light.GetOrigin = AT_LightGetOrigin;
		ati_ftbl_light.SetOrigin = AT_LightSetOrigin;
		ati_ftbl_light.Delete = AT_LightDelete;
		ati_ftbl_light.Destroy = AT_LightDestroy;
		ati_ftbl_light.Com = AT_LightCom;
		ati_ftbl_light.Think1 = AT_LightThink1;
		ati_ftbl_light.InitFromClass = AT_LightInitFromClass;
	}

	obj = NEWTYPE( at_light_t );

	obj->ati.state = ATIState_none;
	obj->ati.ftbl = &ati_ftbl_light;

	return obj;
}

