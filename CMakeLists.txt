cmake_minimum_required(VERSION 2.8)

project(libbluedevil)
set(GENERIC_LIB_VERSION "1.1")
set(GENERIC_LIB_SOVERSION "1")
set(VERSION ${GENERIC_LIB_VERSION})
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

find_package(Qt4 REQUIRED)

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})

set(libbluedevil_SRCS
    bluedevilmanager.cpp
    bluedeviladapter.cpp
    bluedevildevice.cpp
)

QT4_ADD_DBUS_INTERFACE(libbluedevil_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/bluez/bluez-manager.xml bluezmanager)
QT4_ADD_DBUS_INTERFACE(libbluedevil_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/bluez/bluez-adapter.xml bluezadapter)
set(libbluedevil_xml ${CMAKE_CURRENT_SOURCE_DIR}/bluez/bluez-device.xml)
set_source_files_properties(${libbluedevil_xml} PROPERTIES INCLUDE "bluedevildevice.h")
QT4_ADD_DBUS_INTERFACE(libbluedevil_SRCS ${libbluedevil_xml} bluezdevice)
QT4_AUTOMOC(${libbluedevil_SRCS})

add_library(bluedevil SHARED ${libbluedevil_SRCS})

target_link_libraries(bluedevil ${QT_QTCORE_LIBRARY} ${QT_QTDBUS_LIBRARY})

set_target_properties(bluedevil PROPERTIES
   VERSION ${GENERIC_LIB_VERSION}
   SOVERSION ${GENERIC_LIB_SOVERSION}
)

install(TARGETS bluedevil DESTINATION lib)
install(FILES	bluedevilmanager.h
        	bluedeviladapter.h
		bluedevildevice.h DESTINATION include/BlueDevil)

add_subdirectory(test)

if(NOT WIN32) # pkgconfig file
   configure_file(${CMAKE_CURRENT_SOURCE_DIR}/bluedevil.pc.in ${CMAKE_CURRENT_BINARY_DIR}/bluedevil.pc @ONLY)
   install(FILES ${CMAKE_CURRENT_BINARY_DIR}/bluedevil.pc DESTINATION ${CMAKE_INSTALL_PREFIX}/lib/pkgconfig)
endif(NOT WIN32)
