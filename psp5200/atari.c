/*
 * atari.c - main high-level routines
 *
 * Copyright (c) 1995-1998 David Firth
 * Copyright (c) 1998-2006 Atari800 development team (see DOC/CREDITS)
 *
 * This file is part of the Atari800 emulator project which emulates
 * the Atari 400, 800, 800XL, 130XE, and 5200 8-bit computers.
 *
 * Atari800 is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * Atari800 is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Atari800; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>

#include "global.h"

#include "config.h"

#include "antic.h"
#include "atari.h"
#include "binload.h"
#include "cartridge.h"
#include "cassette.h"
#include "cpu.h"
#include "devices.h"
#include "gtia.h"
#include "input.h"
#include "log.h"
#include "memory.h"
#include "monitor.h"
#include "pia.h"
#include "platform.h"
#include "pokey.h"
#include "rtime.h"
#include "sio.h"
#include "util.h"
#if !defined(BASIC) && !defined(CURSES_BASIC)
#include "colours.h"
#include "screen.h"
#endif
#ifndef BASIC
#include "statesav.h"
#ifndef __PLUS
#include "ui.h"
#endif
#endif /* BASIC */
#if defined(SOUND) && !defined(__PLUS)
#include "pokeysnd.h"
#include "sound.h"
#endif

#ifdef __PLUS
#ifdef _WX_
#include "export.h"
#else /* _WX_ */
#include "globals.h"
#include "macros.h"
#include "display_win.h"
#include "misc_win.h"
#include "registry.h"
#include "timing.h"
#include "FileService.h"
#include "Helpers.h"
#endif /* _WX_ */
#endif /* __PLUS */

//LUDO:
#include <zlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "global.h"
#include "psp_sdl.h"
#include "psp_fmgr.h"
#include "psp_kbd.h"
#include "ui1.h"
ATARI_t ATARI;

int machine_type = MACHINE_XLXE;
int ram_size = 16;
int tv_mode = TV_NTSC;
int disable_basic = TRUE;
int enable_sio_patch = TRUE;

int verbose = FALSE;

int display_screen = FALSE;
int nframes = 0;
int refresh_rate = 0;
int sprite_collisions_in_skipped_frames = FALSE;

static double frametime = 0.1;
double fps;
int percent_atari_speed = 120;//100;
#ifdef BENCHMARK
static double benchmark_start_time;
static double Atari_time(void);
#endif

char atari_osa_filename[FILENAME_MAX] = "/fat20a2/hda2/GAME/5200/ATARIOSA.ROM";
char atari_osb_filename[FILENAME_MAX] = "/fat20a2/hda2/GAME/5200/ATARIOSB.ROM";
char atari_xlxe_filename[FILENAME_MAX] = "/fat20a2/hda2/GAME/5200/ATARIXL.ROM";
char atari_5200_filename[FILENAME_MAX] = "/fat20a2/hda2/GAME/5200/ATARI5200.a52";
char atari_basic_filename[FILENAME_MAX] = "/fat20a2/hda2/GAME/5200/ATARIBAS.ROM";

int emuos_mode = 1;	/* 0 = never use EmuOS, 1 = use EmuOS if real OS not available, 2 = always use EmuOS */

#ifdef HAVE_SIGNAL
RETSIGTYPE sigint_handler(int num)
{
	int restart;

	restart = Atari800_Exit(TRUE);
	if (restart) {
		signal(SIGINT, sigint_handler);
		return;
	}

	exit(0);
}
#endif

/* Now we check address of every escape code, to make sure that the patch
   has been set by the emulator and is not a CIM in Atari program.
   Also switch() for escape codes has been changed to array of pointers
   to functions. This allows adding port-specific patches (e.g. modem device)
   using Atari800_AddEsc, Device_UpdateHATABSEntry etc. without modifying
   atari.c/devices.c. Unfortunately it can't be done for patches in Atari OS,
   because the OS in XL/XE can be disabled.
*/
static UWORD esc_address[256];
static EscFunctionType esc_function[256];

void Atari800_ClearAllEsc(void)
{
	int i;
	for (i = 0; i < 256; i++)
		esc_function[i] = NULL;
}

void Atari800_AddEsc(UWORD address, UBYTE esc_code, EscFunctionType function)
{
	esc_address[esc_code] = address;
	esc_function[esc_code] = function;
	dPutByte(address, 0xf2);			/* ESC */
	dPutByte(address + 1, esc_code);	/* ESC CODE */
}

void Atari800_AddEscRts(UWORD address, UBYTE esc_code, EscFunctionType function)
{
	esc_address[esc_code] = address;
	esc_function[esc_code] = function;
	dPutByte(address, 0xf2);			/* ESC */
	dPutByte(address + 1, esc_code);	/* ESC CODE */
	dPutByte(address + 2, 0x60);		/* RTS */
}

/* 0xd2 is ESCRTS, which works same as pair of ESC and RTS (I think so...).
   So this function does effectively the same as Atari800_AddEscRts,
   except that it modifies 2, not 3 bytes in Atari memory.
   I don't know why it is done that way, so I simply leave it
   unchanged (0xf2/0xd2 are used as in previous versions).
*/
void Atari800_AddEscRts2(UWORD address, UBYTE esc_code, EscFunctionType function)
{
	esc_address[esc_code] = address;
	esc_function[esc_code] = function;
	dPutByte(address, 0xd2);			/* ESCRTS */
	dPutByte(address + 1, esc_code);	/* ESC CODE */
}

void Atari800_RemoveEsc(UBYTE esc_code)
{
	esc_function[esc_code] = NULL;
}

void Atari800_RunEsc(UBYTE esc_code)
{
	if (esc_address[esc_code] == regPC - 2 && esc_function[esc_code] != NULL) {
		esc_function[esc_code]();
		return;
	}
#ifdef CRASH_MENU
	regPC -= 2;
	crash_address = regPC;
	crash_afterCIM = regPC + 2;
	crash_code = dGetByte(crash_address);
	ui();
#else /* CRASH_MENU */
	cim_encountered = 1;
	Aprint("Invalid ESC code %02x at address %04x", esc_code, regPC - 2);
#ifndef __PLUS
	if (!Atari800_Exit(TRUE))
		exit(0);
#else /* __PLUS */
	Atari800_Exit(TRUE);
#endif /* __PLUS */
#endif /* CRASH_MENU */
}

void Atari800_PatchOS(void)
{
	int patched = Device_PatchOS();
	if (enable_sio_patch) {
		UWORD addr_l;
		UWORD addr_s;
		UBYTE check_s_0;
		UBYTE check_s_1;
		/* patch Open() of C: so we know when a leader is processed */
		switch (machine_type) {
		case MACHINE_OSA:
		case MACHINE_OSB:
			addr_l = 0xef74;
			addr_s = 0xefbc;
			check_s_0 = 0xa0;
			check_s_1 = 0x80;
			break;
		case MACHINE_XLXE:
			addr_l = 0xfd13;
			addr_s = 0xfd60;
			check_s_0 = 0xa9;
			check_s_1 = 0x03;
			break;
		default:
			return;
		}
		/* don't hurt non-standard OSes that may not support cassette at all  */
		if (dGetByte(addr_l)     == 0xa9 && dGetByte(addr_l + 1) == 0x03
		 && dGetByte(addr_l + 2) == 0x8d && dGetByte(addr_l + 3) == 0x2a
		 && dGetByte(addr_l + 4) == 0x02
		 && dGetByte(addr_s)     == check_s_0
		 && dGetByte(addr_s + 1) == check_s_1
		 && dGetByte(addr_s + 2) == 0x20 && dGetByte(addr_s + 3) == 0x5c
		 && dGetByte(addr_s + 4) == 0xe4) {
			Atari800_AddEsc(addr_l, ESC_COPENLOAD, CASSETTE_LeaderLoad);
			Atari800_AddEsc(addr_s, ESC_COPENSAVE, CASSETTE_LeaderSave);
		}
		Atari800_AddEscRts(0xe459, ESC_SIOV, SIO);
		patched = TRUE;
	}
	else {
		Atari800_RemoveEsc(ESC_COPENLOAD);
		Atari800_RemoveEsc(ESC_COPENSAVE);
		Atari800_RemoveEsc(ESC_SIOV);
	};
	if (patched && machine_type == MACHINE_XLXE) {
		/* Disable Checksum Test */
		dPutByte(0xc314, 0x8e);
		dPutByte(0xc315, 0xff);
		dPutByte(0xc319, 0x8e);
		dPutByte(0xc31a, 0xff);
	}
}

void Warmstart(void)
{
	if (machine_type == MACHINE_OSA || machine_type == MACHINE_OSB) {
		/* RESET key in 400/800 does not reset chips,
		   but only generates RNMI interrupt */
		NMIST = 0x3f;
		NMI();
	}
	else {
		PIA_Reset();
		ANTIC_Reset();
		/* CPU_Reset() must be after PIA_Reset(),
		   because Reset routine vector must be read from OS ROM */
		CPU_Reset();
		/* note: POKEY and GTIA have no Reset pin */
	}
#ifdef __PLUS
	HandleResetEvent();
#endif
}

