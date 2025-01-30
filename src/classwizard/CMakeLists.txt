cmake_minimum_required (VERSION 3.1)

# VASM is an extra asm language configured in our cmake platform,
# to cross-assemble with vasm assembler installed by bebbo gcc
enable_language( VASM )
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)
project(BoopsiWizard LANGUAGES C CXX VERSION 1.0)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(VERSTRING "1.0")

add_executable(KeyboardViewTest_static
		reactioninlines.h
		keyboardviewtest.c
		class_keyboardview.h
		class_keyboardview_private.h
		class_keyboardview_lib.c
		class_keyboardview_dispatch.c
		class_keyboardview_attribs.c
		class_keyboardview_render.c
		class_keyboardview_handleinput.c
		)

target_compile_options(KeyboardViewTest_static PRIVATE -s -save-temps -m68000)
target_compile_definitions(KeyboardViewTest_static PRIVATE KEYBOARDVIEW_STATICLINK=1 )
target_link_options(KeyboardViewTest_static PRIVATE -s )
install(TARGETS KeyboardViewTest_static DESTINATION "${CMAKE_SOURCE_DIR}/..")

# binary class version -- should be a "add_library( SHARED..."
add_executable(keyboardview.gadget
		reactioninlines.h
		minialib.h
		class_keyboardview.h
		class_keyboardview_private.h

		classinit.s
		class_keyboardview_lib.c
		class_keyboardview_dispatch.c
		class_keyboardview_attribs.c
		class_keyboardview_render.c
		class_keyboardview_handleinput.c
		)

target_link_options(keyboardview.gadget PRIVATE -ffreestanding -nostdlib -nostartfiles -Wl,-e,__start)
target_compile_options(keyboardview.gadget PRIVATE -Os -m68020 -fomit-frame-pointer )
target_compile_definitions(keyboardview.gadget PRIVATE PRIVATE VERSION_STRING="${VERSTRING}")


# exe that consume the .class


add_executable(KeyboardViewTest
		reactioninlines.h
		keyboardviewtest.c
		class_keyboardview.h
		)
# target_include_directories(TestControllers PRIVATE . )

target_compile_options(KeyboardViewTest PRIVATE -s -save-temps -m68000 )
target_link_options(KeyboardViewTest PRIVATE -s )
install(TARGETS KeyboardViewTest DESTINATION "${CMAKE_SOURCE_DIR}/..")
