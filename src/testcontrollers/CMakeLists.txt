cmake_minimum_required (VERSION 3.1)

# VASM is an extra asm language configured in our cmake platform,
# to cross-assemble with vasm assembler installed by bebbo gcc
enable_language( VASM )
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)
project(TestControllersProject LANGUAGES C CXX VERSION 1.0)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(InputsCheckup
		reactioninlines.h
		inputscheckup.c
#		keyboardviewclass.c
#		controllersviewclass.c
		class_keyboardview.h
		class_keyboardview_private.h
		class_keyboardview_lib.c
		class_keyboardview_dispatch.c
		class_keyboardview_attribs.c
		class_keyboardview_render.c
		class_keyboardview_handleinput.c
		)
# target_include_directories(TestControllers PRIVATE . )

target_compile_options(InputsCheckup PRIVATE -s -save-temps -m68000)
target_link_options(InputsCheckup PRIVATE -s )
install(TARGETS InputsCheckup DESTINATION "${CMAKE_SOURCE_DIR}/..")
