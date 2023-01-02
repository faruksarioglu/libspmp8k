
#namco system NA only

# a tiny compile is without Neogeo games

COREDEFS += -DNAMCOS2=1

COREDEFS += -DTINY_COMPILE=1
COREDEFS += -DTINY_NAME="\
driver_assault,\
driver_phelios,\
driver_ordyne,\
driver_burnforc,\
driver_dirtfoxj,\
driver_marvland,\
driver_valkyrie,\
driver_dsaber,\
driver_finalap2,\
driver_rthun2,\
driver_sgunner2,\
driver_cosmogng,\
driver_finalap3"

COREDEFS += -DTINY_POINTER="\
&driver_assault,\
&driver_phelios,\
&driver_ordyne,\
&driver_burnforc,\
&driver_dirtfoxj,\
&driver_marvland,\
&driver_valkyrie,\
&driver_dsaber,\
&driver_finalap2,\
&driver_rthun2,\
&driver_sgunner2,\
&driver_cosmogng,\
&driver_finalap3"

# uses these CPUs
CPUS+=M68000@
#CPUS+=N68000@
#CPUS+=Z80@
CPUS+=HD63701@
CPUS+=M6809@
CPUS+=M6808@
CPUS+=HD63705@
#CPUS+=8080@
#CPUS+=M6502@
#CPUS+=Z8000@
CPUS+=M6809@
#CPUS+=HD63701@
#CPUS+=S2650@

# uses these SOUNDs
#SOUNDS+=NAMCONA@
#SOUNDS+=AY8910@
#SOUNDS+=OKIM6295@
SOUNDS+=YM2151@
#SOUNDS+=NAMCO@
SOUNDS+=C140@

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

  

DRVLIBS = \
	$(OBJ)/machine/random.o \

OBJ_NAMCO = $(OBJ)/drivers/namcos2.o 



# MAME specific core objs
COREOBJS += $(OBJ_NAMCO) $(OBJ)/tiny.o 
$(OBJ)/driver.o: src/driver.cpp
$(OBJ)/tiny.o: src/tiny.cpp
$(OBJ)/drivers/namcos2.o: src/drivers/namcos2.cpp
