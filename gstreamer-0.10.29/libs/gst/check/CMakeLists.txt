cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstbufferstraw.c
  gstcheck.c
  gstconsistencychecker.c
)

set(HEADERS
  gstbufferstraw.h
  gstcheck.h
  gstconsistencychecker.h
)

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/libcheck/check.h
  ${CMAKE_CURRENT_BINARY_DIR}/internal-check.h
  COPYONLY
)

add_library(gstcheck-${GST_MAJORMINOR} SHARED ${SOURCES})

target_link_libraries(gstcheck-${GST_MAJORMINOR}
  gstreamer-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)