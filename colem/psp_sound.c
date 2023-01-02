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
//#include <SDL/SDL_mixer.h>
#include <SDL/SDL_image.h>
#include <libgame.h>
#include "Coleco.h"
#include "psp_sdl.h"
#include "psp_sound.h"

//SDL_AudioSpec wanted;
emu_sound_params_t sp;

#define NOISEBSIZE 0x8000
static short noise[NOISEBSIZE];

typedef struct
{
  unsigned int spos;
  unsigned int sinc;
  unsigned int vol;
} Channel;

static short square[]={
32767,32767,32767,32767,
32767,32767,32767,32767,
32767,32767,32767,32767,
32767,32767,32767,32767,
32767,32767,32767,32767,
32767,32767,32767,32767,
32767,32767,32767,32767,
32767,32767,32767,32767,
-32767,-32767,-32767,-32767,
-32767,-32767,-32767,-32767,
-32767,-32767,-32767,-32767,
-32767,-32767,-32767,-32767,
-32767,-32767,-32767,-32767,
-32767,-32767,-32767,-32767,
-32767,-32767,-32767,-32767,
-32767,-32767,-32767,-32767,
};

static int NoiseGen=1;
static Channel chan[4] = {
  {0,0,0},
  {0,0,0},
  {0,0,0} 
};

uint8_t sdl_sound_buffer[128] ;
int bufferpos=0;

 void 
snd_Mixer()
{
  int i;
  short v0,v1,v2,v3;
  long s; 

 // if (sdl_sound_buffer) {

    //uint8_t* scan_buffer = sdl_sound_buffer;
   // if (CV.cv_snd_enable) {
      v0=chan[0].vol;
      v1=chan[1].vol;
      v2=chan[2].vol;
      v3=chan[3].vol;

      for (i=0;i<128;i++) {
        s =(v0*square[(chan[0].spos>>8)&0x3f]);
        s+=(v1*square[(chan[1].spos>>8)&0x3f]);
        s+=(v2*square[(chan[2].spos>>8)&0x3f]);
        s+=(v3*noise[(chan[3].spos>>8)&(NOISEBSIZE-1)]);      
        sdl_sound_buffer[bufferpos] = (uint8_t*)s;
        bufferpos++;
		chan[0].spos += chan[0].sinc;
        chan[1].spos += chan[1].sinc;
        chan[2].spos += chan[2].sinc;
        chan[3].spos += chan[3].sinc;  
      }
      long volume = (SDL_MIX_MAXVOLUME * 30 / 100);
   //sp.buf_size=8192;
  //int col = psp_sdl_rgb(0xa0, 0xa0, 0xa0);


//   if (bufferpos>=127){
   bufferpos=0;
  emuIfSoundPlay(&sp);
 // psp_sdl_fill_rectangle(0,0,300,10,0,0);

 // psp_sdl_print(10, 0, "Sound Playing", col);
 // psp_sdl_flip();
  //SDL_Delay(20);

//}
     // SDL_MixAudio(stream, (unsigned char *)sdl_sound_buffer, len, volume);
   // } else {
   //   memset(sdl_sound_buffer, 0, len);
   // }
 // }
}

void 
Sound_Init(void)
{
  int i;
	
  chan[0].vol = 0;
  chan[1].vol = 0;
  chan[2].vol = 0;
  chan[3].vol = 0;
  chan[0].sinc = 0;
  chan[1].sinc = 0;
  chan[2].sinc = 0;
  chan[3].sinc = 0;
  for(i=0;i<NOISEBSIZE;i++)
  {
   NoiseGen<<=1;
      if(NoiseGen&0x80000000) NoiseGen^=0x08000001;
      noise[i]=(NoiseGen&1? 32767:-32767);
  }
  
  
}

void 
snd_Sound(int C, int F, int V)
{
  chan[C].vol = V;
  chan[C].spos = F>>1;
  

}


/** Play() ***************************************************/
/** Log and play sound of given frequency (Hz) and volume   **/
/** (0..255) via given channel (0..3).                      **/
/*************************************************************/
void Play(int C,int F,int V)
{
  /* Play actual sound */
  snd_Sound(C,F,V); 
  snd_Mixer();
 
}


int 
audio_align_samples(int given)
{
  int actual = 1;
  while (actual < given) {
    actual <<= 1;
  }
  return actual; // return the closest match as 2^n
}

int
psp_sdl_init_sound()
{
 /* wanted.freq     = 44100;
  wanted.format   = AUDIO_S16;
  wanted.channels = 2;
  wanted.samples  = audio_align_samples(wanted.freq / 60 );
  wanted.callback = snd_Mixer;
  wanted.userdata = NULL;

  /* Open the audio device, forcing the desired format */
 // if ( SDL_OpenAudio(&wanted, NULL) < 0 ) {
 //   fprintf(stderr, "Couldn't open audio: %s\n", SDL_GetError());
 //   return(0);
 // }

  //sdl_sound_buffer = malloc(sizeof(uint8_t) * 8192);
  //SDL_Delay(100);        // Give sound some time to init
//  SDL_PauseAudio(0);
sp.rate=44100;
	sp.channels=1;
	sp.buf_size=128;//sp.rate*sp.channels*2/100;
//	uint8_t *sbuf;
//	sbuf=malloc(sp.buf_size * sizeof(uint8_t));
    sp.callback=0;//snd_Mixer;
	sp.depth = 0;	/* 0 seems to mean 16 bits */
      sp.buf=(uint8_t*)sdl_sound_buffer;
 
	//sp.callback = 0;	/* not used for native games, AFAIK */
	 //NULL;//(uint8_t*)_8k8bpcm_data;
int	res = emuIfSoundInit(&sp);

  return(1);
}

void 
audio_pause(void)
{


 // SDL_PauseAudio(1);
}

void 
audio_resume(void)
{
  if (CV.cv_snd_enable) {
 //   SDL_PauseAudio(0);
  }
}

void 
audio_shutdown(void)
{
//  SDL_CloseAudio();
}
