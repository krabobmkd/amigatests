cmake_minimum_required (VERSION 3.1)

set(CMAKE_C_STANDARD 11)
project(ExtraPads3and4 LANGUAGES C VERSION 1.0)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(VERSTRING "1.0")

add_executable(ExtraPads3and4
		# these are generic commodities things
			commodity_main.c
			cx.c 
			window.c 
			app.c 
		# actual code
			extrapads3and4.c
			revision.c
		)


target_compile_options(ExtraPads3and4 PRIVATE -Os -save-temps -m68000 )
target_link_options(ExtraPads3and4 PRIVATE -s )
# target_include_directories(ExtraPads3and4 PUBLIC ${ZLIBPATH} )
# target_link_libraries(ExtraPads3and4 PUBLIC z )

# install(TARGETS ExtraPads3and4 DESTINATION "${CMAKE_SOURCE_DIR}/..")
