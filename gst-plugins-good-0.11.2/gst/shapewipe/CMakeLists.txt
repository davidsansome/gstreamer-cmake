cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstshapewipe.c
)

add_library(gstshapewipe MODULE ${SOURCES})

target_link_libraries(gstshapewipe
  gstbase-${GST_MAJORMINOR}
  gstcontroller-${GST_MAJORMINOR}
  gstvideo-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstshapewipe)
