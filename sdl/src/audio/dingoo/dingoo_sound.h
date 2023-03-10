#ifndef __dingoo_sound_h__
#define __dingoo_sound_h__

#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <libgame.h>
//#include <dingoo/ucos2.h>
//#include <dingoo/audio.h>
#include <ctype.h>

#define min(x, y) (x < y ? x : y)

#define TASK_START_PRIO       5
#define TASK_STK_SIZE_PLAY    1024   // Size of the Play task's stack (# of WORDs)
#define TASK_STK_SIZE_UPDATE  32768  // Size of the Update task's stack (# of WORDs)
#define NO_TASKS              2      // Number of tasks 

#define VOLUME_MAX 30
#define VOLUME_STEP 1

#define SOUNDBUFFERS 3

extern char _lastError[];

int dingooSoundInit(emu_sound_params_t *waveformat, uint32_t bufsize, int (*callback)(void *, uint32_t));
void dingooSoundClose();
void dingooSoundPause(bool b);
bool dingooSoundIsPaused();
void dingooSoundClearBuffers();
void dingooSoundVolumeIncrease();
void dingooSoundVolumeDecrease();
int8_t dingooSoundGetVolume();
void dingooSoundSetVolume(int8_t v);
void _dingooSoundPlay(void *none);
void _dingooSoundUpdate(void *none);
char* dingooSoundLastError();

#endif
