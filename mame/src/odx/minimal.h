/*

  GP2X minimal library v0.A by rlyeh, (c) 2005. emulnation.info@rlyeh (swap it!)

  Thanks to Squidge, Robster, snaff, Reesy and NK, for the help & previous work! :-)

  License
  =======

  Free for non-commercial projects (it would be nice receiving a mail from you).
  Other cases, ask me first.

  GamePark Holdings is not allowed to use this library and/or use parts from it.

*/
#ifndef __MINIMAL_H__
#define __MINIMAL_H__

#include <sys/time.h>
#include <unistd.h>
#include <stdarg.h>

#include <SDL/SDL.h>
#include <libgame.h>
#define ODX_SCREEN_WIDTH 480
#define ODX_SCREEN_HEIGHT 272

#define odx_video_color8(C,R,G,B)  (odx_palette_rgb[C] = ((((R)&0xF8)<<8)|(((G)&0xFC)<<3)|(((B)&0xF8)>>3)))
#define odx_video_color16(R,G,B,A) ((((R)&0xF8)<<8)|(((G)&0xFC)<<3)|(((B)&0xF8)>>3))
#define odx_video_getr16(C) (((C)>>8)&0xF8)
#define odx_video_getg16(C) (((C)>>3)&0xFC)
#define odx_video_getb16(C) (((C)<<3)&0xF8)
//1<<0
#include <libgame.h>
enum  { OD_UP=GE_KEY_UP,         OD_LEFT=GE_KEY_LEFT,       OD_DOWN=GE_KEY_DOWN,  OD_RIGHT=GE_KEY_RIGHT,
        OD_START=GE_KEY_START,  OD_SELECT=GE_KEY_SELECT,    OD_L=GE_KEY_L1,    OD_R=EMU_KEY_R,
        OD_A=GE_KEY_X,       OD_B=GE_KEY_O,        OD_X=GE_KEY_TRIANGLE,    OD_Y=GE_KEY_SQUARE ,

	/* Virtual keys * SELECT + 'button' */
        OD_SEL_START=GE_KEY_UP<<12,  OD_SEL_SELECT=GE_KEY_LEFT<<13,    OD_SEL_L=GE_KEY_L1<<14,    OD_SEL_R=1<<15,
        OD_SEL_A=1<<16,       OD_SEL_B=1<<17,        OD_SEL_X=1<<18,    OD_SEL_Y=1<<19
};

#define OD_KEY_MAX 16

//extern SDL_Surface 				*layer,*video;
extern SDL_Surface 				*video;
extern unsigned char			*od_screen8;
extern unsigned short			*od_screen16;

extern volatile unsigned short	odx_palette[512];
extern unsigned short			odx_palette_rgb[256];

extern int						odx_clock;

extern unsigned int				odx_sound_rate;
extern int						odx_sound_stereo;

extern int						rotate_controls;

#ifdef _GCW0_
extern SDL_Joystick				*odx_joyanalog;
#endif

//extern uint32_t nkeys;
//extern emu_keymap_t keymap;

extern void odx_video_flip(void);
extern void odx_video_flip_single(void);
extern void odx_video_wait_vsync(void);
extern void odx_video_setpalette(void);
//extern uint8_t *audiobuf;
extern unsigned int odx_joystick_read();
extern unsigned int odx_joystick_press();

extern void odx_sound_volume(int vol);
 extern void updateaudiostream(void);

extern void odx_timer_delay(unsigned int ticks);
extern unsigned long odx_timer_read(void);

extern void odx_sound_play(void *buff, int len);
extern void odx_sound_thread_start(void);
extern void odx_sound_thread_stop(void);

extern void odx_init(int ticks_per_second, int bpp, int rate, int bits, int stereo, int Hz);
extern void odx_deinit(void);

extern void odx_set_clock(int mhz);
extern void odx_set_video_mode(int bpp,int width,int height);
extern void odx_clear_video();

extern void odx_printf(char* fmt, ...);
extern void odx_printf_init(void);
extern void odx_gamelist_text_out(int x, int y, char *eltexto);
extern void odx_gamelist_text_out_fmt(int x, int y, char* fmt, ...);
 void odx_text(unsigned short *scr, int x, int y, char *text, int color);

extern int abs_x, abs_y, abs_z;

#define odx_video_wait_vsync()  { }
#define odx_video_setpalette()  { }

#endif
