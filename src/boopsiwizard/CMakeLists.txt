cmake_minimum_required (VERSION 3.1)

# VASM is an extra asm language configured in our cmake platform,
# to cross-assemble with vasm assembler installed by bebbo gcc
set(CMAKE_C_STANDARD 11)
project(BoopsiWizard LANGUAGES C VERSION 1.0)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(VERSTRING "1.0")

# exe that consume the .class
add_executable(BoopsiWizard
		compilers.h
		boopsiinlines.h
		boopsiwizard.c
		)

target_compile_options(BoopsiWizard PRIVATE -Os -save-temps -m68000 )
target_link_options(BoopsiWizard PRIVATE -s )
install(TARGETS BoopsiWizard DESTINATION "${CMAKE_SOURCE_DIR}/..")
