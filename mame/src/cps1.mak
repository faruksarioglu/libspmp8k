
#namco system NA only

# a tiny compile is without Neogeo games

COREDEFS += -DCPS1=1 -DHAS_M68000=1 -DHAS_M68EC020=1 -DHAS_M68010=1

COREDEFS += -DTINY_COMPILE=1
COREDEFS += -DTINY_NAME="\
driver_forgottn,\
driver_lostwrld,\
driver_ghouls,\
driver_strider,\
driver_dwj,\
driver_willow,\
driver_unsquad,\
driver_area88,\
driver_ffight,\
driver_1941,\
driver_mercs,\
driver_msword,\
driver_cawing,\
driver_nemo,\
driver_sf2,\
driver_3wonders,\
driver_kod,\
driver_captcomm,\
driver_knights,\
driver_varth,\
driver_megaman,\
driver_wof,\
driver_dino,\
driver_punisher,\
driver_slammast,\
driver_pnickj,\
driver_pang3"

COREDEFS += -DTINY_POINTER="\
&driver_forgottn,\
&driver_lostwrld,\
&driver_ghouls,\
&driver_strider,\
&driver_dwj,\
&driver_willow,\
&driver_unsquad,\
&driver_area88,\
&driver_ffight,\
&driver_1941,\
&driver_mercs,\
&driver_msword,\
&driver_cawing,\
&driver_nemo,\
&driver_sf2,\
&driver_3wonders,\
&driver_kod,\
&driver_captcomm,\
&driver_knights,\
&driver_varth,\
&driver_megaman,\
&driver_wof,\
&driver_dino,\
&driver_punisher,\
&driver_slammast,\
&driver_pnickj,\
&driver_pang3"

# uses these CPUs
CPUS+=M68000@
CPUS=+M68010@
CPUS+=M68EC020@
#CPUS+=N68000@
CPUS+=Z80@
#CPUS+=HD63701@
#CPUS+=M6809@
#CPUS+=M6808@
#CPUS+=HD63705@
#CPUS+=8080@
#CPUS+=M6502@
#CPUS+=Z8000@
#CPUS+=M6809@
#CPUS+=HD63701@
#CPUS+=S2650@

# uses these SOUNDs
#SOUNDS+=NAMCONA@
#SOUNDS+=AY8910@
SOUNDS+=OKIM6295@
SOUNDS+=YM2151@
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
SOUNDS+=QSOUND@
  

##DRVLIBS = \

OBJ_NAMCO = $(OBJ)/machine/kabuki.o 



# MAME specific core objs
COREOBJS += $(OBJ_NAMCO) $(OBJ)/tiny.o $(OBJ)/drivers/cps1.o 
#$(OBJ)/driver.o
#$(OBJ)/cheat.o
$(OBJ)/driver.o: src/driver.cpp
$(OBJ)/tiny.o: src/tiny.cpp
$(OBJ)/machine/kabuki.o: src/machine/kabuki.cpp
$(OBJ)/cpu/m68000/m68kmame.o: src/cpu/m68000/m68kmame.cpp