void Coldstart(void)
{
	PIA_Reset();
	ANTIC_Reset();
	/* CPU_Reset() must be after PIA_Reset(),
	   because Reset routine vector must be read from OS ROM */
	CPU_Reset();
	/* note: POKEY and GTIA have no Reset pin */
#ifdef __PLUS
	HandleResetEvent();
#endif
	/* reset cartridge to power-up state */
	CART_Start();
	/* set Atari OS Coldstart flag */
	dPutByte(0x244, 1);
	/* handle Option key (disable BASIC in XL/XE)
	   and Start key (boot from cassette) */
	consol_index = 2;
	consol_table[2] = 0x0f;
	if (disable_basic && !loading_basic) {
		/* hold Option during reboot */
		consol_table[2] &= ~CONSOL_OPTION;
	}
	if (hold_start) {
		/* hold Start during reboot */
		consol_table[2] &= ~CONSOL_START;
	}
	consol_table[1] = consol_table[2];
}

static int load_image(const char *filename, UBYTE *buffer, int nbytes)
{
	FILE *f;
	int len;

	f = fopen(filename, "rb");
	if (f == NULL) {
		Aprint("Error loading ROM image: %s", filename);
		return FALSE;
	}
	len = fread(buffer, 1, nbytes, f);
	fclose(f);
	if (len != nbytes) {
		Aprint("Error reading %s", filename);
		return FALSE;
	}
	return TRUE;
}

#include "emuos.h"

#define COPY_EMUOS(padding) do { \
		memset(atari_os, 0, padding); \
		memcpy(atari_os + (padding), emuos_h, 0x2000); \
	} while (0)

static int load_roms(void)
{
	switch (machine_type) {
	case MACHINE_OSA:
		if (emuos_mode == 2)
			COPY_EMUOS(0x0800);
		else if (!load_image("/fat20a2/hda2/GAME/5200/roms/ATARIOSA.rom", atari_os, 0x2800)) {
			if (emuos_mode == 1)
				COPY_EMUOS(0x0800);
			else
				return FALSE;
		}
		else
			have_basic = load_image("/fat20a2/hda2/GAME/5200/roms/ATARIBAS.rom", atari_basic, 0x2000);
		break;
	case MACHINE_OSB:
		if (emuos_mode == 2)
			COPY_EMUOS(0x0800);
		else if (!load_image("/fat20a2/hda2/GAME/5200/roms/ATARIOSB.rom", atari_os, 0x2800)) {
			if (emuos_mode == 1)
				COPY_EMUOS(0x0800);
			else
				return FALSE;
		}
		else
			have_basic = load_image("/fat20a2/hda2/GAME/5200/roms/ATARIBAS.rom", atari_basic, 0x2000);
		break;
	case MACHINE_XLXE:
		if (emuos_mode == 2)
			COPY_EMUOS(0x2000);
		else if (!load_image("/fat20a2/hda2/GAME/5200/roms/ATARIXL.rom", atari_os, 0x4000)) {
			if (emuos_mode == 1)
				COPY_EMUOS(0x2000);
			else
				return FALSE;
		}
		else if (!load_image("/fat20a2/hda2/GAME/5200/roms/ATARIBAS.rom", atari_basic, 0x2000))
			return FALSE;
		xe_bank = 0;
		break;
	case MACHINE_5200:
		if (!load_image("/fat20a2/hda2/GAME/5200/roms/ATARI5200.a52", atari_os, 0x800))
			return FALSE;//atari_5200_filename,
		break;
	}
	return TRUE;
}

int Atari800_InitialiseMachine(void)
{
	Atari800_ClearAllEsc();
	if (!load_roms()) return FALSE;
	MEMORY_InitialiseMachine();
	Device_UpdatePatches();
	
	return TRUE;
}

int Atari800_DetectFileType(const char *filename)
{
	UBYTE header[4];
	int file_length;
	FILE *fp = fopen(filename, "rb");
	if (fp == NULL)
		return AFILE_ERROR;
	if (fread(header, 1, 4, fp) != 4) {
		fclose(fp);
		return AFILE_ERROR;
	}
	switch (header[0]) {
	case 0:
		if (header[1] == 0 && (header[2] != 0 || header[3] != 0) /* && file_length < 37 * 1024 */) {
			fclose(fp);
			return AFILE_BAS;
		}
		break;
	case 0x1f:
		if (header[1] == 0x8b) {
#ifndef HAVE_LIBZ
			fclose(fp);
			Aprint("\"%s\" is a compressed file.", filename);
			Aprint("This executable does not support compressed files. You can uncompress this file");
			Aprint("with an external program that supports gzip (*.gz) files (e.g. gunzip)");
			Aprint("and then load into this emulator.");
			return AFILE_ERROR;
#else /* HAVE_LIBZ */
			gzFile gzf;
			fclose(fp);
			gzf = gzopen(filename, "rb");
			if (gzf == NULL)
				return AFILE_ERROR;
			if (gzread(gzf, header, 4) != 4) {
				gzclose(gzf);
				return AFILE_ERROR;
			}
			gzclose(gzf);
			if (header[0] == 0x96 && header[1] == 0x02)
				return AFILE_ATR_GZ;
			if (header[0] == 'A' && header[1] == 'T' && header[2] == 'A' && header[3] == 'R')
				return AFILE_STATE_GZ;
			return AFILE_XFD_GZ;
#endif /* HAVE_LIBZ */
		}
		break;
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		if ((header[1] >= '0' && header[1] <= '9') || header[1] == ' ') {
			fclose(fp);
			return AFILE_LST;
		}
		break;
	case 'A':
		if (header[1] == 'T' && header[2] == 'A' && header[3] == 'R') {
			fclose(fp);
			return AFILE_STATE;
		}
		break;
	case 'C':
		if (header[1] == 'A' && header[2] == 'R' && header[3] == 'T') {
			fclose(fp);
			return AFILE_CART;
		}
		break;
	case 'F':
		if (header[1] == 'U' && header[2] == 'J' && header[3] == 'I') {
			fclose(fp);
			return AFILE_CAS;
		}
		break;
	case 0x96:
		if (header[1] == 0x02) {
			fclose(fp);
			return AFILE_ATR;
		}
		break;
	case 0xf9:
	case 0xfa:
		fclose(fp);
		return AFILE_DCM;
	case 0xff:
		if (header[1] == 0xff && (header[2] != 0xff || header[3] != 0xff)) {
			fclose(fp);
			return AFILE_XEX;
		}
		break;
	default:
		break;
	}
	file_length = Util_flen(fp);
	fclose(fp);
	/* 40K or a-power-of-two between 4K and CART_MAX_SIZE */
	if (file_length >= 4 * 1024 && file_length <= CART_MAX_SIZE
	 && ((file_length & (file_length - 1)) == 0 || file_length == 40 * 1024))
		return AFILE_ROM;
	/* BOOT_TAPE is a raw file containing a program booted from a tape */
	if ((header[1] << 7) == file_length)
		return AFILE_BOOT_TAPE;
	if ((file_length & 0x7f) == 0)
		return AFILE_XFD;
	return AFILE_ERROR;
}

int Atari800_OpenFile(const char *filename, int reboot, int diskno, int readonly)
{
	int type = Atari800_DetectFileType(filename);
	switch (type) {
	case AFILE_ATR:
	case AFILE_XFD:
	case AFILE_ATR_GZ:
	case AFILE_XFD_GZ:
	case AFILE_DCM:
		if (!SIO_Mount(diskno, filename, readonly))
			return AFILE_ERROR;
		if (reboot)
			Coldstart();
		break;
	case AFILE_XEX:
	case AFILE_BAS:
	case AFILE_LST:
		if (!BIN_loader(filename))
			return AFILE_ERROR;
		break;
	case AFILE_CART:
	case AFILE_ROM:
		/* TODO: select format for ROMs; switch 5200 ? */
		if (CART_Insert(filename) != 0) {
# if 1 //LUDO:
      return AFILE_ERROR;
# else
      cart_type = CART_5200_32;
# endif
    }
		if (reboot)
			Coldstart();
		break;
	case AFILE_CAS:
	case AFILE_BOOT_TAPE:
		if (!CASSETTE_Insert(filename))
			return AFILE_ERROR;
		if (reboot) {
			hold_start = TRUE;
			Coldstart();
		}
		break;
	case AFILE_STATE:
	case AFILE_STATE_GZ:
#ifdef BASIC
		Aprint("State files are not supported in BASIC version");
		return AFILE_ERROR;
#else
		if (!ReadAtariState(filename, "rb"))
			return AFILE_ERROR;
		/* Don't press Option */
		consol_table[1] = consol_table[2] = 0xf;
		break;
#endif
	default:
		break;
	}
	return type;
}

#ifndef __PLUS

void Atari800_FindROMImages(const char *directory, int only_if_not_set)
{
	static char * const rom_filenames[5] = {
		atari_osa_filename,
		atari_osb_filename,
		atari_xlxe_filename,
		atari_5200_filename,
		atari_basic_filename
	};

strcpy(rom_filenames[0], "/fat20a2/hda2/GAME/5200/roms/ATARIOSA.ROM");
strcpy(rom_filenames[1], "/fat20a2/hda2/GAME/5200/roms/ATARIOSB.ROM");
strcpy(rom_filenames[2], "/fat20a2/hda2/GAME/5200/roms/ATARIXLXE.ROM");
strcpy(rom_filenames[3], "/fat20a2/hda2/GAME/5200/roms/ATARI5200.ROM");
strcpy(rom_filenames[4], "/fat20a2/hda2/GAME/5200/roms/ATARIBASIC.ROM");

}



/* If another default path config path is defined use it
   otherwise use the default one */
#ifndef DEFAULT_CFG_NAME
#define DEFAULT_CFG_NAME ".atari800.cfg"
#endif

static char rtconfig_filename[FILENAME_MAX];

