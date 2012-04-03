cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gsty4mencode.c
)

add_library(gsty4m MODULE ${SOURCES})

target_link_libraries(gsty4m
  gstbase-${GST_MAJORMINOR}
  gstvideo-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gsty4m)
