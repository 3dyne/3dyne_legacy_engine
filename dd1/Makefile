#-------------------------------------------------------------------------------
# 3dyne Legacy Engine GPL Source Code
# 
# Copyright (C) 1996-2012 Matthias C. Berger & Simon Berger.
# 
# This file is part of the 3dyne Legacy Engine GPL Source Code ("3dyne Legacy
# Engine Source Code").
#   
# 3dyne Legacy Engine Source Code is free software: you can redistribute it
# and/or modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation, either version 3 of the License,
# or (at your option) any later version.
# 
# 3dyne Legacy Engine Source Code is distributed in the hope that it will be
# useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
# Public License for more details.
# 
# You should have received a copy of the GNU General Public License along with
# 3dyne Legacy Engine Source Code.  If not, see
# <http://www.gnu.org/licenses/>.
# 
# In addition, the 3dyne Legacy Engine Source Code is also subject to certain
# additional terms. You should have received a copy of these additional terms
# immediately following the terms and conditions of the GNU General Public
# License which accompanied the 3dyne Legacy Engine Source Code.
# 
# Contributors:
#     Matthias C. Berger (mcb77@gmx.de) - initial API and implementation
#     Simon Berger (simberger@gmail.com) - initial API and implementation
#-------------------------------------------------------------------------------
# Generated automatically from Makefile.in by configure.

# variables:
# TARGET  -- target
# DEBUG   -- set true of false

# supported targets:
# linux
# win32 (Xmingw32)

ifeq ($(target), )
target := $(shell uname | tr '[A-Z]' '[a-z]')
endif

ifeq ($(target), linux)
CC	= gcc
LD	= ld

TFLAGS	=
LIBS = -lgii -lglide2x
CFLAGS = -Wall -pipe
hostsystem	= linux.i386

ifeq ($(debug), true )
OPTIMIZE = -g
else
OPTIMIZE = -O6 -mpentium -funroll-loops -fomit-frame-pointer -ffast-math
endif
CFLAGS += $(OPTIMIZE)
endif

ifeq ($(target), win32) 
AS	= i586-mingw32-as
TFLAGS	= -b i586-mingw32
CC 	= gcc
LD 	= i586-mingw32-ld
DLLTOOL	= i586-mingw32-dlltool
LIBS = -liberty -lm -luser32 -lcrtdll -lglide2x -lfxmisc
hostsystem	= win32.x86
ifeq ($(debug), true )
OPTIMIZE = -g
else
OPTIMIZE = -O6 -mpentium -funroll-loops -fomit-frame-pointer -ffast-math
endif
CFLAGS += $(OPTIMIZE)
CFLAGS += -D__MSC__
endif

CC += $(TFLAGS)
LD += $(TFLAGS)



COPY = cp
RM = rm -f
INCLUDES = -I. -I$(TDDIR)/shared
DEFINES = 

OBJ = 	$(hostsystem)/l_main.o \
	$(hostsystem)/l_util.o \
	$(hostsystem)/l_tables.o \
	$(hostsystem)/shock.o \
	$(hostsystem)/vec.o \
	$(hostsystem)/l_player.o \
	$(hostsystem)/l_phys.o \
	$(hostsystem)/l_spring.o

#LIBDL = @HAVE_LIBDL@

all: $(hostsystem)/depend $(OBJ) $(hostsystem)/life.so

$(hostsystem)/depend: *.c *.h
	makedepend $(INCLUDES) -I/usr/local/include -p$(hostsystem)/ -f- *.c > $(hostsystem)/depend 2> /dev/null

clean:
	$(RM) $(hostsystem)/*.o $(hostsystem)/*.so
	$(RM) $(hostsystem)/depend

./$(hostsystem)/%.o: %.c
	$(CC) $(INCLUDES) $(DEFINES) $(CFLAGS) -c $< -o $@


linux.i386/life.so: $(OBJ)
	$(RM) $@
	$(CC) -shared -o $@ $(OBJ) -lm
	ldd -r $@

win32.x86/life.so: $(OBJ)
	$(RM) $@
	$(LD) -s --base-file base.tmp --dll -o $(hostsystem)/life.so $(OBJ) $(LIBS)
	$(DLLTOOL) --def $(hostsystem)/life.def --output-exp life.exp --dllname $(hostsystem)/life.so --base-file base.tmp --as $(AS)
	$(LD) -s --base-file base.tmp life.exp -dll -o $(hostsystem)/life.so $(OBJ) $(LIBS)
	$(DLLTOOL) --dllname $(hostsystem)/life.so --def $(hostsystem)/life.def \
		--base-file base.tmp --output-exp life.exp --as $(AS)
		
	$(LD) life.exp --dll -o $(hostsystem)/life.so  $(OBJ) $(LIBS)


include $(hostsystem)/depend