int Atari800_ReadConfig(const char *alternate_config_filename)
{
	FILE *fp;
	const char *fname = rtconfig_filename;
	char string[256];
#ifndef BASIC
	int was_obsolete_dir = FALSE;
#endif

#ifdef SUPPORTS_ATARI_CONFIGINIT
	Atari_ConfigInit();
#endif

	/* if alternate config filename is passed then use it */
	if (alternate_config_filename != NULL && *alternate_config_filename > 0) {
		Util_strlcpy(rtconfig_filename, alternate_config_filename, FILENAME_MAX);
	}
	/* else use the default config name under the HOME folder */
	else {
		char *home = getenv("HOME");
		if (home != NULL)
			Util_catpath(rtconfig_filename, home, DEFAULT_CFG_NAME);
		else
			strcpy(rtconfig_filename, DEFAULT_CFG_NAME);
	}

	fp = fopen(fname, "r");
	if (fp == NULL) {
		Aprint("User config file '%s' not found.", rtconfig_filename);

#ifdef SYSTEM_WIDE_CFG_FILE
		/* try system wide config file */
		fname = SYSTEM_WIDE_CFG_FILE;
		Aprint("Trying system wide config file: %s", fname);
		fp = fopen(fname, "r");
#endif
		if (fp == NULL) {
			Aprint("No configuration file found, will create fresh one from scratch:");
			return FALSE;
		}
	}

	fgets(string, sizeof(string), fp);

	Aprint("Using Atari800 config file: %s\nCreated by %s", fname, string);

	while (fgets(string, sizeof(string), fp)) {
		char *ptr;
		Util_chomp(string);
		ptr = strchr(string, '=');
		if (ptr != NULL) {
			*ptr++ = '\0';
			Util_trim(string);
			Util_trim(ptr);

			if (strcmp(string, "OS/A_ROM") == 0)
				Util_strlcpy(atari_osa_filename, ptr, sizeof(atari_osa_filename));
			else if (strcmp(string, "OS/B_ROM") == 0)
				Util_strlcpy(atari_osb_filename, ptr, sizeof(atari_osb_filename));
			else if (strcmp(string, "XL/XE_ROM") == 0)
				Util_strlcpy(atari_xlxe_filename, ptr, sizeof(atari_xlxe_filename));
			else if (strcmp(string, "BASIC_ROM") == 0)
				Util_strlcpy(atari_basic_filename, ptr, sizeof(atari_basic_filename));
			else if (strcmp(string, "5200_ROM") == 0)
				Util_strlcpy(atari_5200_filename, ptr, sizeof(atari_5200_filename));
#ifdef BASIC
			else if (strcmp(string, "ATARI_FILES_DIR") == 0
				  || strcmp(string, "SAVED_FILES_DIR") == 0
				  || strcmp(string, "DISK_DIR") == 0 || strcmp(string, "ROM_DIR") == 0
				  || strcmp(string, "EXE_DIR") == 0 || strcmp(string, "STATE_DIR") == 0)
				/* do nothing */;
#else
			else if (strcmp(string, "ATARI_FILES_DIR") == 0) {
				if (n_atari_files_dir >= MAX_DIRECTORIES)
					Aprint("All ATARI_FILES_DIR slots used!");
				else
					Util_strlcpy(atari_files_dir[n_atari_files_dir++], ptr, FILENAME_MAX);
			}
			else if (strcmp(string, "SAVED_FILES_DIR") == 0) {
				if (n_saved_files_dir >= MAX_DIRECTORIES)
					Aprint("All SAVED_FILES_DIR slots used!");
				else
					Util_strlcpy(saved_files_dir[n_saved_files_dir++], ptr, FILENAME_MAX);
			}
			else if (strcmp(string, "DISK_DIR") == 0 || strcmp(string, "ROM_DIR") == 0
				  || strcmp(string, "EXE_DIR") == 0 || strcmp(string, "STATE_DIR") == 0) {
				/* ignore blank and "." values */
				if (ptr[0] != '\0' && (ptr[0] != '.' || ptr[1] != '\0'))
					was_obsolete_dir = TRUE;
			}
#endif
			else if (strcmp(string, "H1_DIR") == 0)
				Util_strlcpy(atari_h_dir[0], ptr, FILENAME_MAX);
			else if (strcmp(string, "H2_DIR") == 0)
				Util_strlcpy(atari_h_dir[1], ptr, FILENAME_MAX);
			else if (strcmp(string, "H3_DIR") == 0)
				Util_strlcpy(atari_h_dir[2], ptr, FILENAME_MAX);
			else if (strcmp(string, "H4_DIR") == 0)
				Util_strlcpy(atari_h_dir[3], ptr, FILENAME_MAX);
			else if (strcmp(string, "HD_READ_ONLY") == 0)
				h_read_only = Util_sscandec(ptr);

			else if (strcmp(string, "PRINT_COMMAND") == 0) {
				if (!Device_SetPrintCommand(ptr))
					Aprint("Unsafe PRINT_COMMAND ignored");
			}

			else if (strcmp(string, "SCREEN_REFRESH_RATIO") == 0)
				refresh_rate = Util_sscandec(ptr);
			else if (strcmp(string, "DISABLE_BASIC") == 0)
				disable_basic = Util_sscanbool(ptr);

			else if (strcmp(string, "ENABLE_SIO_PATCH") == 0) {
				enable_sio_patch = Util_sscanbool(ptr);
			}
			else if (strcmp(string, "ENABLE_H_PATCH") == 0) {
				enable_h_patch = Util_sscanbool(ptr);
			}
			else if (strcmp(string, "ENABLE_P_PATCH") == 0) {
				enable_p_patch = Util_sscanbool(ptr);
			}
			else if (strcmp(string, "ENABLE_R_PATCH") == 0) {
				enable_r_patch = Util_sscanbool(ptr);
			}

			else if (strcmp(string, "ENABLE_NEW_POKEY") == 0) {
#ifdef SOUND
				enable_new_pokey = Util_sscanbool(ptr);
#endif
			}
			else if (strcmp(string, "STEREO_POKEY") == 0) {
#ifdef STEREO_SOUND
				stereo_enabled = Util_sscanbool(ptr);
#endif
			}
			else if (strcmp(string, "SPEAKER_SOUND") == 0) {
#ifdef CONSOLE_SOUND
				console_sound_enabled = Util_sscanbool(ptr);
#endif
			}
			else if (strcmp(string, "SERIO_SOUND") == 0) {
#ifdef SERIO_SOUND
				serio_sound_enabled = Util_sscanbool(ptr);
#endif
			}
			else if (strcmp(string, "MACHINE_TYPE") == 0) {
				if (strcmp(ptr, "Atari OS/A") == 0)
					machine_type = MACHINE_OSA;
				else if (strcmp(ptr, "Atari OS/B") == 0)
					machine_type = MACHINE_OSB;
				else if (strcmp(ptr, "Atari XL/XE") == 0)
					machine_type = MACHINE_XLXE;
				else if (strcmp(ptr, "Atari 5200") == 0)
					machine_type = MACHINE_5200;
				else
					Aprint("Invalid machine type: %s", ptr);
			}
			else if (strcmp(string, "RAM_SIZE") == 0) {
				if (strcmp(ptr, "16") == 0)
					ram_size = 16;
				else if (strcmp(ptr, "48") == 0)
					ram_size = 48;
				else if (strcmp(ptr, "52") == 0)
					ram_size = 52;
				else if (strcmp(ptr, "64") == 0)
					ram_size = 64;
				else if (strcmp(ptr, "128") == 0)
					ram_size = 128;
				else if (strcmp(ptr, "192") == 0)
					ram_size = 192;
				else if (strcmp(ptr, "320 (RAMBO)") == 0)
					ram_size = RAM_320_RAMBO;
				else if (strcmp(ptr, "320 (COMPY SHOP)") == 0)
					ram_size = RAM_320_COMPY_SHOP;
				else if (strcmp(ptr, "576") == 0)
					ram_size = 576;
				else if (strcmp(ptr, "1088") == 0)
					ram_size = 1088;
				else
					Aprint("Invalid RAM size: %s", ptr);
			}
			else if (strcmp(string, "DEFAULT_TV_MODE") == 0) {
				if (strcmp(ptr, "PAL") == 0)
					tv_mode = TV_PAL;
				else if (strcmp(ptr, "NTSC") == 0)
					tv_mode = TV_NTSC;
				else
					Aprint("Invalid TV Mode: %s", ptr);
			}
			else {
				Aprint("Unrecognized variable: %s", string);
			}
		}
		else {
			Aprint("Ignored config line: %s", string);
		}
	}

	fclose(fp);
#ifndef BASIC
	if (was_obsolete_dir) {
		Aprint(
			"DISK_DIR, ROM_DIR, EXE_DIR and STATE_DIR configuration options\n"
			"are no longer supported. Please use ATARI_FILES_DIR\n"
			"and SAVED_FILES_DIR in your Atari800 configuration file.");
	}
#endif
	return TRUE;
}

