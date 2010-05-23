cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  dataprotocol.c
)

set(HEADERS
  dataprotocol.h
)

add_library(gstdataprotocol-${GST_MAJORMINOR} SHARED ${SOURCES})

target_link_libraries(gstdataprotocol-${GST_MAJORMINOR}
  gstreamer-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_lib(gstdataprotocol)
install_headers(gst/dataprotocol ${HEADERS})
