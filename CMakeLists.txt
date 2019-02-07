cmake_minimum_required(VERSION 2.8)

project(BOOMRTX)

option(BUILD_TESTS "Build Tests" ON)

if( BUILD_TESTS )
    add_subdirectory( tests )
endif( BUILD_TESTS )
