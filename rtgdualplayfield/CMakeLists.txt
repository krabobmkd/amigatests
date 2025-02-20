cmake_minimum_required (VERSION 3.1)

# VASM is an extra asm language configured in our cmake platform,
# to cross-assemble with vasm assembler installed by bebbo gcc
enable_language( VASM )
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 11)
project(RtgDualpf LANGUAGES C CXX VERSION 1.0)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

add_executable(testrtgdpf 
		testrtgdpf.c 
		rtgdpfs.h
		rtgdpfs_factory.c 
		rtgdpfs_factory.h		
		rtgdpfs_P96.c 
		rtgdpfs_P96.h
		rtgdpfs_OCS.c 
		rtgdpfs_OCS.h		
		../datatypebm8b.c
		../datatypebm8b.h		
		)
target_include_directories(testrtgdpf PRIVATE . ..)

target_compile_options(testrtgdpf PRIVATE -s -save-temps -m68000) 
target_link_options(testrtgdpf PRIVATE -s )
