cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstinputselector.c
  gstoutputselector.c
  gstselector.c
)

add_glib_marshal(SOURCES gstselector-marshal gst_selector_marshal gst_private.h)

add_library(gstselector MODULE ${SOURCES})

target_link_libraries(gstselector
  gstbase-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstselector)