int Atari800_WriteConfig(void)
{
	FILE *fp;
	int i;
	static const char * const machine_type_string[4] = {
		"OS/A", "OS/B", "XL/XE", "5200"
	};

	fp = fopen(rtconfig_filename, "w");
	if (fp == NULL) {
		perror(rtconfig_filename);
		Aprint("Cannot write to config file: %s", rtconfig_filename);
		return FALSE;
	}
	Aprint("Writing config file: %s", rtconfig_filename);

	fprintf(fp, "%s\n", ATARI_TITLE);
	fprintf(fp, "OS/A_ROM=%s\n", atari_osa_filename);
	fprintf(fp, "OS/B_ROM=%s\n", atari_osb_filename);
	fprintf(fp, "XL/XE_ROM=%s\n", atari_xlxe_filename);
	fprintf(fp, "BASIC_ROM=%s\n", atari_basic_filename);
	fprintf(fp, "5200_ROM=%s\n", atari_5200_filename);
#ifndef BASIC
	for (i = 0; i < n_atari_files_dir; i++)
		fprintf(fp, "ATARI_FILES_DIR=%s\n", atari_files_dir[i]);
	for (i = 0; i < n_saved_files_dir; i++)
		fprintf(fp, "SAVED_FILES_DIR=%s\n", saved_files_dir[i]);
#endif
	for (i = 0; i < 4; i++)
		fprintf(fp, "H%c_DIR=%s\n", '1' + i, atari_h_dir[i]);
	fprintf(fp, "HD_READ_ONLY=%d\n", h_read_only);

#ifdef HAVE_SYSTEM
	fprintf(fp, "PRINT_COMMAND=%s\n", print_command);
#endif

#ifndef BASIC
	fprintf(fp, "SCREEN_REFRESH_RATIO=%d\n", refresh_rate);
#endif

	fprintf(fp, "MACHINE_TYPE=Atari %s\n", machine_type_string[machine_type]);

	fprintf(fp, "RAM_SIZE=");
	switch (ram_size) {
	case RAM_320_RAMBO:
		fprintf(fp, "320 (RAMBO)\n");
		break;
	case RAM_320_COMPY_SHOP:
		fprintf(fp, "320 (COMPY SHOP)\n");
		break;
	default:
		fprintf(fp, "%d\n", ram_size);
		break;
	}

	fprintf(fp, (tv_mode == TV_PAL) ? "DEFAULT_TV_MODE=PAL\n" : "DEFAULT_TV_MODE=NTSC\n");

	fprintf(fp, "DISABLE_BASIC=%d\n", disable_basic);
	fprintf(fp, "ENABLE_SIO_PATCH=%d\n", enable_sio_patch);
	fprintf(fp, "ENABLE_H_PATCH=%d\n", enable_h_patch);
	fprintf(fp, "ENABLE_P_PATCH=%d\n", enable_p_patch);
#ifdef R_IO_DEVICE
	fprintf(fp, "ENABLE_R_PATCH=%d\n", enable_r_patch);
#endif

#ifdef SOUND
	fprintf(fp, "ENABLE_NEW_POKEY=%d\n", enable_new_pokey);
#ifdef STEREO_SOUND
	fprintf(fp, "STEREO_POKEY=%d\n", stereo_enabled);
#endif
#ifdef CONSOLE_SOUND
	fprintf(fp, "SPEAKER_SOUND=%d\n", console_sound_enabled);
#endif
#ifdef SERIO_SOUND
	fprintf(fp, "SERIO_SOUND=%d\n", serio_sound_enabled);
#endif
#endif /* SOUND */

#ifdef SUPPORTS_ATARI_CONFIGSAVE
	Atari_ConfigSave(fp);
#endif

	fclose(fp);
	return TRUE;
}

#endif /* __PLUS */

int Atari800_Initialise(int *argc, char *argv[])
{
	  int col = psp_sdl_rgb(0x0, 0x0, 0x0);

# if 0 //LUDO:
	int i, j;
# endif
	const char *rom_filename = NULL;
	const char *run_direct = NULL;
#ifndef BASIC
	const char *state_file = NULL;
#endif
	const char *rtconfig_filename = NULL;
	int got_config;
	int help_only = FALSE;
	memset(&ATARI, 0, sizeof(ATARI_t));

	sprintf(ATARI.atari_home_dir, "/fat20a2/hda2/GAME/5200/");
	atari_default_settings();
//	psp_joy_default_settings();
	psp_kbd_default_settings();

	psp_sdl_init();
psp_sdl_print(10, 10, "SDL_INIT OK", col);
  psp_sdl_flip();

	atari_update_save_name("");
	atari_load_settings();
	psp_sdl_print(10, 20, "lOAD sETTINGS OK", col);
  psp_sdl_flip();

	atari_kbd_load();
	psp_sdl_print(10, 30, "kBD lOAD OK", col);
  psp_sdl_flip();

	atari_joy_load();
	psp_sdl_print(10, 40, "JOY lOAD OK", col);
  psp_sdl_flip();

	//atari_load_cheat();
	got_config = 0;//Atari800_ReadConfig(rtconfig_filename);

	/* try to find ROM images if the configuration file is not found
	   or it does not specify some ROM paths (blank paths count as specified) */
	char atari800_exe_rom_dir[FILENAME_MAX];
	sprintf(atari800_exe_rom_dir, "/fat20a2/hda2/GAME/5200/roms/");
	Atari800_FindROMImages(atari800_exe_rom_dir, FALSE); /* current directory */
	psp_sdl_print(10, 50, "FIND ROM IMAGES OK", col);
  psp_sdl_flip();



#if !defined(BASIC) && !defined(CURSES_BASIC)
	Palette_Initialise(argc, argv);
#endif
	Device_Initialise(argc, argv);
	psp_sdl_print(10, 60, "DEVICE INIT OK", col);
  psp_sdl_flip();

	RTIME_Initialise(argc, argv);
	SIO_Initialise (argc, argv);
	CASSETTE_Initialise(argc, argv);
		psp_sdl_print(10, 70, "CASETTE INIT OK", col);
  psp_sdl_flip();

#ifndef BASIC
	INPUT_Initialise(argc, argv);
		psp_sdl_print(10, 80, "INPUT INIT OK", col);
  psp_sdl_flip();

#endif
#ifndef DONT_DISPLAY
	/* Platform Specific Initialisation */
	Atari_Initialise(argc, argv);
		psp_sdl_print(10, 90, "ATARI INITIALISE OK", col);
  psp_sdl_flip();

#endif
#if !defined(BASIC) && !defined(CURSES_BASIC)
	Screen_Initialise(argc, argv);
		psp_sdl_print(10, 100, "SCREEN INITIALISE OK", col);
  psp_sdl_flip();

#endif
	/* Initialise Custom Chips */
	ANTIC_Initialise(argc, argv);
	GTIA_Initialise(argc, argv);
	PIA_Initialise(argc, argv);
	POKEY_Initialise(argc, argv);
	psp_sdl_print(10, 110, "POKEY INIT OK", col);
  psp_sdl_flip();
  //SDL_Delay(10);

#ifndef __PLUS

	if (help_only) {
		Atari800_Exit(FALSE);
		return FALSE;
	}
	/* Configure Atari System */
	Atari800_InitialiseMachine();
  psp_sdl_print(10, 120, "atari800 machine Initialise OK", col);
  psp_sdl_flip();
  //SDL_Delay(20);

#else /* __PLUS */

	if (!InitialiseMachine()) {
#ifndef _WX_
		if (bUpdateRegistry)
			WriteAtari800Registry();
#endif
		return FALSE;
	}

#endif /* __PLUS */
	
# if 0 //LUDO:
	/* Auto-start files left on the command line */
rom_filename=(char*)select_file("/fat20a2/hda2/GAME/5200/","a52|atr|rom", 12);
		switch (Atari800_OpenFile(rom_filename, 1, 1, FALSE)) {
		case AFILE_ERROR:
			break;
		case AFILE_ATR:
		case AFILE_XFD:
		case AFILE_ATR_GZ:
		case AFILE_XFD_GZ:
		case AFILE_DCM:
			break;
		default:
			break;
		}
	
# endif

	/* Install requested ROM cartridge */
	 rom_filename=(char*)select_file("/fat20a2/hda2/GAME/5200/","a52|atr|rom", 12);//"/fat20a2/hda2/GAME/5200/galaxian.a52";
	if (rom_filename) {
		int r = CART_Insert(rom_filename);
		psp_sdl_print(10, 130, "cart insert OK", col);
  psp_sdl_flip();
  SDL_Delay(20);

		if (r < 0) {
			Aprint("Error inserting cartridge \"%s\": %s", rom_filename,
			r == CART_CANT_OPEN ? "Can't open file" :
			r == CART_BAD_FORMAT ? "Bad format" :
			r == CART_BAD_CHECKSUM ? "Bad checksum" :
			"Unknown error");
			psp_sdl_print(10, 140, "cart insert fail", col);
  psp_sdl_flip();
  //SDL_Delay(2000);
		}
		if (r > 0) {

			cart_type = select_cart_type(r);
			CART_Start();
		}
		if (cart_type != CART_NONE) {
			int for5200 = CART_IsFor5200(cart_type);
			if (for5200 && machine_type != MACHINE_5200) {
				machine_type = MACHINE_5200;
				ram_size = 16;
				Atari800_InitialiseMachine();
			}
			else if (!for5200 && machine_type == MACHINE_XLXE) {
				machine_type = MACHINE_XLXE;
				ram_size = 64;
				Atari800_InitialiseMachine();
			}
		}
	}

	/* Load Atari executable, if any */
	//	 run_direct=(char*)select_file(NULL,"a52|atr|rom", 12);//"/fat20a2/hda2/GAME/5200/galaxian.a52";

	if (run_direct != NULL){
		machine_type = MACHINE_5200;
		ram_size = 64;
		Atari800_InitialiseMachine();
		BIN_loader(run_direct);
	}

#ifndef BASIC
	/* Load state file */
	if (state_file != NULL) {
		if (ReadAtariState(state_file, "rb"))
			/* Don't press Option */
			consol_table[1] = consol_table[2] = 0x0f;
	}
#endif

#ifdef HAVE_SIGNAL
	/* Install CTRL-C Handler */
	signal(SIGINT, sigint_handler);
#endif

#ifdef __PLUS
#ifndef _WX_
	/* Update the Registry if any parameters were specified */
	if (bUpdateRegistry)
		WriteAtari800Registry();
	Timer_Start(FALSE);
#endif /* _WX_ */
	g_ulAtariState &= ~ATARI_UNINITIALIZED;
#endif /* __PLUS */

#ifdef BENCHMARK
	benchmark_start_time = Atari_time();
#endif
	return TRUE;
}

