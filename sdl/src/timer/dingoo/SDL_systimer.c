/*
    SDL - Simple DirectMedia Layer
    Copyright (C) 1997-2009 Sam Lantinga

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    Sam Lantinga
    slouken@libsdl.org
*/

#include "SDL_config.h"
#include <libgame.h>
#ifdef SDL_TIMER_DINGOO

//#include <dingoo/ucos2.h>
//#include <dingoo/time.h>

#include "SDL_thread.h"
#include "SDL_timer.h"
#include "../SDL_timer_c.h"

static uint32_t start;

void SDL_StartTicks(void)
{
	/* Set first ticks value. */
	start = libgame_utime();
}

Uint32 SDL_GetTicks(void)
{
	uint32_t now = libgame_utime();
	return(now - start);
}

void SDL_Delay(Uint32 ms)
{
	if (ms <= 5000)
	{
		cyg_thread_delay(ms / 10); // Not worth it using 10.01 for small values because of rounding
		//delay_ms(ms % 10);
	}
	else
	{
		cyg_thread_delay(ms / 10.01);
	}
}

/* This is only called if the event thread is not running */
int SDL_SYS_TimerInit(void)
{
	return 0;
}

void SDL_SYS_TimerQuit(void)
{
}

int SDL_SYS_StartTimer(void)
{
	start = libgame_utime();
	return 0;
}

void SDL_SYS_StopTimer(void)
{
}

#endif
