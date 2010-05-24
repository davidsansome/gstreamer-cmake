cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  mpegpsmux.c
  mpegpsmux_aac.c
  mpegpsmux_h264.c
  psmux.c
  psmuxstream.c
)

add_library(gstmpegpsmux MODULE ${SOURCES})

target_link_libraries(gstmpegpsmux
  gstbase-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstmpegpsmux)
