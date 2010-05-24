cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstnetbuffer.c
)

set(HEADERS
  gstnetbuffer.h
)

add_library(gstnetbuffer-${GST_MAJORMINOR} SHARED ${SOURCES})

target_link_libraries(gstnetbuffer-${GST_MAJORMINOR}
  gstreamer-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_lib(gstnetbuffer)
install_headers(gst/netbuffer ${HEADERS})