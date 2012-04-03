cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstflvdemux.c
  gstflvmux.c
)

add_library(gstflv MODULE ${SOURCES})

target_link_libraries(gstflv
  gstbase-${GST_MAJORMINOR}
  gstaudio-${GST_MAJORMINOR}
  gstpbutils-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstflv)
