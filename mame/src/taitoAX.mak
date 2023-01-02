
#Taito Asuka & Taito X system  only

# driver_rastan,\ driver_rainbow,\a tiny compile is without Neogeo games
COREDEFS += -DTAITOAX=1 -DBUILD_YM2610B -DHAS_YM2203=1 -DHAS_YM2610B=1 -DTINY_COMPILE=1
COREDEFS += -DTINY_NAME="\
driver_spaceskr,\
driver_spacecr,\
driver_junglek,\
driver_piratpet,\
driver_alpine,\
driver_timetunl,\
driver_wwestern,\
driver_frontlin,\
driver_elevator,\
driver_tinstar,\
driver_waterski,\
driver_bioatack,\
driver_hwrace,\
driver_tetrist,\
driver_nastar,\
driver_ashura,\
driver_hitice,\
driver_rambo3,\
driver_puzbobb,\
driver_spacedx,\
driver_masterw,\
driver_silentd,\
driver_viofight,\
driver_crimec,\
driver_kikstart"

COREDEFS += -DTINY_POINTER="\
&driver_spaceskr,\
&driver_spacecr,\
&driver_junglek,\
&driver_piratpet,\
&driver_alpine,\
&driver_timetunl,\
&driver_wwestern,\
&driver_frontlin,\
&driver_elevator,\
&driver_tinstar,\
&driver_waterski,\
&driver_bioatack,\
&driver_hwrace,\
&driver_tetrist,\
&driver_nastar,\
&driver_ashura,\
&driver_hitice,\
&driver_rambo3,\
&driver_puzbobb,\
&driver_spacedx,\
&driver_masterw,\
&driver_silentd,\
&driver_viofight,\
&driver_crimec,\
&driver_kikstart"


# uses these CPUs
#CPUS+=M68000@
 CPUS+=N68000@
 CPUS+=Z80@
 CPUS+=M68705@
 
# uses these SOUNDs
SOUNDS+=MSM5205@
SOUNDS+=YM2151@
SOUNDS+=YM2610@
SOUNDS+=YM2610B@


SOUNDS+=OKIM6295@
SOUNDS+=2203@
SOUNDS+=ADPCM@
SOUNDS+=YM2203@
#SOUNDS+=YM2610B@

SOUNDS+=DAC@
SOUNDS+=AY8910@

#SOUNDS+=UPD7759@
#SOUNDS+=YM2413@
#SOUNDS+=SN76496@
#SOUNDS+=YM3812@
#SOUNDS+=YM3438@
#SOUNDS+=YM2612@

#SOUNDS+=X1_010@
#SOUNDS+=CUSTOM@

fgDUMMY=\
	 $(OBJ)/machine/tait8741.o $(OBJ)/drivers/taito_b.o $(OBJ)/drivers/rastan.o $(OBJ)/machine/arkanoid.o   $(OBJ)/drivers/taitosj.o\
	#$(OBJ)/drivers/taito_b.o $(OBJ)/drivers/taito_l.o   $(OBJ)/vidhrdw/taito_b.o $(OBJ)/vidhrdw/taito_l.o $(OBJ)/vidhrdw/taitoic.o $(OBJ)/machine/taitosj.o  \
	#$(OBJ)/vidhrdw/taito_b.o $(OBJ)/sound/ay8910.o $(OBJ)/drivers/taito_b.o $(OBJ)/vidhrdw/taitosj.o   \
	
# $(OBJ)/drivers/rainbow.o $(OBJ)/machine/rainbow.o $(OBJ)/vidhrdw/rastan.o $(OBJ)/drivers/rastan.o $(OBJ)/machine/timekpr.o \$(OBJ)/machine/mb87078.o $(OBJ)/drivers/opwolf.o $(OBJ)/vidhrdw/othunder.o $(OBJ)/drivers/othunder.o \
#$(OBJ)/machine/volfied.o $(OBJ)/vidhrdw/volfied.o $(OBJ)/drivers/volfied.o \	
#$(OBJ)/vidhrdw/wgp.o $(OBJ)/drivers/wgp.o \	
#$(OBJ)/vidhrdw/topspeed.o $(OBJ)/drivers/topspeed.o \
#$(OBJ)/vidhrdw/darius.o $(OBJ)/drivers/darius.o \
#$(OBJ)/vidhrdw/ninjaw.o $(OBJ)/drivers/ninjaw.o \
#$(OBJ)/vidhrdw/warriorb.o $(OBJ)/drivers/warriorb.o \		
#$(OBJ)/vidhrdw/slapshot.o $(OBJ)/drivers/slapshot.o  $(OBJ)/vidhrdw/taito_z.o $(OBJ)/drivers/taito_z.o \
DRVLIBS = \
	$(OBJ)/vidhrdw/taitoic.o $(OBJ)/sndhrdw/taitosnd.o \
	 $(OBJ)/machine/cchip.o \

#$(OBJ)/machine/bonzeadv.o  $(OBJ)/vidhrdw/asuka.o 	
#$(OBJ)/vidhrdw/taito_s.o	$(OBJ)/vidhrdw/taito_f2.o $(OBJ)/drivers/taito_f2.o 
#$(OBJ)/drivers/taito_x.o 
#	$(OBJ)/drivers/asuka.o \
#	$(OBJ)/vidhrdw/taito_f3.o $(OBJ)/sndhrdw/taito_f3.o $(OBJ)/drivers/taito_f3.o 
#	$(OBJ)/vidhrdw/groundfx.o $(OBJ)/drivers/groundfx.o 
#	$(OBJ)/vidhrdw/undrfire.o $(OBJ)/drivers/undrfire.o 
#	$(OBJ)/vidhrdw/gunbustr.o $(OBJ)/drivers/gunbustr.o 
#	$(OBJ)/vidhrdw/superchs.o $(OBJ)/drivers/superchs.o 
#	$(OBJ)/drivers/taitojc.o 

#	$(OBJ)/vidhrdw/taito_l.o $(OBJ)/drivers/taito_l.o 


#  MAME specific core objs $(OBJ)/driver.o
COREOBJS +=  $(OBJ)/driver.o  $(fgDUMMY) $(DRVLIBS) 
#$(OBJ)/tiny.o 
$(OBJ)/driver.o: src/driver.cpp  
 
