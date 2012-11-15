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



// g_client.c

#include "interfaces.h"

#include "lib_container.h"
#include "lib_queue.h"

#include "g_client.h"
#include "g_library.h"
#include "interfaces.h"

#include "events.h"

// hack:
#include "l_wrap.h"

/*
  ==============================
  G_InitClient

  ==============================
*/
void G_InitClient( g_client_t *cl )
{
	INITTYPE( cl, g_client_t );

	cl->state = gClientState_try_init;

	SHV_Printf( "\n" );
	SHV_Printf( " --- Init Client ---\n" );

	__chkptr( cl );


	sprintf( cl->lib.name, "cl" );
	if ( G_LibLoad( &cl->lib ) )
	{
		Error( "can't load client dll\n" );
	}
	cl->lib.api.Init();

	cl->lib.api.Com( "CL_WorldSpring" );
//	ImportLife( "dd1" );
//	le.L_StartUp();

//	tmp = SHP_GetVar( "gc_map" );
//	__named_message( "gc_map is '%s'\n", tmp->string );
//	le.WorldSpring( tmp->string );	

	cl->mode = gClientMode_local;
	cl->state = gClientState_is_init;

	G_ClientResetInput( cl );

	SHV_Printf( "\n" );
	SHV_Printf( " --- Init Client done ---\n" );	
}


/*
  ==============================
  G_CleanUpClient

  ==============================
*/
void G_CleanUpClient( g_client_t *cl )
{
	__chkptr( cl );

	SHV_Printf( "\n" );
	SHV_Printf( " --- Clean Up Client ---\n" );

	cl->lib.api.CleanUp();
	G_LibUnload( &cl->lib );

//	RejectLife();

	cl->state = gClientState_none;
	cl->mode = gClientMode_none;

	SHV_Printf( "\n" );
	SHV_Printf( " --- Clean Up Client done ---\n" );
}

#if 0
/*
  ==============================
  G_ClientSetInputByteIter

  ==============================
*/
void G_ClientSetInputByteIter( g_client_t *cl, byte_iter_t *bi )
{
	__chkptr( cl );
	__chkptr( bi );

	cl->bi_in = bi;

	cl->lib.api.SetInputByteIter( bi );
}

/*
  ==============================
  G_ClientSetOutputByteIter

  ==============================
*/
void G_ClientSetOutputByteIter( g_client_t *cl, byte_iter_t *bi )
{
	__chkptr( cl );
	__chkptr( bi );

	cl->bi_out = bi;
	
	cl->lib.api.SetOutputByteIter( bi );
}
#endif

/*
  ==============================
  G_ClientFillInput

  ==============================
*/
void G_ClientResetInput( g_client_t *cl )
{
	cl->buf_in_size = 0;
}

void G_ClientFillInput( g_client_t *cl, char *buf, int size )
{
	int		i;

	if ( cl->buf_in_size + size >= G_CLIENT_INPUT_BUF_SIZE )
	{
		__error( "buffer overflow\n" );
	}

	for ( i = 0; i < size; i++ ) 
	{
		cl->buf_in[cl->buf_in_size++] = buf[i];
	}		      
}

/*
  ==============================
  G_ClientGetOutput

  ==============================
*/
void G_ClientResetOutput( g_client_t *cl )
{
	cl->buf_out_size = 0;
}

char * G_ClientGetOutputBase( g_client_t *cl )
{
	return cl->buf_out;
}

int G_ClientGetOutputSize( g_client_t *cl )
{
	return cl->buf_out_size;
}

/*
  ==============================
  G_ClientRunFrame

  ==============================
*/
void G_ClientRunFrame( g_client_t *cl )
{
// 	sh_var_t		*tmp;
// 	unique_t	id;

	__chkptr( cl );

	if ( cl->state != gClientState_is_init )
		__error( "state is not init\n" );

// 	tmp = SHP_GetVar( "gc_id_bind_view" );
// 	id = tmp->ivalue;
//	le.ClientBindView( id );

	cl->ri_model_num = 0;
	cl->ri_sprite_num = 0;
	cl->ri_psys_num = 0;
	cl->ri_local_light_num = 0;
	cl->ri_halo_num = 0;

	cl->di_rect_num = 0;
	cl->di_text_num = 0;

	cl->ri_view_player = NULL;
	cl->ri_view_spectator = NULL;
	cl->ri_view_sky = NULL;

//	le.L_RunFrame();

	{
		byte_iter_t	bi_in;
		byte_iter_t	bi_out;

		char	com_call[256];
		const char	*com_result;

		ByteIter_Init( &bi_in, cl->buf_in, cl->buf_in_size );
		ByteIter_Init( &bi_out, cl->buf_out, G_CLIENT_OUTPUT_BUF_SIZE );

		cl->lib.api.SetInputByteIter( &bi_in );
		cl->lib.api.SetOutputByteIter( &bi_out );

		sprintf( com_call, "CL_RunFrame %u", ms_rfbegin );
		com_result = cl->lib.api.Com( com_call );
		
		if ( com_result )
		{
//			printf( "com_result: '%s'\n", com_result );
		}

		cl->buf_out_size = ByteIter_GetOfs( &bi_out );
	}
	
	G_ClientResetInput( cl );

	//
	// apply lightsource changes
	//
//	for ( i = 0; i < cl->ri_lightsource_num; i++ )
//	{
//		G_HandleRenderInfoLightSource( cl->ri_lightsources[i] );
//	}
	
	//
	// apply shape manipulations
	//
//	G_ApplyWFShapeInfos( cl );
	
	//
	// run field particle systems
	//
	
//	for ( i = 0; i < cl->wf_fpsys_num; i++ )
//	{
//		G_FieldParticleSystemRun( cl->wf_fpsyss[i]->fpsys );
//	}	
}
