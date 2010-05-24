cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  mpegpacketiser.c
  mpegvideoparse.c
)

add_library(gstmpegvideoparse MODULE ${SOURCES})

target_link_libraries(gstmpegvideoparse
  gstbase-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstmpegvideoparse)
