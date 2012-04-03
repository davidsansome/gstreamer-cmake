cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstwavpackparse.c
  gstflacparse.c
  gstmpegaudioparse.c
  gstamrparse.c
  plugin.c
  gstaacparse.c
  gstdcaparse.c
  gstac3parse.c
)

add_library(gstaudioparsers MODULE ${SOURCES})

target_link_libraries(gstaudioparsers
  gstbase-${GST_MAJORMINOR}
  gsttag-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstaudioparsers)

