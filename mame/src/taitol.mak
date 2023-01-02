
#Taito Asuka & Taito X system  only

COREDEFS += -DTAITOL=1 -DTINY_COMPILE=1
COREDEFS += -DTINY_NAME="\
driver_raimais,\
driver_fhawk,\
driver_champwr,\
driver_kurikint,\
driver_plotting,\
driver_puzznic,\
driver_horshoes,\
driver_palamed,\
driver_cachat,\
driver_tubeit,\
driver_cubybop,\
driver_plgirls,\
driver_plgirls2"

COREDEFS += -DTINY_POINTER="\
&driver_raimais,\
&driver_fhawk,\
&driver_champwr,\
&driver_kurikint,\
&driver_plotting,\
&driver_puzznic,\
&driver_horshoes,\
&driver_palamed,\
&driver_cachat,\
&driver_tubeit,\
&driver_cubybop,\
&driver_plgirls,\
&driver_plgirls2"


# uses these CPUs
#CPUS+=M68000@
# CPUS+=N68000@
 CPUS+=Z80@
#CPUS+=M68705@
 
SOUNDS+=YM2610@
#SOUNDS+=YM2610B@


#SOUNDS+=OKIM6295@
SOUNDS+=2203@
SOUNDS+=ADPCM@
SOUNDS+=YM2203@
#SOUNDS+=YM2610B@

#SOUNDS+=DAC@
#SOUNDS+=AY8910@
#SOUNDS+=CUSTOM@

fgDUMMY= $(OBJ)/drivers/taito_l.o 
	#$(OBJ)/vidhrdw/taito_l.o
	#$(OBJ)/vidhrdw/taitoic.o 
	#$(OBJ)/vidhrdw/taito_b.o $(OBJ)/sound/ay8910.o $(OBJ)/drivers/taito_b.o $(OBJ)/vidhrdw/taitosj.o   
	
DRVLIBS = $(OBJ)/vidhrdw/taitoic.o 
#$(OBJ)/sndhrdw/taitosnd.o 
	 #$(OBJ)/machine/cchip.o \

#  MAME specific core objs $(OBJ)/driver.o
COREOBJS += $(OBJ)/tiny.o  $(fgDUMMY) $(DRVLIBS) 
$(OBJ)/tiny.o: src/tiny.cpp 
$(OBJ)/driver.o: src/driver.cpp  
 
