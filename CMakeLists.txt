cmake_minimum_required(VERSION 2.8)

project(BOOMRTX)

option(BUILD_TESTS "Build Tests" ON)
option(BUILD_TOOLS "Build Tools" ON)

add_subdirectory( common )
add_subdirectory( mcu )

if( BUILD_TESTS )
    add_subdirectory( tests )
endif( BUILD_TESTS )

if( BUILD_TOOLS )
	add_subdirectory( tools )
endif( BUILD_TOOLS )
