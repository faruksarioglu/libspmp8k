/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>
#include <SDL/SDL_mixer.h>
#include <SDL/SDL_image.h>
#include <png.h>

#include "global.h"
#include "atari.h"
#include "psp_danzeff.h"
#include "psp_sdl.h"
#include "psp_gu.h"


static unsigned int __attribute__((aligned(16))) list[4096];

void
psp_sdl_gu_init(void)
{
}


void 
psp_sdl_gu_stretch(SDL_Rect* srcrect, SDL_Rect* dstrect)
{
  SDL_BlitSurface(blit_surface, srcrect, back_surface, dstrect);
}


