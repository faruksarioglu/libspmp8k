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

/* Dingoo SLCD SDL video driver implementation
 */

#include "SDL_video.h"
#include "SDL_mouse.h"
#include "../SDL_sysvideo.h"
#include "../SDL_pixels_c.h"
#include "../../events/SDL_events_c.h"

#include "SDL_slcd_video.h"
#include "SDL_slcd_events_c.h"
#include "SDL_slcd_mouse_c.h"
#include "splash.h"
//#include <dingoo/slcd.h>
//#include <dingoo/cache.h>
#include <libgame.h>
typedef uint8_t uint24_t[3];
int BPP;
#define SLCDVID_DRIVER_NAME "slcd"
emu_graph_params_t gp;
uint16_t *screen_base16;
uint8_t *screen_base8;
uint32_t *screen_base32;

uint16_t palette[256];
  
mcatch_point2d_t pos = {
	.x = 0,
	.y = 0
};

/* Initialization/Query functions */
static int SLCD_VideoInit(_THIS, SDL_PixelFormat *vformat);
static SDL_Rect **SLCD_ListModes(_THIS, SDL_PixelFormat *format, Uint32 flags);
static SDL_Surface *SLCD_SetVideoMode(_THIS, SDL_Surface *current, int width, int height, int bpp, Uint32 flags);
static int SLCD_SetColors(_THIS, int firstcolor, int ncolors, SDL_Color *colors);
static void SLCD_VideoQuit(_THIS);

/* Hardware surface functions */
static int SLCD_AllocHWSurface(_THIS, SDL_Surface *surface);
static int SLCD_LockHWSurface(_THIS, SDL_Surface *surface);
static void SLCD_UnlockHWSurface(_THIS, SDL_Surface *surface);
static void SLCD_FreeHWSurface(_THIS, SDL_Surface *surface);

/* etc. */
static void SLCD_UpdateRects(_THIS, int numrects, SDL_Rect *rects);

/* SLCD driver bootstrap functions */

static int SLCD_Available(void)
{
	return 1; // Always available
}

static void SLCD_DeleteDevice(SDL_VideoDevice *device)
{
	SDL_free(device->hidden);
	SDL_free(device);
}

static SDL_VideoDevice *SLCD_CreateDevice(int devindex)
{
	SDL_VideoDevice *device;

	/* Initialize all variables that we clean on shutdown */
	device = (SDL_VideoDevice *)SDL_malloc(sizeof(SDL_VideoDevice));
	if ( device ) {
		SDL_memset(device, 0, (sizeof *device));
		device->hidden = (struct SDL_PrivateVideoData *)
				SDL_malloc((sizeof *device->hidden));
	}
	if ( (device == NULL) || (device->hidden == NULL) ) {
		SDL_OutOfMemory();
		if ( device ) {
			SDL_free(device);
		}
		return(0);
	}
	SDL_memset(device->hidden, 0, (sizeof *device->hidden));

	/* Set the function pointers */
	device->VideoInit = SLCD_VideoInit;
	device->ListModes = SLCD_ListModes;
	device->SetVideoMode = SLCD_SetVideoMode;
	device->CreateYUVOverlay = NULL;
	device->SetColors = SLCD_SetColors;
	device->UpdateRects = SLCD_UpdateRects;
	device->VideoQuit = SLCD_VideoQuit;
	device->AllocHWSurface = SLCD_AllocHWSurface;
	device->CheckHWBlit = NULL;
	device->FillHWRect = NULL;
	device->SetHWColorKey = NULL;
	device->SetHWAlpha = NULL;
	device->LockHWSurface = SLCD_LockHWSurface;
	device->UnlockHWSurface = SLCD_UnlockHWSurface;
	device->FlipHWSurface = NULL;
	device->FreeHWSurface = SLCD_FreeHWSurface;
	device->SetCaption = NULL;
	device->SetIcon = NULL;
	device->IconifyWindow = NULL;
	device->GrabInput = NULL;
	device->GetWMInfo = NULL;
	device->InitOSKeymap = SLCD_InitOSKeymap;
	device->PumpEvents = SLCD_PumpEvents;

	device->free = SLCD_DeleteDevice;

	return device;
}

VideoBootStrap SLCD_bootstrap = {
	SLCDVID_DRIVER_NAME, "SDL Dingoo SLCD video driver",
	SLCD_Available, SLCD_CreateDevice
};


int SLCD_VideoInit(_THIS, SDL_PixelFormat *vformat)
{
	int i;

	/* Initialize all variables that we clean on shutdown */
	for ( i=0; i<SDL_NUMMODES; ++i ) {
		SDL_modelist[i] = SDL_malloc(sizeof(SDL_Rect));
		SDL_modelist[i]->x = SDL_modelist[i]->y = 0;
	}

	/* Modes sorted largest to smallest */
	SDL_modelist[0]->w = 480; SDL_modelist[0]->h = 272;
	SDL_modelist[1]->w = 384; SDL_modelist[1]->h = 272;
	SDL_modelist[2]->w = 384; SDL_modelist[2]->h = 224;
	SDL_modelist[3]->w = 320; SDL_modelist[3]->h = 240;
	SDL_modelist[4] = NULL;

	/* Determine the screen depth */
	/* we change this during the SDL_SetVideoMode implementation... */
	vformat->BitsPerPixel = 16; // FIXME ?
	vformat->BytesPerPixel = 2;

	// Clear the screen to black, but don't actually update it to the display
	// This is mostly useful for apps which does not update the whole screen,
	// better to have those parts black than still seeing the launcher...
	//uint16_t a=gDisplayDev->getShadowBuffer();
	//memset(&a, 0, 480*272* 2);
	//gDisplayDev->setShadowBuffer(&a);
	/* We're done! */
      // gDisplayDev->clear();
	return(1);
}