UNALIGNED_STAT_DEF(atari_screen_write_long_stat)
UNALIGNED_STAT_DEF(pm_scanline_read_long_stat)
UNALIGNED_STAT_DEF(memory_read_word_stat)
UNALIGNED_STAT_DEF(memory_write_word_stat)
UNALIGNED_STAT_DEF(memory_read_aligned_word_stat)
UNALIGNED_STAT_DEF(memory_write_aligned_word_stat)

int Atari800_Exit(int run_monitor)
{
	int restart;

#ifdef __PLUS
	if (cim_encountered)
		g_ulAtariState |= ATARI_CRASHED;
#endif

	if (verbose) {
		Aprint("Current frames per second: %f", fps);
	}
	restart = Atari_Exit(run_monitor);
#ifndef __PLUS
	if (!restart) {
	//	SIO_Exit();	/* umount disks, so temporary files are deleted */
	}
#endif /* __PLUS */
	return restart;
}

UBYTE Atari800_GetByte(UWORD addr)
{
	UBYTE byte = 0xff;
	switch (addr & 0xff00) {
	case 0x4f00:
	case 0x8f00:
		CART_BountyBob1(addr);
		byte = 0;
		break;
	case 0x5f00:
	case 0x9f00:
		CART_BountyBob2(addr);
		byte = 0;
		break;
	case 0xd000:				/* GTIA */
	case 0xc000:				/* GTIA - 5200 */
		byte = GTIA_GetByte(addr);
		break;
	case 0xd200:				/* POKEY */
	case 0xe800:				/* POKEY - 5200 */
	case 0xeb00:				/* POKEY - 5200 */
		byte = POKEY_GetByte(addr);
		break;
	case 0xd300:				/* PIA */
		byte = PIA_GetByte(addr);
		break;
	case 0xd400:				/* ANTIC */
		byte = ANTIC_GetByte(addr);
		break;
	case 0xd500:				/* bank-switching cartridges, RTIME-8 */
		byte = CART_GetByte(addr);
		break;
	default:
		break;
	}

	return byte;
}

void Atari800_PutByte(UWORD addr, UBYTE byte)
{
	switch (addr & 0xff00) {
	case 0x4f00:
	case 0x8f00:
		CART_BountyBob1(addr);
		break;
	case 0x5f00:
	case 0x9f00:
		CART_BountyBob2(addr);
		break;
	case 0xd000:				/* GTIA */
	case 0xc000:				/* GTIA - 5200 */
		GTIA_PutByte(addr, byte);
		break;
	case 0xd200:				/* POKEY */
	case 0xe800:				/* POKEY - 5200 AAA added other pokey space */
	case 0xeb00:				/* POKEY - 5200 */
		POKEY_PutByte(addr, byte);
		break;
	case 0xd300:				/* PIA */
		PIA_PutByte(addr, byte);
		break;
	case 0xd400:				/* ANTIC */
		ANTIC_PutByte(addr, byte);
		break;
	case 0xd500:				/* bank-switching cartridges, RTIME-8 */
		CART_PutByte(addr, byte);
		break;
	default:
		break;
	}
}

void Atari800_UpdatePatches(void)
{
	switch (machine_type) {
	case MACHINE_OSA:
	case MACHINE_OSB:
		/* Restore unpatched OS */
		dCopyToMem(atari_os, 0xd800, 0x2800);
		/* Set patches */
		Atari800_PatchOS();
		Device_UpdatePatches();
		break;
	case MACHINE_XLXE:
		/* Don't patch if OS disabled */
		if ((PORTB & 1) == 0)
			break;
		/* Restore unpatched OS */
		dCopyToMem(atari_os, 0xc000, 0x1000);
		dCopyToMem(atari_os + 0x1800, 0xd800, 0x2800);
		/* Set patches */
		Atari800_PatchOS();
		Device_UpdatePatches();
		break;
	default:
		break;
	}
}

static double Atari_time(void)
{
return libgame_utime()*1e-3;
}


static void Atari_sleep(double s)
{
	if (s > 0) {
		cyg_thread_delay(s);
	}
}

#ifdef USE_CURSES
void curses_clear_screen(void);
#endif

#if defined(BASIC) || defined(VERY_SLOW) || defined(CURSES_BASIC)

#ifdef CURSES_BASIC
void curses_display_line(int anticmode, const UBYTE *screendata);
#endif

static int scanlines_to_dl;

/* steal cycles and generate DLI */
static void basic_antic_scanline(void)
{
	static UBYTE IR = 0;
	static const UBYTE mode_scanlines[16] =
		{ 0, 0, 8, 10, 8, 16, 8, 16, 8, 4, 4, 2, 1, 2, 1, 1 };
	static const UBYTE mode_bytes[16] =
		{ 0, 0, 40, 40, 40, 40, 20, 20, 10, 10, 20, 20, 20, 40, 40, 40 };
	static const UBYTE font40_cycles[4] = { 0, 32, 40, 47 };
	static const UBYTE font20_cycles[4] = { 0, 16, 20, 24 };
#ifdef CURSES_BASIC
	static int scanlines_to_curses_display = 0;
	static UWORD screenaddr = 0;
	static UWORD newscreenaddr = 0;
#endif

	int bytes = 0;
	if (--scanlines_to_dl <= 0) {
		if (DMACTL & 0x20) {
			IR = ANTIC_GetDLByte(&dlist);
			xpos++;
		}
		else
			IR &= 0x7f;	/* repeat last instruction, but don't generate DLI */
		switch (IR & 0xf) {
		case 0:
			scanlines_to_dl = ((IR >> 4) & 7) + 1;
			break;
		case 1:
			if (DMACTL & 0x20) {
				dlist = ANTIC_GetDLWord(&dlist);
				xpos += 2;
			}
			scanlines_to_dl = (IR & 0x40) ? 1024 /* no more DL in this frame */ : 1;
			break;
		default:
			if (IR & 0x40 && DMACTL & 0x20) {
#ifdef CURSES_BASIC
				screenaddr =
#endif
					ANTIC_GetDLWord(&dlist);
				xpos += 2;
			}
			/* can't steal cycles now, because DLI must come first */
			/* just an approximation: doesn't check HSCROL */
			switch (DMACTL & 3) {
			case 1:
				bytes = mode_bytes[IR & 0xf] * 8 / 10;
				break;
			case 2:
				bytes = mode_bytes[IR & 0xf];
				break;
			case 3:
				bytes = mode_bytes[IR & 0xf] * 12 / 10;
				break;
			default:
				break;
			}
#ifdef CURSES_BASIC
			/* Normally, we would call curses_display_line here,
			   and not use scanlines_to_curses_display at all.
			   That would however cause incorrect color of the "MEMORY"
			   menu item in Self Test - it isn't set properly
			   in the first scanline. We therefore postpone
			   curses_display_line call to the next scanline. */
			scanlines_to_curses_display = 2;
			newscreenaddr = screenaddr + bytes;
#endif
			/* just an approximation: doesn't check VSCROL */
			scanlines_to_dl = mode_scanlines[IR & 0xf];
			break;
		}
	}
	if (scanlines_to_dl == 1 && (IR & 0x80)) {
		GO(NMIST_C);
		NMIST = 0x9f;
		if (NMIEN & 0x80) {
			GO(NMI_C);
			NMI();
		}
	}
#ifdef CURSES_BASIC
	if (--scanlines_to_curses_display == 0) {
		curses_display_line(IR & 0xf, memory + screenaddr);
		/* 4k wrap */
		if (((screenaddr ^ newscreenaddr) & 0x1000) != 0)
			screenaddr = newscreenaddr - 0x1000;
		else
			screenaddr = newscreenaddr;
	}
#endif
	xpos += bytes;
	/* steal cycles in font modes */
	switch (IR & 0xf) {
	case 2:
	case 3:
	case 4:
	case 5:
		xpos += font40_cycles[DMACTL & 3];
		break;
	case 6:
	case 7:
		xpos += font20_cycles[DMACTL & 3];
		break;
	default:
		break;
	}
}

#define BASIC_LINE GO(LINE_C); xpos -= LINE_C - DMAR; screenline_cpu_clock += LINE_C; ypos++

static void basic_frame(void)
{
	/* scanlines 0 - 7 */
	ypos = 0;
	do {
		POKEY_Scanline();		/* check and generate IRQ */
		BASIC_LINE;
	} while (ypos < 8);

	scanlines_to_dl = 1;
	/* scanlines 8 - 247 */
	do {
		POKEY_Scanline();		/* check and generate IRQ */
		basic_antic_scanline();
		BASIC_LINE;
	} while (ypos < 248);

	/* scanline 248 */
	POKEY_Scanline();			/* check and generate IRQ */
	GO(NMIST_C);
	NMIST = 0x5f;				/* Set VBLANK */
	if (NMIEN & 0x40) {
		GO(NMI_C);
		NMI();
	}
	BASIC_LINE;

	/* scanlines 249 - 261(311) */
	do {
		POKEY_Scanline();		/* check and generate IRQ */
		BASIC_LINE;
	} while (ypos < max_ypos);
}

#endif /* defined(BASIC) || defined(VERY_SLOW) || defined(CURSES_BASIC) */

