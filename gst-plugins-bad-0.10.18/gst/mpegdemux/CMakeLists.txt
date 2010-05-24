cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${LIBOIL_INCLUDE_DIRS})

set(SOURCES
  flumpegdemux.c
  flutspatinfo.c
  flutspmtinfo.c
  flutspmtstreaminfo.c
  gstmpegdemux.c
  gstmpegdesc.c
  gstmpegtsdemux.c
  gstpesfilter.c
  gstsectionfilter.c
  mpegtspacketizer.c
  mpegtsparse.c
)

add_library(gstmpegdemux MODULE ${SOURCES})

target_link_libraries(gstmpegdemux
  gstbase-${GST_MAJORMINOR}
  gsttag-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
  ${LIBOIL_LIBRARIES}
)

install_plugin(gstmpegdemux)
