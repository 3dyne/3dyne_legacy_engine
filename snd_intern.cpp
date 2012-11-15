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



// snd_common.c
#include "interfaces.h"

extern vec3d_t		snd_origin, snd_view;

void SND_CalcDistanceAndStereo( fp_t *dist, fp_t *dot, vec3d_t origin )
{
	vec3d_t		distvec;

#if 1
	Vec3dSub( distvec, origin, snd_origin );
	*dist = Vec3dLen( distvec );


//	printf( "dist: %f\n", *dist );

	if( *dist >= 1024.0 )
	{
		*dist = 0.0;
		*dot = 0.0;
		return;
	}

	*dist = 1024.0 - *dist;
	*dist /= 1024.0;
	
	Vec3dUnify( distvec );
	*dot = Vec2dDotProduct( snd_view, distvec );
	
#endif


}

void SND_SetViewData( vec3d_t origin, vec3d_t view )
{
	Vec3dCopy( snd_origin, origin );
	Vec3dCopy( snd_view, view );
}

static void ClipShort(int *v)                                                   
{                                                                               
        if (*v < -32768)
                *v = -32768;
        if (*v > 32767)
                *v = 32767;
}

#if 1
// ============================================================================
//  The idea for the following code was taken from the echo plugin of the
//  xmms-0.9. Anyone keen on writing a midi-control routine for my Midiverb 3?
// ============================================================================

static samplepair_t *ebuf;
static int w_ofs;
static sh_var_t	*snd_echodelay, *snd_echofb, *snd_echovol, *snd_doecho;
static sh_var_t *music;

void SND_AddEcho( samplepair_t *pb, int frames )
{
	int	r_ofs;
	int	i;

	r_ofs = w_ofs - ( 22050*snd_echodelay->fvalue);

	if( r_ofs < 0 )
		r_ofs += 22050;

//	printf( "%d %d %d %d\n", frames, w_ofs, r_ofs );

	for( i = 0; i < frames; i++ )
	{
		int	r, l, rbuf, lbuf, rout, lout;



		r = pb[i].right;
		l = pb[i].left;
		
		rbuf = ebuf[r_ofs].right;
		lbuf = ebuf[r_ofs].left;
		
		rout = r + rbuf * snd_echovol->fvalue;
		rbuf = r + rbuf * snd_echofb->fvalue;

		ClipShort( &rout );
		ClipShort( &rbuf );

		lout = l + lbuf * snd_echovol->fvalue;
		lbuf = l + lbuf * snd_echofb->fvalue;

		ClipShort( &lout );
		ClipShort( &lbuf );

		ebuf[w_ofs].right = rbuf;
		ebuf[w_ofs].left = lbuf;

		pb[i].right = rout;
		pb[i].left = lout;
	
		if( ++r_ofs >= 22050 )
			r_ofs -= 22050;

		if( ++w_ofs >= 22050 )
			w_ofs -= 22050;
	}
}
#endif


void SND_PaintStereo16( samplepair_t *paintbuf, sst_spec_t *spec, int frames, int framedelta )
{
	int	i, i2, sframes;
	snd_channel_t	*ch;

	fp_t	dist;
    fp_t rscale = 0.0;
    fp_t lscale = 0.0;
// 	int	locked = 0;

	// we do not need to lock the data, as this function only will be called from the thread himself, if available. . If we did, SDL would deadlock, for example. The only thing that must be shure, is that the threaded function is not called twice at the same time.


	for( i2 = 0; i2 < spec->chnum; i2++ )
	{
		ch = &spec->ch[i2];

		if( !ch->data )
		{
			continue;
		}
//		printf( "channel: %d %p %d\n", i2, ch->data, ch->size );

		if( ch->type == SND_TYPE_NORMAL /*|| ch->type == SND_TYPE_LOOP*/ )
		{
			fp_t		dot;

			SND_CalcDistanceAndStereo( &dist, &dot, ch->origin );
			
			rscale = 1.0 + dot;
			lscale = 1.0 - dot;

			rscale *= dist;
			lscale *= dist;

			if( rscale < 0.0 )
				rscale = 0.0;
			if( lscale < 0.0 )
				lscale = 0.0;
			
		}
		else if( ch->type == SND_TYPE_LOCAL )
		{
//			printf( "local\n" );
			rscale = 1.0;
			lscale = 1.0;
		}
				
//		printf( "dist: %f\n", dist );

		if( ch->firsttime )
			ch->firsttime = 0;
		else
			ch->pointer+=framedelta;

		if( ( ch->size - ch->pointer ) < frames )
		{
			sframes = ch->size - ch->pointer;
		} else
			sframes = frames;

		
//		printf( "painting %d %d %d\n", sframes, ch->pointer, ch->size );

		for( i = 0; i < sframes; i++ )
		{
		       
			paintbuf[i].left += (ch->data[ch->pointer+i]*ch->lvol/*/256*/)*lscale;
			paintbuf[i].right += (ch->data[ch->pointer+i]*ch->rvol/*/256*/)*rscale;
			/*
			if( i == sframes )
			{
				si.SH_Printf( "loop\n" );
				ch->pointer = 0;
			}
			*/
		}



		if( sframes != frames )
			ch->data = NULL;
	}
//	printf( "framedelta: %d\n", frames );


	if( snd_doecho->ivalue )
		SND_AddEcho( paintbuf, frames );


}	

// ========================================
// init internal sound functions.
// One call at game startup must be enough.
// ========================================

void SND_InternStartUp()
{
	int	ebsize;

	__named_message( "\n" );

	ebsize = sizeof( samplepair_t ) *30000;
		
	printf( "ebsize: %d\n", ebsize );

	ebuf = (samplepair_t *)MM_Malloc( ebsize );
	w_ofs = 0;

	snd_echodelay = SHP_GetVar( "snd_echodelay" );
	snd_echofb = SHP_GetVar( "snd_echofb" );
	snd_echovol = SHP_GetVar( "snd_echovol" );
	snd_doecho = SHP_GetVar( "snd_doecho" );
	music = SHP_GetVar( "music" );


}

void SND_InternShutDown()
{
	MM_Free( ebuf );
}