void 
Atari800_Frame(void)
{
	static int refresh_counter = 0;
//	ge_key_data_t keys;
/*	if (c.keys & GE_KEY_DOWN) key_code=AKEY_5200_0;
	if (c.keys & GE_KEY_UP) key_code=AKEY_5200_1;
	if (c.keys & GE_KEY_LEFT) key_code=AKEY_5200_2;
	if (c.keys & GE_KEY_RIGHT) key_code=AKEY_5200_3;
	if (c.keys & GE_KEY_X) key_code=AKEY_5200_4;
	if (c.keys & GE_KEY_O) key_code=AKEY_5200_5;
	if (c.keys & GE_KEY_START) key_code=AKEY_5200_START;
	if (c.keys & GE_KEY_SELECT) key_code=AKEY_5200_HASH;
*/	
//NativeGE_getKeyInput4Ntv(&keys);
   //     if (keys.keys & GE_KEY_DOWN) {
     //   	key_code=AKEY_5200_1;
	//	ret_key_code=ATARIK_DOWN;
       	//	}
    
	// if (keys.keys & GE_KEY_UP) {
      //  key_code=AKEY_5200_0;
	//		ret_key_code=ATARIK_DOWN;
       	//	}
    // if (keys.keys & GE_KEY_LEFT) {
      //  	key_code=AKEY_5200_3;
      // 		ret_key_code=ATARIK_LEFT;
       	//
		 //  	}
    //// if (keys.keys & GE_KEY_RIGHT) {
        //	key_code=ATARIK_F2;
       	//ret_key_code=ATARIK_RIGHT;
      // 			}
   //  if (keys.keys & GE_KEY_X) {
     //   	key_code=ATARIK_F4;
       	//	ret_key_code=ATARIK_RETURN;
	//	   }
    // if (keys.keys & GE_KEY_START) {
      //  	key_code=AKEY_5200_START;
       	//	}
       	//	if (keys.keys & GE_KEY_O) {
        //	key_code=AKEY_EXIT;
       	//	}
   	switch (key_code) {
	case AKEY_COLDSTART:
		Coldstart();
		break;
	case AKEY_WARMSTART:
		Warmstart();
		break;
	case AKEY_EXIT:
		Atari800_Exit(FALSE);
		exit(0);
	case AKEY_UI:
		ui();
		break;
	default:
		break;
	}

Device_Frame();


GTIA_Frame();
INPUT_Frame();
Sound_Update();
//#ifdef BASIC
//	basic_frame();
//#else /* BASIC */
	if (++refresh_counter >= refresh_rate) {
		refresh_counter = 0;
    	ANTIC_Frame(TRUE);
	//	INPUT_DrawMousePointer();
		Screen_DrawAtariSpeed();
	//	Screen_DrawDiskLED();
//#endif /* CURSES_BASIC */
		display_screen = TRUE;
	}
	else {
		ANTIC_Frame(sprite_collisions_in_skipped_frames);
		display_screen = FALSE;
	}
//#endif /* BASIC */
	POKEY_Frame();
}

#ifndef BASIC

void MainStateSave(void)
{
	UBYTE temp;
	int default_tv_mode;
	int os = 0;
	int default_system = 3;
	int pil_on = FALSE;

	if (tv_mode == TV_PAL) {
		temp = 0;
		default_tv_mode = 1;
	}
	else {
		temp = 1;
		default_tv_mode = 2;
	}
	SaveUBYTE(&temp, 1);

	switch (machine_type) {
	case MACHINE_OSA:
		temp = ram_size == 16 ? 5 : 0;
		os = 1;
		default_system = 1;
		break;
	case MACHINE_OSB:
		temp = ram_size == 16 ? 5 : 0;
		os = 2;
		default_system = 2;
		break;
	case MACHINE_XLXE:
		switch (ram_size) {
		case 16:
			temp = 6;
			default_system = 3;
			break;
		case 64:
			temp = 1;
			default_system = 3;
			break;
		case 128:
			temp = 2;
			default_system = 4;
			break;
		case 192:
			temp = 9;
			default_system = 8;
			break;
		case RAM_320_RAMBO:
		case RAM_320_COMPY_SHOP:
			temp = 3;
			default_system = 5;
			break;
		case 576:
			temp = 7;
			default_system = 6;
			break;
		case 1088:
			temp = 8;
			default_system = 7;
			break;
		}
		break;
	case MACHINE_5200:
		temp = 4;
		default_system = 6;
		break;
	}
	SaveUBYTE(&temp, 1);

	SaveINT(&os, 1);
	SaveINT(&pil_on, 1);
	SaveINT(&default_tv_mode, 1);
	SaveINT(&default_system, 1);
}

void MainStateRead(void)
{
	/* these are all for compatibility with previous versions */
	UBYTE temp;
	int default_tv_mode;
	int os;
	int default_system;
	int pil_on;

	ReadUBYTE(&temp, 1);
	int new_tv_mode = (temp == 0) ? TV_PAL : TV_NTSC;
  if (new_tv_mode != tv_mode) {
    tv_mode = new_tv_mode;
    Palette_SetVideoSystem(tv_mode);
    CalcPalette();
    SetPalette();
  }

	ReadUBYTE(&temp, 1);
	ReadINT(&os, 1);
	switch (temp) {
	case 0:
		machine_type = os == 1 ? MACHINE_OSA : MACHINE_OSB;
		ram_size = 48;
		break;
	case 1:
		machine_type = MACHINE_XLXE;
		ram_size = 64;
		break;
	case 2:
		machine_type = MACHINE_XLXE;
		ram_size = 128;
		break;
	case 3:
		machine_type = MACHINE_XLXE;
		ram_size = RAM_320_COMPY_SHOP;
		break;
	case 4:
		machine_type = MACHINE_5200;
		ram_size = 16;
		break;
	case 5:
		machine_type = os == 1 ? MACHINE_OSA : MACHINE_OSB;
		ram_size = 16;
		break;
	case 6:
		machine_type = MACHINE_XLXE;
		ram_size = 16;
		break;
	case 7:
		machine_type = MACHINE_XLXE;
		ram_size = 576;
		break;
	case 8:
		machine_type = MACHINE_XLXE;
		ram_size = 1088;
		break;
	case 9:
		machine_type = MACHINE_XLXE;
		ram_size = 192;
		break;
	default:
		machine_type = MACHINE_XLXE;
		ram_size = 64;
		Aprint("Warning: Bad machine type read in from state save, defaulting to 800 XL");
		break;
	}

	ReadINT(&pil_on, 1);
	ReadINT(&default_tv_mode, 1);
	ReadINT(&default_system, 1);
	load_roms();
	/* XXX: what about patches? */
}

#endif

void
myPowerSetClockFrequency(int a_clock)
{
}

//LUDO:
void
atari_default_settings()
{
  //LUDO:
  ATARI.atari_snd_enable    = 1;
  ATARI.atari_render_mode   = ATARI_RENDER_NORMAL;
  ATARI.atari_vsync = 1;
  ATARI.danzeff_trans = 1;
  ATARI.atari_speed_limiter = 200;
  ATARI.psp_skip_cur_frame  = 0;
  ATARI.psp_skip_max_frame  = 3;
  ATARI.psp_cpu_clock       = 270;
  ATARI.psp_screenshot_id   = 0;
  ATARI.atari_view_fps      = 1;

  //myPowerSetClockFrequency(ATARI.psp_cpu_clock);
}

static int
loc_atari_save_settings(char *chFileName)
{
  FILE* FileDesc;
  int   error = 0;

  FileDesc = fopen(chFileName, "w");
  if (FileDesc != (FILE *)0 ) {

    fprintf(FileDesc, "psp_cpu_clock=%d\n"      , ATARI.psp_cpu_clock);
    fprintf(FileDesc, "psp_skip_max_frame=%d\n" , ATARI.psp_skip_max_frame);
    fprintf(FileDesc, "atari_snd_enable=%d\n"   , ATARI.atari_snd_enable);
    fprintf(FileDesc, "atari_render_mode=%d\n"  , ATARI.atari_render_mode);
    fprintf(FileDesc, "atari_vsync=%d\n"        , ATARI.atari_vsync);
    fprintf(FileDesc, "danzeff_trans=%d\n"      , ATARI.danzeff_trans);
    fprintf(FileDesc, "atari_speed_limiter=%d\n", ATARI.atari_speed_limiter);
    fprintf(FileDesc, "atari_view_fps=%d\n"     , ATARI.atari_view_fps);

    fclose(FileDesc);

  } else {
    error = 1;
  }

  return error;
}

int
atari_save_settings(void)
{
  char  FileName[MAX_PATH+1];
  int   error;

  error = 1;

  snprintf(FileName, MAX_PATH, "%s/set/%s.set", ATARI.atari_home_dir, ATARI.atari_save_name);
  error = loc_atari_save_settings(FileName);

  return error;
}

