/*
 *  Copyright (C) 2006 Ludovic Jacomme (ludovic.jacomme@gmail.com)
 *
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
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <errno.h>
//#include <sys/time.h>
#include <fcntl.h>
#include <libgame.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <SDL/SDL.h>
#include <text.h>
#include "global.h"
#include "Coleco.h"
#include "psp_sdl.h"
#include "psp_menu.h"

extern SDL_Surface *back_surface;

#include "psp_kbd.h"
#include "psp_sdl.h"
#include "psp_fmgr.h"

# define PSP_FMGR_MIN_TIME         150000
#define dirent _ecos_dirent
#define DIR _ecos_DIR
#define readdir _ecos_readdir
#define opendir _ecos_opendir
#define closedir _ecos_closedir
#define mkdir _ecos_mkdir

static struct _ecos_dirent files[PSP_FMGR_MAX_ENTRY];
static struct _ecos_dirent *sortfiles[PSP_FMGR_MAX_ENTRY];
static int nfiles;
static int user_file_format = 0;
ge_key_data_t wait_for_key(void)
{
    static int auto_repeat = 0;
    static ge_key_data_t okeys;
    ge_key_data_t keys;
    NativeGE_getKeyInput4Ntv(&keys);
    uint32_t last_press_tick = NativeGE_getTime();
    while (auto_repeat && keys.keys == okeys.keys && NativeGE_getTime() < last_press_tick + 100) {
        NativeGE_getKeyInput4Ntv(&keys);
    }
    if (auto_repeat && keys.keys == okeys.keys)
        return keys;
    auto_repeat = 0;
    last_press_tick = NativeGE_getTime();
    while (keys.keys && keys.keys == okeys.keys) {
        NativeGE_getKeyInput4Ntv(&keys);
        if (NativeGE_getTime() > last_press_tick + 500) {
            auto_repeat = 1;
            okeys = keys;
            return keys;
        }
    }
    okeys = keys;
    while (okeys.keys == keys.keys) {
        NativeGE_getKeyInput4Ntv(&keys);
    }
    okeys = keys;
    return keys;
}

static void invert(uint16_t *start, int size)
{
    while (size) {
        *start = ~(*start);
        start++;
        size--;
    }
}

int select_file(const char *start, const char *extension, char **file, int font_size)
{
    char wd[256];
    getcwd(wd, 256);
    struct _ecos_dirent *dents;
    struct _ecos_stat *stats;
    dents = malloc(sizeof(struct _ecos_dirent) * 10);
    stats = malloc(sizeof(struct _ecos_stat) * 10);
    int num_dents = 10;

    if (start) {
        if (_ecos_chdir(start) < 0)
            return -1;
    }

    _ecos_DIR *dir;
reload_dir:
    dir = _ecos_opendir(".");
    if (!dir)
        return -2;
    int dent_count = 0;
    struct _ecos_dirent *de;
    while ((de = _ecos_readdir(dir))) {
        _ecos_stat(de->d_name, &stats[dent_count]);
        if (extension) {
            if (!_ECOS_S_ISDIR(stats[dent_count].st_mode)) {
                char *ext = rindex(de->d_name, '.');
                if (!ext|| !strcasestr(extension, ext + 1)) //
                    continue;
            }
        }
        memcpy(&dents[dent_count], de, sizeof(struct _ecos_dirent));
        dent_count++;
        if (dent_count >= num_dents) {
            num_dents *= 2;
            dents = realloc(dents, sizeof(struct _ecos_dirent) * num_dents);
            stats = realloc(stats, sizeof(struct _ecos_stat) * num_dents);
        }
    }
    _ecos_closedir(dir);

    int screen_width = gDisplayDev->getWidth();
    int screen_height = gDisplayDev->getHeight();
    int max_entries_displayed = screen_height / font_size;
    int current_top = 0;
    int current_entry = 0;

    int old_size = text_get_font_size();
    text_set_font_size(font_size);
    
    for (;;) {
        int i;
        uint16_t *fb = gDisplayDev->getShadowBuffer();
        memset(fb, 0xff, screen_width * screen_height * 2);
        for (i = current_top;
             i < current_top + max_entries_displayed && i < current_top + dent_count; i++) {
            if (_ECOS_S_ISDIR(stats[i].st_mode)) {
                int cw = text_draw_character('[', 0, (i - current_top) * font_size);
                cw += text_render(dents[i].d_name, cw, (i - current_top) * font_size);
                text_draw_character(']', cw, (i - current_top) * font_size);
            }
            else {
                text_render(dents[i].d_name, 0, (i - current_top) * font_size);
            }
        }
        invert(fb + (current_entry - current_top) * screen_width * font_size, font_size * screen_width);
        cache_sync();
        gDisplayDev->flip();
        ge_key_data_t keys = wait_for_key();
        if ((keys.keys & GE_KEY_UP) && current_entry > 0) {
            current_entry--;
            if (current_entry < current_top)
                current_top--;
        }
        else if ((keys.keys & GE_KEY_DOWN) && current_entry < dent_count - 1) {
            current_entry++;
            if ((current_entry - current_top + 1) * font_size > screen_height)
                current_top++;
        }
        else if (keys.keys & GE_KEY_O) {
            if (_ECOS_S_ISDIR(stats[current_entry].st_mode)) {
                _ecos_chdir(dents[current_entry].d_name);
                goto reload_dir;
            }
            else
                break;
        }
    }
    char buf[256];
    getcwd(buf, 256);
    char *filename = malloc(strlen(dents[current_entry].d_name) + 1 + strlen(buf) + 1);
    sprintf(filename, "%s/%s", buf, dents[current_entry].d_name);
    *file = filename;
    free(dents);
    free(stats);
    free(de);
    free(buf);

    _ecos_chdir(wd);
    text_set_font_size(old_size);
    return 0;
}

static char user_filedir_kbd[PSP_FMGR_MAX_NAME];
static char user_filedir_set[PSP_FMGR_MAX_NAME];
static char user_filedir_rom[PSP_FMGR_MAX_NAME];
static char user_filedir_save[PSP_FMGR_MAX_NAME];

int 
psp_fmgr_menu(int format)
{
  static int  first = 1;

  char *user_filedir;
  char user_filename[PSP_FMGR_MAX_NAME];
  struct stat       aStat;
  int               file_format;
  int               er;
  int               ret;

  user_file_format = format;
  ret = 0;

  if (first) {
    first = 0;
    strcpy(user_filedir_kbd, ".");
    strcpy(user_filedir_set, user_filedir_kbd);
    strcpy(user_filedir_rom, user_filedir_kbd);
    strcpy(user_filedir_save, user_filedir_kbd);
    strcat(user_filedir_kbd, "/kbd/");
    strcat(user_filedir_set, "/set/");
    strcat(user_filedir_rom, "/roms/");
    strcat(user_filedir_save, "/save/");
  }

  if (format == FMGR_FORMAT_KBD) user_filedir = user_filedir_kbd;
  else
  if (format == FMGR_FORMAT_SET) user_filedir = user_filedir_set;
  else
  if (format == FMGR_FORMAT_STATE) user_filedir = user_filedir_save;
  else                             user_filedir = user_filedir_rom;

  //psp_kbd_wait_no_button();
  select_file(user_filedir,"rom",user_filename,12);
    //file_format = psp_fmgr_getExtId(user_filename);
     er = cv_load_rom(user_filename, 1);
  return ret;
}
