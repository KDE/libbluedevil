include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR})

set (bluedeviltest_SRCS bluedeviltest.cpp)
add_executable(bluedeviltest ${bluedeviltest_SRCS})
target_link_libraries(bluedeviltest
    Qt5::Core
    Qt5::DBus
    bluedevil)

set (adaptertest_SRCS adaptertest.cpp)
add_executable(adaptertest ${adaptertest_SRCS})
target_link_libraries(adaptertest
    Qt5::Core
    Qt5::DBus
    bluedevil)
