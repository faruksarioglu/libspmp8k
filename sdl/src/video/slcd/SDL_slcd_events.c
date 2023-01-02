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

#include "SDL.h"
#include "../../events/SDL_sysevents.h"
#include "../../events/SDL_events_c.h"
#include <libgame.h>
#include "SDL_slcd_video.h"
#include "SDL_slcd_events_c.h"

//#include <sml/control.h>
//#include <sml/timer.h>

static SDLKey keymap[32];
static SDL_keysym *TranslateKey(int scancode, SDL_keysym *keysym);
static int posted = 0;

static Uint8 dingooIgnoreOSEvents = 0;

void SLCD_PumpEvents(_THIS)
{

	const Uint8 *keystate;
 	keystate = SDL_GetKeyState(NULL);
    uint32_t nkeys;
 	ge_key_data_t keys;
	 NativeGE_getKeyInput4Ntv(&keys);
	
   // nkeys = emuIfKeyGetInput(&keymap);
    if ( keys.keys & GE_KEY_UP) 
		SDLK_UP== SDL_PRESSED;
	if ( keys.keys & GE_KEY_DOWN) 
		SDLK_DOWN== SDL_PRESSED;
    if (keys.keys & GE_KEY_LEFT)
        SDLK_LEFT== SDL_PRESSED;
    if (keys.keys & GE_KEY_RIGHT)
        SDLK_RIGHT==SDL_PRESSED;
    if (keys.keys & GE_KEY_X)
       SDLK_SPACE== SDL_PRESSED ;
    if (keys.keys & GE_KEY_START)
        SDLK_LCTRL==SDL_PRESSED;
    if (keys.keys & GE_KEY_SELECT)
        SDLK_RETURN== SDL_PRESSED;
    if (keys.keys& EMU_KEY_R)
        SDLK_ESCAPE== SDL_PRESSED;
}

void SLCD_InitOSKeymap(_THIS)
{
	char* env;
	env = SDL_getenv("DINGOO_IGNORE_OS_EVENTS");
	if (env)
		dingooIgnoreOSEvents = 1;

	int i;
	for (i = 0; i < 32; i++)
		keymap[i] = SDLK_UNKNOWN;

	keymap[GE_KEY_SELECT] = SDLK_PAUSE;
	keymap[GE_KEY_O] = SDLK_LCTRL;
	keymap[GE_KEY_X] = SDLK_LALT;
	keymap[GE_KEY_SQUARE] = SDLK_SPACE;
	keymap[GE_KEY_TRIANGLE] = SDLK_LSHIFT;
	keymap[GE_KEY_X] = SDLK_RETURN;
	keymap[GE_KEY_SELECT] = SDLK_ESCAPE;
	keymap[GE_KEY_L1] = SDLK_TAB;
	keymap[EMU_KEY_R] = SDLK_BACKSPACE;
	keymap[GE_KEY_UP] = SDLK_UP;
	keymap[GE_KEY_DOWN] = SDLK_DOWN;
	keymap[GE_KEY_LEFT] = SDLK_LEFT;
	keymap[GE_KEY_RIGHT] = SDLK_RIGHT;
}

static SDL_keysym *TranslateKey(int scancode, SDL_keysym *keysym)
{
	/* Set the keysym information */
	keysym->scancode = scancode;
	keysym->sym = keymap[scancode];
	keysym->mod = KMOD_NONE;

	/* If UNICODE is on, get the UNICODE value for the key */
	keysym->unicode = 0;
	if ( SDL_TranslateUNICODE ) {
		/* Populate the unicode field with the ASCII value */
		keysym->unicode = scancode;
	}
	return(keysym);
}

/* end of SDL_slcd_events.c ... */

