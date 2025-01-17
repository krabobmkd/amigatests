cmake_minimum_required (VERSION 3.1)

# VASM is an extra asm language configured in our cmake platform,
# to cross-assemble with vasm assembler installed by bebbo gcc
enable_language( VASM )
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)
project(RtgDualpf LANGUAGES C CXX VERSION 1.0)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(TestControllers
		reactioninlines.h
		testcontrollers.c
		keyboardviewclass.c
		controllersviewclass.c
		)
# target_include_directories(TestControllers PRIVATE . )

target_compile_options(TestControllers PRIVATE -s -save-temps -m68000)
target_link_options(TestControllers PRIVATE -s )
