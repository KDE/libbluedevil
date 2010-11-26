project(libbluedevil)

cmake_minimum_required(VERSION 2.6.4)
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/modules ${CMAKE_MODULE_PATH})

find_package(Qt4 REQUIRED)

#
### lib64/32 support kde like
#
set(LIB_SUFFIX "" CACHE STRING "Define suffix for lib directory (32/64)")

set(BIN_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/bin)
set(LIB_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/lib${LIB_SUFFIX})

set(INSTALL_TARGETS_DEFAULT_ARGS  RUNTIME DESTINATION "${BIN_INSTALL_DIR}"
                                  LIBRARY DESTINATION "${LIB_INSTALL_DIR}"
                                  ARCHIVE DESTINATION "${LIB_INSTALL_DIR}" COMPONENT Devel )


set(GENERIC_LIB_VERSION "1.8")
set(GENERIC_LIB_SOVERSION "1")
set(VERSION ${GENERIC_LIB_VERSION})

add_subdirectory(bluedevil)

option(LIBBLUEDEVIL_BUILD_API_DOCS "Build libbluedevil API documentation")

##################  apidox ################################
if(LIBBLUEDEVIL_BUILD_API_DOCS)
  find_package(Doxygen)

  if(DOXYGEN_EXECUTABLE)
    configure_file(${libbluedevil_SOURCE_DIR}/.Doxyfile.cmake ${libbluedevil_BINARY_DIR}/Doxyfile)

    if(EXISTS ${QT_DOC_DIR}/html)
      set(QTDOCS "${QT_DOC_DIR}/html")
    else(EXISTS ${QT_DOC_DIR}/html)
      set(QTDOCS "http://doc.qt.nokia.com/4.6/")
    endif(EXISTS ${QT_DOC_DIR}/html)

    add_custom_target(
      apidox ALL
      COMMAND ${DOXYGEN_EXECUTABLE} Doxyfile
      COMMAND doc/html/installdox -l qt4.tag@${QTDOCS} doc/html/*.html)
  endif(DOXYGEN_EXECUTABLE)
endif(LIBBLUEDEVIL_BUILD_API_DOCS)
