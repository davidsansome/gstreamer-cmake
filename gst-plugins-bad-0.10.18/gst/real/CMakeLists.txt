cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstreal.c
  gstrealaudiodec.c
  gstrealvideodec.c
)

add_library(gstreal MODULE ${SOURCES})

target_link_libraries(gstreal
  gstbase-${GST_MAJORMINOR}
  gstaudio-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstreal)
