# Define the applications properties here:

LIB_NAME = SDLg
DINGOO_SDK=../../../../..


# Define the compiler settings here:

CPP       = arm-none-eabi-g++
CC        = arm-none-eabi-gcc
AR        = arm-none-eabi-ar

SOURCE    = \
	. \
	./libc_ext \
	./src \
	./src/audio \
	./src/audio/dingoo \
	./src/cdrom \
	./src/cdrom/dummy \
	./src/cpuinfo \
	./src/events \
	./src/file \
	./src/joystick \
	./src/joystick/dummy \
	./src/loadso/dummy \
	./src/main/dummy \
	./src/stdlib \
	./src/timer \
	./src/timer/dummy \
	./src/timer/dingoo \
	./src/thread \
	./src/thread/generic \
	./src/video \
	./src/video/dummy \
	./src/video/slcd \
	./SDL_image \
	./SDL_ttf \
	./SDL_mixer \

INCLUDE   = -I. -I./include -I$(DINGOO_SDK)/include -I$(DINGOO_SDK)/include/png -I$(DINGOO_SDK)/include/zlib -I$(DINGOO_SDK)/include/mikmod

SDL_MIXER = -DWAV_MUSIC -DOGG_MUSIC -DOGG_USE_TREMOR -DMOD_MUSIC
W_OPTS    = -Wall -Wextra -finline-functions -fomit-frame-pointer -msoft-float -fno-builtin -fno-exceptions -Wno-unused-parameter
CPP_OPTS  =  -O3 $(INCLUDE) -mcpu=arm926ej-s -msoft-float -fno-pic $(W_OPTS)  -c
CC_OPTS   =  -O3 $(INCLUDE) -mcpu=arm926ej-s -msoft-float -fno-pic $(W_OPTS) $(SDL_MIXER) -c
CC_OPTS_A = $(CC_OPTS) -D_ASSEMBLER_

LIB_PATH  = $(DINGOO_SDK)/libgame



# Find all source files

SRC_CPP = $(foreach dir, $(SOURCE), $(wildcard $(dir)/*.cpp))
SRC_C   = $(foreach dir, $(SOURCE), $(wildcard $(dir)/*.c))
SRC_H_INC = $(foreach dir, $(SOURCE), $(wildcard $(dir)/include/*.h))
SRC_H_TTF = ./SDL_ttf/SDL_ttf.h
SRC_H_IMG = ./SDL_image/SDL_image.h
SRC_H_MIX = ./SDL_mixer/SDL_mixer.h
SRC_H_DRV = ./SDL_draw/SDL_draw.h

SRC_H   = $(SRC_H_INC) $(SRC_H_TTF) $(SRC_H_IMG) $(SRC_H_MIX) $(SRC_H_DRV)
SRC_S   = $(foreach dir, $(SOURCE), $(wildcard $(dir)/*.S))
OBJ_CPP = $(patsubst %.cpp, %.o, $(SRC_CPP))
OBJ_C   = $(patsubst %.c, %.o, $(SRC_C))
OBJ_S   = $(patsubst %.S, %.o, $(SRC_S))
OBJ     = $(OBJ_CPP) $(OBJ_C) $(OBJ_S)

# Debug
#$(warning $(OBJ_C))
#$(warning $(SRC_H))

# Compile rules.

.PHONY : all

all : $(LIB_PATH)/lib$(LIB_NAME).a

$(LIB_PATH)/lib$(LIB_NAME).a : $(OBJ)
#	$(AR) rcs $(LIB_PATH)/lib$(LIB_NAME).a *.o
	$(AR) rcs $(LIB_PATH)/lib$(LIB_NAME).a $^
	make headers

$(OBJ_CPP) : %.o : %.cpp
	$(CPP) $(CPP_OPTS) -o $@ $<

$(OBJ_C) : %.o : %.c
	$(CC) $(CC_OPTS) -o $@ $<

$(OBJ_S) : %.o : %.S
	$(CC) $(CC_OPTS_A) -o $@ $<



# Clean rules

.PHONY : clean

clean :
	rm -f $(OBJ)
	rm -f $(LIB_PATH)/lib$(LIB_NAME).a
	#rm -rf $(DINGOO_SDK)/include/$(LIB_NAME)



# Header rules

.PHONY: headers

headers:
	#rm -rf $(DINGOO_SDK)/include/$(LIB_NAME)
	mkdir $(DINGOO_SDK)/include/$(LIB_NAME)
	cp $(SRC_H) $(DINGOO_SDK)/include/$(LIB_NAME)
