cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstapp.c
)

add_library(gstapp MODULE ${SOURCES})

target_link_libraries(gstapp
  gstapp-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstapp)
