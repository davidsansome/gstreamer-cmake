cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstsdpmessage.c
)

set(HEADERS
  gstsdp.h
  gstsdpmessage.h
)

add_library(gstsdp-${GST_MAJORMINOR} SHARED ${SOURCES})

target_link_libraries(gstsdp-${GST_MAJORMINOR}
  gstreamer-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_lib(gstsdp)
install_headers(gst/sdp ${HEADERS})