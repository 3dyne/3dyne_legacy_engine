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



// snd_exports.h

#ifndef snd_exports_h
#define snd_exports_h



#define TYPE_LOCAL ( 1 )
#define TYPE_NORMAL ( 2 )



typedef struct {
	int	left;
	int	right;
} samplepair_t;

typedef struct {
	short	*data; // 16bit only
	int	pointer; // in frames! ( 2-bytes )
	int	size; // in frames!
	int	num; // unique id of sound
	fp_t	rvol, lvol;
	int	type;
	int	pri;	// priority: higher or equal priority overides
	int	firsttime;
	
	vec3d_t		origin;
} snd_channel_t;

typedef struct snd_sst_s {
	int	(*StartUp)( struct snd_sst_s *self );
	void	(*ShutDown)( struct snd_sst_s *self );
	void	(*Update)( struct snd_sst_s *self, fp_t time );
	void	(*Lock)( void ); // lock ch
	void	(*Unlock)( void ); // unlock ch

	int	isup;

	void	*private;

// values that must be set at when calling StartUp
	snd_channel_t	*ch; // Lock/Unlock construct  must be called while changing the data, since the sounddriver may run asyncron/threaded.
	int	chnum;


// values to be changed at runtime. I think single values do not need locking (!?)
	fp_t	rvol, lvol;
	
	
} snd_sst_t;

typedef struct sst_import_s {
// shock
	void	(*ShockHandler)();

	// misc
	void	(*SH_Printf)( unsigned char*, ... );
	sh_var_t *(*SH_SetVar)( unsigned char*, unsigned char*, unsigned int );
	sh_var_t *(*SH_GetVar)( unsigned char* );
	void	(*SH_ParseBuf)( unsigned char* );

	// memory
	void	*(*MM_Malloc)( int );
	void	(*MM_Free)( void * );

	// io
	g_resource_t * (*GI_ResourceSearch)( char *name );
} sst_import_t;

typedef struct sst_export_s {
	snd_sst_t *(*CreateObj)( void );
	void (*DestroyObj)( snd_sst_t * sst );
	void (*SetViewData)( vec3d_t pos, vec3d_t view );
} sst_export_t;


#endif
