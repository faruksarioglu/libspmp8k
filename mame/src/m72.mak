
#namco system NA only

# a tiny compile is without Neogeo games

COREDEFS += -DM72=1  -DHAS_V30=1 -DHAS_M68000=1 -DHAS_DAC -DHAS_YM2151
#-DHAS_M68EC020=1 -DHAS_M68010=1

COREDEFS += -DTINY_COMPILE=1
COREDEFS += -DTINY_NAME="\
driver_rtype,\
driver_bchopper,\
driver_mrheli,\
driver_nspirit,\
driver_imgfight,\
driver_loht,\
driver_xmultipl,\
driver_dbreed,\
driver_rtype2,\
driver_majtitle,\
driver_hharry,\
driver_poundfor,\
driver_airduel,\
driver_gallop,\
driver_kengo"

COREDEFS += -DTINY_POINTER="\
&driver_rtype,\
&driver_bchopper,\
&driver_mrheli,\
&driver_nspirit,\
&driver_imgfight,\
&driver_loht,\
&driver_xmultipl,\
&driver_dbreed,\
&driver_rtype2,\
&driver_majtitle,\
&driver_hharry,\
&driver_poundfor,\
&driver_airduel,\
&driver_gallop,\
&driver_kengo"

# uses these CPUs
CPUS+=M68000@
#CPUS=+M68010@
#CPUS+=M68EC020@
#CPUS+=N68000@
CPUS+=Z80@
#CPUS+=HD63701@
#CPUS+=M6809@
#CPUS+=M6808@
#CPUS+=HD63705@
#CPUS+=8080@
#CPUS+=M6502@
CPUS+=Z8000@
#CPUS+=M6809@
#CPUS+=HD63701@
#CPUS+=S2650@
CPUS+=V30@
# uses these SOUNDs
#SOUNDS+=NAMCONA@
#SOUNDS+=AY8910@
#SOUNDS+=OKIM6295@
SOUNDS+=YM2151@
#SOUNDS+=NAMCO@
#SOUNDS+=C140@
#SOUNDS+=YM2610B@
SOUNDS+=DAC@
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

OBJ_NAMCO = $(OBJ)/machine/irem_cpu.o 



# MAME specific core objs
COREOBJS += $(OBJ_NAMCO) $(OBJ)/tiny.o $(OBJ)/drivers/m72.o 
#$(OBJ)/driver.o
#$(OBJ)/cheat.o
$(OBJ)/driver.o: src/driver.cpp
$(OBJ)/tiny.o: src/tiny.cpp
#$(OBJ)/machine/kabuki.o: src/machine/kabuki.cpp
#$(OBJ)/cpu/m68000/m68kmame.o: src/cpu/m68000/m68kmame.cpp
$(OBJ)/machine/irem_cpu.o: src/machine/irem_cpu.cpp