SDL_Rect **SLCD_ListModes(_THIS, SDL_PixelFormat *format, Uint32 flags)
{
	//if(format->BitsPerPixel != 16)
 	//	return NULL;

//	if ( flags & SDL_FULLSCREEN ) {
//		return SDL_modelist;
//	} else {
//		return (SDL_Rect **) -1;
//	}
	return SDL_modelist;
}




SDL_Surface *SLCD_SetVideoMode(_THIS, SDL_Surface *current,
				int width, int height, int bpp, Uint32 flags)
{
	//if (width != 480 || height != 272 || bpp != 16)
//	{
	//	SDL_SetError("Wrong format!");
	//	return NULL;
//	}
   BPP= bpp;
	if ( this->hidden->buffer ) {
		SDL_free( this->hidden->buffer );
	}

	this->hidden->buffer = SDL_malloc(width * height*(bpp/8));
	if ( ! this->hidden->buffer ) {
		SDL_SetError("Couldn't allocate buffer for requested mode");
		return(NULL);
	}

/* 	printf("Setting mode %dx%d\n", width, height); */

	SDL_memset(this->hidden->buffer, 0, width * height*(bpp/8) );

	/* Allocate the new pixel format for the screen */
	if ( ! SDL_ReallocFormat(current, bpp, 0, 0, 0, 0) ) {
		SDL_free(this->hidden->buffer);
		this->hidden->buffer = NULL;
		SDL_SetError("Couldn't allocate new pixel format for requested mode");
		return(NULL);
	}

	/* Set up the new mode framebuffer */
	current->flags = flags & SDL_FULLSCREEN;
	this->hidden->w = current->w = width;
	this->hidden->h = current->h = height;
	current->pitch = current->w * (bpp / 8);
	current->pixels = this->hidden->buffer;
	
	gp.pixels = malloc(width * height * bpp / 8);
    gp.width = width;
    gp.height = height;
    gp.has_palette = 0;//1;
    gp.palette = palette;
    gp.src_clip_x = 0;
    gp.src_clip_y = 0;
    gp.src_clip_w = width;
    gp.src_clip_h = height;
    emuIfGraphInit(&gp);
    gDisplayDev->setShadowBuffer(gDisplayDev->getFrameBuffer());
 	
 	 screen_base16 = (uint16_t *)gp.pixels;
	 
	uint24_t *data=dgen_splash_data.pixel_data;;
	int fb_w = width;
	    int i, j;


		uint16_t *fb =(uint16_t *)screen_base16+ width-320 + (height-120)/2 * fb_w;
		for (i = 0; i < 120; i++) {
        for (j = 0; j < 320; j++)
            if (data[j][0]+data[j][1]+data[j][2] != 0)
                fb[j] = ((data[j][0]&0xf8) << 8)|((data[j][1]& 0xfc) << 3) |((data[j][02])>>3);
        fb += fb_w;
        data += 320;
    }
	 cache_sync();//__dcache_writeback_all();
	 gDisplayDev->flip();//lcd_set_frame();
cyg_thread_delay(100);

	
	
	/* We're done */
	return(current);
}

/* We don't actually allow hardware surfaces other than the main one */
static int SLCD_AllocHWSurface(_THIS, SDL_Surface *surface)
{
	return(-1);
}
static void SLCD_FreeHWSurface(_THIS, SDL_Surface *surface)
{
	return;
}

/* We need to wait for vertical retrace on page flipped displays */
static int SLCD_LockHWSurface(_THIS, SDL_Surface *surface)
{
	return(0);
}

static void SLCD_UnlockHWSurface(_THIS, SDL_Surface *surface)
{
	return;
}

static void SLCD_UpdateRects(_THIS, int numrects, SDL_Rect *rects)
{
	if (this->hidden->buffer == NULL)
		return;

	// Will probably crash if user provides out of boundaries rects,
	// but according to docs that's the user's responsibility to check

	int i, j,width;
	width=this->hidden->w;
	for (i = 0; i < numrects; i++)
	{
		int consecutive = rects[i].w;
		int skipBetween = width - consecutive;
		int rows = rects[i].h;
		int start = (rects[i].y * width) + rects[i].x;
		
	uint16_t* tempDispBuff16 = (uint16_t*)screen_base16;//gDisplayDev->getShadowBuffer;
				uint16_t* tempDrawBuff16 = (uint16_t*)this->hidden->buffer;
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
  //gDisplayDev->setShadowBuffer(this->hidden->buffer);
   cache_sync();//__dcache_writeback_all();
	 //gDisplayDev->flip();//lcd_set_frame();
    emuIfGraphShow();

}

int SLCD_SetColors(_THIS, int firstcolor, int ncolors, SDL_Color *colors)
{
	int i;
	for(i=firstcolor;i < ncolors;i++)
	{
		palette[i]=MAKE_RGB565(colors[i].r,colors[i].g,colors[i].b);
		
	}
	gp.has_palette = 1;
    gp.palette = palette;
    emuIfGraphChgView(&gp);
    emuIfGraphShow();

	/* do nothing of note. */
	return(1);
}

/* Note:  If we are terminated, this could be called in the middle of
   another SDL video routine -- notably UpdateRects.
*/
void SLCD_VideoQuit(_THIS)
{
	int i;

	/* Free video mode lists */
	for ( i=0; i<SDL_NUMMODES; ++i ) {
		if ( SDL_modelist[i] != NULL ) {
			SDL_free(SDL_modelist[i]);
			SDL_modelist[i] = NULL;
		}
	}
	if (this->screen->pixels != NULL)
	{
		SDL_free(this->screen->pixels);
		this->screen->pixels = NULL;
	}
//	emuIfGraphCleanup();
emuIfGraphCleanup();

}
