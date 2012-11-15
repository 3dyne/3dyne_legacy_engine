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



// layers.c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "lib_math.h"
#include "lib_container.h"

#include "layer_defs.h"
#include "layers.h"
#include "shock.h"
/*
  ==============================
  QSort_LayerCompareFunc

  ==============================
*/
int QSort_LayerCompareFunc( const void *l1, const void *l2 )
{
	if ( (*(layer_t**)(l1))->ordinal >
	     (*(layer_t**)(l2))->ordinal )
		return 1;
	else if ( (*(layer_t**)(l1))->ordinal <
		  (*(layer_t**)(l2))->ordinal )
		return -1;
	else
	{
		if ( l1 != l2 )
			__error( "same ordinal in diffent layers\n" );
	}
	return 0;
}

/*
  ==============================
  LayerList_BuildArrays

  ==============================
*/
void LayerList_BuildArrays( u_list_t *layer_list,
			    layers_characteristic_t **ca_p,
			    layers_array_t **la_p )
{
	u_list_iter_t		iter;
	layer_t			*layer;

	int			layernum;
	layer_t			*layers[LAYERS_MAX_LAYERS];

	layernum = 0;
	U_ListIterInit( &iter, layer_list );
	for ( ; ( layer = (layer_t *)U_ListIterNext( &iter ) ) ; )
	{
		if ( layernum >= LAYERS_MAX_LAYERS )
			__error( "reached LAYERS_MAX_LAYERS\n" );

		layers[layernum] = layer;
		layernum++;
	}

	// sort layers by ordinal
	qsort( (void*)&layers[0], layernum, sizeof( layer_t * ), QSort_LayerCompareFunc );
	
	
	if ( la_p )
	{
		size_t		size;		
		int		i;

		size = (size_t)&(((layers_array_t *)0)->layers[layernum]);
		*la_p = (layers_array_t *)NEWBYTES( size );
		(*la_p)->layernum = layernum;

		for ( i = 0; i < layernum; i++ )
		{
			(*la_p)->layers[i] = layers[i];
		}
	}

	if ( ca_p )
	{
		size_t		size;
		int		i;

		size = (size_t)&(((layers_characteristic_t *)0)->layers[layernum]);
		*ca_p = (layers_characteristic_t *)NEWBYTES( size );
		(*ca_p)->layernum = layernum;

		for ( i = 0; i < layernum; i++ )
		{
			(*ca_p)->layers[i].ordinal = layers[i]->ordinal;
			memcpy( &(*ca_p)->layers[i].glstate, layers[i]->glstate, sizeof( layer_gl_state_t ) );
		}
	}
}

/*
  ==============================
  LayersCharacteristic_Compare

  ==============================
*/
bool_t Layers_CompareCharacteristics( layers_characteristic_t *lc1, layers_characteristic_t *lc2 )
{
	int		i;

	if ( lc1->layernum != lc2->layernum )
		return false;

	for ( i = 0; i < lc1->layernum; i++ )
	{	       
		if ( memcmp( &lc1->layers[i], &lc2->layers[i], sizeof( layer_characteristic_t ) ) )
			return false;
	}

	return true;
}


/*
  ==============================
  Layers_DumpLayerArray

  ==============================
*/
void Layers_DumpLayerArray( layers_array_t *la )
{
	int		i;

	printf( "Layers_DumpLayerArray:\n" );
	printf( " layernum: %d\n", la->layernum );
	
	for ( i = 0; i < la->layernum; i++ )
	{
		printf( "ordinal: %d, ", la->layers[i]->ordinal );
	}
	printf( "\n" );
}


/*
  ==============================
  Layers_DumpCharacteristicArray

  ==============================
*/
void Layers_DumpCharacteristicArray( layers_characteristic_t *ca )
{
	int		i;

	printf( "Layers_DumpCharacteristicArray:\n" );
	printf( " layernum: %d\n", ca->layernum );

	for ( i = 0; i < ca->layernum; i++ )
	{
		printf( "ordinal: %d, ", ca->layers[i].ordinal );
	}
	printf( "\n" );
}
