include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${CMAKE_CURRENT_BINARY_DIR} ${QT_INCLUDES})

set (bluedeviltest_SRCS bluedeviltest.cpp)

qt4_automoc(${bluedeviltest_SRCS})

add_executable(bluedeviltest ${bluedeviltest_SRCS})

target_link_libraries(bluedeviltest ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTDBUS_LIBRARY} bluedevil)