static int
loc_atari_load_settings(char *chFileName)
{
  char  Buffer[512];
  char *Scan;
  unsigned int Value;
  FILE* FileDesc;

  FileDesc = fopen(chFileName, "r");
  if (FileDesc == (FILE *)0 ) return 0;

  while (fgets(Buffer,512, FileDesc) != (char *)0) {

    Scan = strchr(Buffer,'\n');
    if (Scan) *Scan = '\0';
    /* For this #@$% of windows ! */
    Scan = strchr(Buffer,'\r');
    if (Scan) *Scan = '\0';
    if (Buffer[0] == '#') continue;

    Scan = strchr(Buffer,'=');
    if (! Scan) continue;

    *Scan = '\0';
    Value = atoi(Scan+1);

    if (!strcasecmp(Buffer,"psp_cpu_clock"))      ATARI.psp_cpu_clock = Value;
    else
    if (!strcasecmp(Buffer,"psp_skip_max_frame")) ATARI.psp_skip_max_frame = Value;
    else
    if (!strcasecmp(Buffer,"atari_snd_enable"))   ATARI.atari_snd_enable = Value;
    else
    if (!strcasecmp(Buffer,"atari_render_mode"))  ATARI.atari_render_mode = Value;
    else
    if (!strcasecmp(Buffer,"atari_vsync")) ATARI.atari_vsync = Value;
    else
    if (!strcasecmp(Buffer,"danzeff_trans")) ATARI.danzeff_trans = Value;
    else
    if (!strcasecmp(Buffer,"atari_speed_limiter"))  ATARI.atari_speed_limiter = Value;
    else
    if (!strcasecmp(Buffer,"atari_view_fps"))     ATARI.atari_view_fps = Value;
  }

  fclose(FileDesc);

  myPowerSetClockFrequency(ATARI.psp_cpu_clock);

  return 0;
}

int
atari_load_settings()
{
  char  FileName[MAX_PATH+1];
  int   error;

  error = 1;

  snprintf(FileName, MAX_PATH, "%s/set/%s.set", ATARI.atari_home_dir, ATARI.atari_save_name);
  error = loc_atari_load_settings(FileName);

  return error;
}

int
atari_load_file_settings(char *FileName)
{
  return loc_atari_load_settings(FileName);
}

static int 
loc_load_rom(char *TmpName)
{
  CART_Remove();

	machine_type = MACHINE_XLXE;
	ram_size = 64;
  Atari800_InitialiseMachine();

  int error = (Atari800_OpenFile(TmpName, 1, 1, FALSE) == AFILE_ERROR);
  return error;
}

static int 
loc_load_cart(char *TmpName)
{
  int ret = CART_Insert(TmpName);
  if (ret >= 0) {
    if (ret != 0) cart_type = CART_5200_32;
	  machine_type = MACHINE_5200;
	  ram_size = 16;
		Atari800_InitialiseMachine();
	  Coldstart();
  }
  return ret < 0;
}

void
atari_update_save_name(char *Name)
{
  char        TmpFileName[MAX_PATH];
  struct stat aStat;
  int         index;
  char       *SaveName;
  char       *Scan1;
  char       *Scan2;

  SaveName = strrchr(Name,'/');
  if (SaveName != (char *)0) SaveName++;
  else                       SaveName = Name;

  if (!strncasecmp(SaveName, "sav_", 4)) {
    Scan1 = SaveName + 4;
    Scan2 = strrchr(Scan1, '_');
    if (Scan2 && (Scan2[1] >= '0') && (Scan2[1] <= '5')) {
      strncpy(ATARI.atari_save_name, Scan1, MAX_PATH);
      ATARI.atari_save_name[Scan2 - Scan1] = '\0';
    } else {
      strncpy(ATARI.atari_save_name, SaveName, MAX_PATH);
    }
  } else {
    strncpy(ATARI.atari_save_name, SaveName, MAX_PATH);
  }

  if (ATARI.atari_save_name[0] == '\0') {
    strcpy(ATARI.atari_save_name,"default");
  }

  for (index = 0; index < ATARI_MAX_SAVE_STATE; index++) {
    ATARI.atari_save_state[index].used  = 0;
    memset(&ATARI.atari_save_state[index].date, 0, sizeof(time_t));
    ATARI.atari_save_state[index].thumb = 0;

    snprintf(TmpFileName, MAX_PATH, "%s/save/sav_%s_%d.sta", ATARI.atari_home_dir, ATARI.atari_save_name, index);
    if (! stat(TmpFileName, &aStat)) {
      ATARI.atari_save_state[index].used = 1;
      ATARI.atari_save_state[index].date = aStat.st_mtime;
      snprintf(TmpFileName, MAX_PATH, "%s/save/sav_%s_%d.png", ATARI.atari_home_dir, ATARI.atari_save_name, index);
      if (! stat(TmpFileName, &aStat)) {
        if (psp_sdl_load_thumb_png(ATARI.atari_save_state[index].surface, TmpFileName)) {
          ATARI.atari_save_state[index].thumb = 1;
        }
      }
    }
  }

  ATARI.comment_present = 0;
  snprintf(TmpFileName, MAX_PATH, "%s/txt/%s.txt", ATARI.atari_home_dir, ATARI.atari_save_name);
  if (! stat(TmpFileName, &aStat)) {
    ATARI.comment_present = 1;
  }
}

void
reset_save_name()
{
  atari_update_save_name("");
}

typedef struct thumb_list {
  struct thumb_list *next;
  char              *name;
  char              *thumb;
} thumb_list;

static thumb_list* loc_head_thumb = 0;

static void
loc_del_thumb_list()
{
  while (loc_head_thumb != 0) {
    thumb_list *del_elem = loc_head_thumb;
    loc_head_thumb = loc_head_thumb->next;
    if (del_elem->name) free( del_elem->name );
    if (del_elem->thumb) free( del_elem->thumb );
    free(del_elem);
  }
}

static void
loc_add_thumb_list(char* filename)
{
  thumb_list *new_elem;
  char tmp_filename[MAX_PATH];

  strcpy(tmp_filename, filename);
  char* save_name = tmp_filename;

  /* .png extention */
  char* Scan = strrchr(save_name, '.');
  if ((! Scan) || (strcasecmp(Scan, ".png"))) return;
  *Scan = 0;

  if (strncasecmp(save_name, "sav_", 4)) return;
  save_name += 4;

  Scan = strrchr(save_name, '_');
  if (! Scan) return;
  *Scan = 0;

  /* only one png for a give save name */
  new_elem = loc_head_thumb;
  while (new_elem != 0) {
    if (! strcasecmp(new_elem->name, save_name)) return;
    new_elem = new_elem->next;
  }

  new_elem = (thumb_list *)malloc( sizeof( thumb_list ) );
  new_elem->next = loc_head_thumb;
  loc_head_thumb = new_elem;
  new_elem->name  = strdup( save_name );
  new_elem->thumb = strdup( filename );
}

void
load_thumb_list()
{
 
}

int
load_thumb_if_exists(char *Name)
{
  return 0;
}

typedef struct comment_list {
  struct comment_list *next;
  char              *name;
  char              *filename;
} comment_list;

static comment_list* loc_head_comment = 0;

static void
loc_del_comment_list()
{
}

static void
loc_add_comment_list(char* filename)
{
}

void
load_comment_list()
{
}

char*
load_comment_if_exists(char *Name)
{
  return 0;
}

void
atari_kbd_load(void)
{
	  return (0);

}

int
atari_kbd_save(void)
{
	  return (0);

}

void
atari_joy_load(void)
{
	  return (0);

}

int
atari_joy_save(void)
{
  return (0);
}

void
emulator_reset(void)
{
  Coldstart();
}

int
atari_load_rom(char *FileName, int zip_format)
{
  char  SaveName[MAX_PATH+1];
  char*  ExtractName;
  char*  scan;
  int    format;
  int    error;
  size_t unzipped_size;

  error = 1;

  if (zip_format) {

    ExtractName = find_possible_filename_in_zip( FileName, "rom.bin.atr");
    if (ExtractName) {
      strncpy(SaveName, FileName, MAX_PATH);
      scan = strrchr(SaveName,'.');
      if (scan) *scan = '\0';
      atari_update_save_name(SaveName);
      const char* rom_buffer = extract_file_in_memory( FileName, ExtractName, &unzipped_size);
      if (rom_buffer) {
        snprintf(SaveName, MAX_PATH, "roms/%s", ExtractName);
        FILE* extracted_fd = fopen(SaveName, "w");
        if (extracted_fd) {
          fwrite( rom_buffer, unzipped_size, 1, extracted_fd);
          fclose(extracted_fd);
          free(rom_buffer);
          error = loc_load_rom( SaveName );
          unlink( ExtractName );
        }
      }
    }

  } else {
    strncpy(SaveName,FileName,MAX_PATH);
    scan = strrchr(SaveName,'.');
    if (scan) *scan = '\0';
    atari_update_save_name(SaveName);
    error = loc_load_rom(FileName);
  }

  if (! error ) {
    atari_kbd_load();
    atari_joy_load();
    atari_load_cheat();
    atari_load_settings();
  }

  return error;
}

int
atari_load_cart(char *FileName, int zip_format)
{
  char  SaveName[MAX_PATH+1];
  char*  ExtractName;
  char*  scan;
  int    format;
  int    error;
  size_t unzipped_size;

  error = 1;

  if (zip_format) {

    ExtractName = find_possible_filename_in_zip( FileName, ".a52");
    if (ExtractName) {
      strncpy(SaveName, FileName, MAX_PATH);
      scan = strrchr(SaveName,'.');
      if (scan) *scan = '\0';
      atari_update_save_name(SaveName);
      const char* rom_buffer = extract_file_in_memory( FileName, ExtractName, &unzipped_size);
      if (rom_buffer) {
        snprintf(SaveName, MAX_PATH, "roms/%s", ExtractName);
        FILE* extracted_fd = fopen(SaveName, "w");
        if (extracted_fd) {
          fwrite( rom_buffer, unzipped_size, 1, extracted_fd);
          fclose(extracted_fd);
          free(rom_buffer);
          error = loc_load_cart( SaveName );
          unlink( ExtractName );
        }
      }
    }

  } else {
    strncpy(SaveName,FileName,MAX_PATH);
    scan = strrchr(SaveName,'.');
    if (scan) *scan = '\0';
    atari_update_save_name(SaveName);
    error = loc_load_cart(FileName);
  }

  if (! error ) {
    atari_kbd_load();
    atari_joy_load();
    atari_load_cheat();
    atari_load_settings();
  }

  return error;
}

