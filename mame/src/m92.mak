
#namco system NA only

# a tiny compile is without Neogeo games -DHAS_M68000=1 

COREDEFS += -DM92=1  -DHAS_V30=1 -DHAS_V33=1 -DHAS_YM2151 -DHAS_YM3012
#-DHAS_M68EC020=1 -DHAS_M68010=1

COREDEFS += -DTINY_COMPILE=1
COREDEFS += -DTINY_NAME="\
driver_gunforce,\
driver_bmaster,\
driver_lethalth,\
driver_thndblst,\
driver_uccops,\
driver_mysticri,\
driver_hook,\
driver_rtypeleo,\
driver_inthunt,\
driver_kaiteids,\
driver_nbbatman,\
driver_psoldier"

COREDEFS += -DTINY_POINTER="\
&driver_gunforce,\
&driver_bmaster,\
&driver_lethalth,\
&driver_thndblst,\
&driver_uccops,\
&driver_mysticri,\
&driver_hook,\
&driver_rtypeleo,\
&driver_inthunt,\
&driver_kaiteids,\
&driver_nbbatman,\
&driver_psoldier"

# uses these CPUs
#CPUS+=M68000@
#CPUS+=AUDIO_CPU@
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
#CPUS+=Z8000@
#CPUS+=M6809@
#CPUS+=HD63701@
#CPUS+=S2650@
CPUS+=V30@
CPUS+=V33@
#CPUS+=ARMNEC@

# uses these SOUNDs
#SOUNDS+=NAMCONA@
#SOUNDS+=AY8910@
#SOUNDS+=OKIM6295@
SOUNDS+=YM2151@
#SOUNDS+=NAMCO@
#SOUNDS+=C140@
#SOUNDS+=YM2610B@
#SOUNDS+=DAC@
SOUNDS+=IREMGA20@
#SOUNDS+=UPD7759@
#SOUNDS+=YM2413@
#SOUNDS+=SN76496@
SOUNDS+=YM3012@
#SOUNDS+=YM3438@
#SOUNDS+=YM2612@
#SOUNDS+=X1_010@
#SOUNDS+=CUSTOM@
#SOUNDS+=SAMPLES@
#SOUNDS+=QSOUND@
  

##DRVLIBS = \

OBJ_NAMCO = $(OBJ)/machine/irem_cpu.o 

# MAME specific core objs
COREOBJS += $(OBJ_NAMCO) $(OBJ)/tiny.o $(OBJ)/drivers/m92.o 
#$(OBJ)/cpu/nec_armnec/armnecintrf.o: src/cpu/nec_armnec/armnecintrf.cpp
#$(OBJ)/cpu/nec_armnec/armV33.o: src/cpu/nec_armnec/armV33.S
#$(OBJ)/cpu/nec_armnec/armV30.o: src/cpu/nec_armnec/armV30.S
#$(OBJ)/driver.o
#$(OBJ)/cheat.o
$(OBJ)/driver.o: src/driver.cpp
$(OBJ)/tiny.o: src/tiny.cpp
#$(OBJ)/machine/kabuki.o: src/machine/kabuki.cpp
#$(OBJ)/cpu/m68000/m68kmame.o: src/cpu/m68000/m68kmame.cpp
$(OBJ)/machine/irem_cpu.o: src/machine/irem_cpu.cpp