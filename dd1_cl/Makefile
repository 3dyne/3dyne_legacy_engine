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


build_date = `date`
build_host = `uname -n`
build_version = 0.1
build_type = tech demo
build_comment = If you have it be glad!

ifeq ($(target), linux)
#CC	= gcc -V2.7.2.3
CPP	= cpp 
CC = gcc  -fPIC
LD	= ld
#LD = ld

TFLAGS	=
LIBS =
CFLAGS = -Wall -pipe
hostsystem	= linux.i386

ifeq ($(debug), true)
OPTIMIZE = -g
else
OPTIMIZE = -O6 -funroll-loops -fomit-frame-pointer -ffast-math
endif
CFLAGS += $(OPTIMIZE)
endif


ifeq ($(target), irix)
TFLAGS  =
CC      = cc
LD	= ld
TFLAGS	= 
CPP	= cpp

LIBS =
CFLAGS = -O2 -Xansi -Xcpluscomm
#CFLAGS = -O2 -L/usr/lib

DEFINES = -D irix_mips

hostsystem      = irix.mips
ifeq ($(debug), true)
OPTIMIZE = -g
else
OPTIMIZE = -O2
endif
CFLAGS += $(OPTIMIZE)
endif


ifeq ($(target), win32) 
AS	= i686-pc-mingw32-as
TFLAGS =
#	= -b i586-mingw32
CC 	= i686-pc-mingw32-gcc -m32
CPP	= cpp
LD 	= i686-pc-mingw32-ld
DLLTOOL	= i686-pc-mingw32-dlltool
LIBS = -lm -luser32 -lmsvcrt
hostsystem	= win32.x86
SYS_OBJ	= $(hostsystem)/win32.o
ifeq ($(debug), true)
OPTIMIZE = -g
else
OPTIMIZE = -O3  -funroll-loops -fomit-frame-pointer -ffast-math
endif
CFLAGS += $(OPTIMIZE)
CFLAGS += -D__MSC__
endif

CC += $(TFLAGS)
LD += $(TFLAGS)



COPY = cp
RM = rm -f
INCLUDES = -I. -I../shared
DEFINES += -D S_INCLIENT -D trace_functions

BUILD_DEFINES = -D BHOSTSYSTEM="\"$(hostsystem)\"" -D BDATE="\"$(build_date)\"" -D BHOST="\"$(build_host)\"" -D BVERSION="\"$(build_version)\"" -D DD_BTYPE="\"$(build_type)\"" -D BCOMMENT="\"$(build_comment)\"" 

OBJ = 	$(hostsystem)/client.o \
	$(hostsystem)/cl_main.o \
	$(hostsystem)/cl_util.o \
	$(hostsystem)/cl_proxy.o \
	$(hostsystem)/cl_gfx.o \
	$(hostsystem)/cl_model.o \
	$(hostsystem)/shock.o \
\
	$(hostsystem)/hud_player.o \
\
	$(hostsystem)/cl_atplayer.o \
	$(hostsystem)/cl_atview.o \
	$(hostsystem)/cl_atitem.o \
	$(hostsystem)/cl_atpsys.o \
	$(hostsystem)/cl_atswitch.o \
	$(hostsystem)/cl_atlight.o \
	$(hostsystem)/cl_atrocket.o \
	$(hostsystem)/CL_Explosion.o \
	$(hostsystem)/CL_Debris.o \
\
	$(hostsystem)/lib_queue.o \
	$(hostsystem)/lib_container.o \
	$(hostsystem)/lib_math.o \
	$(hostsystem)/lib_hobj.o \
	$(hostsystem)/lib_hmanager.o \
	$(hostsystem)/lib_parse.o \
	$(hostsystem)/lib_model.o \
	$(hostsystem)/lib_psys.o \
	$(hostsystem)/lib_pack.o \
	$(hostsystem)/shared.o

DLLNAME = cl

#LIBDL = @HAVE_LIBDL@

all: prepare_build $(hostsystem)/depend $(OBJ) $(hostsystem)/$(DLLNAME).so

prepare_build:
	@echo building for target $(hostsystem)
	if [  -f $(hostsystem)/version.h ]; then \
		echo $(hostsystem)/version.h found; \
	else \
		echo build $(hostsystem)/version.h; \
		echo > version.h.in; \
		$(CPP) -undef -dM $(BUILD_DEFINES) version.h.in $(hostsystem)/version.h; \
	fi;

	ln -f -s $(hostsystem)/version.h version.h

$(hostsystem)/depend: *.c *.h
	makedepend $(INCLUDES) -I/usr/local/include -p$(hostsystem)/ -f- *.c > $(hostsystem)/depend 2> /dev/null
	makedepend $(INCLUDES) -I/usr/local/include -p$(hostsystem)/ -f- ../shared/*.c >> $(hostsystem)/depend

clean:
	$(RM) $(hostsystem)/*.o $(hostsystem)/*.so
	$(RM) $(hostsystem)/depend

./$(hostsystem)/%.o: %.c
	$(CC) $(INCLUDES) $(DEFINES) $(CFLAGS) -c $< -o $@

./$(hostsystem)/%.o: ../shared/%.c
	$(CC) $(INCLUDES) $(DEFINES) $(CFLAGS) -c $< -o $@


linux.i386/$(DLLNAME).so: $(OBJ)
	$(RM) $@
	$(CC) -shared -o $@ $(OBJ) -lm
	ldd -r $@

irix.mips/$(DLLNAME).so: $(OBJ)
	$(RM) $@
	$(CC) -shared -o $@ $(OBJ) -lm


win32.x86/$(DLLNAME).so: $(OBJ) $(SYS_OBJ)
	$(RM) $@
	$(CC) -shared -o $@ $(OBJ) -lm

#	$(RM) $@
#	$(LD) -e _DllStartup@12 -s --base-file base.tmp --dll -o $(hostsystem)/$(DLLNAME).so $(OBJ) $(SYS_OBJ) $(LIBS)
#
#	$(DLLTOOL) --def def.def --output-exp exp.exp --dllname $(hostsystem)/$(DLLNAME).so --base-file base.tmp --as $(AS)
#
#	$(LD) -e _DllStartup@12 -s --base-file base.tmp exp.exp -dll -o $(hostsystem)/$(DLLNAME).so $(OBJ) $(SYS_OBJ) $(LIBS)
#
#	$(DLLTOOL) --dllname $(hostsystem)/$(DLLNAME).so --def def.def \
#		--base-file base.tmp --output-exp exp.exp --as $(AS)
#
#	$(LD) -e _DllStartup@12 exp.exp --dll -o $(hostsystem)/$(DLLNAME).so  $(OBJ) $(SYS_OBJ) $(LIBS)

#	$(DLLTOOL) --def $(hostsystem)/life.def --output-exp life.o --output-lib $(hostsystem)/life.a --as $(AS)

#	$(LD) -o $(hostsystem)/life.so life.o $(OBJ) $(LIBS)

include $(hostsystem)/depend
