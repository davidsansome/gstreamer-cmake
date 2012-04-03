cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstcutter.c
)

add_library(gstcutter MODULE ${SOURCES})

target_link_libraries(gstcutter
  gstbase-${GST_MAJORMINOR}
  gstaudio-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstcutter)
