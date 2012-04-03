cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstid3demux.c
)

add_library(gstid3demux MODULE ${SOURCES})

target_link_libraries(gstid3demux
  gstbase-${GST_MAJORMINOR}
  gsttag-${GST_MAJORMINOR}
  gstpbutils-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstid3demux)
