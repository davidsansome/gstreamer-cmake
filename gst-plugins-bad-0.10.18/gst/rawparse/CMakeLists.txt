cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstaudioparse.c
  gstrawparse.c
  gstvideoparse.c
  plugin.c
)

add_library(gstrawparse MODULE ${SOURCES})

target_link_libraries(gstrawparse
  gstbase-${GST_MAJORMINOR}
  gstvideo-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstrawparse)
