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

/* Allow access to a raw mixing buffer */

#include "SDL.h"
#include "SDL_audio.h"
#include "SDL_audio_c.h"
#include "SDL_audiomem.h"
#include "SDL_sysaudio.h"

#define _THIS SDL_AudioDevice *_this

static SDL_AudioDriver current_audio;
static SDL_AudioDevice *open_devices[16];

/* !!! FIXME: These are wordy and unlocalized... */
#define DEFAULT_OUTPUT_DEVNAME "System audio output device"
#define DEFAULT_INPUT_DEVNAME "System audio capture device"


/*
 * Not all of these will be compiled and linked in, but it's convenient
 *  to have a complete list here and saves yet-another block of #ifdefs...
 *  Please see bootstrap[], below, for the actual #ifdef mess.
 */
extern AudioBootStrap BSD_AUDIO_bootstrap;
extern AudioBootStrap DSP_bootstrap;
extern AudioBootStrap ALSA_bootstrap;
extern AudioBootStrap PULSEAUDIO_bootstrap;
extern AudioBootStrap QSAAUDIO_bootstrap;
extern AudioBootStrap SUNAUDIO_bootstrap;
extern AudioBootStrap ARTS_bootstrap;
extern AudioBootStrap ESD_bootstrap;
extern AudioBootStrap NAS_bootstrap;
extern AudioBootStrap XAUDIO2_bootstrap;
extern AudioBootStrap DSOUND_bootstrap;
extern AudioBootStrap WINMM_bootstrap;
extern AudioBootStrap PAUDIO_bootstrap;
extern AudioBootStrap BEOSAUDIO_bootstrap;
extern AudioBootStrap COREAUDIO_bootstrap;
extern AudioBootStrap SNDMGR_bootstrap;
extern AudioBootStrap DISKAUD_bootstrap;
extern AudioBootStrap DUMMYAUD_bootstrap;
extern AudioBootStrap DCAUD_bootstrap;
extern AudioBootStrap DART_bootstrap;
extern AudioBootStrap NDSAUD_bootstrap;
extern AudioBootStrap FUSIONSOUND_bootstrap;
extern AudioBootStrap ANDROIDAUD_bootstrap;
extern AudioBootStrap PSPAUD_bootstrap;
extern AudioBootStrap SNDIO_bootstrap;

/* Available audio drivers */
static const AudioBootStrap *const bootstrap[] = {
#if SDL_AUDIO_DRIVER_PULSEAUDIO
    &PULSEAUDIO_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_ALSA
    &ALSA_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_SNDIO
    &SNDIO_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_BSD
    &BSD_AUDIO_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_OSS
    &DSP_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_QSA
    &QSAAUDIO_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_SUNAUDIO
    &SUNAUDIO_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_ARTS
    &ARTS_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_ESD
    &ESD_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_NAS
    &NAS_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_XAUDIO2
    &XAUDIO2_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_DSOUND
    &DSOUND_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_WINMM
    &WINMM_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_PAUDIO
    &PAUDIO_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_BEOSAUDIO
    &BEOSAUDIO_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_COREAUDIO
    &COREAUDIO_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_DISK
    &DISKAUD_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_DUMMY
    &DUMMYAUD_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_FUSIONSOUND
    &FUSIONSOUND_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_ANDROID
    &ANDROIDAUD_bootstrap,
#endif
#if SDL_AUDIO_DRIVER_PSP
    &PSPAUD_bootstrap,
#endif
    NULL
};

static SDL_AudioDevice *
get_audio_device(SDL_AudioDeviceID id)
{
        return NULL;
    
}


/* stubs for audio drivers that don't need a specific entry point... */
static void
SDL_AudioDetectDevices_Default(int iscapture, SDL_AddAudioDevice addfn)
{                               /* no-op. */
}

static void
SDL_AudioThreadInit_Default(_THIS)
{                               /* no-op. */
}

static void
SDL_AudioWaitDevice_Default(_THIS)
{                               /* no-op. */
}

static void
SDL_AudioPlayDevice_Default(_THIS)
{                               /* no-op. */
}

static Uint8 *
SDL_AudioGetDeviceBuf_Default(_THIS)
{
    return NULL;
}

static void
SDL_AudioWaitDone_Default(_THIS)
{                               /* no-op. */
}

