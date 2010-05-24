cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${LIBOIL_INCLUDE_DIRS})

set(SOURCES
  gstdeinterlace.c
  tvtime/greedy.c
  tvtime/greedyh.c
  tvtime/vfir.c
  tvtime/tomsmocomp.c
  tvtime/weavetff.c
  tvtime/weavebff.c
  tvtime/weave.c
  tvtime/linear.c
  tvtime/linearblend.c
  tvtime/scalerbob.c
)

add_library(gstdeinterlace MODULE ${SOURCES})

target_link_libraries(gstdeinterlace
  gstbase-${GST_MAJORMINOR}
  gstvideo-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
  ${LIBOIL_LIBRARIES}
)

install_plugin(gstdeinterlace)
