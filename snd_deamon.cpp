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



// snd_deamon.c

#include "interfaces.h"
#include "l_wrap.h"



#define SND_FRAMETIME	( 0.1 )





vec3d_t	snd_origin;
vec3d_t snd_view;


static snd_channel_t	channels[SND_CHANNEL_NUM];


// static res_sound_cache_t *snd;
// static g_resource_t	*r;


static int	oldtime;

static sst_spec_t	myspec;
static int	sst_isup = 0;

static int	snd_num;
static sh_var_t		*snd_sfxvol;
static sh_var_t		*snd_errval;	
// static sh_var_t		*snd_mussong;
// static sh_var_t		*var_snd_sst;

//snd_sst_t *SST_CreateObj();
//void SST_DestroyObj( snd_sst_t *sst );

// static int		sh;
static int		sst_isloaded = 0;

// static char		curmusic[256];

void SND_StartUp()
{
	int	i;
// 	char	dllname[256];

	SND_InternStartUp();




	if( !((sh_var_t *)SHP_GetVar( "sound" ))->ivalue )
	{
		__named_message( "\tsound disabled\n" );
		sst_isup = 0;
		return;
	}

	for( i = 0; i < SND_CHANNEL_NUM; i++ )
	{
		channels[i].data = NULL;
	}

	Vec3dInit( snd_origin, 0.0, 0.0, 0.0 );
	
	snd_sfxvol = SHP_GetVar( "snd_sfxvol" );
	snd_errval = SHP_GetVar( "snd_errval" );



	oldtime = 0;
	
	snd_num = 0;
	sst_isloaded = 0;

	myspec.ch = channels;
	myspec.chnum = SND_CHANNEL_NUM;

	if( SST_StartUp( &myspec ) )
	{
		__warning( "SST_StartUp failed. sound disabled\n" );
		sst_isup = 0;
	} else
	{
		sst_isup = 1;
	}

	printf( "sst_isup: %d\n", sst_isup );


#if 0
	var_snd_sst = SHP_GetVar( "snd_sst" );

	if( !strlen( var_snd_sst->string ) )
	{
		__warning( "no snd sst set. failed.\n" );
// bla
		return;
	}



	sprintf( dllname, "./%s", var_snd_sst->string );
	SYS_DllAddSuffix( dllname, SYS_DLLSSYSTEM );

	si.ShockHandler = ShockHandler;
	si.SH_Printf = SHV_Printf;
	si.SH_GetVar = SHP_GetVar;
	si.SH_SetVar = SHP_SetVar;
	si.SH_ParseBuf = SHP_ParseBuf;
	si.MM_Malloc = _MM_Malloc;
	si.MM_Free = _MM_Free;
	si.GI_ResourceSearch = GI_ResourceSearch;


	sh = SYS_DllBegin( dllname );
	      

	__named_message( "\tImporting sound sst: '%s'\n", dllname );

	SST_GetAPI = SYS_DllSym( sh, "SST_GetAPI" );
	__chkptr( SST_GetAPI );
	
	tmp = SST_GetAPI( &si );
	se = *tmp;

	sst_isloaded = 1;

	snd_sst = se.CreateObj();
	if( !snd_sst )
	{
		__warning( "CreateObj failed\n" );
		return;
	}
	


	snd_sst->ch = channels;
	snd_sst->chnum = SND_CHANNEL_NUM;

	if( snd_sst->StartUp( snd_sst ) )
		__warning( "sound startup failed\n" );
	else
	{
		__named_message( "\tsound sst is up\n" );
		r = G_ResourceSearch( g_rs, "sound.test1" );

		snd = ( res_sound_cache_t * ) r->res_cache;
	}
	
	
	SHP_SetVar( "snd_musname", "1", 0 );
#endif

	MUS_StartUp();
	//MUS_PlayByName( "mp3.test1" );

}

void SND_ShutDown() 
{
	if( !sst_isup )
	{
		__warning( "sst is not up\n" );
		return;
	}

	SST_ShutDown();
}



/*
===================
 SND_AddJob( g_resource_t sound resource, type of sound (local, position), priority, origin )
 
 add ( positional ) sound job. lower priority lose if now channels left.
===================
*/

