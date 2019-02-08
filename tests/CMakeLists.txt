
project(BOOMRTX_TESTS)

include_directories( ${BOOMRTX_SOURCE_DIR}/common/ )
include_directories( ${PROJECT_SOURCE_DIR} )

# Global source fetching
file ( GLOB_RECURSE COMMON_CPP ${BOOMRTX_SOURCE_DIR}/common/*.cpp )
file ( GLOB_RECURSE COMMON_HEADERS ${BOOMRTX_SOURCE_DIR}/common/*.h )

add_executable( tests ${COMMON_CPP} ${COMMON_HEADERS} tests.cpp )
target_link_libraries( tests ${DEPS} )
