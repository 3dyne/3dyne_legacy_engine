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
add_definitions( -Dlinux_i386 -DS_INSERVER -Dtrace_function )
# add_definitions( -DBHOSTSYSTEM=xxx -DBDATE=yyy -DBHOST=zzz -DBVERSION=000 -DDD_BTYPE=debug -DBCOMMENT="wooooot!!!11" )
include_directories( ../shared . )

set( CMAKE_CXX_FLAGS "-Wall" )

set( SRC server.cpp
	sv_main.cpp
	../shared/shock.cpp
	l_util.cpp
	l_phys.cpp
	at_player.cpp
	at_spectator.cpp
	at_item.cpp
	at_psys.cpp
	at_switch.cpp
	at_light.cpp
	at_rocket.cpp
	SV_Catapult.cpp
	SV_Teleport.cpp
	../shared/lib_queue.cpp
	../shared/lib_container.cpp
	../shared/lib_math.cpp
	../shared/lib_hobj.cpp
	../shared/lib_hmanager.cpp
	../shared/lib_parse.cpp
	../shared/lib_model.cpp
	../shared/lib_psys.cpp
	../shared/lib_pack.cpp
	../shared/shared.cpp )
	
	
add_library( sv SHARED ${SRC} )
