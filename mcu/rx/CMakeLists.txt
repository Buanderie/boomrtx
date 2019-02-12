
Project(BOOMRTX_RX)

# Find common source files
file( GLOB BOOMRTX_COMMON_CPP ${BOOMRTX_SOURCE_DIR}/common/*.cpp )
file( GLOB BOOMRTX_COMMON_HEADERS ${BOOMRTX_SOURCE_DIR}/common/*.h )

# Find libraries source files
file( GLOB SCOOP_CPP ${BOOMRTX_SOURCE_DIR}/mcu/common/SCoop/SchedulerARMAVR/*.cpp )
file( GLOB SCOOP_HEADERS ${BOOMRTX_SOURCE_DIR}/mcu/common/SCoop/SchedulerARMAVR/*.h )

# Aggregate shit
set( ALL_DEPS_CPP "${ALL_DEPS_CPP};${BOOMRTX_COMMON_CPP};${SCOOP_CPP}" )
set( ALL_DEPS_HEADERS "${ALL_DEPS_HEADERS};${BOOMRTX_COMMON_HEADERS};${SCOOP_HEADERS}" )
set( ALL_DEPS "${ALL_DEPS_CPP};${ALL_DEPS_HEADERS}" )

add_custom_target(mcu_rx DEPENDS ${ALL_DEPS} )

# do the copying
foreach( file_cpp ${ALL_DEPS_CPP})
    add_custom_command(
    TARGET mcu_rx
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy ${file_cpp} ${PROJECT_SOURCE_DIR}
)
endforeach( file_i )

# do the copying
foreach( file_i ${ALL_DEPS_HEADERS})
    add_custom_command(
    TARGET mcu_rx
    PRE_BUILD
    COMMAND ${CMAKE_COMMAND}
    ARGS -E copy ${file_i} ${PROJECT_SOURCE_DIR}
)
endforeach( file_i )

add_custom_command( TARGET mcu_rx
					POST_BUILD
					COMMAND ${ARDUINO_BINARY} --verify ${PROJECT_SOURCE_DIR}/rx.ino --board teensy:avr:teensy2:speed=16 -v )

# add_custom_command( TARGET mcu_tx
# 					POST_BUILD
#					COMMAND ${ARDUINO_BINARY} --upload ${PROJECT_SOURCE_DIR}/tx.ino --board teensy:avr:teensypp2:speed=16 -v )