static int
loc_atari_save_cheat(char *chFileName)
{
  FILE* FileDesc;
  int   cheat_num;
  int   error = 0;

  FileDesc = fopen(chFileName, "w");
  if (FileDesc != (FILE *)0 ) {

    for (cheat_num = 0; cheat_num < ATARI_MAX_CHEAT; cheat_num++) {
      Atari_cheat_t* a_cheat = &ATARI.atari_cheat[cheat_num];
      if (a_cheat->type != ATARI_CHEAT_NONE) {
        fprintf(FileDesc, "%d,%x,%x,%s\n", 
                a_cheat->type, a_cheat->addr, a_cheat->value, a_cheat->comment);
      }
    }
    fclose(FileDesc);

  } else {
    error = 1;
  }

  return error;
}

int
atari_save_cheat(void)
{
  char  FileName[MAX_PATH+1];
  int   error;

  error = 1;

  snprintf(FileName, MAX_PATH, "%s/cht/%s.cht", ATARI.atari_home_dir, ATARI.atari_save_name);
  error = loc_atari_save_cheat(FileName);

  return error;
}

static int
loc_atari_load_cheat(char *chFileName)
{
  char  Buffer[512];
  char *Scan;
  char *Field;
  unsigned int  cheat_addr;
  unsigned int  cheat_value;
  unsigned int  cheat_type;
  char         *cheat_comment;
  int           cheat_num;
  FILE* FileDesc;

  memset(ATARI.atari_cheat, 0, sizeof(ATARI.atari_cheat));
  cheat_num = 0;

  FileDesc = fopen(chFileName, "r");
  if (FileDesc == (FILE *)0 ) return 0;

  while (fgets(Buffer,512, FileDesc) != (char *)0) {

    Scan = strchr(Buffer,'\n');
    if (Scan) *Scan = '\0';
    /* For this #@$% of windows ! */
    Scan = strchr(Buffer,'\r');
    if (Scan) *Scan = '\0';
    if (Buffer[0] == '#') continue;

    /* %d, %x, %x, %s */
    Field = Buffer;
    Scan = strchr(Field, ',');
    if (! Scan) continue;
    *Scan = 0;
    if (sscanf(Field, "%d", &cheat_type) != 1) continue;
    Field = Scan + 1;
    Scan = strchr(Field, ',');
    if (! Scan) continue;
    *Scan = 0;
    if (sscanf(Field, "%x", &cheat_addr) != 1) continue;
    Field = Scan + 1;
    Scan = strchr(Field, ',');
    if (! Scan) continue;
    *Scan = 0;
    if (sscanf(Field, "%x", &cheat_value) != 1) continue;
    Field = Scan + 1;
    cheat_comment = Field;

    if (cheat_type <= ATARI_CHEAT_NONE) continue;

    Atari_cheat_t* a_cheat = &ATARI.atari_cheat[cheat_num];

    a_cheat->type  = cheat_type;
    a_cheat->addr  = cheat_addr;
    a_cheat->value = cheat_value;
    strncpy(a_cheat->comment, cheat_comment, sizeof(a_cheat->comment));
    a_cheat->comment[sizeof(a_cheat->comment)-1] = 0;

    if (++cheat_num >= ATARI_MAX_CHEAT) break;
  }
  fclose(FileDesc);

  return 0;
}

int
atari_load_cheat()
{
  char  FileName[MAX_PATH+1];
  int   error;

  error = 1;

  snprintf(FileName, MAX_PATH, "%s/cht/%s.cht", ATARI.atari_home_dir, ATARI.atari_save_name);
  error = loc_atari_load_cheat(FileName);

  return error;
}

int
atari_load_file_cheat(char *FileName)
{
  return loc_atari_load_cheat(FileName);
}

void
atari_apply_cheats()
{
  u8 *a_ram = get_memory_ram();

  int cheat_num;
  for (cheat_num = 0; cheat_num < ATARI_MAX_CHEAT; cheat_num++) {
    Atari_cheat_t* a_cheat = &ATARI.atari_cheat[cheat_num];
    if (a_cheat->type == ATARI_CHEAT_ENABLE) {
      a_ram[a_cheat->addr % ATARI_RAM_SIZE] = a_cheat->value;
    }
  }
}


// Called from original UI
void
atari_load_rom_settings(char* FileName)
{
  char  SaveName[MAX_PATH+1];
  strncpy(SaveName,FileName,MAX_PATH);
  char* scan = strrchr(SaveName,'.');
  if (scan) *scan = '\0';
  atari_update_save_name(SaveName);
  atari_kbd_load();
  atari_joy_load();
  atari_load_cheat();
  atari_load_settings();
}

static int
loc_load_state(char *filename)
{
  int error;
  error = ! ReadAtariState(filename, "rb");
  return error;
}

int
atari_load_state(char *FileName)
{
  char  SaveName[MAX_PATH+1];
  int   error;
  char  *scan;

  error = 1;

  strncpy(SaveName,FileName,MAX_PATH);
  scan = strrchr(SaveName,'.');
  if (scan) *scan = '\0';
  atari_update_save_name(SaveName);
  error = loc_load_state(FileName);

  if (! error ) {

    atari_kbd_load();
    atari_joy_load();
    atari_load_cheat();
    atari_load_settings();
  }

  return error;
}

static int
loc_atari_save_state(char *filename)
{
  int error;
	error = ! SaveAtariState(filename, "wb", TRUE);
  return error;
}

int
atari_snapshot_save_slot(int save_id)
{
  char        FileName[MAX_PATH+1];
  struct stat aStat;
  int         error;

  error = 1;

  if (save_id < ATARI_MAX_SAVE_STATE) {
    snprintf(FileName, MAX_PATH, "%s/save/sav_%s_%d.sta", ATARI.atari_home_dir, ATARI.atari_save_name, save_id);
    error = loc_atari_save_state(FileName);
    if (! error) {
      if (! stat(FileName, &aStat)) {
        ATARI.atari_save_state[save_id].used  = 1;
        ATARI.atari_save_state[save_id].thumb = 0;
        ATARI.atari_save_state[save_id].date  = aStat.st_mtime;
        snprintf(FileName, MAX_PATH, "%s/save/sav_%s_%d.png", ATARI.atari_home_dir, ATARI.atari_save_name, save_id);
        if (psp_sdl_save_thumb_png(ATARI.atari_save_state[save_id].surface, FileName)) {
          ATARI.atari_save_state[save_id].thumb = 1;
        }
      }
    }
  }

  return error;
}

int
atari_snapshot_load_slot(int load_id)
{
  char  FileName[MAX_PATH+1];
  int   error;

  error = 1;

  if (load_id < ATARI_MAX_SAVE_STATE) {
    snprintf(FileName, MAX_PATH, "%s/save/sav_%s_%d.sta", ATARI.atari_home_dir, ATARI.atari_save_name, load_id);
    error = loc_load_state(FileName);
  }
  return error;
}

int
atari_snapshot_del_slot(int save_id)
{
  char  FileName[MAX_PATH+1];
  struct stat aStat;
  int   error;

  error = 1;

  if (save_id < ATARI_MAX_SAVE_STATE) {
    snprintf(FileName, MAX_PATH, "%s/save/sav_%s_%d.sta", ATARI.atari_home_dir, ATARI.atari_save_name, save_id);
    error = remove(FileName);
    if (! error) {
      ATARI.atari_save_state[save_id].used  = 0;
      ATARI.atari_save_state[save_id].thumb = 0;
      memset(&ATARI.atari_save_state[save_id].date, 0, sizeof(time_t));

      /* We keep always thumbnail with id 0, to have something to display in the file requester */ 
      if (save_id != 0) {
        snprintf(FileName, MAX_PATH, "%s/save/sav_%s_%d.png", ATARI.atari_home_dir, ATARI.atari_save_name, save_id);
        if (! stat(FileName, &aStat)) {
          remove(FileName);
        }
      }
    }
  }

  return error;
}

void
atari_treat_command_key(int atari_idx)
{
  int new_render;

  switch (atari_idx) 
  {
    case ATARIC_FPS: ATARI.atari_view_fps = ! ATARI.atari_view_fps;
    break;
    case ATARIC_JOY: ATARI.psp_reverse_analog = ! ATARI.psp_reverse_analog;
    break;
    case ATARIC_RENDER: 
      psp_sdl_black_screen();
      new_render = ATARI.atari_render_mode + 1;
      if (new_render > ATARI_LAST_RENDER) new_render = 0;
      ATARI.atari_render_mode = new_render;
    break;
    case ATARIC_LOAD: //psp_main_menu_load_current();
    break;
    case ATARIC_SAVE: //psp_main_menu_save_current(); 
    break;
    case ATARIC_RESET: 
       psp_sdl_black_screen();
       emulator_reset(); 
    break;
    case ATARIC_AUTOFIRE: 
       kbd_change_auto_fire(! ATARI.atari_auto_fire);
    break;
    case ATARIC_DECFIRE: 
      if (ATARI.atari_auto_fire_period > 0) ATARI.atari_auto_fire_period--;
    break;
    case ATARIC_INCFIRE: 
      if (ATARI.atari_auto_fire_period < 19) ATARI.atari_auto_fire_period++;
    break;
    case ATARIC_SCREEN: psp_screenshot_mode = 10;
    break;
  }
}
