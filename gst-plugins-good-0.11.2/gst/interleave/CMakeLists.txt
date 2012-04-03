cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  deinterleave.c
  interleave.c
  plugin.c
)

add_library(gstinterleave MODULE ${SOURCES})

target_link_libraries(gstinterleave
  gstbase-${GST_MAJORMINOR}
  gstaudio-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstinterleave)
