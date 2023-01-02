
#Taito Asuka & Taito X system  only

# driver_rastan,\ driver_rainbow,\a tiny compile is without Neogeo games
COREDEFS += -DTAITOF2=1 -DTINY_COMPILE=1
COREDEFS += -DTINY_NAME="\
driver_finalb,\
driver_dondokod,\
driver_megab,\
driver_thundfox,\
driver_cameltry,\
driver_liquidk,\
driver_gunfront,\
driver_growl,\
driver_runark,\
driver_hthero,\
driver_koshien,\
driver_yuyugogo,\
driver_ninjak,\
driver_solfigtr,\
driver_pulirula,\
driver_deadconx,\
driver_dinorex,\
driver_qcrayon,\
driver_driveout"

COREDEFS += -DTINY_POINTER="\
&driver_finalb,\
&driver_dondokod,\
&driver_megab,\
&driver_thundfox,\
&driver_cameltry,\
&driver_liquidk,\
&driver_gunfront,\
&driver_growl,\
&driver_runark,\
&driver_hthero,\
&driver_koshien,\
&driver_yuyugogo,\
&driver_ninjak,\
&driver_solfigtr,\
&driver_pulirula,\
&driver_deadconx,\
&driver_dinorex,\
&driver_qcrayon,\
&driver_driveout"

# uses these CPUs
CPUS+=M68000@
#CPUS+=N68000@
 CPUS+=Z80@
#CPUS+=M68705@
 
# uses these SOUNDs
#SOUNDS+=MSM5205@
#SOUNDS+=YM2151@
SOUNDS+=YM2610@
SOUNDS+=YM2610B@
#SOUNDS+=OKIM6295@
#SOUNDS+=2203@
SOUNDS+=ADPCM@
#SOUNDS+=YM2203@
#SOUNDS+=YM2610B@
#SOUNDS+=DAC@
#SOUNDS+=AY8910@
#SOUNDS+=UPD7759@
#SOUNDS+=YM2413@
#SOUNDS+=SN76496@
#SOUNDS+=YM3812@
#SOUNDS+=YM3438@
#SOUNDS+=YM2612@
#SOUNDS+=X1_010@
#SOUNDS+=CUSTOM@

fgDUMMY= $(OBJ)/drivers/taito_f2.o
	
DRVLIBS = $(OBJ)/sndhrdw/rastan.o $(OBJ)/vidhrdw/taitoic.o $(OBJ)/machine/cchip.o
	
#  MAME specific core objs $(OBJ)/driver.o
COREOBJS +=  $(OBJ)/tiny.o  $(fgDUMMY) $(DRVLIBS) 
$(OBJ)/tiny.o: src/tiny.cpp
$(OBJ)/driver.o: src/driver.cpp  
 
