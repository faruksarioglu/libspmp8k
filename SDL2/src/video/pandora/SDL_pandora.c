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

#if SDL_VIDEO_DRIVER_PANDORA

/* SDL internals */
#include "../SDL_sysvideo.h"
#include "SDL_version.h"
#include "SDL_syswm.h"
#include "SDL_loadso.h"
#include "SDL_events.h"
#include "../../events/SDL_mouse_c.h"
#include "../../events/SDL_keyboard_c.h"
#include <libgame.h>

/* PND declarations */
#include "SDL_pandora.h"
//#include "SDL_pandora_events.h"

/* WIZ declarations */
//#include "GLES/gl.h"
//#ifdef WIZ_GLES_LITE
emu_graph_params_t gp;
uint16_t rgb;
uint16_t RealScreenBuffer1[480*272*2];
uint16_t srv_colortab_hi[128];		/* for mapping atari 8-bit colors to 32-bit colors */
uint32_t srv_colortab_med[128];		/* for soft scanlines */
uint32_t srv_colortab_low[128];
uint16_t *ScreenBuffer;			/* pointer to screen buffer (set to RealScreenBuffer 1-4) */
uint16_t srv_screen_fb[480 * 272];
mcatch_loadimg_t srv_screen;
unsigned short srv_screen_pal[256];
unsigned char srv_screen_handle;
    ge_key_data_t keys;
  
mcatch_point2d_t pos = {
	.x = 0,
	.y = 0
};

//static NativeWindowType hNativeWnd = 0; /* A handle to the window we will create. */
//#endif

static SDL_bool PND_initialized = SDL_FALSE;

static SDL_DisplayMode display_modes[] =
{
	/* Only one screen */
	{
		.format = SDL_PIXELFORMAT_RGB565,
		.w = 480,
		.h = 272,
		.refresh_rate = 60,
	},

	/* Aggregated display (two screens) with no gap. */
	{
		.format = SDL_PIXELFORMAT_RGB888,
		.w = 480,
		.h = 272,
		.refresh_rate = 60,
	},

	{
		.w = 0,
	}
};



static int
PND_available(void)
{
    return 1;
}

static void
PND_destroy(SDL_VideoDevice * device)
{
        SDL_free(device);

}

static SDL_VideoDevice *
PND_create()
{
  SDL_VideoDevice *device;

    /* Initialize all variables that we clean on shutdown */
    device = SDL_calloc(1, sizeof(SDL_VideoDevice));
    if (!device) {
        SDL_OutOfMemory();
        return NULL;
    }

	device->driverdata = SDL_calloc(1, sizeof(SDL_VideoDevice));
    if (!device) {
		SDL_free(device);
        SDL_OutOfMemory();
        return NULL;
    }

    device->VideoInit = PND_videoinit;
    device->VideoQuit = PND_videoquit;
    device->GetDisplayModes = PND_getdisplaymodes;
    device->SetDisplayMode = PND_setdisplaymode;
    device->CreateWindow = PND_createwindow;
    device->CreateWindowFramebuffer = PND_createwindowframebuffer;
    device->CreateWindowFrom = PND_createwindowfrom;
   /* device->SetWindowTitle = PND_setwindowtitle;
    device->SetWindowIcon = PND_setwindowicon;
    device->SetWindowPosition = PND_setwindowposition;
    device->SetWindowSize = PND_setwindowsize;
    device->ShowWindow = PND_showwindow;
    device->HideWindow = PND_hidewindow;
    device->RaiseWindow = PND_raisewindow;
    device->MaximizeWindow = PND_maximizewindow;
    device->MinimizeWindow = PND_minimizewindow;
    device->RestoreWindow = PND_restorewindow;
   */ device->SetWindowGrab = PND_setwindowgrab;
    device->DestroyWindow = PND_destroywindow;
    //device->GetWindowWMInfo = PND_getwindowwminfo;
    device->UpdateWindowFramebuffer = PND_UpdateWindowFramebuffer;
    device->DestroyWindowFramebuffer = PND_DestroyWindowFramebuffer;

    /* !!! FIXME: implement SetWindowBordered */

    return device;
}