static void
SDL_AudioCloseDevice_Default(_THIS)
{                               /* no-op. */
}

static void
SDL_AudioDeinitialize_Default(void)
{                               /* no-op. */
}

static int
SDL_AudioOpenDevice_Default(_THIS, const char *devname, int iscapture)
{
    return -1;
}

static void
SDL_AudioLockDevice_Default(SDL_AudioDevice * device)
{
}

static void
SDL_AudioUnlockDevice_Default(SDL_AudioDevice * device)
{
}


static void
finalize_audio_entry_points(void)
{
}

/* Streaming functions (for when the input and output buffer sizes are different) */
/* Write [length] bytes from buf into the streamer */
static void
SDL_StreamWrite(SDL_AudioStreamer * stream, Uint8 * buf, int length)
{
}

/* Read [length] bytes out of the streamer into buf */
static void
SDL_StreamRead(SDL_AudioStreamer * stream, Uint8 * buf, int length)
{
}

static int
SDL_StreamLength(SDL_AudioStreamer * stream)
{
    return (stream->write_pos - stream->read_pos) % stream->max_len;
}

/* Initialize the stream by allocating the buffer and setting the read/write heads to the beginning */
#if 0
static int
SDL_StreamInit(SDL_AudioStreamer * stream, int max_len, Uint8 silence)
{
    return 0;
}
#endif

/* Deinitialize the stream simply by freeing the buffer */
static void
SDL_StreamDeinit(SDL_AudioStreamer * stream)
{
    SDL_free(stream->buffer);
}


/* The general mixing thread function */
int SDLCALL
SDL_RunAudio(void *devicep)
{
    return (0);
}


static SDL_AudioFormat
SDL_ParseAudioFormat(const char *string)
{
    return 0;
}

int
SDL_GetNumAudioDrivers(void)
{
    return (SDL_arraysize(bootstrap) - 1);
}

const char *
SDL_GetAudioDriver(int index)
{
    return (0);
}

int
SDL_AudioInit(const char *driver_name)
{

    return (0);
}

/*
 * Get the current audio driver name
 */
const char *
SDL_GetCurrentAudioDriver()
{
    return current_audio.name;
}

static void
free_device_list(char ***devices, int *devCount)
{
    }

static
void SDL_AddCaptureAudioDevice(const char *_name)
{
    }

static
void SDL_AddOutputAudioDevice(const char *_name)
{
    }


int
SDL_GetNumAudioDevices(int iscapture)
{
    
    return 0;
}


const char *
SDL_GetAudioDeviceName(int index, int iscapture)
{
    return NULL;
}


static void
close_audio_device(SDL_AudioDevice * device)
{
    }


/*
 * Sanity check desired AudioSpec for SDL_OpenAudio() in (orig).
 *  Fills in a sanitized copy in (prepared).
 *  Returns non-zero if okay, zero on fatal parameters in (orig).
 */
static int
prepare_audiospec(const SDL_AudioSpec * orig, SDL_AudioSpec * prepared)
{
    
    return 1;
}


static SDL_AudioDeviceID
open_audio_device(const char *devname, int iscapture,
                  const SDL_AudioSpec * desired, SDL_AudioSpec * obtained,
                  int allowed_changes, int min_id)
{
    return 1;
}


int
SDL_OpenAudio(SDL_AudioSpec * desired, SDL_AudioSpec * obtained)
{
    
    return (1);
}

SDL_AudioDeviceID
SDL_OpenAudioDevice(const char *device, int iscapture,
                    const SDL_AudioSpec * desired, SDL_AudioSpec * obtained,
                    int allowed_changes)
{
    return open_audio_device(device, iscapture, desired, obtained,
                             allowed_changes, 2);
}

SDL_AudioStatus
SDL_GetAudioDeviceStatus(SDL_AudioDeviceID devid)
{
}


SDL_AudioStatus
SDL_GetAudioStatus(void)
{
    return SDL_GetAudioDeviceStatus(1);
}

void
SDL_PauseAudioDevice(SDL_AudioDeviceID devid, int pause_on)
{
    
}

void
SDL_PauseAudio(int pause_on)
{
  
}


void
SDL_LockAudioDevice(SDL_AudioDeviceID devid)
{
  }

