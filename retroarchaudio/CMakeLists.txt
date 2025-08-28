cmake_minimum_required (VERSION 3.1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 11)
project(RetroArchAudio LANGUAGES C VERSION 1.0)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# exe that consume the .class
add_executable(RetroArchAudioTest
			amiga_retroarch_audio.c
			amiga_retroarch_audio.h
			amiga_retroarch_audio_test.c
		)


target_compile_options(RetroArchAudioTest PRIVATE -Os -save-temps -m68020 )
target_link_options(RetroArchAudioTest PRIVATE -s )
# target_include_directories(RetroArchAudioTest PUBLIC ${ZLIBPATH} )
#target_link_libraries(BoopsiWizard PUBLIC z )

install(TARGETS RetroArchAudioTest DESTINATION "${CMAKE_SOURCE_DIR}/..")
