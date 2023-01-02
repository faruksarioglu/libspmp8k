
#ifndef __UI_H__
#define __UI_H__
#include <libgame.h>
#include <stdlib.h>
//int select_file(const char *start, const char *extension, char **file, int font_size);
const char* select_file(const char *start, const char *extension, int font_size);
const char* select_rom(); 
ge_key_data_t wait_for_key(void);
void map_buttons(emu_keymap_t *keymap);
void load_buttons(emu_keymap_t *keymap);
#endif
