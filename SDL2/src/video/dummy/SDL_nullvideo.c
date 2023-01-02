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
#include "SDL_config.h"

#if SDL_VIDEO_DRIVER_DUMMY

/* Dummy SDL video driver implementation; this is just enough to make an
 *  SDL-based application THINK it's got a working video driver, for
 *  applications that call SDL_Init(SDL_INIT_VIDEO) when they don't need it,
 *  and also for use as a collection of stubs when porting SDL to a new
 *  platform for which you haven't yet written a valid video driver.
 *
 * This is also a great way to determine bottlenecks: if you think that SDL
 *  is a performance problem for a given platform, enable this driver, and
 *  then see if your application runs faster without video overhead.
 *
 * Initial work by Ryan C. Gordon (icculus@icculus.org). A good portion
 *  of this was cut-and-pasted from Stephane Peter's work in the AAlib
 *  SDL video driver.  Renamed to "DUMMY" by Sam Lantinga.
 */

#include "SDL_video.h"
#include "SDL_mouse.h"
#include "../SDL_sysvideo.h"
#include "../SDL_pixels_c.h"
#include "../../events/SDL_events_c.h"

#include "SDL_nullvideo.h"
#include "SDL_nullevents_c.h"
#include "SDL_nullframebuffer_c.h"

#define DUMMYVID_DRIVER_NAME "dummy"
#include <libgame.h>

#define SLCDVID_DRIVER_NAME "slcd"
emu_graph_params_t gp;
uint16_t rgb;
uint16_t RealScreenBuffer1[480*272*2];
uint16_t srv_colortab_hi[128];		/* for mapping atari 8-bit colors to 32-bit colors */
uint32_t srv_colortab_med[128];		/* for soft scanlines */
uint32_t srv_colortab_low[128];
uint16_t *ScreenBuffer;			/* pointer to screen buffer (set to RealScreenBuffer 1-4) */
uint16_t srv_screen_fb[480 * 272*2];
mcatch_loadimg_t srv_screen;
unsigned short srv_screen_pal[256];
unsigned char srv_screen_handle;
    ge_key_data_t keys;
  
mcatch_point2d_t pos = {
	.x = 0,
	.y = 0
};

/* Initialization/Query functions */
static int DUMMY_VideoInit(_THIS);
static int DUMMY_SetDisplayMode(_THIS, SDL_VideoDisplay * display, SDL_DisplayMode * mode);
static void DUMMY_VideoQuit(_THIS);

/* DUMMY driver bootstrap functions */

static int
DUMMY_Available(void)
{
    const char *envr = SDL_getenv("SDL_VIDEODRIVER");
    if ((envr) && (SDL_strcmp(envr, DUMMYVID_DRIVER_NAME) == 0)) {
        return (1);
    }

    return (0);
}

static void
DUMMY_DeleteDevice(SDL_VideoDevice * device)
{
    SDL_free(device);
}

static SDL_VideoDevice *
DUMMY_CreateDevice(int devindex)
{
    SDL_VideoDevice *device;

    /* Initialize all variables that we clean on shutdown */
    device = (SDL_VideoDevice *) SDL_calloc(1, sizeof(SDL_VideoDevice));
    if (!device) {
        SDL_OutOfMemory();
        SDL_free(device);
        return (0);
    }

    /* Set the function pointers */
    device->VideoInit = DUMMY_VideoInit;
    device->VideoQuit = DUMMY_VideoQuit;
    device->SetDisplayMode = DUMMY_SetDisplayMode;
    device->PumpEvents = DUMMY_PumpEvents;
    device->CreateWindowFramebuffer = SDL_DUMMY_CreateWindowFramebuffer;
    device->UpdateWindowFramebuffer = SDL_DUMMY_UpdateWindowFramebuffer;
    device->DestroyWindowFramebuffer = SDL_DUMMY_DestroyWindowFramebuffer;

    device->free = DUMMY_DeleteDevice;

    return device;
}

VideoBootStrap DUMMY_bootstrap = {
    DUMMYVID_DRIVER_NAME, "SDL dummy video driver",
    DUMMY_Available, DUMMY_CreateDevice
};


