cmake_minimum_required(VERSION 2.8)

project(libbluedevil)
set(GENERIC_LIB_VERSION "1.0")
set(GENERIC_LIB_SOVERSION "1")

find_package(Qt4 REQUIRED)

include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR})

set(libbluedevil_SRCS
    bluedevilmanager.cpp
    bluedeviladapter.cpp
)

QT4_GENERATE_DBUS_INTERFACE(bluez/bluez.h bluez/bluez-manager.xml)
QT4_ADD_DBUS_INTERFACE(libbluedevil_SRCS bluez/bluez-manager.xml bluezmanager)
QT4_GENERATE_DBUS_INTERFACE(bluez/bluez.h bluez/bluez-adapter.xml)
QT4_ADD_DBUS_INTERFACE(libbluedevil_SRCS bluez/bluez-adapter.xml bluezadapter)
QT4_AUTOMOC(${libbluedevil_SRCS})

add_library(bluedevil SHARED ${libbluedevil_SRCS})

target_link_libraries(bluedevil ${QT_QTCORE_LIBRARY} ${QT_QTDBUS_LIBRARY})

set_target_properties(bluedevil PROPERTIES
   VERSION ${GENERIC_LIB_VERSION}
   SOVERSION ${GENERIC_LIB_SOVERSION}
)

install(TARGETS bluedevil DESTINATION lib)

add_subdirectory(test)
