cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstadder.c
)

add_orc(SOURCES gstadderorc)
add_library(gstadder MODULE ${SOURCES})

target_link_libraries(gstadder
  gstbase-${GST_MAJORMINOR}
  gstaudio-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
  ${ORC_LIBRARIES}
)

install_plugin(gstadder)
