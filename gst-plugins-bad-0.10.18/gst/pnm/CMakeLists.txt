cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstpnm.c
  gstpnmdec.c
  gstpnmenc.c
  gstpnmutils.c
)

add_library(gstpnm MODULE ${SOURCES})

target_link_libraries(gstpnm
  gstbase-${GST_MAJORMINOR}
  gstvideo-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstpnm)
