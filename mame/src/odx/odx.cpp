/***************************************************************************

  osdepend.c

  OS dependant stuff (display handling, keyboard scan...)
  This is the only file which should me modified in order to port the
  emulator to a different system.

***************************************************************************/

#include "driver.h"
#include <signal.h>
#include <time.h>
#include <ctype.h>
#include "stdarg.h"
#include "string.h"
#include "minimal.h"
#include <libgame.h>
#include <text.h>
int  msdos_init_sound(void);
void msdos_init_input(void);
void msdos_shutdown_sound(void);
void msdos_shutdown_input(void);
int  frontend_help (int argc, char **argv);
void parse_cmdline (int argc, char **argv, int game);
void init_inpdir(void);
void odx_text(unsigned short *scr, int x, int y, char *text, int color);

static FILE *errorlog;

/* put here anything you need to do when the program is started. Return 0 if */
/* initialization was successful, nonzero otherwise. */
int osd_init(void)
{
	if (msdos_init_sound())
		return 0;
	msdos_init_input();
	return 0;
}


/* put here cleanup routines to be executed when the program is terminated. */
void osd_exit(void)
{
	msdos_shutdown_sound();
	msdos_shutdown_input();
}

/* fuzzy string compare, compare short string against long string        */
/* e.g. astdel == "Asteroids Deluxe". The return code is the fuzz index, */
/* we simply count the gaps between maching chars.                       */
int fuzzycmp (const char *s, const char *l)
{
	int gaps = 0;
	int match = 0;
	int last = 1;

	for (; *s && *l; l++)
	{
		if (*s == *l)
			match = 1;
		else if (*s >= 'a' && *s <= 'z' && (*s - 'a') == (*l - 'A'))
			match = 1;
		else if (*s >= 'A' && *s <= 'Z' && (*s - 'A') == (*l - 'a'))
			match = 1;
		else
			match = 0;

		if (match)
			s++;

		if (match != last)
		{
			last = match;
			if (!match)
				gaps++;
		}
	}

	/* penalty if short string does not completely fit in */
	for (; *s; s++)
		gaps++;

	return gaps;
}
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

