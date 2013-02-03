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
cmake_minimum_required( VERSION 2.8 )

add_subdirectory( dd1_cl )
add_subdirectory( dd1_sv )

if(WIN32)
  add_subdirectory( 3dyne_extlibs )
endif()
#set( CMAKE_CXX_FLAGS "-Wall" )
#set( CMAKE_C_FLAGS "-Wall" )



if(WIN32)
  set( CMAKE_CXX_FLAGS "-D_SCL_SECURE_NO_WARNINGS /INCREMENTAL:NO" )

  include_directories( 3dyne_extlibs/jpeg-6b 3dyne_extlibs/bzip2-1.0.6 3dyne_extlibs/SDL-1.2.15/include )
  #if( ${SDL_ROOT} )
   # include_directories( ${SDL_ROOT}/include )
  #  LINK_DIRECTORIES( ${SDL_ROOT}/lib/x86 )	
  #endif()

  add_definitions( -Dwin32_x86 -DGL_LINKED -D__USE_IBASE -DS_INGAME -DBHOSTSYSTEM="win32.x86") 
  
  #set( SS_SRC r_wgl.cpp i_win32.cpp wgl_service.cpp i_vkmap.cpp )
  set( SS_SRC r_sdl )
else()

  add_definitions( -Dlinux_i386 -DGL_LINKED -D__USE_IBASE -DS_INGAME -DBHOSTSYSTEM="linux.i386") 
  set( CMAKE_CXX_FLAGS "-Wall" )
  
#   set( SS_SRC r_glx.cpp i_x11.cpp glx_service.cpp )
  set( SS_SRC r_sdl )
endif()


# set( CMAKE_C_COMPILER g++ )
include_directories( shared . r_fake r_gen_gl)


#add_definitions( -Dlinux_i386 -D__USE_IBASE -DS_INGAME ) 

# add_definitions( -DBHOSTSYSTEM=xxx -DBDATE=yyy -DBHOST=zzz -DBVERSION=000 -DDD_BTYPE=debug -DBCOMMENT="wooooot!!!11" )

#



set( ENGINE_SRC g_main.cpp
        baked_hobj.cpp
	la_service.cpp
	la_varviews.cpp
	g_resources.cpp
	res_tga.cpp
	res_gltex.cpp
	res_gltex_jpeg.cpp
	res_submd.cpp
	res_moves.cpp
	res_sound.cpp
	res_lump.cpp
	res_mp3.cpp
	g_gl.cpp
	g_map.cpp
	g_model.cpp
	g_psys.cpp
	db_lightmap.cpp
	db_shape.cpp
	db_uoc.cpp
	sh_input.cpp
	sh_parser.cpp
	sh_alias.cpp
	sh_visual.cpp
	sh_menu.cpp
	snd_deamon.cpp
	snd_intern.cpp
	mus_mp3.cpp
	sst_sdl.cpp
	ca_service.cpp
	ib_service.cpp
	shared/shock.cpp
	sys_time.cpp
	sys_console.cpp
	l_api.cpp
	a_hud.cpp
	sys_dll.cpp
	sys_env.cpp
	gl_service.cpp
	r_gltex.cpp
	r_draw2d.cpp
	r_glbackend.cpp
	gc_service.cpp
	gc_util.cpp
	gc_ccl.cpp
	gc_csv.cpp
	g_console.cpp
	g_server.cpp
	g_client.cpp
	g_time.cpp
	g_library.cpp
	shared/shared.cpp
	shared/lib_math.cpp
	shared/lib_bezier.cpp
	shared/lib_container.cpp
	shared/lib_tag.cpp
	shared/lib_hobj.cpp
	shared/lib_model.cpp
	shared/lib_psys.cpp
	shared/lib_hmanager.cpp
	shared/lib_pack.cpp
	shared/lib_uoc.cpp
	shared/lib_queue.cpp
	shared/lib_parse.cpp
	shared/lib_digest.cpp
	u_expand.cpp
	u_va.cpp
	u_token.cpp
	u_hobj.cpp
	u_pixel.cpp
	u_mempack.cpp
	tfunc.cpp
	l_wrap.cpp
	)
	
set( R_FAKE_SRC r_fake/g_blockmap.cpp
	r_fake/g_bmlink.cpp
	r_fake/g_bmobjects.cpp
	r_fake/g_ccmap3.cpp
	r_fake/g_map3cache.cpp
	r_fake/g_trace.cpp
	r_fake/r_data.cpp
	r_fake/r_frustum.cpp
	r_fake/r_hvis.cpp
	r_fake/r_hvisclip.cpp
	r_fake/r_initdata.cpp
	r_fake/r_initgl.cpp
	r_fake/r_math.cpp
	r_fake/r_misc.cpp
	r_fake/r_plane.cpp
	r_fake/r_screendump.cpp
	r_fake/r_sky.cpp
	r_fake/r_sprite.cpp
	r_fake/r_videoscape.cpp
	r_fake/render3.cpp
	r_fake/r_shapes.cpp
	r_fake/r_light.cpp
	r_fake/r_light_sw.cpp
	r_fake/r_light_gl.cpp
	r_fake/r_lightmap.cpp
	r_fake/r_shpctrljobs.cpp
	r_fake/J_AmbientColor.cpp
	r_fake/J_ModulateAlpha.cpp
	r_fake/J_ModulateColor.cpp
	r_fake/J_ShiftTexCoord.cpp
	r_fake/J_EnvMap.cpp
	r_fake/J_DetailMap.cpp
	r_fake/J_SurfaceDeform.cpp )
	
	
set( R_GEN_GL_SRC r_gen_gl/r_real.cpp
	r_gen_gl/shape_instance.cpp
	r_gen_gl/shape_util.cpp
	r_gen_gl/shape_db.cpp
	r_gen_gl/layer_instance.cpp
	r_gen_gl/layer.cpp
	r_gen_gl/layers.cpp
	r_gen_gl/tf2d.cpp
	r_gen_gl/mesh.cpp
	r_gen_gl/tess.cpp
	r_gen_gl/tess_curved_face.cpp
	r_gen_gl/tess_simple_face.cpp
	r_gen_gl/tess_curved_surface.cpp
	r_gen_gl/tess_meshtile.cpp
	r_gen_gl/tess_glmesh.cpp
	r_gen_gl/trify_generic_polygon.cpp
	r_gen_gl/imgcache.cpp
	r_gen_gl/unitex.cpp
	r_gen_gl/shader.cpp
	)


add_executable( dd ${ENGINE_SRC} ${R_FAKE_SRC} ${R_GEN_GL_SRC} ${SS_SRC} ) 
#target_link_libraries( dd dl m X11  Xxf86dga Xxf86vm Xext pthread jpeg SDL bz2 )
add_executable( disk_hash disk_hash.cpp )

if(WIN32)
  
  target_link_libraries( dd jpeg bz2 ws2_32 opengl32 winmm SDL  )
else()
  target_link_libraries( dd dl m X11  Xxf86dga Xxf86vm Xext pthread jpeg SDL bz2 GL )
endif()