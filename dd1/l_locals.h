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



// l_locals.h
/*
  ============================================================
  this file is the universal header for all files in the life
  dll

  This header is not visible to the main game. stuff visible
  to the game must lie in l_export.h
  ============================================================
*/

#ifndef l_locals_h
#define l_locals_h

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "s_config.h"
#include "s_mem.h"

#include "l_sysspec.h"
#include <ctype.h>
#include "a_shared.h"
#include "a_stuff.h" 
#include "lib_math.h"
#include "lib_container.h"
#include "lib_hobj.h"
#include "lib_model.h"
#include "lib_hmanagerdefs.h"
#include "lib_psys.h"
#include "lib_pack.h"

#include "math.h"
//#define FROM_LIFE
#include "wf_info_defs.h"
#include "u_defs.h"
#include "g_bmdefs.h"
#include "g_tracedefs.h"
#include "g_resourcesdefs.h"
#include "res_submddefs.h"
#include "res_movesdefs.h"
#include "shock.h"

#include "l_exports.h"

#ifdef NEWTYPE
#undef NEWTYPE
#define NEWTYPE(__type)	(memset(li.MM_Malloc( sizeof(__type)), 0, sizeof(__type)))
#endif

// ============================================================
// constants

#define L_ARCHENUM	( 128 ) // fixme: dynamic number management ...
#define L_PLAYERNUM	( 4 ) // d.t.o.

#define ZONE_LIFE	( 1 )  // only interesting mm zone for life

#define ATI_STRINGSIZE	( 32 )

//
// field types
#define F_INT		( 0 )
#define F_FLOAT		( 1 )
#define F_VECTOR	( 2 )
#define F_STRING	( 3 )


#define SAVE_TAGSIZE	( 16 )
#define SAVE_MAXINDENT	( 32 )

#define SND_TYPE_NORMAL ( 2 )

// ============================================================
// macros

#define drand( x )	( drandv[x&0x3ff] )

#define SP_ATMOFS( x )	( int )&((( arche_t * )0 )->x )   // get offset of arche_t member
//#define SP_FOFS( x, y )	( int )&((( x * )0 )->y )



// ============================================================
// types

typedef enum {
	PlaneSlope_none,
	PlaneSlope_floor,
	PlaneSlope_slope,
	PlaneSlope_wall
} planeSlope;

typedef struct comiter_s
{
	char	*ptr;
	int	pos;
	int	size;
	int	wasnull;
	char	curtoken[256];
	char	*(*GetNext)();
} comiter_t;

typedef struct ati_ftbl_s {
	void	(*InitFromClass)( void *self, hobj_t *cls );
	void	(*Delete)( void *self );
	void	(*Destroy)( void *self );
	void	(*GetOrigin)( void *self, vec3d_t vec );
	void	(*SetOrigin)( void *self, vec3d_t vec );
	void	(*Com)( void *self, char *in, char *out );
	void	(*Think1)( void *self );	
	void	(*Think2)( void *self );	
} ati_ftbl_t;

typedef enum
{
	ATIState_none,
	ATIState_wakeup,
	ATIState_run,
	ATIState_sleep,
	ATIState_dead,

	ATIState_num
} atiState;

typedef struct ati_s {
	atiState	state;

	char	type[32];
	char	name[32];

	unique_t	self_id;		// primary search key

	struct ati_s	*prev;		// in one of these lists:
	struct ati_s	*next;		// wakeup, run, sleep, dead
	
	ati_ftbl_t	*ftbl;
} ati_t;

typedef struct at_list_s
{
	ati_t		head;
	ati_t		tail;
} at_list_t;	// ! that's not a special archetype !

typedef struct move_volume_s
{
	vec3d_t		from;
	vec3d_t		to;
	vec3d_t		rel_min;
	vec3d_t		rel_max;
} move_volume_t;

//
// special archetypes
//

typedef struct at_player_s {
	ati_t	ati;

	g_event_t	*ev;	// latest event arrived for this name
	playermod_t	pmod;	// hack: setup from event

	vec3d_t	origin;
	vec3d_t	dir;
	fp_t	lon, lat, speedscale;

	move_volume_t		move_vol;	// for trace and collidemap

	wf_dlight_info_t	carbide;
	wf_sprite_info_t	mark;		// first visualization of a player
} at_player_t;

typedef struct at_testobj_s {
	ati_t	ati;
	vec3d_t	origin;

//	mdi_testobj_t	*mdi;
	md_t		*md;

	wf_model_info_t		wf_info;
} at_testobj_t;

typedef struct at_rocket_s
{
	ati_t	ati;
	vec3d_t	origin;

	fp_t	speedscale;
	vec3d_t	dir;

	int	wf_count;
	int	wf_dead;

	md_t	*md;
	wf_model_info_t		wf_info;
	wf_sprite_info_t	halo;
} at_rocket_t;