const char* select_rom()
{
    int dent_count=15;
	 char* f;
	 f=malloc(16);//=" ";
	 char* dents[dent_count];
#ifdef TAITOAX
	dents[0]= "alpine";
	dents[1]= "hwrace";
	dents[2]= "masterw";
	dents[3]= "spacedx";
	dents[4]= "puzbobb";
	dents[5]= "hitice";
	dents[6]= "ashura";
	dents[7]= "nastar";
	dents[8]= "tetrist";
	dents[9]= "silentd";
    dents[10]= "tinstar";
    dents[11]= "elevator";
    dents[12]= "viofight";
    dents[13]= "crimec";
    dents[14]= "crimec";
#endif
#ifdef WILIAMS
	dents[0]= "joust2";
	dents[1]= "joust";
	dents[2]= "inferno";
	dents[3]= "tshoot";
	dents[4]= "mysticm";
	dents[5]= "blaster";
	dents[6]= "sinistar";
	dents[7]= "splat";
	dents[8]= "bubbles";
	dents[9]= "joust";
    dents[10]= "robotron";
    dents[11]= "stargate";
    dents[12]= "colony7";
    dents[13]= "mayday";
    dents[14]= "defence";
    dents[15]= "defender";
#endif
#ifdef WMS
	dents[0]= "mk";
	dents[1]= "mk2";
	dents[2]= "narc";
	dents[3]= "narc3";
	dents[4]= "trog";
	dents[5]= "strkforc";
	dents[6]= "hiimpact";
	dents[7]= "term2";
	dents[8]= "mkla1";
	dents[9]= "totcarn";
    dents[10]= "mk3";
    dents[11]= "openice";
    dents[12]= "exterm";
    dents[13]= "";
    dents[14]= "";
    dents[15]= "";
#endif

#ifdef JALECO
	dents[0]= "astyanax";//-baddump
	dents[1]= "avspirit";//+
	dents[2]= "phantasm";//ym ok
	dents[3]= "butasan";//
	dents[4]= "chimerab";//+
	dents[5]= "valtric";//
	dents[6]= "argus";//++
	dents[7]= "iganinju";//++
	dents[8]= "ginganin";//++
	dents[9]= "lomakai";//+
    dents[10]= "makaiden";//++
    dents[11]= "ginganin";//--p47
    dents[12]= "peekaboo";//++
    dents[13]= "pinbo";//++ym2151?rodlandj
    dents[14]= "psychic5";//++ym2151stdragon
    dents[15]= "soldamj";//ym2151 ok
    dents[16]= "tshingen";//ym ok
	dents[17]="sonicwi";
	dents[18]="karatblz";//load
	dents[19]="spinlbrk";//load
	dents[20]="turbofrc";//load
	dents[21]="aerofgt";//++
	

#endif


#ifdef TOAPLAN
	dents[0]= "rallybik";//++
	dents[1]= "fireshrk";//--
	dents[2]= "vimana";//--
	dents[3]= "tekipaki";//--
	dents[4]= "samesame";//--
	dents[5]= "ghox";//++
	dents[6]= "";//++
	dents[7]= "";//==
	dents[8]= "";//++
	dents[9]= "";//++
    dents[10]= "";//++
    dents[11]= "";//--mem
    dents[12]= "";//--
    dents[13]= "";//dosya eksik
    dents[14]= "";//--
    dents[15]= "";//mem
	dents[16]= "";//mem
	dents[17]= "";//mem
#endif

#ifdef KONAMI
	dents[0]= "mia";//++
	dents[1]= "mia2";//
	dents[2]= "tmnt";
	dents[3]= "punkshot";
	dents[4]= "punksht2";
	dents[5]= "lgtnfght";
	dents[6]= "trigon";
	dents[7]= "blswhstl";
	dents[8]= "detatwin";//
	dents[9]= "tmnt2";//--
    dents[10]= "ssriders";//++
    dents[11]= "thndrx2";//++
    dents[12]= "gradius3";//++
    dents[13]= "nemesis";//++
    dents[14]= "konamigt";//++
    dents[15]= "rf2";//++
	dents[16]= "twinbee";//++
	dents[17]= "gradius";//++
	dents[18]= "gwarrior";//++
	dents[19]= "salamand";//++
	dents[20]= "lifefrce";//++
	dents[21]= "crimfght";//
	dents[22]= "jackal";//++
	dents[23]= "topgunr";//++
	dents[24]= "bladestl";//++
	dents[25]= "devilw";//--
	dents[26]= "majuu";
	dents[27]= "vulcan";
	dents[28]= "cuebrick";
	dents[29]= "fround";

#endif


#ifdef M72
	dents[0]= "rtype";
	dents[1]= "bchopper";
	dents[2]= "mrheli";
	dents[3]= "nspirit";
	dents[4]= "imgfight";
	dents[5]= "loht";
	dents[6]= "xmultipl";
	dents[7]= "dbreed";
	dents[8]= "rtype2";
	dents[9]= "majtitle";
    dents[10]= "hharry";
    dents[11]= "poundfor";
    dents[12]= "airduel";
    dents[13]= "gallop";
    dents[14]= "kengo";
#endif

#ifdef M92
	dents[0]= "gunforce";
	dents[1]= "bmaster";
	dents[2]= "lethalth";
	dents[3]= "thndblst";
	dents[4]= "uccops";
	dents[5]= "mysticri";
	dents[6]= "hook";
	dents[7]= "rtypeleo";
	dents[8]= "inthunt";
	dents[9]= "kaiteids";
    dents[10]= "nbbatman";
    dents[11]= "psoldier";
    dents[12]= "";
    dents[13]= "";
    dents[14]= "";
#endif

#ifdef DEAST  
	dents[0]= "cobracom";//forgottn
	dents[1]= "ghostb";//lostwrld
	dents[2]= "srdarwin";//ghouls
	dents[3]= "gondo";//strider
	dents[4]= "oscar";//dwj-v
	dents[5]= "lastmiss";//video
	dents[6]= "shackled";//sf2
	dents[7]= "breywood";//unsq//captcomm
	dents[8]= "csilver";//++3wonders//knights
	dents[9]= "garyoret";//++kod//wof//dino//punisher
    dents[10]= "hbarrel";//video//audio
    dents[11]= "baddudes";//++
    dents[12]= "robocop";  //++
	dents[13]= "hippodrm";//++
	dents[14]= "ffantasy";//video
	dents[15]= "slyspy";//video//audio streams ok
	dents[16]= "secretag";//video
	dents[17]= "midres";//++
	dents[18]= "bouldash";//++
	dents[19]= "tumblep";//++
	dents[20]= "stadhero";//++
	dents[21]= "twocrude";//--
	dents[22]= "madmotor";//--
	dents[23]= "cbuster";//--
	dents[24]= "darkseal";--
	dents[25]= "supbtime";//++
	dents[26]= "cninja";//--

#endif

#ifdef CPS1  
	dents[0]= "mtwins";
	dents[1]= "unsquad";
	dents[2]= "area88";
	dents[3]= "ffight";
	dents[4]= "1941";
	dents[5]= "msword";
	dents[6]= "cawing";
	dents[7]= "nemo";
	dents[8]= "pnickj";
	dents[9]= "varth";
    dents[10]= "forgottn";
    dents[11]= "lostwrld";
    dents[12]= "ghouls";
	dents[13]= "strider";
	dents[14]= "dwj";
	dents[15]= "willow";
	dents[16]= "kod";//++
	dents[17]= "3wonders";//++
	dents[18]= "mercs";//++
	dents[19]= "wof";//--
	dents[20]= "megaman";//--
	dents[21]= "knights";//--
	dents[22]= "dino";//--
	dents[23]= "captcomm";//--
	dents[24]= "punisher";//--
	dents[25]= "pang3";//--
	dents[26]= "sf2";//
	
#endif
#ifdef NAMCO_NA  
	dents[0]= "bosco";
	dents[1]= "digdug";
	dents[2]= "galaga";
	dents[3]= "mappy";
	dents[4]= "olibochu";
	dents[5]= "locomotn";
	dents[6]= "pacland";
	dents[7]= "phozon";
	dents[8]= "skykid";
	dents[9]= "hopmappy";
    dents[10]= "rallyx";
    dents[11]= "roishtar";
    dents[12]= "rthunder";  
	dents[13]= "blazer";
	dents[14]= "quester";
	dents[15]= "splatter";
	dents[16]= "blastoff";
	dents[17]= "assault";
	dents[18]="polepos";
	dents[19]="superpac";
	dents[20]="xevious";
	dents[21]="wndrmomo";
	dents[22]="dangseed";
	dents[23]="rompers";//++
	dents[24]="galaga88";//++
	dents[25]="timeplt";//ram
	dents[26]="grobda";
	dents[27]="pistoldm";
	dents[28]="puzlclub";
	dents[29]="shadowld";
	dents[30]="ordyne";//ram
	/*superpac,tankbatt,toypop,xevious,finalap2,wndrmomo,sgunner2,dsaber,
	valkyrie,phelios,rompers,galaga88,pacmania,shadowld,topracer,polepos,warpwarp
	phozon, grobda, geebee*/
#endif

#ifdef KANECO
	dents[0]= "galpanic";//
	dents[1]= "airbustr";
	dents[2]= "ohmygod";//
	dents[3]= "splash";//
	dents[4]= "";//
	dents[5]= "";//
	dents[6]= "";//
	dents[7]= "";//
	dents[8]= "";//
    dents[9]= "";
#endif

#ifdef TAITOL
	dents[0]= "raimais";//-
	dents[1]= "fhawk";//++
	dents[2]= "champwr";//++
	dents[3]= "kurikint";//++
	dents[4]= "plotting";//++
	dents[5]= "puzznic";//+
	dents[6]= "horshoes";//+
	dents[7]= "palamed";//--
	dents[8]= "cachat";//
    dents[9]= "tubeit";
	dents[10]= "cubybop";
    dents[11]= "plgirls";//mem
    dents[12]= "plgirls2";  //++
	dents[13]= "";//++
	dents[14]= "";//--
	dents[15]="";//--
	dents[16]="";//--
	dents[17]="";//--
	dents[18]="";
	dents[19]="";
#endif

#ifdef TAITOF2
	dents[0]= "finalb";//++
	dents[1]= "dondokod";//++
	dents[2]= "megab";//yok
	dents[3]= "thundfox";//++
	dents[4]= "cameltry";//
	dents[5]= "liquidk";//+
	dents[6]= "gunfront";//+
	dents[7]= "growl";//--
	dents[8]= "runark";//
    dents[9]= "hthero";
	dents[10]= "koshien";
    dents[11]= "yuyugogo";//mem
    dents[12]= "ninjak";  //++
	dents[13]= "solfigtr";//++
	dents[14]= "pulirula";//--
	dents[15]="deadconx";//--
	dents[16]="dinorex";//--
	dents[17]="qcrayon";//--
	dents[18]="driveout";
	dents[19]="driveout";
#endif

#ifdef SETA
	dents[0]= "tndrcade";//+
	dents[1]= "twineagl";//-
	dents[2]= "calibr50";//+
	dents[3]= "drgnunit";//+
	dents[4]= "downtown";//-
	dents[5]= "arbalest";//-
	dents[6]= "metafox";//-
	dents[7]= "wrofaero";//-
	dents[8]= "zingzip";//mem
    dents[9]= "msgundam";
	dents[10]= "gpilots";
    dents[11]= "joyjoy";//++
    dents[12]= "lresort";  //++
	dents[13]= "eightman";//++
	dents[14]= "legendos";//video
	
#endif

#ifdef NEOMAME 
	dents[0]= "nam1975";//
	dents[1]= "puzzldpr";//
	dents[2]= "ridhero";//
	dents[3]= "alpham2";//
	dents[4]= "cyberlip";//
	dents[5]= "superspy";//
	dents[6]= "mutnat";//
	dents[7]= "kotm";//
	dents[8]= "burningf";//
	dents[9]= "sengoku";//
    dents[10]= "gpilots";
    dents[11]= "joyjoy";//++
    dents[12]= "lresort";  //++
	dents[13]= "eightman";//++
	dents[14]= "legendos";//video
	dents[15]= "roboarmy";//
	dents[16]= "fatfury1";//
	dents[17]= "androdun";//
	dents[18]= "pbobble";//
	dents[19]= "gururin";//
	dents[20]= "ncommand";//
	dents[21]= "ncombat";//
	dents[22]= "crsword";//
	dents[23]= "panicbom";//
	dents[24]= "puzzldpr";//
	dents[25]= "";//++
	dents[26]= "";//sprite init
	
#endif

  	int font_size=8;
    int screen_width = 480;
    int screen_height = 272;
    int max_entries_displayed = screen_height / font_size/2;
    int current_top = 0;
    int current_entry = 0;

   // int old_size = text_get_font_size();
   // text_set_font_size(font_size);
    // gDisplayDev->clear();
    // gDisplayDev->flip();
      	unsigned short *fb = (unsigned short *)video->pixels;//gDisplayDev->getShadowBuffer();
        //memset(gDisplayDev->getShadowBuffer(), 0xff, screen_width * screen_height * 2);
      
	for (;;) {
		
        memset(fb, 0xcc, 480 * 272 * 2);
		SDL_Flip(video);
		int i;
               cache_sync();

		for (i = current_top;
             i < current_top + max_entries_displayed && i < current_top + dent_count; i++) {
        
                odx_text(fb, 240-8*8, (i - current_top) * font_size,dents[i],23);
               cache_sync();
		SDL_Flip(video);
	    }
    
        invert(fb + (current_entry - current_top) * 480 * font_size*2, font_size * 480*2);
        cache_sync();
        		SDL_Flip(video);

       // gDisplayDev->flip();
        ge_key_data_t keys = wait_for_key();
        if ((keys.keys & GE_KEY_UP) && current_entry > 0) {
            current_entry--;
            if (current_entry < current_top)
                current_top--;
        }
        else if ((keys.keys & GE_KEY_DOWN) && current_entry < dent_count - 1) {
            current_entry++;
            if ((current_entry - current_top + 1) * font_size*2 > 272)
                current_top++;
        }
        else if (keys.keys & GE_KEY_O) {
            sprintf(f ,"%s", dents[current_entry]);
			   // *file = f;
		    	break;
        }
    }
       	
		        cache_sync();

    //char buf[256];
    //free(dents);
    //text_set_font_size(old_size);
    return f;
    //else return 0;	
}

