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



// snd_intern.h

#ifndef snd_intern_h
#define snd_intern_h

#include "defs.h"

void SND_InternStartUp();
void SND_InternShutDown();

void SND_CalcDistanceAndStereo( fp_t *dist, fp_t *dot, vec3d_t origin );
void SND_SetViewData( vec3d_t origin, vec3d_t view );
void SND_AddEcho( samplepair_t *pb, int frames );
void SND_PaintStereo16( samplepair_t *paintbuf, sst_spec_t *spec, int frames, int framedelta );

// this is the hardware independent API of the snd driver. By now we only support SDL

int SST_StartUp( sst_spec_t *spec );
void SST_ShutDown( void );
void SST_Update( fp_t ftime ); // this is not used by any recent sound driver. It is called 10 times/sec

void SST_Lock( void ); // lock the driver to modify channel data.
void SST_Unlock( void );

// this is the common, wave based, music API
void MUS_StartUp();
void MUS_ShutDown();
void MUS_PaintStereo16( samplepair_t *pb, int frames );
void MUS_PlayByName( const char *name );

#endif
