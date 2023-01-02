
#namco system NA only

# a tiny compile is without Neogeo games

COREDEFS += -DKONAMI=1 -DHAS_M68000=1 

COREDEFS += -DTINY_COMPILE=1
COREDEFS += -DTINY_NAME="\
driver_mia,\
driver_mia2,\
driver_tmnt,\
driver_punkshot,\
driver_punksht2,\
driver_lgtnfght,\
driver_trigon,\
driver_blswhstl,\
driver_detatwin,\
driver_tmnt2,\
driver_ssriders,\
driver_gradius3,\
driver_nemesis,\
driver_konamigt,\
driver_rf2,\
driver_twinbee,\
driver_gradius,\
driver_gwarrior,\
driver_salamand,\
driver_lifefrce,\
driver_jackal,\
driver_topgunr,\
driver_bladestl,\
driver_crimfght,\
driver_devilw,\
driver_majuu,\
driver_vulcan,\
driver_cuebrick,\
driver_fround,\
driver_thndrx2"

COREDEFS += -DTINY_POINTER="\
&driver_mia,\
&driver_mia2,\
&driver_tmnt,\
&driver_punkshot,\
&driver_punksht2,\
&driver_lgtnfght,\
&driver_trigon,\
&driver_blswhstl,\
&driver_detatwin,\
&driver_tmnt2,\
&driver_ssriders,\
&driver_gradius3,\
&driver_nemesis,\
&driver_konamigt,\
&driver_rf2,\
&driver_twinbee,\
&driver_gradius,\
&driver_gwarrior,\
&driver_salamand,\
&driver_lifefrce,\
&driver_jackal,\
&driver_topgunr,\
&driver_bladestl,\
&driver_crimfght,\
&driver_devilw,\
&driver_majuu,\
&driver_vulcan,\
&driver_cuebrick,\
&driver_fround,\
&driver_thndrx2"

# uses these CPUs
CPUS+=M68000@
CPUS+=KONAMI@
#CPUS=+M68010@
#CPUS+=M68EC020@
#CPUS+=N68000@
CPUS+=Z80@
#CPUS+=HD63701@
CPUS+=M6809@
#CPUS+=M6808@
#CPUS+=HD63705@
#CPUS+=8080@
#CPUS+=M6502@
#CPUS+=Z8000@
#CPUS+=M6809@
CPUS+=HD6309@
#CPUS+=S2650@

# uses these SOUNDs
#SOUNDS+=NAMCONA@
SOUNDS+=AY8910@
#SOUNDS+=OKIM6295@
SOUNDS+=YM2151@
SOUNDS+=K007232@
SOUNDS+=K053260@
SOUNDS+=K005289@
SOUNDS+=VLM5030@
SOUNDS+=UPD7759@
SOUNDS+=SAMPLES@
SOUNDS+=CUSTOM@
SOUNDS+=YM2203@

#SOUNDS+=NAMCO@
#SOUNDS+=C140@
#SOUNDS+=YM2610B@
#SOUNDS+=DAC@
#SOUNDS+=UPD7759@
#SOUNDS+=YM2413@
#SOUNDS+=SN76496@
#SOUNDS+=YM3812@
#SOUNDS+=YM3438@
#SOUNDS+=YM2612@
#SOUNDS+=X1_010@
#SOUNDS+=CUSTOM@
#SOUNDS+=SAMPLES@
#SOUNDS+=QSOUND@
  

##DRVLIBS = \

OBJ_NAMCO = $(OBJ)/vidhrdw/konamiic.o 



# MAME specific core objs
COREOBJS += $(OBJ_NAMCO) $(OBJ)/tiny.o $(OBJ)/drivers/tmnt.o $(OBJ)/drivers/gradius3.o $(OBJ)/drivers/nemesis.o \
	$(OBJ)/drivers/crimfght.o $(OBJ)/drivers/jackal.o $(OBJ)/drivers/bladestl.o $(OBJ)/drivers/twin16.o 
#$(OBJ)/driver.o
#$(OBJ)/cheat.o
$(OBJ)/driver.o: src/driver.cpp
$(OBJ)/tiny.o: src/tiny.cpp
$(OBJ)/machine/kabuki.o: src/machine/kabuki.cpp
$(OBJ)/cpu/m68000/m68kmame.o: src/cpu/m68000/m68kmame.cpp