int main (int argc, char **argv)
{
	int res, i, j = 0, game_index;
   	char *playbackname = NULL;
	int use_fame=0;
   	extern int video_scale;
	extern int video_border;
	extern int video_aspect;
	extern int throttle;

	memset(&options,0,sizeof(options));

	/* these two are not available in mame.cfg */
	errorlog = 0;
	game_index = -1;

			errorlog = fopen("/fat20a2/hda2/GAME/mame4all/error.log","wa");
			use_fame=0;
			video_scale=1;
			//video_scale=4;
			video_border=0;
			video_aspect=0;
			throttle=1;
		//	fullscreen=true;
	/*	if (strcasecmp(argv[i],"-horizscale") == 0)
			video_scale=1;
		if (strcasecmp(argv[i],"-halfscale") == 0)
			video_scale=2;
		if (strcasecmp(argv[i],"-bestscale") == 0)
			video_scale=3;
		if (strcasecmp(argv[i],"-fastscale") == 0)
			video_scale=4;
		if (strcasecmp(argv[i],"-fullscreen") == 0)
			fullscreen=true;
	*/
	odx_init(50000,16,44100,16,0,60);
	init_inpdir(); /* Init input directory for opening .inp for playback */
  
  //  for (i = 0; i < 20; i++)
   // load_buttons(&keymap);
    
    
  
  const char gn1;
  const char gname[16];
  //gn1=;
  sprintf (gname,"%s",select_rom());
    if (game_index == -1)
    {
        {
            for (i = 0; drivers[i] && (game_index == -1); i++)
            {
                if (strcasecmp(gname, drivers[i]->name) == 0)
                {
                    game_index = i;
                     printf("game found %s: %s\n",gname,drivers[game_index]->name);
                    break;
                }
            }

            /* educated guess on what the user wants to play */
            if (game_index == -1)
            {
                printf("game not found: %s\n try fuzzy",gname);
				int fuzz = 9999; /* best fuzz factor so far */

                for (i = 0; (drivers[i] != 0); i++)
                {
                    int tmp;
                    tmp = fuzzycmp(gname, drivers[i]->description);
                    /* continue if the fuzz index is worse */
                    if (tmp > fuzz)
                        continue;

                    /* on equal fuzz index, we prefer working, original games */
                    if (tmp == fuzz)
                    {
						/* game is a clone */
						if (drivers[i]->clone_of != 0
								&& !(drivers[i]->clone_of->flags & NOT_A_DRIVER))
                        {
                            /* if the game we already found works, why bother. */
                            /* and broken clones aren't very helpful either */
                            if ((!drivers[game_index]->flags & GAME_NOT_WORKING) ||
                                (drivers[i]->flags & GAME_NOT_WORKING))
                                continue;
                        }
                        else continue;
                    }

                    /* we found a better match */
                    game_index = i;
                    fuzz = tmp;
                }

                if (game_index != -1)
                    printf("fuzzy name compare, running %s\n",drivers[game_index]->name);
            }
        }

        if (game_index == -1)
        {
            printf("Game \"%s\" not supported\n", argv[j]);
            return 1;
        }
    }

	/* parse generic (os-independent) options */
	parse_cmdline (argc, argv, game_index);

{	/* Mish:  I need sample rate initialised _before_ rom loading for optional rom regions */
	extern int soundcard;

	if (soundcard == 0) {    /* silence, this would be -1 if unknown in which case all roms are loaded */
		Machine->sample_rate = 0; /* update the Machine structure to show that sound is disabled */
		options.samplerate=0;
	}
}

	/* handle record which is not available in mame.cfg */

	/* Replace M68000 by FAME */
#ifdef HAS_FAME
	if (use_fame) 
	{
		for (i=0;i<MAX_CPU;i++)
		{
			int *type=(int*)&(drivers[game_index]->drv->cpu[i].cpu_type);
            #ifdef NEOMAME
			if (((*type)&0xff)==CPU_M68000)
            #else
			if (((*type)&0xff)==CPU_M68000  ) //|| ((*type)&0xff)==CPU_M68010
            #endif
			{
				*type=((*type)&(~0xff))|CPU_FAME;
			}
		}
	}
#endif
/*
#ifdef HAS_ARMNEC
		for (i=0;i<MAX_CPU;i++)
		{
			int *type=(int*)&(drivers[game_index]->drv->cpu[i].cpu_type);
			if (((*type)&0xff)==CPU_V30)
			{
				*type=((*type)&(~0xff))|CPU_ARMV30;
			}
		}
		for (i=0;i<MAX_CPU;i++)
		{
			int *type=(int*)&(drivers[game_index]->drv->cpu[i].cpu_type);
			if (((*type)&0xff)==CPU_V33)
			{
				*type=((*type)&(~0xff))|CPU_ARMV33;
			}
		}
#endif    
  */  
    // Remove the mouse usage for certain games
    if ( (strcasecmp(drivers[game_index]->name,"hbarrel")==0) || (strcasecmp(drivers[game_index]->name,"hbarrelw")==0) ||
         (strcasecmp(drivers[game_index]->name,"midres")==0) || (strcasecmp(drivers[game_index]->name,"midresu")==0) ||
         (strcasecmp(drivers[game_index]->name,"midresj")==0) || (strcasecmp(drivers[game_index]->name,"tnk3")==0) ||
         (strcasecmp(drivers[game_index]->name,"tnk3j")==0) || (strcasecmp(drivers[game_index]->name,"ikari")==0) ||
         (strcasecmp(drivers[game_index]->name,"ikarijp")==0) || (strcasecmp(drivers[game_index]->name,"ikarijpb")==0) ||
         (strcasecmp(drivers[game_index]->name,"victroad")==0) || (strcasecmp(drivers[game_index]->name,"dogosoke")==0) ||
         (strcasecmp(drivers[game_index]->name,"gwar")==0) || (strcasecmp(drivers[game_index]->name,"gwarj")==0) ||
         (strcasecmp(drivers[game_index]->name,"gwara")==0) || (strcasecmp(drivers[game_index]->name,"gwarb")==0) ||
         (strcasecmp(drivers[game_index]->name,"bermudat")==0) || (strcasecmp(drivers[game_index]->name,"bermudaj")==0) ||
         (strcasecmp(drivers[game_index]->name,"bermudaa")==0) || (strcasecmp(drivers[game_index]->name,"mplanets")==0) ||
         (strcasecmp(drivers[game_index]->name,"forgottn")==0) || (strcasecmp(drivers[game_index]->name,"lostwrld")==0) ||
         (strcasecmp(drivers[game_index]->name,"gondo")==0) || (strcasecmp(drivers[game_index]->name,"makyosen")==0) ||
         (strcasecmp(drivers[game_index]->name,"topgunr")==0) || (strcasecmp(drivers[game_index]->name,"topgunbl")==0) ||
         (strcasecmp(drivers[game_index]->name,"tron")==0) || (strcasecmp(drivers[game_index]->name,"tron2")==0) ||
         (strcasecmp(drivers[game_index]->name,"kroozr")==0) ||(strcasecmp(drivers[game_index]->name,"crater")==0) ||
         (strcasecmp(drivers[game_index]->name,"dotron")==0) || (strcasecmp(drivers[game_index]->name,"dotrone")==0) ||
         (strcasecmp(drivers[game_index]->name,"zwackery")==0) || (strcasecmp(drivers[game_index]->name,"ikari3")==0) ||
         (strcasecmp(drivers[game_index]->name,"searchar")==0) || (strcasecmp(drivers[game_index]->name,"sercharu")==0) ||
         (strcasecmp(drivers[game_index]->name,"timesold")==0) || (strcasecmp(drivers[game_index]->name,"timesol1")==0) ||
         (strcasecmp(drivers[game_index]->name,"btlfield")==0) || (strcasecmp(drivers[game_index]->name,"aztarac")==0))
    {
        extern int use_mouse;
        use_mouse=0;
    }

    /* go for it */
    printf ("bunu aciyorum: %s (%s)...\n",drivers[game_index]->description,drivers[game_index]->name);
    res = run_game (game_index);

	/* close open files */
	if (errorlog) fclose (errorlog);
	if (options.playback) osd_fclose (options.playback);
	if (options.record)   osd_fclose (options.record);
	if (options.language_file) osd_fclose (options.language_file);

	if (res!=0)
	{
		/* wait a key press */
		odx_video_flip_single();
		odx_joystick_press();
	}

   	odx_deinit();

   // execl("mame.dge", "mame.dge", "cache", NULL);

	exit (res);
}

void CLIB_DECL logerror(const char *text,...)
{
	if (errorlog)
	{
		va_list arg;
		va_start(arg,text);
		vfprintf(errorlog,text,arg);
		va_end(arg);
	}
}