int
DUMMY_VideoInit(_THIS)
{
    SDL_DisplayMode mode;

    /* Use a fake 32-bpp desktop mode */
    mode.format = SDL_PIXELFORMAT_RGB565;
    mode.w = 480;
    mode.h = 272;
    mode.refresh_rate = 0;
    mode.driverdata = NULL;
    if (SDL_AddBasicVideoDisplay(&mode) < 0) {
        return -1;
    }

    SDL_zero(mode);
    SDL_AddDisplayMode(&_this->displays[0], &mode);
MCatchFreeImage(srv_screen_handle);
	int res;
	mcatch_rect_t rect;
	MCatchInitGraph();
	rect.x = 0;
	rect.y = 0;
	rect.width = 480;
	rect.height = 272;
	MCatchSetFrameBuffer(480, 272);
	MCatchSetDisplayScreen(&pos);
	MCatchEnableFeature(3);
	MCatchSetColorROP(COLOR_ROP_NOP);
	srv_screen.data		= RealScreenBuffer1; //srv_screen_fb;
	srv_screen.width	= 480;
	srv_screen.height	= 272;
	srv_screen.img_type	= IMG_TYPE_8BPP;
	srv_screen.unk2		= 0;
	srv_screen.pal_data	= srv_colortab_hi;
	srv_screen.pal_size	= 128;
	srv_screen.unk3		= 0xffffff80;
	MCatchLoadImage(&srv_screen, &srv_screen_handle);
	MCatchSetColorROP(0xcc);
	ScreenBuffer = RealScreenBuffer1;
	gDisplayDev->setShadowBuffer(ScreenBuffer);
	gDisplayDev->setFrameBuffer(RealScreenBuffer1);
	memset(RealScreenBuffer1, 0,480*272);
    MCatchBitblt(srv_screen_handle, &rect, &pos);
	MCatchFlush();
	MCatchPaint();
	
    gDisplayDev->clear();

    /* We're done! */
    return 0;
}

static int
DUMMY_SetDisplayMode(_THIS, SDL_VideoDisplay * display, SDL_DisplayMode * mode)
{
    return 0;
}

void
DUMMY_VideoQuit(_THIS)
{
}
#define DUMMY_SURFACE   "_SDL_DummySurface"

int SDL_DUMMY_CreateWindowFramebuffer(_THIS, SDL_Window * window, Uint32 * format, void ** pixels, int *pitch)
{
    SDL_Surface *surface;
    const Uint32 surface_format = SDL_PIXELFORMAT_RGB888;
    int w, h;
    int bpp;
    Uint32 Rmask, Gmask, Bmask, Amask;

    /* Free the old framebuffer surface */
    surface = (SDL_Surface *) SDL_GetWindowData(window, DUMMY_SURFACE);
    SDL_FreeSurface(surface);

    /* Create a new one */
    SDL_PixelFormatEnumToMasks(surface_format, &bpp, &Rmask, &Gmask, &Bmask, &Amask);
    SDL_GetWindowSize(window, &w, &h);
    surface = SDL_CreateRGBSurface(0, w, h, bpp, Rmask, Gmask, Bmask, Amask);
    if (!surface) {
        return -1;
    }

    /* Save the info and return! */
    SDL_SetWindowData(window, DUMMY_SURFACE, surface);
    *format = surface_format;
    *pixels = surface->pixels;
    *pitch = surface->pitch;
    return 0;
}

int SDL_DUMMY_UpdateWindowFramebuffer(_THIS, SDL_Window * window, const SDL_Rect * rects, int numrects)
{
    static int frame_number;
    SDL_Surface *surface;

    surface = (SDL_Surface *) SDL_GetWindowData(window, DUMMY_SURFACE);
    if (!surface) {
        return SDL_SetError("Couldn't find dummy surface for window");
    }
	int i, j,width;
	width=480;
	for (i = 0; i < numrects; i++)
	{
		int consecutive = rects[i].w;
		int skipBetween = width - consecutive;
		int rows = rects[i].h;
		int start = (rects[i].y * width) + rects[i].x;

		uint16_t* tempDispBuff16 = &RealScreenBuffer1;//gDisplayDev->getShadowBuffer;
		uint16_t* tempDrawBuff16 = (uint16_t*)surface;
		uint16_t* tempDispEnd16;

		tempDispBuff16 += start;
		tempDrawBuff16 += start;

		while (rows > 0)
		{
			tempDispEnd16 = tempDispBuff16;
			tempDispEnd16 += consecutive;

			while(tempDispBuff16 < tempDispEnd16)
			{
				*(tempDispBuff16++) = *(tempDrawBuff16++);
			}

			tempDispBuff16 += skipBetween;
			tempDrawBuff16 += skipBetween;

			rows--;
	
		}

	}
//gDisplayDev->setShadowBuffer(tempDispBuff16);
  gDisplayDev->setShadowBuffer(surface->pixels);
   cache_sync();//__dcache_writeback_all();
	gDisplayDev->flip;//lcd_set_frame();



}

void SDL_DUMMY_DestroyWindowFramebuffer(_THIS, SDL_Window * window)
{
    SDL_Surface *surface;

    surface = (SDL_Surface *) SDL_SetWindowData(window, DUMMY_SURFACE, NULL);
    SDL_FreeSurface(surface);
}

#endif /* SDL_VIDEO_DRIVER_DUMMY */

/* vi: set ts=4 sw=4 expandtab: */
