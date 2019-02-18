cmake_minimum_required(VERSION 3.1.0)

project(BOOMCONFIG)

# Find common source files
file( GLOB BOOMRTX_COMMON_CPP ${BOOMRTX_SOURCE_DIR}/common/*.cpp )
file( GLOB BOOMRTX_COMMON_HEADERS_A ${BOOMRTX_SOURCE_DIR}/common/*.h )
file( GLOB BOOMRTX_COMMON_HEADERS_B ${BOOMRTX_SOURCE_DIR}/common/*.tpp )
set( BOOMRTX_COMMON_HEADERS "${BOOMRTX_COMMON_HEADERS_A};${BOOMRTX_COMMON_HEADERS_B}" )
include_directories( ${BOOMRTX_SOURCE_DIR}/common/ )

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed
set(CMAKE_AUTOMOC ON)

# Create code from a list of Qt designer ui files
set(CMAKE_AUTOUIC ON)

# Create code from resources files
set(CMAKE_AUTORCC ON)

find_package(Qt5 COMPONENTS Core Gui Widgets SerialPort )
if( Qt5_FOUND )
if ( Qt5_VERSION VERSION_LESS 5.11.0 )
        message(FATAL_ERROR "Minimum supported Qt5 version is 5.11.0 !")
    endif()
endif()

# Populate a CMake variable with the sources
set(	helloworld_SRCS
    	mainwindow.ui
    	mainwindow.cpp
	propertyframe.ui
	propertyframe.cpp
	ledindicator.cpp
	settingsdialog.ui
	settingsdialog.cpp
	qroundprogressbar.h
	qroundprogressbar.cpp
	${BOOMRTX_COMMON_CPP}
	${BOOMRTX_COMMON_HEADERS}
	qdarkstyle/style.qrc
	boomrtx.qrc
    	main.cpp
)

# Tell CMake to create the helloworld executable
add_executable(boomconfig WIN32 ${helloworld_SRCS})

# Use the Widgets module from Qt 5
target_link_libraries(boomconfig Qt5::Widgets Qt5::Core Qt5::SerialPort)
