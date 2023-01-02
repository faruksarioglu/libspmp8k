/*
  Simple DirectMedia Layer
  Copyright (C) 1997-2013 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely, subject to the following restrictions:

  1. The origin of this software must not be misrepresented; you must not
     claim that you wrote the original software. If you use this software
     in a product, an acknowledgment in the product documentation would be
     appreciated but is not required.
  2. Altered source versions must be plainly marked as such, and must not be
     misrepresented as being the original software.
  3. This notice may not be removed or altered from any source distribution.
*/

#ifndef __SDL_PANDORA_H__
#define __SDL_PANDORA_H__

//#include <GLES/egl.h>

#include "SDL_config.h"
#include "../SDL_sysvideo.h"

typedef struct SDL_VideoData
{
    	struct {
		int bg_id;
		void *vram_pixels;           /* where the pixel data is stored (a pointer into VRAM) */
		void *pixels;				 /* area in user frame buffer */
		int length;
	} main;

    int pitch, bpp;             /* useful information about the texture */
    struct {
        int x, y;
    } scale;                    /* x/y stretch (24.8 fixed point) */

    struct {
        int x, y;
    } scroll;                   /* x/y offset */
    int rotate;                 /* -32768 to 32767, texture rotation */

	/* user frame buffer - todo: better way to do double buffering */
	void *pixels;
	int pixels_length;

} SDL_VideoData;


typedef struct SDL_DisplayData
{
uint16_t buffer;
} SDL_DisplayData;


typedef struct SDL_WindowData
{
    SDL_bool uses_gles;         /* if true window must support OpenGL ES */
    SDL_Surface display;
   // EGLConfig gles_configs[32];
   // EGLint gles_config;         /* OpenGL ES configuration index      */
   // EGLContext gles_context;    /* OpenGL ES context                  */
   // EGLint gles_attributes[256];        /* OpenGL ES attributes for context   */
   // EGLSurface gles_surface;    /* OpenGL ES target rendering surface */

} SDL_WindowData;


/****************************************************************************/
/* SDL_VideoDevice functions declaration                                    */
/****************************************************************************/

/* Display and window functions */
int PND_videoinit(_THIS);
void PND_videoquit(_THIS);
void PND_getdisplaymodes(_THIS, SDL_VideoDisplay * display);
int PND_setdisplaymode(_THIS, SDL_VideoDisplay * display, SDL_DisplayMode * mode);
int PND_createwindow(_THIS, SDL_Window * window);
int PND_createwindowfrom(_THIS, SDL_Window * window, const void *data);
void PND_createwindowframebuffer(_THIS,SDL_Window * window, Uint32 *format, void **pixels, int *pitch);
void PND_UpdateWindowFramebuffer(_THIS, SDL_Window * window, SDL_Rect * rects, int numrects);
int PND_DestroyWindowFramebuffer(_THIS, SDL_Window * window);
void PND_setwindowtitle(_THIS, SDL_Window * window);
void PND_setwindowicon(_THIS, SDL_Window * window, SDL_Surface * icon);
void PND_setwindowposition(_THIS, SDL_Window * window);
void PND_setwindowsize(_THIS, SDL_Window * window);
void PND_showwindow(_THIS, SDL_Window * window);
void PND_hidewindow(_THIS, SDL_Window * window);
void PND_raisewindow(_THIS, SDL_Window * window);
void PND_maximizewindow(_THIS, SDL_Window * window);
void PND_minimizewindow(_THIS, SDL_Window * window);
void PND_restorewindow(_THIS, SDL_Window * window);
void PND_setwindowgrab(_THIS, SDL_Window * window);
void PND_destroywindow(_THIS, SDL_Window * window);

/* Window manager function */
SDL_bool PND_getwindowwminfo(_THIS, SDL_Window * window,
                             struct SDL_SysWMinfo *info);


#endif /* __SDL_PANDORA_H__ */

/* vi: set ts=4 sw=4 expandtab: */
