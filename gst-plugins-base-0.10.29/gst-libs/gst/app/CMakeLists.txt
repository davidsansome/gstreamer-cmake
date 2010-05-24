cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstappsrc.c
  gstappbuffer.c
  gstappsink.c
)

set(HEADERS
  gstappsrc.h
  gstappbuffer.h
  gstappsink.h
)

add_glib_marshal(SOURCES gstapp-marshal __gst_app_marshal gst_private.h)

add_library(gstapp-${GST_MAJORMINOR} SHARED ${SOURCES})

target_link_libraries(gstapp-${GST_MAJORMINOR}
  gstreamer-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_lib(gstapp)
install_headers(gst/app ${HEADERS})