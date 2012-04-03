cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstgamma.c
  gstvideobalance.c
  gstvideoflip.c
  plugin.c
)

add_library(gstvideofilter MODULE ${SOURCES})

target_link_libraries(gstvideofilter
  gstbase-${GST_MAJORMINOR}
  gstvideo-${GST_MAJORMINOR}
  gstinterfaces-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstvideofilter)