void
SDL_LockAudio(void)
{
    SDL_LockAudioDevice(1);
}

void
SDL_UnlockAudioDevice(SDL_AudioDeviceID devid)
{
  }

void
SDL_UnlockAudio(void)
{
    SDL_UnlockAudioDevice(1);
}

void
SDL_CloseAudioDevice(SDL_AudioDeviceID devid)
{
  
}

void
SDL_CloseAudio(void)
{
    SDL_CloseAudioDevice(1);
}

void
SDL_AudioQuit(void)
{
  }

#define NUM_FORMATS 10
static int format_idx;
static int format_idx_sub;
static SDL_AudioFormat format_list[NUM_FORMATS][NUM_FORMATS] = {
    {AUDIO_U8, AUDIO_S8, AUDIO_S16LSB, AUDIO_S16MSB, AUDIO_U16LSB,
     AUDIO_U16MSB, AUDIO_S32LSB, AUDIO_S32MSB, AUDIO_F32LSB, AUDIO_F32MSB},
    {AUDIO_S8, AUDIO_U8, AUDIO_S16LSB, AUDIO_S16MSB, AUDIO_U16LSB,
     AUDIO_U16MSB, AUDIO_S32LSB, AUDIO_S32MSB, AUDIO_F32LSB, AUDIO_F32MSB},
    {AUDIO_S16LSB, AUDIO_S16MSB, AUDIO_U16LSB, AUDIO_U16MSB, AUDIO_S32LSB,
     AUDIO_S32MSB, AUDIO_F32LSB, AUDIO_F32MSB, AUDIO_U8, AUDIO_S8},
    {AUDIO_S16MSB, AUDIO_S16LSB, AUDIO_U16MSB, AUDIO_U16LSB, AUDIO_S32MSB,
     AUDIO_S32LSB, AUDIO_F32MSB, AUDIO_F32LSB, AUDIO_U8, AUDIO_S8},
    {AUDIO_U16LSB, AUDIO_U16MSB, AUDIO_S16LSB, AUDIO_S16MSB, AUDIO_S32LSB,
     AUDIO_S32MSB, AUDIO_F32LSB, AUDIO_F32MSB, AUDIO_U8, AUDIO_S8},
    {AUDIO_U16MSB, AUDIO_U16LSB, AUDIO_S16MSB, AUDIO_S16LSB, AUDIO_S32MSB,
     AUDIO_S32LSB, AUDIO_F32MSB, AUDIO_F32LSB, AUDIO_U8, AUDIO_S8},
    {AUDIO_S32LSB, AUDIO_S32MSB, AUDIO_F32LSB, AUDIO_F32MSB, AUDIO_S16LSB,
     AUDIO_S16MSB, AUDIO_U16LSB, AUDIO_U16MSB, AUDIO_U8, AUDIO_S8},
    {AUDIO_S32MSB, AUDIO_S32LSB, AUDIO_F32MSB, AUDIO_F32LSB, AUDIO_S16MSB,
     AUDIO_S16LSB, AUDIO_U16MSB, AUDIO_U16LSB, AUDIO_U8, AUDIO_S8},
    {AUDIO_F32LSB, AUDIO_F32MSB, AUDIO_S32LSB, AUDIO_S32MSB, AUDIO_S16LSB,
     AUDIO_S16MSB, AUDIO_U16LSB, AUDIO_U16MSB, AUDIO_U8, AUDIO_S8},
    {AUDIO_F32MSB, AUDIO_F32LSB, AUDIO_S32MSB, AUDIO_S32LSB, AUDIO_S16MSB,
     AUDIO_S16LSB, AUDIO_U16MSB, AUDIO_U16LSB, AUDIO_U8, AUDIO_S8},
};

SDL_AudioFormat
SDL_FirstAudioFormat(SDL_AudioFormat format)
{
  }

SDL_AudioFormat
SDL_NextAudioFormat(void)
{
  
}

void
SDL_CalculateAudioSpec(SDL_AudioSpec * spec)
{
  }


/*
 * Moved here from SDL_mixer.c, since it relies on internals of an opened
 *  audio device (and is deprecated, by the way!).
 */
void
SDL_MixAudio(Uint8 * dst, const Uint8 * src, Uint32 len, int volume)
{
  
}

/* vi: set ts=4 sw=4 expandtab: */