int SND_AddJob( g_resource_t *res, int type, int pri, vec3d_t origin )
{
	res_sound_cache_t	*s;
	int		i;

	if( !sst_isup )
		return 0;


	s = (res_sound_cache_t*)res->res_cache;
	if( !s )
	{
		__warning( "snd->res_cache == NULL\n" );
		return 0;
	}

	if( !pri )
	{
		__warning( "no priority. failed\n" );
		return 0;
	}

	
	for( i = 0; i < SND_CHANNEL_NUM; i++ )
	{
		if( ( channels[i].pri < pri ) || ( !channels[i].data ))
			break;
	}
	
	// hmm no unimportant sound to drop. drop equal
	
	if( i >= SND_CHANNEL_NUM )
	{
		for( i = 0; i < SND_CHANNEL_NUM; i++ )
		{
			if( channels[i].pri == pri )
				break;
		}
	}

	if( i >= SND_CHANNEL_NUM )
	{
		__warning( "no channel free\n" );
		return 0;
	}


	SST_Lock();
	channels[i].data = ( short * )s->sound;
	channels[i].size = s->size / 2;
	channels[i].pointer = 0;
	channels[i].lvol = snd_sfxvol->fvalue;
	channels[i].rvol = snd_sfxvol->fvalue;
	channels[i].type = type;
	channels[i].firsttime = 1;
	channels[i].pri = pri;
	Vec3dCopy( channels[i].origin, origin );

	snd_num++;
	channels[i].num = snd_num;
	SST_Unlock();


	return snd_num;
}	

int SND_AddJobByName( const char *name, int type, int pri, vec3d_t origin )
{
	int rc;
	g_resource_t	*res;

	if( !sst_isup )
		return 0;

	if( !g_rs )
		return 0;

	res = G_ResourceSearch( g_rs, name );
	rc = SND_AddJob( res, type, pri, origin );

	return rc;
}

int SND_JobIsActive( int id )
{
	int	i;

	if( !sst_isup )
		return 0;
	
	for( i = 0; i < SND_CHANNEL_NUM; i++ )
	{
		if( !channels[i].data )
			continue;
		
		if( channels[i].num == id )
			return 1;
	}
	
	return 0;
}

// 
// reset job and ( optionally )give it a new sound

void SND_JobChangeSound( int id, g_resource_t *res )
{
	int	i;

	if( !sst_isup )
		return;
	
	for( i = 0; i < SND_CHANNEL_NUM; i++ )
	{
		if( !channels[i].data )
			continue;
		
		if( channels[i].num == id )
		{
			SST_Lock();
			if( res )
			{
				res_sound_cache_t	*s;
				s = (res_sound_cache_t*)res->res_cache;
				channels[i].data = ( short * )s->sound;
				channels[i].size = s->size / 2;
			}
			channels[i].lvol = snd_sfxvol->fvalue;
			channels[i].rvol = snd_sfxvol->fvalue;
			channels[i].pointer = 0;
			channels[i].firsttime = 1;
			SST_Unlock();
		}
	}
}

void SND_StopAllJobs()
{
	int	i;
	
	if( !sst_isup )
		return;
	
	SST_Lock();

	for( i = 0; i < SND_CHANNEL_NUM; i++ )
	{
		channels[i].data = NULL;
	}

	SST_Unlock();
}
	
void SND_Update()
{
	int	time, now;
	fp_t ftime;
	
	if( !sst_isup )
		return;
	      

	if( !oldtime )
	{
		oldtime = ms_wfbegin;
		return;
	}
#if 0
	if( !channels[0].data )
		SND_AddJob( r, vec );
#endif

//	snd_sst->lvol = snd_sfxvol->fvalue/**256.0*/;
//	snd_sst->rvol = snd_sfxvol->fvalue/**256.0*/;

#if 1		
	now = SYS_GetMsec();
	time = now - oldtime;
	
	ftime = ( time ) / 1000.0;
#endif







#if 0
	soundframes = ftime * 22050.0;
//	SHV_Printf( "%d %d", time, soundframes );	

	soundframes += (snd_errval->fvalue * 20.0);
	SHV_Printf( "%d soundframes %d", soundframes, time );
#endif
//	ms1 = SYS_GetMsec();

	SND_SetViewData( snd_origin, snd_view );	

	SST_Update( ftime );
	oldtime = now;
//	ms2 = SYS_GetMsec();
//	SHV_Printf( "sound: %d\n", ms2 - ms1 );
}



