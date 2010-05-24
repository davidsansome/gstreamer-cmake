cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstdvddemux.c
  gstmpegclock.c
  gstmpegdemux.c
  gstmpegpacketize.c
  gstmpegparse.c
  gstmpegstream.c
)

add_library(gstmpegstream MODULE ${SOURCES})

target_link_libraries(gstmpegstream
  gstbase-${GST_MAJORMINOR}
  gstaudio-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstmpegstream)
