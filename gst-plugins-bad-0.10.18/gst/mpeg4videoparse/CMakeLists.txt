cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  mpeg4videoparse.c
)

add_library(gstmpeg4videoparse MODULE ${SOURCES})

target_link_libraries(gstmpeg4videoparse
  gstbase-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstmpeg4videoparse)
