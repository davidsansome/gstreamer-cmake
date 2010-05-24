cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstcddabasesrc.c
)

set(HEADERS
  gstcddabasesrc.h
)

add_library(gstcdda-${GST_MAJORMINOR} SHARED ${SOURCES})

target_link_libraries(gstcdda-${GST_MAJORMINOR}
  gstreamer-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_lib(gstcdda)
install_headers(gst/cdda ${HEADERS})