typedef struct at_item_s
{
	ati_t	ati;
	vec3d_t	origin;

	md_t	*md;
	wf_model_info_t		wf_info;
} at_item_t;

typedef struct at_pos_s {
	ati_t	ati;
	vec3d_t	origin;
} at_pos_t;

typedef struct at_sound_s {
	ati_t	ati;
	vec3d_t	origin;
	g_resource_t	*res;
	int	flag;
	int	job;
} at_sound_t;

typedef struct at_waypoint_s {
	ati_t	ati;
	vec3d_t	origin;
} at_waypoint_t;

typedef struct at_dlight_s {
	ati_t	ati;
	vec3d_t	origin;

	vec3d_t	color;
	fp_t	intensity;

	wf_dlight_info_t	wf_info;
	wf_sprite_info_t	halo;
	int    wf_num_till_dead;

	vec3d_t		dir;	// moving dlight

} at_dlight_t;

typedef struct at_psys_s {
	ati_t	ati;
	vec3d_t		origin;
	vec3d_t		rel_min, rel_max;

	field_space_t			*target_space;	// should be reached by the particle systems' field space if set
	int				wf_num_till_dead;

	field_particle_system_t		*fpsys;
	wf_fpsys_info_t			wf_fpsys;
} at_psys_t;

typedef struct at_switch_s {
	ati_t	ati;

	unique_t	self_id;	// id of switch

	move_volume_t	move_vol;	// for collide map

	vec3d_t		origin;
	fp_t		dist;		// bound sphere
	int		state;

	int		style;	       

	// internal
	int		wf_count;
	int		wf_suspended;	// world frames till switch accept next 'use'
} at_switch_t;

typedef struct at_light_s
{
	ati_t	ati;

	vec3d_t		origin;
	int		run_count;

	int		state;
	int		style;
	int		wf_count;

	unique_t	lightsource_id;
	unique_t	switch_id;

	wf_light_info_t	wf_light;

	// facelight stuff
	// bound box for facelight's shape search
	vec3d_t		min, max;      

	unique_t	shape_id;	// this shape is the lightsource
	wf_shape_info_t	wf_shape;	// shape vertex color manipulation ...

} at_light_t;

typedef struct arche_s {
	unsigned char	classname[32];
	unsigned int	inuse;
	unsigned int	flags;
	unsigned int	ids;
	unsigned int	ags[AG_MAXNUM];
	
	struct arche_s	*next;
	struct arche_s	*prev;

	playermod_t	*pmod;
	
	fp_t		lon, lat;
	
	vec3d_t		origin;
	vec3d_t		dir;
	fp_t		speedscale;
} arche_t;


	

typedef struct {
	a_player_t	*players;
	int		playernum;
} l_game_t;

typedef struct {
	char	*key;
	int	at_ofs;
//	int	ag_ofs;
	int	ftype;
} at_field_t;

typedef struct {
	char	*type;
	void	(*spawnfunc)( arche_t* );
} sp_field_t;


typedef struct {
	unsigned char	*key;
	unsigned int	ofs;
	unsigned int	ftype;
} save_field_t;

typedef struct {
	void (*spawnfunc)( arche_t* );
	int (*sizeoffunc)();
	void (*savefunc)( void * );
	unsigned char *name;
} ag_funcs_t;


// ============================================================
// global vars

extern l_export_t		myexport;
extern l_import_t	li;

extern float	drandv[];
extern arche_t	*archets;

extern u_list_t *waynet;

extern at_list_t	l_wakeup;
extern at_list_t	l_run;
extern at_list_t	l_sleep;
extern at_list_t	l_dead;

extern hobj_t		*l_lightsource_root;


extern int		at_num_player;
extern at_player_t	*at_players[];


extern hobj_t	*at_prefs;	

extern a_player_t	*players;

extern ag_funcs_t	ag_funcs[];

extern int	local_player;
extern int	world_frame;


// ============================================================
// function prototypes

// ========================================
// l_main.c

arche_t *L_GetArcheTBase();
void L_StartUp();
void L_ShutDown();

// arche_t *L_GetArcheT( unsigned int );
void L_FreeArcheT( arche_t * );

void L_RunFrame();


// ========================================
// l_agregate.c
int AG_Checksum();


void SP_AGRender( arche_t * );
int AG_SizeofRender();
void SAVE_AGRender( ag_render_t* );

void SP_AGTest( arche_t * );
int AG_SizeofTest();
void SAVE_AGTest( ag_test_t* );


// ========================================
// l_player.c

void SP_LocalPlayer( arche_t * );

