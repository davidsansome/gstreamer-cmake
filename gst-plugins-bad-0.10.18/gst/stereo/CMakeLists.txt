cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gststereo.c
)

add_library(gststereo MODULE ${SOURCES})

target_link_libraries(gststereo
  gstbase-${GST_MAJORMINOR}
  gstcontroller-${GST_MAJORMINOR}
  gstaudio-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gststereo)