VideoBootStrap PND_bootstrap = {
    "pandora",
    "SDL Pandora Video Driver",
    PND_available,
    PND_create
};

/*****************************************************************************/
/* SDL Video and Display initialization/handling functions                   */
/*****************************************************************************/
int
PND_videoinit(_THIS)
{
   
   SDL_VideoDisplay display;
    SDL_DisplayMode mode;

	SDL_zero(mode);

    mode.format = SDL_PIXELFORMAT_RGB565; // should be SDL_PIXELFORMAT_ABGR1555;
    mode.w = 480;
    mode.h = 272;
    mode.refresh_rate = 60;

	SDL_zero(display);

	display.desktop_mode = mode;

	SDL_AddVideoDisplay(&display);

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
	srv_screen.data		= srv_screen_fb;
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
	gDisplayDev->setShadowBuffer(&RealScreenBuffer1);
	gDisplayDev->setFrameBuffer(gDisplayDev->getShadowBuffer());
	memset(RealScreenBuffer1, 0,480*272);
    MCatchBitblt(srv_screen_handle, &rect, &pos);
	MCatchFlush();
	MCatchPaint();
   
 return 1;
}

void
PND_videoquit(_THIS)
{

}

void
PND_getdisplaymodes(_THIS, SDL_VideoDisplay * display)
{
SDL_DisplayMode *mode;

	for (mode = display_modes; mode->w; mode++) {
		mode->driverdata = mode; /* point back to self */
		SDL_AddDisplayMode(display, mode);
	}
}

int
PND_setdisplaymode(_THIS, SDL_VideoDisplay * display, SDL_DisplayMode * mode)
{
  	return 0;
}

int
PND_createwindow(_THIS, SDL_Window * window)
{

	/* Nintendo DS windows are always fullscreen */
    window->flags |= SDL_WINDOW_FULLSCREEN;
    return 0;
}

