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



// sys_time.c

// support.c



#if defined(linux_i386) || defined(irix_mips)
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
void SYS_TimerCallback(int);

#elif defined(win32_x86)
#include <sys/timeb.h>
#include <windows.h>
#if 0
DWORD _stdcall timeGetTime( void ); // mmsystem.h is fucked up
#else
#ifdef __GNUC__
#define NONAMELESSUNION
#endif
#include <mmsystem.h>

#endif
void SYS_TimerCallback( HWND, UINT, UINT, DWORD );
static UINT     timerid;

#endif
#include "interfaces.h"

void (*TimerCallback)();


/* time */

#define LOOPS_SLEEP_MIN ( 20 )
#define LOOPS_SLEEP_OVH ( 20 )
#define USEC_OVH        ( 0 )

//static usec_t sleep_min_usec = 0;
//static usec_t sleep_ovh_usec = 0;

void GetTimeval( timeval_t* time_ptr )
{

	#if defined(unix) || defined (__unix__)
	gettimeofday( time_ptr, 0);
	#elif defined(__WIN32__)
	time_ptr->tv_sec = 0;
//        time_ptr->tv_usec = (int)((GetTickCount() / (double)CLOCKS_PER_SEC) * 1000000.0);
	time_ptr->tv_usec = timeGetTime() * 1000; // horray windows has a msec timer!
	#endif
}

void CopyTimeval( timeval_t* from_ptr, timeval_t* to_ptr) {
	to_ptr->tv_sec = from_ptr->tv_sec;
	to_ptr->tv_usec = from_ptr->tv_usec;
}
 
usec_t DiffTimeval( timeval_t* now_ptr, timeval_t* past_ptr) {
	/* now should be greater than past */
	usec_t usec_diff;
  
	if ( now_ptr->tv_sec > past_ptr->tv_sec ) {
		usec_diff = ( now_ptr->tv_usec + ( now_ptr->tv_sec - past_ptr->tv_sec ) 
			      * 1000000 ) - past_ptr->tv_usec;
	} else {
		usec_diff = now_ptr->tv_usec - past_ptr->tv_usec;
	}
	return usec_diff;
}


void SYS_TimerStart( unsigned int diff, void (*handler)())
{
#if 0
#if defined(linux_i386) || defined(irix_mips)
	
	struct itimerval value;
	__named_message( "starting timer\n" );
	value.it_interval.tv_sec = 0;
	value.it_interval.tv_usec = diff;
	
	value.it_value.tv_sec = 0;
	value.it_value.tv_usec = diff;

	signal( SIGALRM, SYS_TimerCallback );
	setitimer( ITIMER_REAL, &value, NULL);

#elif defined(win32_x86)
	timerid = SetTimer( NULL, 0, diff / 1000, SYS_TimerCallback );
	if( !timerid)
	{
		__error( "timerid = 0\n" );
	}	
#endif

	TimerCallback = handler;
#endif
}

unsigned int SYS_GetMsec()
{
	static unsigned int	msecs_base = 0;
	unsigned int	msecs = 0;

#if defined (linux_i386) || defined(irix_mips)
	timeval_t	tv_now;


	gettimeofday( &tv_now, 0 );

	if ( msecs_base == 0 )
	{
		msecs_base = tv_now.tv_sec;	
	}
       
	msecs = tv_now.tv_usec / 1000 + (tv_now.tv_sec - msecs_base) * 1000;

#elif defined(  win32_x86 )
	msecs = timeGetTime();  // windows: 12 points
#endif
	return msecs;
}	

unsigned int SYS_GetSec()
{
	unsigned int	time;
	
#if defined (linux_i386) || defined(irix_mips)
	timeval_t	tv_now;

	gettimeofday( &tv_now, 0 );

	time = tv_now.tv_sec;

#elif defined( win32_x86 )
	time = SYS_GetMsec() / 1000; // todo: we need an absolute date.
#endif


	return time;

}
	

void SYS_SleepMsec( int msec )
{
	int	waserror;
	struct timeval	tv;
	
#if defined (linux_i386)
	tv.tv_sec = msec / 1000;
	tv.tv_usec = (msec%1000)*1000;

	do
	{
		errno = 0;
		waserror = select( 0, NULL, NULL, NULL, &tv );
	} while( waserror && (errno == EINTR ) );
#elif defined (win32_x86)
	Sleep( msec );
#else
#error SYS_Sleep not implemented on this system
#endif
}



#if defined(linux_i386) || defined(irix_mips)
void SYS_TimerCallback( int )
{
	signal( SIGALRM, SYS_TimerCallback );
	
	TimerCallback();
}

#elif defined(win32_x86)
void SYS_TimerCallback( HWND bla1, UINT bla2, UINT arg_eventid, DWORD bla3 )
{
	__named_message( "here!\n" );
	TimerCallback();
}
#endif	
