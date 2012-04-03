cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstdynudpsink.c
  gstmultiudpsink.c
  gstudp.c
  gstudpnetutils.c
  gstudpsink.c
  gstudpsrc.c
)

set(ENUM_HEADERS
  gstudp.h
)

add_glib_marshal(SOURCES gstudp-marshal gst_udp_marshal gst/gst_private.h)
add_glib_enumtypes(SOURCES gstudp-enumtypes GST_INTERFACES ${ENUM_HEADERS})

add_library(gstudp MODULE ${SOURCES})

target_link_libraries(gstudp
  gstbase-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstudp)
