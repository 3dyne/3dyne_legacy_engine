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



// life.h

#ifndef __l_export_h
#define __l_export_h

#include "wf_info_defs.h"

#define AG_MAXNUM	( 4 )

#define AGID_RENDER	( 0 )
#define AGID_TEST	( 1 )

#define AT_PLAYERNUM	( 8 )
#define AT_ISPLAYER	( 1 )


#define AG( offs, agnum ) ( ( char*)offs + (( arche_t * )offs )->ags[agnum] )
#define AG_RENDER( offs ) ( (( ag_render_t*)AG( offs, AGID_RENDER )))
#define AG_TEST( offs ) ( (( ag_test_t*)AG( offs, AGID_TEST )))

typedef enum {
	AT_MONSTER,
	AT_CLIENT,
	AT_ITEM,
	AT_TRIGGER
} attype_t;
 

// arche_t information visible to the game / render ( model, animation ... )
typedef struct {
	int		num;     // arche_t index
	vec3d_t		origin;  // the game must provide mcb's vec3d_t
	float		roll, pitch, yaw;
	vec3d_t		min, max;  // modelbox
	int		mindex, mindex2, mindex3, mindex4;  // CA index of models 
	int		frame;  // current frame
} ag_render_t;


typedef struct {
	int	a;
	int	b;
	int	c;
} ag_test_t;


// this will be replaced by the backbone

typedef struct playermod_s {
	fp_t	speedscale;
	fp_t	strafespeedscale;
//	vec3d_t	dir;
	fp_t	lon, lat;
	int	jump;
	int	shoot;
	int	use;
}
playermod_t;

#ifdef S_INLIFE
// hack: must be the same as in g_defs2.h
typedef struct g_event_s
{
	char	buf[256];
} g_event_t;
#endif

// =====

typedef struct {
	void	(*ShockHandler)();
	
	// misc
	void	(*SH_Printf)( const char*, ... );
	sh_var_t *(*SH_SetVar)( const char*, const char*, unsigned int );
	sh_var_t *(*SH_GetVar)( const char* );
	void	(*SH_ParseBuf)( const char* );

	// memory
	void	*(*MM_Malloc)( int );
	void	(*MM_Free)( void * );

	// game
	void	(*G_SetRenderHintAngleIpo)( unsigned int msec_duration, fp_t lat, fp_t lat_delta, fp_t lon, fp_t lon_delta );
	g_trace_t *(*TraceBB)( vec3d_t min, vec3d_t max, vec3d_t trymin, vec3d_t trymax );
	void	(*G_SetRenderHintOriginIpo)( unsigned int msec_duration, vec3d_t pos1, vec3d_t pos2 );

  void	(*GI_RegisterDLightForWF)( wf_dlight_info_t *info );

	void	(*GI_RegisterWFInfo)( wfInfoType type, void *wfinfo );

	// database stuff
	void	(*GI_SelectShapesInBoundBox)( vec3d_t min, vec3d_t max, void (*action_func)(unique_t shape_id ) );

	// io
	g_resource_t * (*GI_ResourceSearch)( const char *name );
	g_resource_t * (*GI_ResourceAttach)( char *name, char *user_name,
		  int (*user_callback_func)(g_resource_t *r, int reason) );
	void (*GI_ResourceDetach)( char *name, char *user_name );
	hobj_t * (*GI_ReadClass)( char *path );

	int (*SND_AddJob)( g_resource_t *res, int type, int pri, vec3d_t origin );
	int (*SND_JobIsActive)( int job );
	void (*SND_JobChangeSound)( int id, g_resource_t *res );
} l_import_t;

typedef struct {
	void	(*testfunc)( void );
	void	(*L_StartUp)();
	void	(*L_ShutDown)();
	void	(*L_RunFrame)();

	void	(*WorldSpring)( unsigned char * );
	void	(*SpawnPlayer)( char * );

	// client comm
	void	(*ClientSendEvent)( g_event_t *ev );
	g_event_t *(*ClientPollEvent)( void );

	// bind view
	void	(*ClientBindView)( unique_t id_bind_view );
} l_export_t;

//
// header for memory tags in experimental at memory manager

typedef struct {
	void	*next, *prev;
	unsigned int	size;
} atm_header_t;

l_export_t	*L_GetApi( l_import_t* );

#endif
