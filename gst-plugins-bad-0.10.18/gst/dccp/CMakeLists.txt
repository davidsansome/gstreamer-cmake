cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstdccp.c
  gstdccpclientsink.c
  gstdccpclientsrc.c
  gstdccpplugin.c
  gstdccpserversink.c
  gstdccpserversrc.c
)

add_library(gstdccp MODULE ${SOURCES})

target_link_libraries(gstdccp
  gstbase-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstdccp)
