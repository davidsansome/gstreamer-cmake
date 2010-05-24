cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstsdpdemux.c
  gstsdpelem.c
)

add_library(gstsdp MODULE ${SOURCES})

target_link_libraries(gstsdp
  gstbase-${GST_MAJORMINOR}
  gstinterfaces-${GST_MAJORMINOR}
  gstrtp-${GST_MAJORMINOR}
  gstsdp-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstsdp)
