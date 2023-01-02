#CPUDEFS += -DHAS_FAME=1 -DUSE_FAME_C_CORE=1 -DFAME_GLOBAL_CONTEXT  -DFAME_FETCHBITS=12 -DFAME_DATABITS=12
CPUDEFS += 
#OBJDIRS += $(OBJ)/cpu/fame $(OBJ)/cpu/m68000
#CPUOBJS += $(OBJ)/cpu/fame/f68000.o $(OBJ)/cpu/fame/famec.o
#CPUOBJS += $(OBJ)/cpu/m68000/m68kmame.o

# $(OBJ)/odx/fastmem.o $(OBJ)/odx/minimal.o
OSOBJS = $(OBJ)/odx/odx.o $(OBJ)/odx/fastmem.o $(OBJ)/odx/minimal.o \
	$(OBJ)/odx/video.o $(OBJ)/odx/blit.o \
	$(OBJ)/odx/sound.o $(OBJ)/odx/input.o $(OBJ)/odx/fileio.o \
	$(OBJ)/odx/config.o $(OBJ)/odx/fronthlp.o

FEOBJS =  $(OBJ)/odx/fastmem.o $(OBJ)/odx/minimal.o $(OBJ)/odx/odx_frontend.o

