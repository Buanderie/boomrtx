
set(ARDUINO_PATH "/usr/share/arduino" CACHE PATH "Path to the Arduino IDE")
set(ARDUINO_BINARY ${ARDUINO_PATH}/arduino)

add_subdirectory( tx )
add_subdirectory( rx )
