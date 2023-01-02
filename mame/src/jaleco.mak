
#namco system NA only

# a tiny compile is without Neogeo games

COREDEFS += -DHAS_Y8950=1 -DJALECO=1 -DHAS_M68000=1 

COREDEFS += -DTINY_COMPILE=1
COREDEFS += -DTINY_NAME="\
driver_64street,\
driver_astyanax,\
driver_avspirit,\
driver_phantasm,\
driver_bigstrik,\
driver_chimerab,\
driver_cybattlr,\
driver_edf,\
driver_hachoo,\
driver_iganinju,\
driver_kickoff,\
driver_lomakai,\
driver_makaiden,\
driver_p47,\
driver_peekaboo,\
driver_rodlandj,\
driver_stdragon,\
driver_soldamj,\
driver_exerion,\
driver_pinbo,\
driver_psychic5,\
driver_ginganin,\
driver_argus,\
driver_valtric,\
driver_pspikes,\
driver_karatblz,\
driver_spinlbrk,\
driver_turbofrc,\
driver_aerofgt,\
driver_sonicwi,\
driver_tshingen"

COREDEFS += -DTINY_POINTER="\
&driver_64street,\
&driver_astyanax,\
&driver_avspirit,\
&driver_phantasm,\
&driver_bigstrik,\
&driver_chimerab,\
&driver_cybattlr,\
&driver_edf,\
&driver_hachoo,\
&driver_iganinju,\
&driver_kickoff,\
&driver_lomakai,\
&driver_makaiden,\
&driver_p47,\
&driver_peekaboo,\
&driver_rodlandj,\
&driver_stdragon,\
&driver_soldamj,\
&driver_exerion,\
&driver_pinbo,\
&driver_psychic5,\
&driver_ginganin,\
&driver_argus,\
&driver_valtric,\
&driver_pspikes,\
&driver_karatblz,\
&driver_spinlbrk,\
&driver_turbofrc,\
&driver_aerofgt,\
&driver_sonicwi,\
&driver_tshingen"

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
CPUS+=M6502@
#CPUS+=Z8000@
#CPUS+=M6809@
CPUS+=HD6309@
#CPUS+=S2650@

# uses these SOUNDs
#SOUNDS+=NAMCONA@
SOUNDS+=AY8910@
SOUNDS+=Y8950@
SOUNDS+=OKIM6295@
SOUNDS+=YM2151@
#SOUNDS+=K007232@
#SOUNDS+=K053260@
#SOUNDS+=K005289@
#SOUNDS+=VLM5030@
#SOUNDS+=UPD7759@
#SOUNDS+=SAMPLES@
#SOUNDS+=CUSTOM@
SOUNDS+=YM2203@

#SOUNDS+=NAMCO@
#SOUNDS+=C140@
SOUNDS+=YM2610@
#SOUNDS+=DAC@
#SOUNDS+=UPD7759@
#SOUNDS+=YM2413@
#SOUNDS+=SN76496@
SOUNDS+=YM3812@
#SOUNDS+=YM3438@
#SOUNDS+=YM2612@
#SOUNDS+=X1_010@
#SOUNDS+=CUSTOM@
#SOUNDS+=SAMPLES@
#SOUNDS+=QSOUND@
  

##DRVLIBS = \

OBJ_NAMCO =



# MAME specific core objs
COREOBJS += $(OBJ_NAMCO)  $(OBJ)/tiny.o $(OBJ)/drivers/aerofgt.o $(OBJ)/drivers/argus.o $(OBJ)/drivers/ginganin.o $(OBJ)/drivers/megasys1.o $(OBJ)/drivers/pinbo.o $(OBJ)/drivers/exerion.o $(OBJ)/drivers/psychic5.o
#$(OBJ)/driver.o
#$(OBJ)/cheat.o
$(OBJ)/driver.o: src/driver.cpp
$(OBJ)/tiny.o: src/tiny.cpp
$(OBJ)/machine/kabuki.o: src/machine/kabuki.cpp
$(OBJ)/cpu/m68000/m68kmame.o: src/cpu/m68000/m68kmame.cpp
