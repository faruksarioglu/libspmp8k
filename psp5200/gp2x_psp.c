#include <stdio.h>
#include <stdlib.h>
#include <SDL.h>
#include <SDL_events.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>

#if defined(DINGUX_MODE)
#include <linux/soundcard.h>
#endif

#include "global.h"
#include "gp2x_psp.h"
#include "gp2x_cpu.h"

/* For internal use only */
# define GP2X_CTRL_UPRIGHT    0x10000
# define GP2X_CTRL_UPLEFT     0x20000
# define GP2X_CTRL_DOWNRIGHT  0x40000
# define GP2X_CTRL_DOWNLEFT   0x80000


static int    loc_Volume = 50;

static int    loc_LastEventMask    = 0;
static int    loc_CurrEventMask    = 0;
static int    loc_CurrEventButtons = 0;
static u32    loc_LastTimeStamp    = 0;
static u32    loc_CurrTimeStamp    = 0;

static int    loc_VolumeButtons = 0;
static int    loc_VolumePress   = 0;
static u32    loc_LastVolumeTimeStamp = 0;

# define GP2X_MIN_TIME_VOLUME  300000
# define GP2X_MIN_TIME_REPEAT  100000

static inline int
gp2xConvertMaskToButtons(int Mask)
{
  int keys = Mask & PSP_CTRL_MASK;
  return keys;
}

void
gp2xTreatVolume(ge_key_data_t* c)
{
  }




void
gp2xPowerSetClockFrequency(int freq)
{
#if defined(GP2X_MODE) || defined(WIZ_MODE) || defined(DINGUX_MODE)
  if ((freq >= GP2X_MIN_CLOCK) && (freq <= GP2X_MAX_CLOCK)) {
  //  cpu_set_clock(freq);
  }
# endif
}

#if defined(DINGUX_MODE)
# if 0
static int 
dingux_readvolume()
{
  char *mixer_device = "/dev/mixer";
  int mixer;
  int basevolume = 50;
  mixer = open(mixer_device, O_RDONLY);
  if (ioctl(mixer, SOUND_MIXER_READ_VOLUME, &basevolume) == -1) return -1;
  close(mixer);
  return  (basevolume>>8) & basevolume ;
}
# endif

static int 
dingux_setvolume(int involume)
{
  char *mixer_device = "/dev/mixer";
  int mixer;
  int newvolume = involume;
  if (newvolume > 100) newvolume = 100;
  if (newvolume < 0) newvolume = 0;
  int oss_volume = newvolume | (newvolume << 8); // set volume for both channels
  mixer = open(mixer_device, O_WRONLY);
  if (ioctl(mixer, SOUND_MIXER_WRITE_VOLUME, &oss_volume) == -1) return -1;
  close(mixer);
  return newvolume;
}
# endif

int 
gp2xInitSoundVolume()
{
# if defined(DINGUX_MODE)
  dingux_setvolume( loc_Volume );
# endif
}

int
gp2xGetSoundVolume()
{
  return loc_Volume;
}

void
gp2xDecreaseVolume()
{
  loc_Volume -= 2;
  if (loc_Volume < 0) loc_Volume = 0;
# if defined(DINGUX_MODE)
  dingux_setvolume( loc_Volume );
# endif
}

void
gp2xIncreaseVolume()
{
  loc_Volume += 2;
  if (loc_Volume > 100) loc_Volume = 100;
# if defined(DINGUX_MODE)
  dingux_setvolume( loc_Volume );
# endif
}

# if defined(GP2X_MODE)
int 
gp2xInsmodMMUhack(void)
{
# ifdef GP2X_MMU_HACK
	int mmufd = open("/dev/mmuhack", O_RDWR);
	if(mmufd < 0) {
		system("/sbin/insmod ./drivers/mmuhack.o");
		mmufd = open("/dev/mmuhack", O_RDWR);
	}
	if(mmufd < 0) return 0;
 	close(mmufd); 
# endif
	return 1;
}

int 
gp2xRmmodMMUhack(void)
{
# ifdef GP2X_MMU_HACK
  system("/sbin/rmmod mmuhack");
# endif
  return 0;
}  

# endif
