
#Taito Asuka & Taito X system  only

# driver_rastan,\ driver_rainbow,\a tiny compile is without Neogeo games
COREDEFS += -DDEAST=1 -DTINY_COMPILE=1
COREDEFS += -DTINY_NAME="\
driver_cobracom,\
driver_ghostb,\
driver_srdarwin,\
driver_gondo,\
driver_oscar,\
driver_lastmiss,\
driver_shackled,\
driver_breywood,\
driver_csilver,\
driver_garyoret,\
driver_hbarrel,\
driver_baddudes,\
driver_robocop,\
driver_hippodrm,\
driver_ffantasy,\
driver_slyspy,\
driver_secretag,\
driver_midres,\
driver_tumblep,\
driver_stadhero,\
driver_twocrude,\
driver_madmotor,\
driver_cbuster,\
driver_darkseal,\
driver_supbtime,\
driver_cninja,\
driver_bouldash"

COREDEFS += -DTINY_POINTER="\
&driver_cobracom,\
&driver_ghostb,\
&driver_srdarwin,\
&driver_gondo,\
&driver_oscar,\
&driver_lastmiss,\
&driver_shackled,\
&driver_breywood,\
&driver_csilver,\
&driver_garyoret,\
&driver_hbarrel,\
&driver_baddudes,\
&driver_robocop,\
&driver_hippodrm,\
&driver_ffantasy,\
&driver_slyspy,\
&driver_secretag,\
&driver_midres,\
&driver_tumblep,\
&driver_stadhero,\
&driver_twocrude,\
&driver_madmotor,\
&driver_cbuster,\
&driver_darkseal,\
&driver_supbtime,\
&driver_cninja,\
&driver_bouldash"


# uses these CPUs
 CPUS+=M68000@
 CPUS+=N68000@
 CPUS+=Z80@
 CPUS+=M68705@
 CPUS+=H6280@
 CPUS+=M6502@
 CPUS+=HD6309@
 CPUS+=M6809@

# uses these SOUNDs
SOUNDS+=OKIM6295@
SOUNDS+=YM3812@
SOUNDS+=YM3012@

SOUNDS+=ADPCM@
SOUNDS+=YM2203@
SOUNDS+=MSM5205@
SOUNDS+=YM3526@

#SOUNDS+=DAC@
#SOUNDS+=AY8910@
#SOUNDS+=MSM5205@
SOUNDS+=YM2151@
#SOUNDS+=YM2610@
#SOUNDS+=YM2610B@
#SOUNDS+=YM2610B@
#SOUNDS+=UPD7759@
#SOUNDS+=YM2413@
#SOUNDS+=SN76496@
#SOUNDS+=YM3438@
#SOUNDS+=YM2612@
#SOUNDS+=X1_010@
#SOUNDS+=CUSTOM@

fgDUMMY=\
	$(OBJ)/drivers/dec0.o $(OBJ)/drivers/dec8.o $(OBJ)/drivers/stadhero.o $(OBJ)/drivers/madmotor.o \
	$(OBJ)/drivers/cbuster.o $(OBJ)/drivers/darkseal.o $(OBJ)/drivers/supbtime.o $(OBJ)/drivers/cninja.o $(OBJ)/drivers/tumblep.o
	
# $(OBJ)/drivers/rainbow.o $(OBJ)/machine/rainbow.o $(OBJ)/vidhrdw/rastan.o $(OBJ)/drivers/rastan.o $(OBJ)/machine/timekpr.o \$(OBJ)/machine/mb87078.o $(OBJ)/drivers/opwolf.o $(OBJ)/vidhrdw/othunder.o $(OBJ)/drivers/othunder.o \

DRVLIBS = \
	 


#  MAME specific core objs $(OBJ)/driver.o
COREOBJS +=  $(OBJ)/driver.o  $(fgDUMMY) $(DRVLIBS) 
#$(OBJ)/tiny.o 
#$(OBJ)/driver.o: src/driver.cpp  
 
