# mkdir build
# cd build
# cmake ..  -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=../../amigacommonlibs/cmake/Modules/Platform/m68k-amigaos.cmake
# --- compile with:
# cmake --build .
cmake_minimum_required (VERSION 3.1)

# declare VASM as an extra language configured in our cmake platform,
# to cross-assemble with vasm assembler installed by bebbo gcc.
# this allow old style assembler, (default ASM gcc language is the gcc assembler "as" with special syntax.)
enable_language( VASM )
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)
project(ReactionDemoForGcc LANGUAGES C CXX VERSION 1.0)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# this option system makes sure each cpu options are the same for each libs.
# options can only be bool:
option(MAME_USE_HARDFLOAT "compile using direct 68882 FPU, will not work on economic 060" OFF)
option(AMIGA_USE_68030 " use 68030" ON)
option(AMIGA_USE_68040 " use 68040" OFF)
option(AMIGA_USE_68060 " use 68060" OFF)
# option(DOMAMELOG " do output logs" ON)

option(COMPILE_TESTS " compile extra dev tools/ test units" OFF)

if(AMIGA)

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

# this has zlib and things...
set(COMMONLIBS "../../amigacommonlibs")

add_executable(reactiondemo reactiondemo.c reactionImages.c  )
target_compile_definitions(reactiondemo PUBLIC )
target_link_options(reactiondemo PRIVATE -s ${AMIGA_COMPILER_OPTIONS})

# reaction.lib is installed with vbcc sdk, which is installed with bebbo gcc.
# this is just needed for RadioButtons. That said, this lib looks deprecated in modern boopsi.
set( REACTIONLIB_PATH "${M68KAMIGA_ROOT_PATH}/m68k-amigaos/vbcc/lib/reaction.lib" )

target_link_libraries(reactiondemo ${REACTIONLIB_PATH} )
