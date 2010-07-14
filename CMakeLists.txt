project(libbluedevil)

cmake_minimum_required(VERSION 2.6.4)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

find_package(Qt4 REQUIRED)

set(GENERIC_LIB_VERSION "1.5")
set(GENERIC_LIB_SOVERSION "1")
set(VERSION ${GENERIC_LIB_VERSION})

add_subdirectory(bluedevil)
