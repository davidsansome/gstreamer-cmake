cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstfrei0r.c
  gstfrei0rfilter.c
  gstfrei0rmixer.c
  gstfrei0rsrc.c
)

add_library(gstfrei0r MODULE ${SOURCES})

target_link_libraries(gstfrei0r
  gstbase-${GST_MAJORMINOR}
  gstvideo-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstfrei0r)
