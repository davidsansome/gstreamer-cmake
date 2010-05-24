cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_subdirectory(tsmux)

set(SOURCES
  mpegtsmux.c
  mpegtsmux_aac.c
  mpegtsmux_h264.c
)

add_library(gstmpegtsmux MODULE ${SOURCES})

target_link_libraries(gstmpegtsmux
  tsmux
  gstbase-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstmpegtsmux)
