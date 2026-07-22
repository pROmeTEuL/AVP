# Aliens vs Predator Linux - http://icculus.org/avp/
# CMake 2.8 project
CMAKE_MINIMUM_REQUIRED(VERSION 3.8)

# default to Release
IF(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    MESSAGE(STATUS "No build type specified, defaulting to Release")
    SET(CMAKE_BUILD_TYPE Release CACHE STRING "Build type; one of: Debug Release RelWithDebInfo MinSizeRel" FORCE)
    SET_PROPERTY(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
ENDIF(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)

PROJECT(avp)

# required dependencies
INCLUDE(FindSDL2.cmake)
INCLUDE(FindSDL)
INCLUDE(FindOpenGL)
INCLUDE(FindOpenAL)

# Use SDL 1.2 if it is installed, else use SDL 2.0.
IF(NOT SDL_TYPE)
    SET(SDL_TYPE AUTO CACHE STRING "SDL Version; one of: AUTO SDL SDL2")
    SET_PROPERTY(CACHE SDL_TYPE PROPERTY STRINGS "AUTO" "SDL" "SDL2")
ENDIF(NOT SDL_TYPE)

IF(NOT SDL_TYPE STREQUAL "AUTO" AND NOT SDL_TYPE STREQUAL "SDL" AND NOT SDL_TYPE STREQUAL "SDL2")
    MESSAGE(FATAL_ERROR "Invalid SDL_TYPE setting ${SDL_TYPE}; must be one of AUTO SDL SDL2")
ENDIF(NOT SDL_TYPE STREQUAL "AUTO" AND NOT SDL_TYPE STREQUAL "SDL" AND NOT SDL_TYPE STREQUAL "SDL2")

IF(SDL_TYPE STREQUAL "AUTO")
    IF(SDL_FOUND)
	MESSAGE(STATUS "SDL 1.2 found; using that.")
        SET(SDL_TYPE "SDL")
    ENDIF(SDL_FOUND)
ENDIF(SDL_TYPE STREQUAL "AUTO")

IF(SDL_TYPE STREQUAL "AUTO")
    IF(SDL2_FOUND)
        MESSAGE(STATUS "SDL 2.0 found; using that.")
        SET(SDL_TYPE "SDL2")
    ENDIF(SDL2_FOUND)
ENDIF(SDL_TYPE STREQUAL "AUTO")

IF(SDL_TYPE STREQUAL "AUTO")
  MESSAGE(FATAL_ERROR "SDL 1.2 or SDL 2.0 is required but CMake couldn't find it.")
ENDIF(SDL_TYPE STREQUAL "AUTO")

IF(SDL_TYPE STREQUAL "SDL")
    IF(NOT SDL_FOUND)
        MESSAGE(FATAL_ERROR "SDL 1.2 was requested but CMake couldn't find it.")
    ENDIF(NOT SDL_FOUND)
ENDIF(SDL_TYPE STREQUAL "SDL")

IF(SDL_TYPE STREQUAL "SDL2")
    IF(NOT SDL2_FOUND)
        MESSAGE(FATAL_ERROR "SDL 2.0 was requested but CMake couldn't find it.")
    ENDIF(NOT SDL2_FOUND)
    MESSAGE(WARNING "SDL 2.0 support is EXPERIMENTAL and INCOMPLETE.")
ENDIF(SDL_TYPE STREQUAL "SDL2")

IF(NOT OPENGL_FOUND)
    MESSAGE(FATAL_ERROR "OpenGL is required but CMake couldn't find it")
ENDIF(NOT OPENGL_FOUND)

IF(NOT OPENAL_FOUND)
    MESSAGE(FATAL_ERROR "OpenAL is required but CMake couldn't find it")
ENDIF(NOT OPENAL_FOUND)

# Sources and headers are listed explicitly (not globbed), grouped by
# folder and sorted by name within each folder, so IDEs (Qt Creator,
# CLion, Visual Studio, ...) present the full project tree in a sane,
# deterministic order. Headers are included purely so IDEs index them;
# only the .cpp files below actually get compiled.
#
# A handful of .cpp files under src/ are deliberately left out of this
# list: legacy/unported Win95 code (alternate DirectPlay networking, the
# old D3D immediate-mode renderer, alt-tab surface tracking, etc.) that
# predates the OpenGL/SDL port and was never part of the build.
SET(source
    src/avp/ai_sight.cpp
    src/avp/ai_sight.h
    src/avp/avpview.cpp
    src/avp/avpview.h
    src/avp/bh_agun.cpp
    src/avp/bh_agun.h
    src/avp/bh_ais.cpp
    src/avp/bh_ais.h
    src/avp/bh_alien.cpp
    src/avp/bh_alien.h
    src/avp/bh_binsw.cpp
    src/avp/bh_binsw.h
    src/avp/bh_cable.cpp
    src/avp/bh_cable.h
    src/avp/bh_corpse.cpp
    src/avp/bh_corpse.h
    src/avp/bh_deathvol.cpp
    src/avp/bh_deathvol.h
    src/avp/bh_debri.cpp
    src/avp/bh_debri.h
    src/avp/bh_dummy.cpp
    src/avp/bh_dummy.h
    src/avp/bh_fan.cpp
    src/avp/bh_fan.h
    src/avp/bh_far.cpp
    src/avp/bh_far.h
    src/avp/bh_fhug.cpp
    src/avp/bh_fhug.h
    src/avp/bh_gener.cpp
    src/avp/bh_gener.h
    src/avp/bh_ldoor.cpp
    src/avp/bh_ldoor.h
    src/avp/bh_lift.cpp
    src/avp/bh_lift.h
    src/avp/bh_light.cpp
    src/avp/bh_light.h
    src/avp/bh_lnksw.cpp
    src/avp/bh_lnksw.h
    src/avp/bh_ltfx.cpp
    src/avp/bh_ltfx.h
    src/avp/bh_marin.cpp
    src/avp/bh_marin.h
    src/avp/bh_mission.cpp
    src/avp/bh_mission.h
    src/avp/bh_near.cpp
    src/avp/bh_near.h
    src/avp/bh_paq.h
    src/avp/bh_pargen.cpp
    src/avp/bh_pargen.h
    src/avp/bh_plachier.cpp
    src/avp/bh_plachier.h
    src/avp/bh_plift.cpp
    src/avp/bh_plift.h
    src/avp/bh_pred.cpp
    src/avp/bh_pred.h
    src/avp/bh_queen.cpp
    src/avp/bh_queen.h
    src/avp/bh_rubberduck.cpp
    src/avp/bh_rubberduck.h
    src/avp/bh_selfdest.cpp
    src/avp/bh_selfdest.h
    src/avp/bh_snds.cpp
    src/avp/bh_snds.h
    src/avp/bh_spcl.cpp
    src/avp/bh_spcl.h
    src/avp/bh_swdor.cpp
    src/avp/bh_swdor.h
    src/avp/bh_track.cpp
    src/avp/bh_track.h
    src/avp/bh_types.cpp
    src/avp/bh_types.h
    src/avp/bh_videoscreen.cpp
    src/avp/bh_videoscreen.h
    src/avp/bh_waypt.cpp
    src/avp/bh_waypt.h
    src/avp/bh_weap.cpp
    src/avp/bh_weap.h
    src/avp/bh_xeno.cpp
    src/avp/bh_xeno.h
    src/avp/bonusabilities.cpp
    src/avp/bonusabilities.h
    src/avp/cconvars.cpp
    src/avp/cdtrackselection.cpp
    src/avp/cdtrackselection.h
    src/avp/cheatmodes.cpp
    src/avp/cheatmodes.h
    src/avp/comp_map.cpp
    src/avp/comp_shp.cpp
    src/avp/comp_shp.h
    src/avp/consolelog.cpp
    src/avp/consolelog.hpp
    src/avp/davehook.cpp
    src/avp/davehook.h
    src/avp/deaths.cpp
    src/avp/decal.cpp
    src/avp/decal.h
    src/avp/detaillevels.cpp
    src/avp/detaillevels.h
    src/avp/dynamics.cpp
    src/avp/dynamics.h
    src/avp/dynblock.cpp
    src/avp/dynblock.h
    src/avp/equates.h
    src/avp/equipmnt.cpp
    src/avp/equipmnt.h
    src/avp/extents.cpp
    src/avp/extents.h
    src/avp/game.cpp
    src/avp/game.h
    src/avp/game_statistics.cpp
    src/avp/game_statistics.h
    src/avp/gamecmds.cpp
    src/avp/gamedef.h
    src/avp/gamevars.cpp
    src/avp/hmodel.cpp
    src/avp/hmodel.h
    src/avp/hud.cpp
    src/avp/hud.h
    src/avp/huddefs.h
    src/avp/inventry.cpp
    src/avp/inventry.h
    src/avp/langenum.h
    src/avp/language.cpp
    src/avp/language.h
    src/avp/lighting.cpp
    src/avp/lighting.h
    src/avp/load_shp.cpp
    src/avp/load_shp.h
    src/avp/los.cpp
    src/avp/los.h
    src/avp/ltfx_exp.h
    src/avp/mempool.cpp
    src/avp/mempool.h
    src/avp/messagehistory.cpp
    src/avp/messagehistory.h
    src/avp/missions.cpp
    src/avp/missions.hpp
    src/avp/movement.cpp
    src/avp/paintball.cpp
    src/avp/paintball.h
    src/avp/particle.cpp
    src/avp/particle.h
    src/avp/pfarlocs.cpp
    src/avp/pfarlocs.h
    src/avp/pheromon.cpp
    src/avp/pheromon.h
    src/avp/player.cpp
    src/avp/player.h
    src/avp/pmove.cpp
    src/avp/pmove.h
    src/avp/projfont.h
    src/avp/projtext.h
    src/avp/psnd.cpp
    src/avp/psnd.h
    src/avp/psndproj.cpp
    src/avp/psndproj.h
    src/avp/pvisible.cpp
    src/avp/pvisible.h
    src/avp/savegame.cpp
    src/avp/savegame.h
    src/avp/scream.cpp
    src/avp/scream.h
    src/avp/secstats.cpp
    src/avp/sequnces.h
    src/avp/sfx.cpp
    src/avp/sfx.h
    src/avp/shapes/cube.cpp
    src/avp/stratdef.cpp
    src/avp/stratdef.h
    src/avp/support/command.hpp
    src/avp/support/consbind.cpp
    src/avp/support/consbind.hpp
    src/avp/support/consbtch.cpp
    src/avp/support/consbtch.hpp
    src/avp/support/coordstr.cpp
    src/avp/support/coordstr.hpp
    src/avp/support/daemon.cpp
    src/avp/support/daemon.h
    src/avp/support/dcontext.hpp
    src/avp/support/indexfnt.cpp
    src/avp/support/indexfnt.hpp
    src/avp/support/ourbool.h
    src/avp/support/r2base.cpp
    src/avp/support/r2base.h
    src/avp/support/r2pos666.cpp
    src/avp/support/r2pos666.hpp
    src/avp/support/reflist.cpp
    src/avp/support/reflist.hpp
    src/avp/support/refobj.cpp
    src/avp/support/refobj.hpp
    src/avp/support/rentrntq.cpp
    src/avp/support/rentrntq.h
    src/avp/support/scstring.cpp
    src/avp/support/scstring.hpp
    src/avp/support/strtab.cpp
    src/avp/support/strtab.hpp
    src/avp/support/strutil.cpp
    src/avp/support/strutil.h
    src/avp/support/tallfont.hpp
    src/avp/support/trig666.cpp
    src/avp/support/trig666.hpp
    src/avp/support/wrapstr.cpp
    src/avp/support/wrapstr.hpp
    src/avp/targeting.cpp
    src/avp/targeting.h
    src/avp/track.cpp
    src/avp/track.h
    src/avp/triggers.cpp
    src/avp/triggers.h
    src/avp/weapons.cpp
    src/avp/weapons.h
    src/avp/win95/avpchunk.cpp
    src/avp/win95/avpchunk.hpp
    src/avp/win95/avpreg.hpp
    src/avp/win95/bmp2.h
    src/avp/win95/cheat.cpp
    src/avp/win95/cheat.h
    src/avp/win95/chtcodes.cpp
    src/avp/win95/d3d_hud.cpp
    src/avp/win95/d3d_hud.h
    src/avp/win95/d3d_render.h
    src/avp/win95/datatype.h
    src/avp/win95/ddplat.cpp
    src/avp/win95/dp_func.h
    src/avp/win95/dplayext.h
    src/avp/win95/endianio.cpp
    src/avp/win95/endianio.h
    src/avp/win95/ffread.cpp
    src/avp/win95/ffread.hpp
    src/avp/win95/ffstdio.cpp
    src/avp/win95/ffstdio.h
    src/avp/win95/font.h
    src/avp/win95/frontend/avp_envinfo.cpp
    src/avp/win95/frontend/avp_envinfo.h
    src/avp/win95/frontend/avp_intro.cpp
    src/avp/win95/frontend/avp_intro.h
    src/avp/win95/frontend/avp_menudata.cpp
    src/avp/win95/frontend/avp_menudata.h
    src/avp/win95/frontend/avp_menugfx.hpp
    src/avp/win95/frontend/avp_menus.cpp
    src/avp/win95/frontend/avp_menus.h
    src/avp/win95/frontend/avp_mp_config.cpp
    src/avp/win95/frontend/avp_mp_config.h
    src/avp/win95/frontend/avp_userprofile.cpp
    src/avp/win95/frontend/avp_userprofile.h
    src/avp/win95/gadgets/ahudgadg.cpp
    src/avp/win95/gadgets/ahudgadg.hpp
    src/avp/win95/gadgets/conscmnd.cpp
    src/avp/win95/gadgets/conscmnd.hpp
    src/avp/win95/gadgets/conssym.cpp
    src/avp/win95/gadgets/conssym.hpp
    src/avp/win95/gadgets/consvar.cpp
    src/avp/win95/gadgets/consvar.hpp
    src/avp/win95/gadgets/gadget.cpp
    src/avp/win95/gadgets/gadget.h
    src/avp/win95/gadgets/hudgadg.cpp
    src/avp/win95/gadgets/hudgadg.hpp
    src/avp/win95/gadgets/rootgadg.cpp
    src/avp/win95/gadgets/rootgadg.hpp
    src/avp/win95/gadgets/t_ingadg.cpp
    src/avp/win95/gadgets/t_ingadg.hpp
    src/avp/win95/gadgets/teletype.cpp
    src/avp/win95/gadgets/teletype.hpp
    src/avp/win95/gadgets/textexp.cpp
    src/avp/win95/gadgets/textexp.hpp
    src/avp/win95/gadgets/textin.cpp
    src/avp/win95/gadgets/textin.hpp
    src/avp/win95/gadgets/trepgadg.cpp
    src/avp/win95/gadgets/trepgadg.hpp
    src/avp/win95/gameplat.h
    src/avp/win95/gammacontrol.cpp
    src/avp/win95/gammacontrol.h
    src/avp/win95/heap_tem.hpp
    src/avp/win95/hierplace.cpp
    src/avp/win95/hierplace.hpp
    src/avp/win95/hud_data.h
    src/avp/win95/hud_layout.h
    src/avp/win95/hudgfx.h
    src/avp/win95/iofocus.cpp
    src/avp/win95/iofocus.h
    src/avp/win95/jsndsup.cpp
    src/avp/win95/jsndsup.h
    src/avp/win95/kzsort.cpp
    src/avp/win95/kzsort.h
    src/avp/win95/langplat.cpp
    src/avp/win95/modcmds.cpp
    src/avp/win95/modcmds.hpp
    src/avp/win95/multmenu.h
    src/avp/win95/npcsetup.cpp
    src/avp/win95/npcsetup.h
    src/avp/win95/objsetup.cpp
    src/avp/win95/objsetup.hpp
    src/avp/win95/pathchnk.cpp
    src/avp/win95/pathchnk.hpp
    src/avp/win95/pcmenus.h
    src/avp/win95/platsup.cpp
    src/avp/win95/pldghost.cpp
    src/avp/win95/pldghost.h
    src/avp/win95/pldnet.cpp
    src/avp/win95/pldnet.h
    src/avp/win95/progress_bar.cpp
    src/avp/win95/progress_bar.h
    src/avp/win95/projload.cpp
    src/avp/win95/projload.hpp
    src/avp/win95/psndplat.h
    src/avp/win95/scrshot.cpp
    src/avp/win95/scrshot.hpp
    src/avp/win95/strachnk.cpp
    src/avp/win95/strachnk.hpp
    src/avp/win95/system.cpp
    src/avp/win95/system.h
    src/avp/win95/usr_io.cpp
    src/avp/win95/usr_io.h
    src/avp/win95/vision.cpp
    src/avp/win95/vision.h
    src/avp/win95/vmanpset.h
    src/cdplayer.cpp
    src/cdplayer.h
    src/files.cpp
    src/files.h
    src/fixer.h
    src/fmv.cpp
    src/fmv.h
    src/frustum.cpp
    src/frustum.h
    src/globals.cpp
    src/globals.h
    src/include/3dc.h
    src/include/mem3dc.h
    src/include/module.h
    src/include/prototyp.h
    src/include/shape.h
    src/kshape.cpp
    src/kshape.h
    src/map.cpp
    src/mathline.cpp
    src/maths.cpp
    src/maths.h
    src/md5.cpp
    src/md5.h
    src/mem3dc.cpp
    src/mem3dcpp.cpp
    src/menus.cpp
    src/module.cpp
    src/morph.cpp
    src/net.cpp
    src/net.h
    src/object.cpp
    src/oglfunc.cpp
    src/oglfunc.h
    src/openal.cpp
    src/opengl.cpp
    src/opengl.h
    src/shpanim.cpp
    src/sphere.cpp
    src/sphere.h
    src/stubs.cpp
    src/tables.cpp
    src/vdb.cpp
    src/version.cpp
    src/version.h
    src/win95/alt_tab.h
    src/win95/animchnk.cpp
    src/win95/animchnk.hpp
    src/win95/animobs.cpp
    src/win95/animobs.hpp
    src/win95/aw.h
    src/win95/awbmpld.cpp
    src/win95/awiffld.cpp
    src/win95/awpnmld.cpp
    src/win95/awtexld.cpp
    src/win95/awtexld.h
    src/win95/awtexld.hpp
    src/win95/bmpnames.cpp
    src/win95/bmpnames.hpp
    src/win95/cd_player.h
    src/win95/chnkload.cpp
    src/win95/chnkload.h
    src/win95/chnkload.hpp
    src/win95/chnktexi.cpp
    src/win95/chnktexi.h
    src/win95/chnktype.cpp
    src/win95/chnktype.hpp
    src/win95/chunk.cpp
    src/win95/chunk.hpp
    src/win95/chunkpal.cpp
    src/win95/chunkpal.hpp
    src/win95/d3_func.h
    src/win95/db.cpp
    src/win95/db.h
    src/win95/debuglog.cpp
    src/win95/debuglog.h
    src/win95/debuglog.hpp
    src/win95/dummyobjectchunk.cpp
    src/win95/dummyobjectchunk.hpp
    src/win95/dxlog.h
    src/win95/enumchnk.cpp
    src/win95/enumchnk.hpp
    src/win95/enumsch.cpp
    src/win95/enumsch.hpp
    src/win95/envchunk.cpp
    src/win95/envchunk.hpp
    src/win95/fail.cpp
    src/win95/fail.h
    src/win95/fragchnk.cpp
    src/win95/fragchnk.hpp
    src/win95/gsprchnk.cpp
    src/win95/gsprchnk.hpp
    src/win95/hash_tem.hpp
    src/win95/hierchnk.cpp
    src/win95/hierchnk.hpp
    src/win95/huffman.cpp
    src/win95/huffman.hpp
    src/win95/iff.cpp
    src/win95/iff.hpp
    src/win95/iff_ilbm.cpp
    src/win95/iff_ilbm.hpp
    src/win95/ilbm_ext.cpp
    src/win95/ilbm_ext.hpp
    src/win95/inline.h
    src/win95/io.cpp
    src/win95/list_tem.cpp
    src/win95/list_tem.hpp
    src/win95/ltchunk.cpp
    src/win95/ltchunk.hpp
    src/win95/media.cpp
    src/win95/media.hpp
    src/win95/mishchnk.cpp
    src/win95/mishchnk.hpp
    src/win95/mmx_math.h
    src/win95/obchunk.cpp
    src/win95/obchunk.hpp
    src/win95/oechunk.cpp
    src/win95/oechunk.h
    src/win95/our_mem.cpp
    src/win95/ourasert.h
    src/win95/plat_shp.cpp
    src/win95/plat_shp.h
    src/win95/platform.h
    src/win95/plspecfn.cpp
    src/win95/showcmds.h
    src/win95/shpanim.h
    src/win95/shpchunk.cpp
    src/win95/shpchunk.hpp
    src/win95/sndchunk.cpp
    src/win95/sndchunk.hpp
    src/win95/sprchunk.cpp
    src/win95/sprchunk.hpp
    src/win95/texio.cpp
    src/win95/toolchnk.cpp
    src/win95/toolchnk.hpp
    src/win95/txioctrl.cpp
    src/win95/txioctrl.h
    src/win95/videomodes.h
    src/win95/wpchunk.cpp
    src/win95/wpchunk.hpp
    src/win95/zsp.cpp
    src/win95/zsp.hpp
    src/winapi.cpp
)

IF(SDL_TYPE STREQUAL "SDL")
    LIST(APPEND source src/main.cpp)

    # SDL 1.2 on OS X requires this support file
    IF(APPLE AND ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
        LIST(APPEND source src/sdl12/sdlmain.m)
    ENDIF(APPLE AND ${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
ENDIF(SDL_TYPE STREQUAL "SDL")

IF(SDL_TYPE STREQUAL "SDL2")
    LIST(APPEND source src/main2.cpp)
ENDIF(SDL_TYPE STREQUAL "SDL2")

# auto-include directories with source files
FOREACH(sourcefile IN LISTS source)
    GET_FILENAME_COMPONENT(includedir ${sourcefile} DIRECTORY)
    LIST(APPEND include ${includedir})
ENDFOREACH(sourcefile)
INCLUDE_DIRECTORIES(${include})

# manually include src/include

ADD_EXECUTABLE(avp ${source})
TARGET_INCLUDE_DIRECTORIES(avp PRIVATE src/include)
SET_PROPERTY(TARGET avp PROPERTY CXX_STANDARD 23)

# the codebase relies throughout on implicit void*->T* and int->enum
# conversions that were legal C but are ill-formed C++; keep them as
# warnings rather than rewriting every call site.
TARGET_COMPILE_OPTIONS(avp PRIVATE -fpermissive)

# required dependencies
IF(SDL_TYPE STREQUAL "SDL")
    INCLUDE_DIRECTORIES(${SDL_INCLUDE_DIR})
ENDIF(SDL_TYPE STREQUAL "SDL")

IF(SDL_TYPE STREQUAL "SDL2")
    INCLUDE_DIRECTORIES(${SDL2_INCLUDE_DIR})
ENDIF(SDL_TYPE STREQUAL "SDL2")

INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${OPENAL_INCLUDE_DIR})

IF(SDL_TYPE STREQUAL "SDL")
    TARGET_LINK_LIBRARIES(avp ${SDL_LIBRARY})
ENDIF(SDL_TYPE STREQUAL "SDL")

IF(SDL_TYPE STREQUAL "SDL2")
    TARGET_LINK_LIBRARIES(avp ${SDL2_LIBRARY})
ENDIF(SDL_TYPE STREQUAL "SDL2")

TARGET_LINK_LIBRARIES(avp ${OPENGL_gl_LIBRARY})
TARGET_LINK_LIBRARIES(avp ${OPENAL_LIBRARY})
