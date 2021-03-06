
Project(BOOMRTX_RX)

# Sketch source file
set( SKETCH_SRC "${PROJECT_SOURCE_DIR}/rx.ino;${PROJECT_SOURCE_DIR}/trigger.h;${PROJECT_SOURCE_DIR}/ledtrigger.h;${PROJECT_SOURCE_DIR}/relaytrigger.h" )

# Find common source files
file( GLOB BOOMRTX_COMMON_CPP ${BOOMRTX_SOURCE_DIR}/common/*.cpp )
file( GLOB BOOMRTX_COMMON_HEADERS_A ${BOOMRTX_SOURCE_DIR}/common/*.h )
file( GLOB BOOMRTX_COMMON_HEADERS_B ${BOOMRTX_SOURCE_DIR}/common/*.tpp )
set( BOOMRTX_COMMON_HEADERS "${BOOMRTX_COMMON_HEADERS_A};${BOOMRTX_COMMON_HEADERS_B}" )
file( COPY ${BOOMRTX_COMMON_CPP} DESTINATION ${PROJECT_BINARY_DIR} )
file( COPY ${BOOMRTX_COMMON_HEADERS} DESTINATION ${PROJECT_BINARY_DIR} )

# Find common MCU source files
file( GLOB ACEROUTINE_CPP ${BOOMRTX_SOURCE_DIR}/mcu/common/AceRoutine/src/*.cpp )
file( GLOB ACEROUTINE_HEADERS ${BOOMRTX_SOURCE_DIR}/mcu/common/AceRoutine/src/*.h )
file( GLOB HC12_CPP ${BOOMRTX_SOURCE_DIR}/mcu/common/HC12/*.cpp )
file( GLOB HC12_HEADERS ${BOOMRTX_SOURCE_DIR}/mcu/common/HC12/*.h )
set( MCU_COMMON_HEADERS "${ACEROUTINE_HEADERS};${HC12_HEADERS}" )
set( MCU_COMMON_CPP "${ACEROUTINE_CPP};${HC12_CPP}" )
file( COPY ${MCU_COMMON_HEADERS} DESTINATION ${PROJECT_BINARY_DIR} )
file( COPY ${MCU_COMMON_CPP} DESTINATION ${PROJECT_BINARY_DIR} )

# Aggregate shit
set( ALL_DEPS_CPP "${ALL_DEPS_CPP};${BOOMRTX_COMMON_CPP};${MCU_COMMON_CPP}" )
set( ALL_DEPS_HEADERS "${ALL_DEPS_HEADERS};${BOOMRTX_COMMON_HEADERS};${MCU_COMMON_HEADERS}" )
set( ALL_DEPS "${ALL_DEPS_CPP};${ALL_DEPS_HEADERS}" )
set( ALL_SRC "${ALL_DEPS};${SKETCH_SRC}" )

add_custom_target( mcu_rx DEPENDS ${ALL_DEPS} SOURCES ${ALL_SRC} )

file( COPY ${ALL_SRC} DESTINATION ${PROJECT_BINARY_DIR} )

add_custom_command( TARGET mcu_rx
                                        POST_BUILD
                                        COMMAND ${ARDUINO_BINARY} --verify ${PROJECT_BINARY_DIR}/rx.ino --board teensy:avr:teensypp2:speed=16 -v )

# add_custom_command( TARGET mcu_rx
# 					POST_BUILD
#					COMMAND ${ARDUINO_BINARY} --upload ${PROJECT_SOURCE_DIR}/tx.ino --board teensy:avr:teensypp2:speed=16 -v )