void
PND_createwindowframebuffer(_THIS,SDL_Window * window, Uint32 *format, void **pixels, int *pitch)
{
		struct SDL_VideoData *wdata;
    int bpp=16;
    Uint32 Rmask, Gmask, Bmask, Amask;
	const SDL_VideoDisplay *display = SDL_GetDisplayForWindow(window);
	const SDL_DisplayMode *mode = display->driverdata;
	const Uint32 fmt = mode->format;

//	if (fmt != SDL_PIXELFORMAT_ARGB655) {
//		SDL_SetError("Unsupported pixel format (%x)", fmt);
//		return -1;
//	}

 //   if (!SDL_PixelFormatEnumToMasks
 //       (fmt, &bpp, &Rmask, &Gmask, &Bmask, &Amask)) {
 //       SDL_SetError("Unknown texture format");
  //      return -1;
  //  }

	wdata = SDL_calloc(1, sizeof(struct SDL_VideoData));
	if (!wdata) {
		SDL_OutOfMemory();
		return -1;
	}

	if (bpp == 8) {
		wdata->pixels_length = 480*272*2;
	} else {
		wdata->pixels_length = 480*272*2;
	}
	wdata->pixels = SDL_calloc(1, wdata->pixels_length);
	if (!wdata->pixels) {
		SDL_free(wdata);
		SDL_SetError("Not enough memory");
        return -1;
    }

	if (bpp == 8) {
		wdata->main.bg_id = 1;
		wdata->main.length = 480*272;
		wdata->main.pixels = wdata->pixels;
		
	} else {
		wdata->main.bg_id = 2;
		wdata->main.length = 480*272;
		wdata->main.pixels = wdata->pixels;
}

	wdata->pitch = (window->w) * ((bpp+1) / 8);
	wdata->bpp = bpp;
	wdata->rotate = 0;
	wdata->scale.x = 0x100;
	wdata->scale.y = 0x100;
	wdata->scroll.x = 0;
	wdata->scroll.y = 0;

	wdata->main.vram_pixels = &RealScreenBuffer1;
	//cache_sync();

	*format = fmt;
	*pixels = wdata->pixels;
	*pitch = wdata->pitch;

	window->driverdata = wdata;

	return 0;


}
void PND_UpdateWindowFramebuffer(_THIS, SDL_Window * window, SDL_Rect * rects, int numrects)
{

   	struct PND_WindowData *wdata = window->driverdata;

	/* Copy everything. TODO: use rects/numrects. */
//	DC_FlushRange(wdata->pixels, wdata->pixels_length);

//	swiWaitForVBlank();
   //     memset(fb, 0xCCCCCCCC, 480 * 272 * 2);

//  memcpy( wdata->main.vram_pixels,wdata->main.pixels, wdata->main.length);
	int i, j,width;
	width=480;
	for (i = 0; i < numrects; i++)
	{
		int consecutive = rects[i].w;
		int skipBetween = width - consecutive;
		int rows = rects[i].h;
		int start = (rects[i].y * width) + rects[i].x;

		uint16_t* tempDispBuff16 = &RealScreenBuffer1;//gDisplayDev->getShadowBuffer();
		uint16_t* tempDrawBuff16 = (uint16_t*)wdata->main.pixels;
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
  gDisplayDev->setShadowBuffer(wdata->main.pixels);
   cache_sync();//__dcache_writeback_all();
	 gDisplayDev->flip();//lcd_set_frame();

    return 0;
	
}

int PND_DestroyWindowFramebuffer(_THIS, SDL_Window * window)
{
  struct PND_WindowData *wdata = window->driverdata;

    SDL_free(wdata->pixels);
    SDL_free(wdata);
return 0;
}

int
PND_createwindowfrom(_THIS, SDL_Window * window, const void *data)
{
    return -1;
}

void
PND_setwindowtitle(_THIS, SDL_Window * window)
{
}
void
PND_setwindowicon(_THIS, SDL_Window * window, SDL_Surface * icon)
{
}
void
PND_setwindowposition(_THIS, SDL_Window * window)
{
}
void
PND_setwindowsize(_THIS, SDL_Window * window)
{
}
void
PND_showwindow(_THIS, SDL_Window * window)
{
//	struct SDL_VideoData *wdata = window->driverdata;

//	memcpy(wdata->pixels, wdata->main.pixels, wdata->main.length);
//	memcpy(RealScreenBuffer1, (uint16_t*) wdata->main.pixels, wdata->pixels_length);
  // cache_sync();//__dcache_writeback_all();
//	gDisplayDev->flip;//lcd_set_frame();

	
}
void
PND_hidewindow(_THIS, SDL_Window * window)
{
}
void
PND_raisewindow(_THIS, SDL_Window * window)
{
}
void
PND_maximizewindow(_THIS, SDL_Window * window)
{
}
void
PND_minimizewindow(_THIS, SDL_Window * window)
{
}
void
PND_restorewindow(_THIS, SDL_Window * window)
{
}
void
PND_setwindowgrab(_THIS, SDL_Window * window)
{
}
void
PND_destroywindow(_THIS, SDL_Window * window)
{
    SDL_VideoData *phdata = (SDL_VideoData *) _this->driverdata;
   // eglTerminate(phdata->egl_display);
}

/*****************************************************************************/
/* SDL Window Manager function                                               */
/*****************************************************************************/
SDL_bool
PND_getwindowwminfo(_THIS, SDL_Window * window, struct SDL_SysWMinfo *info)
{
    if (info->version.major <= SDL_MAJOR_VERSION) {
        return SDL_TRUE;
    } else {
        SDL_SetError("application not compiled with SDL %d.%d\n",
                     SDL_MAJOR_VERSION, SDL_MINOR_VERSION);
        return SDL_FALSE;
    }

    /* Failed to get window manager information */
    return SDL_FALSE;
}

#endif /* SDL_VIDEO_DRIVER_PANDORA */

/* vi: set ts=4 sw=4 expandtab: */
