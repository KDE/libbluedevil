cmake_minimum_required(VERSION 2.8)

project(libbluedevil)
set(GENERIC_LIB_VERSION "1.5")
set(GENERIC_LIB_SOVERSION "1")
set(VERSION ${GENERIC_LIB_VERSION})
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

find_package(Qt4 REQUIRED)

add_subdirectory(bluedevil)
