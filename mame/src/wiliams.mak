
#namco system NA only

# a tiny compile is without Neogeo games

COREDEFS += -DWILIAMS=1

COREDEFS += -DTINY_COMPILE=1 -DHAS_ADSP2105
COREDEFS += -DTINY_NAME="\
driver_joust2,\
driver_joust,\
driver_inferno,\
driver_tshoot,\
driver_mysticm,\
driver_blaster,\
driver_sinistar,\
driver_splat,\
driver_bubbles,\
driver_joust,\
driver_robotron,\
driver_stargate,\
driver_colony7,\
driver_mayday,\
driver_defence,\
driver_defender"

COREDEFS += -DTINY_POINTER="\
&driver_joust2,\
&driver_joust,\
&driver_inferno,\
&driver_tshoot,\
&driver_mysticm,\
&driver_blaster,\
&driver_sinistar,\
&driver_splat,\
&driver_bubbles,\
&driver_joust,\
&driver_robotron,\
&driver_stargate,\
&driver_colony7,\
&driver_mayday,\
&driver_defence,\
&driver_defender"

# uses these CPUs
#CPUS+=M68000@
#CPUS+=KONAMI@
#CPUS=+M68010@
#CPUS+=M68EC020@
#CPUS+=N68000@
#CPUS+=Z80@
#CPUS+=HD63701@
CPUS+=M6809@
CPUS+=M6808@
#CPUS+=HD63705@
#CPUS+=8080@
#CPUS+=M6502@
#CPUS+=Z8000@
#CPUS+=M6809@
CPUS+=HD6309@
CPUS+=ADSP2100@
CPUS+=ADSP2105@
#CPUS+=S2650@

# uses these SOUNDs
SOUNDS+=DAC@
SOUNDS+=PIA@
#SOUNDS+=NAMCONA@
#SOUNDS+=AY8910@
SOUNDS+=OKIM6295@
SOUNDS+=YM2151@
SOUNDS+=HC55516@
SOUNDS+=WILLIAMS_CVSD@
#SOUNDS+=K007232@
#SOUNDS+=K053260@
#SOUNDS+=K005289@
#SOUNDS+=VLM5030@
#SOUNDS+=UPD7759@
#SOUNDS+=SAMPLES@
SOUNDS+=CUSTOM@
#SOUNDS+=YM2203@

#SOUNDS+=NAMCO@
#SOUNDS+=C140@
#SOUNDS+=YM2610B@
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

OBJ_NAMCO = 



# MAME specific core objs
COREOBJS += $(OBJ_NAMCO) $(OBJ)/tiny.o $(OBJ)/drivers/williams.o 
#$(OBJ)/driver.o
#$(OBJ)/cheat.o
$(OBJ)/driver.o: src/driver.cpp
$(OBJ)/tiny.o: src/tiny.cpp
#$(OBJ)/machine/kabuki.o: src/machine/kabuki.cpp
#$(OBJ)/cpu/m68000/m68kmame.o: src/cpu/m68000/m68kmame.cpp