// ========================================
// l_phys.c
void PlayerPhysic( arche_t * );
void L_RunArcheT( arche_t * );
planeSlope TryMove( vec3d_t v1, vec3d_t v2 );
bool_t TryStraightMove( vec3d_t from, vec3d_t to );

planeSlope TryMoveVolume( move_volume_t *vol );
void		TryMoveVolumeToGround( move_volume_t *vol );

/*
  ==================================================
  l_util.c

  ==================================================
*/

//
// archetype lists
//

void L_InitATList( at_list_t *list );
void L_ATListInsertAtHead( at_list_t *list, ati_t *at );
ati_t * L_ATListRemoveAtHead( at_list_t *list );
bool_t L_ATListIsNotEmpty( at_list_t *list );

ati_t * L_LinkAT( ati_t *prev, ati_t *next, ati_t *insert );
ati_t * L_UnlinkAT( ati_t *remove );
void L_ChangeToWakeUp( ati_t *at );
void L_ChangeToRun( ati_t *at );
void L_ChangeToSleep( ati_t *at );
void L_ChangeToDead( ati_t *at );

//
// move volume
//

void L_InitMoveVolume( move_volume_t *vol );
void L_MoveVolumeSetFromPos( move_volume_t *vol, vec3d_t pos );
void L_MoveVolumeGetFromPos( move_volume_t *vol, vec3d_t pos );
void L_MoveVolumeSetToPos( move_volume_t *vol, vec3d_t pos );
void L_MoveVolumeGetToPos( move_volume_t *vol, vec3d_t pos );
void L_MoveVolumeSetRelativeBoundBox( move_volume_t *vol, vec3d_t rel_min, vec3d_t rel_max );
void L_MoveVolumeGetRelativeBoundBox( move_volume_t *vol, vec3d_t rel_min, vec3d_t rel_max );
void L_MoveVolumeGetFromBoundBox( move_volume_t *vol, vec3d_t abs_min, vec3d_t abs_max );
void L_MoveVolumeGetToBoundBox( move_volume_t *vol, vec3d_t abs_min, vec3d_t abs_max );

void L_MoveVolumeFollow( move_volume_t *vol );
void L_MoveVolumeStay( move_volume_t *vol );

//
// move volume collidemap
//

void L_BeginCollideMap( void );		// called per WF before Think1 run
void L_EndCollideMap( void );		// called per WF after Think1 run
void L_CollideMapAddMoveVolume( ati_t *owner, move_volume_t *vol );	// Think1
void L_CollideMapForEachInSpace( ati_t *owner, move_volume_t *self, void (*action_func())(ati_t *caller, ati_t *collide_with) );	// Think2

//
// switchable map
//

void L_InitSwitchMap( void );
void L_SwitchMapInsertSwitchAble( ati_t *self, unique_t switch_id );
void L_SwitchMapRemoveSwitchAble( ati_t *self );
void L_SwitchMapSelectSwitchAbleWhere_switchid( ati_t *self, unique_t switch_id, void (*action_func())(ati_t *caller, ati_t *switch_able ) );

void U_ParseToken( unsigned char ** );

void *L_FindArcheT( char *type, char *name );

bool_t PositionEqual( vec3d_t p1, vec3d_t p2 );


// ========================================
// l_spring.h
void SP_ArcheTs( unsigned char * );
void WorldSpring( char * );
void PlayerSpring( char *name );


// ========================================
// l_save.c
void SAVE_ArcheTS( unsigned char * );

void SAVE_Begin( FILE *arg_h );
void SAVE_BeginSection( char *name );
void SAVE_EndSection( char *name );
void SAVE_PackInt( char *key, int value );
void SAVE_PackFloat( char *key, float value );
void SAVE_PackVec3D( char *key, vec3d_t value );

// ========================================
// l_waynet.c

void L_InitWayNet();
void L_InsertWaypoint( at_waypoint_t *at );
void *WaynetFindBest( vec3d_t *origin );

// ========================================
// at spawnfuncs
at_testobj_t	 * NEW_testobj( void );
at_pos_t	 * NEW_pos( void );
at_player_t	 * NEW_player( char *name );
at_sound_t	 * NEW_sound( void );
at_waypoint_t	 * NEW_waypoint( void );
at_dlight_t	 * NEW_dlight( void );
at_psys_t	 * NEW_psys( void );
at_switch_t	 * NEW_switch( void );
at_light_t	 * NEW_light( void );
at_rocket_t	 * NEW_rocket( void );
at_item_t	 * NEW_item( void );

// ========================================
// l_model.c

md_t * L_CreateModelFromResourceName( char *name );
void L_ModelSetMoveByName( md_t *md, char *name );

#endif
