#
# Colem port on LibSPMP8k
#
# Copyright (C) 2006 Ludovic Jacomme (ludovic.jacomme@gmail.com)
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
#
TARGET = colem
LIBSPMP8K = ../../..
include $(LIBSPMP8K)/main.cfg
LIBGAME=../../../libgame
CC		= $(TOOLCHAIN)gcc
AS		= $(TOOLCHAIN)as
LD		= $(TOOLCHAIN)gcc
CPP		= $(TOOLCHAIN)cpp
OBJCOPY	= $(TOOLCHAIN)objcopy
MKBING	= ../../tools/bingtools/mkbing
RM		= rm -f
# Define the compiler settings here:

SOURCE    = .
LIBS      =  -lSDL -lpng -lgame -lc -lgcc -lm -lz -lnosys -lstdc++ -L../../libgame

OBJS = gp2x_psp.o Coleco.o ColEm.o SN76489.o Unix.o LibPsp.o Z80.o psp_main.o psp_sdl.o psp_sound.o psp_kbd.o psp_font.o ui1.o

INCLUDE   = -I../../../include -I../../../include/sdl -I../../../libgame -I.

CFLAGS	= -O2 -g -c $(INCLUDE) -I/mingw32/arm-none-eabi/include -nostdlib  -mcpu=arm926ej-s -msoft-float -Wno-unused-parameter -finline-functions -fomit-frame-pointer -msoft-float -fno-builtin -fno-exceptions -Wno-unused-parameter
ASFLAGS	= -mcpu=arm9

MORE_CFLAGS =  -I. -Ilibzip -DGP2X_MODE -static -I$(LIBSPMP8K)/include -I$(LIBSPMP8K)/include/SDL \
-DUNIX -DBPP16 -DLSB_FIRST -DSOUND  -DNO_STDIO_REDIRECT 

CFLAGS +=  $(MORE_CFLAGS) -fsigned-char \
    -funroll-loops -ffast-math -fomit-frame-pointer -fno-strength-reduce -finline-functions
CXXFLAGS = $(CFLAGS) $(MORE_CFLAGS) -fno-exceptions -fno-rtti

LIBS += -L$(LIBSPMP8K)/lib -lSDL -lpng -lziparm -lm -lstdc++

include $(LIBGAME)/libgame.mk
.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

.cpp.o:
	$(CXX) $(CFLAGS) -c $< -o $@

#$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LIBS) -o $(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
