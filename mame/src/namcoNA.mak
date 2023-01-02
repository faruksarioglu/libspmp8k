
#namco system NA only

# a tiny compile is without Neogeo games

COREDEFS += -DNAMCO_NA=1

COREDEFS += -DTINY_COMPILE=1
COREDEFS += -DTINY_NAME="\
driver_timeplt,\
driver_baraduke,\
driver_bosco,\
driver_digdug,\
driver_galaga,\
driver_gaplus,\
driver_geebee,\
driver_grobda,\
driver_locomotn,\
driver_mappy,\
driver_olibochu,\
driver_pacland,\
driver_phozon,\
driver_rallyx,\
driver_skykid,\
driver_superpac,\
driver_tankbatt,\
driver_toypop,\
driver_warpwarp,\
driver_polepos,\
driver_topracer,\
driver_shadowld,\
driver_pacmania,\
driver_galaga88,\
driver_rompers,\
driver_blazer,\
driver_assault,\
driver_phelios,\
driver_valkyrie,\
driver_dsaber,\
driver_sgunner2,\
driver_hopmappy,\
driver_roishtar,\
driver_rthunder,\
driver_wndrmomo,\
driver_ordyne,\
driver_quester,\
driver_splatter,\
driver_blastoff,\
driver_dangseed,\
driver_pistoldm,\
driver_puzlclub,\
driver_xevious"

COREDEFS += -DTINY_POINTER="\
&driver_timeplt,\
&driver_baraduke,\
&driver_bosco,\
&driver_digdug,\
&driver_galaga,\
&driver_gaplus,\
&driver_geebee,\
&driver_grobda,\
&driver_locomotn,\
&driver_mappy,\
&driver_olibochu,\
&driver_pacland,\
&driver_phozon,\
&driver_rallyx,\
&driver_skykid,\
&driver_superpac,\
&driver_tankbatt,\
&driver_toypop,\
&driver_warpwarp,\
&driver_polepos,\
&driver_topracer,\
&driver_shadowld,\
&driver_pacmania,\
&driver_galaga88,\
&driver_rompers,\
&driver_blazer,\
&driver_assault,\
&driver_phelios,\
&driver_valkyrie,\
&driver_dsaber,\
&driver_sgunner2,\
&driver_hopmappy,\
&driver_roishtar,\
&driver_rthunder,\
&driver_wndrmomo,\
&driver_ordyne,\
&driver_quester,\
&driver_splatter,\
&driver_blastoff,\
&driver_dangseed,\
&driver_pistoldm,\
&driver_puzlclub,\
&driver_xevious"

# uses these CPUs
CPUS+=M68000@
#CPUS+=N68000@
CPUS+=Z80@
CPUS+=HD63701@
CPUS+=M6809@
CPUS+=M6808@
CPUS+=HD63705@
CPUS+=8080@
CPUS+=M6502@
CPUS+=Z8000@
CPUS+=M6809@
CPUS+=HD63701@
CPUS+=S2650@

# uses these SOUNDs
SOUNDS+=NAMCONA@
SOUNDS+=AY8910@
#SOUNDS+=OKIM6295@
SOUNDS+=YM2151@
SOUNDS+=NAMCO@
SOUNDS+=C140@

#SOUNDS+=YM2610B@

SOUNDS+=DAC@
#SOUNDS+=UPD7759@
#SOUNDS+=YM2413@
SOUNDS+=SN76496@
#SOUNDS+=YM3812@
#SOUNDS+=YM3438@
#SOUNDS+=YM2612@

#SOUNDS+=X1_010@
SOUNDS+=CUSTOM@
SOUNDS+=SAMPLES@

  

DRVLIBS = \
	$(OBJ)/machine/random.o \
	$(OBJ)/vidhrdw/namcona1.o $(OBJ)/drivers/namcona1.o \
#$(OBJ)/drivers/galaxian.o $(OBJ)/drivers/pengo.o  $(OBJ)/drivers/pacman.o

OBJ_NAMCO = $(OBJ)/machine/scramble.o $(OBJ)/drivers/geebee.o $(OBJ)/drivers/warpwarp.o $(OBJ)/drivers/tankbatt.o \
	$(OBJ)/drivers/rallyx.o $(OBJ)/drivers/bosco.o  $(OBJ)/drivers/locomotn.o $(OBJ)/drivers/galaga.o $(OBJ)/drivers/digdug.o \
	$(OBJ)/drivers/xevious.o $(OBJ)/drivers/superpac.o $(OBJ)/drivers/phozon.o $(OBJ)/drivers/mappy.o $(OBJ)/drivers/grobda.o \
	$(OBJ)/drivers/gaplus.o $(OBJ)/drivers/toypop.o $(OBJ)/drivers/pacland.o $(OBJ)/drivers/skykid.o $(OBJ)/drivers/polepos.o \
	$(OBJ)/drivers/baraduke.o $(OBJ)/drivers/namcos86.o $(OBJ)/drivers/namcos1.o $(OBJ)/drivers/namcos2.o $(OBJ)/drivers/timeplt.o 

$(OBJ)/drivers/cosmic.o: src/vidhrdw/cosmic.cpp src/drivers/cosmic.cpp
$(OBJ)/drivers/cheekyms.o: src/vidhrdw/cheekyms.cpp src/drivers/cheekyms.cpp
$(OBJ)/drivers/ladybug.o: src/vidhrdw/ladybug.cpp src/drivers/ladybug.cpp
$(OBJ)/drivers/mrdo.o: src/vidhrdw/mrdo.cpp src/drivers/mrdo.cpp
$(OBJ)/drivers/docastle.o: src/machine/docastle.cpp src/vidhrdw/docastle.cpp src/drivers/docastle.cpp



# MAME specific core objs
COREOBJS += $(OBJ_NAMCO) $(OBJ)/tiny.o $(OBJ)/drivers/olibochu.o 
#$(OBJ)/driver.o
#$(OBJ)/cheat.o
$(OBJ)/driver.o: src/driver.cpp
$(OBJ)/tiny.o: src/tiny.cpp
$(OBJ)/drivers/olibochu.o: src/drivers/olibochu.cpp
$(OBJ)/drivers/namcos1.o: src/drivers/namcos1.cpp
$(OBJ)/drivers/namcos2.o: src/drivers/namcos2.cpp
$(OBJ)/drivers/namcos86.o: src/drivers/namcos86.cpp
