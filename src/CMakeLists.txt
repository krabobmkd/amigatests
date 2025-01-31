# CMakelists.txt done to refresh old 1999 triumph amiga mame.

cmake_minimum_required (VERSION 3.1)

# VASM is an extra asm language configured in our cmake platform,
# to cross-assemble with vasm assembler installed by bebbo gcc
enable_language( VASM )
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)
project(Tests LANGUAGES C CXX VERSION 1.0)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# this option system makes sure each cpu options are the same for each libs.
# options can only be bool:
option(MAME_USE_HARDFLOAT "compile using direct 68882 FPU, will not work on economic 060" OFF)
option(AMIGA_USE_68030 " use 68030" ON)
option(AMIGA_USE_68040 " use 68040" OFF)
option(AMIGA_USE_68060 " use 68060" OFF)
# option(DOMAMELOG " do output logs" ON)

option(COMPILE_TESTS " compile extra dev tools/ test units" OFF)

# get current git branch id
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  OUTPUT_VARIABLE GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
)
#string(REPLACE "/" "." GIT_BRANCH ${GIT_BRANCH} )
message(WARNING "git branch detected:${GIT_BRANCH}")

if(AMIGA)

#  -fno-rtti -fno-exceptions -fno-strict-aliasing
	# add your generic options here:
	#add_compile_options( -save-temps )
	# this is to have assember .s files in the build.
	list(APPEND AMIGA_COMPILER_OPTIONS -save-temps )

	if(AMIGA_USE_68030)
		list(APPEND AMIGA_COMPILER_OPTIONS -m68030)
	elseif(AMIGA_USE_68040)
		list(APPEND AMIGA_COMPILER_OPTIONS -m68040)
	elseif(AMIGA_USE_68060)
		list(APPEND AMIGA_COMPILER_OPTIONS -m68060)
	else()
		list(APPEND AMIGA_COMPILER_OPTIONS -m68020)
	endif()

	if(MAME_USE_HARDFLOAT)
		list(APPEND AMIGA_COMPILER_OPTIONS -mhard-float -ffast-math)
	else()
		list(APPEND AMIGA_COMPILER_OPTIONS -msoft-float)
	endif()


endif()

set(COMMONLIBS "../../amigacommonlibs")

#add_subdirectory(../mame106 MameLib)

set(AMIGA_AUC_SRC
	aucadicty_main.c
	)

# - - - -

#add_executable(Aucadicty ${AMIGA_AUC_SRC} )
#target_compile_definitions(Aucadicty PUBLIC GIT_BRANCH="${GIT_BRANCH}" )

add_executable(layerexp layerexperiments.c  )
#target_compile_definitions(layerexp PUBLIC GIT_BRANCH="${GIT_BRANCH}" )
target_link_options(layerexp PRIVATE -s ${AMIGA_COMPILER_OPTIONS})
#set_property(TARGET layerexp PROPERTY C_STANDARD 11)
#set_property(TARGET layerexp PROPERTY CXX_STANDARD 11)
#target_link_libraries(layerexp "${M68KAMIGA_ROOT_PATH}/m68k-amigaos/vbcc/lib/reaction.lib" )

add_executable(reactiondemo reactiondemo.c reactionImages.c  )
target_compile_definitions(reactiondemo PUBLIC GIT_BRANCH="${GIT_BRANCH}" )
target_link_options(reactiondemo PRIVATE -s ${AMIGA_COMPILER_OPTIONS})
target_link_libraries(reactiondemo "${M68KAMIGA_ROOT_PATH}/m68k-amigaos/vbcc/lib/reaction.lib" )

add_executable(gelstest gelstest.c datatypebm.c )
#target_compile_definitions(layerexp PUBLIC GIT_BRANCH="${GIT_BRANCH}" )
target_link_options(gelstest PRIVATE -s ${AMIGA_COMPILER_OPTIONS})

add_executable(testScalePixelArray testScalePixelArray.c datatypebmRGB.c )
#target_compile_definitions(layerexp PUBLIC GIT_BRANCH="${GIT_BRANCH}" )
target_link_options(testScalePixelArray PRIVATE -s ${AMIGA_COMPILER_OPTIONS})
