cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstsynaesthesia.c
  synaescope.c
)

add_library(gstsynaesthesia MODULE ${SOURCES})

target_link_libraries(gstsynaesthesia
  gstbase-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstsynaesthesia)
