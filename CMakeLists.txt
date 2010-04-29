project(libbluedevil)

set(libbluedevil_SRCS
    bluedevil.cpp
    bluedeviladapter.cpp
)

QT4_GENERATE_DBUS_INTERFACE(bluez.h bluez.xml)
QT4_ADD_DBUS_INTERFACE(libbluedevil_SRCS bluez.xml bluez)

kde4_add_library(bluedevil SHARED ${libbluedevil_SRCS})

target_link_libraries(bluedevil ${QT_QTCORE_LIBRARY} ${QT_QTDBUS_LIBRARY})

set_target_properties(bluedevil PROPERTIES
   VERSION ${GENERIC_LIB_VERSION}
   SOVERSION ${GENERIC_LIB_SOVERSION}
)

install(TARGETS bluedevil EXPORT kdeworkspaceLibraryTargets ${INSTALL_TARGETS_DEFAULT_ARGS})
