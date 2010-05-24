cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  atoms.c
  atomsrecovery.c
  descriptors.c
  gstqtmoovrecover.c
  gstqtmux.c
  gstqtmuxmap.c
  gstqtmuxplugin.c
  properties.c
)

add_library(gstqtmux MODULE ${SOURCES})

target_link_libraries(gstqtmux
  gstbase-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstqtmux)
