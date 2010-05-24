cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gsttcpplugin.c
  gsttcp.c
  gstmultifdsink.c
  gsttcpclientsrc.c
  gsttcpclientsink.c
  gsttcpserversrc.c
  gsttcpserversink.c
)

set(ENUM_HEADERS
  gsttcp.h
)

add_glib_marshal(SOURCES gsttcp-marshal gst_tcp_marshal gst_private.h)
add_glib_enumtypes(SOURCES gsttcp-enumtypes GST_TCP ${ENUM_HEADERS})

add_library(gsttcp MODULE ${SOURCES})

target_link_libraries(gsttcp
  gstbase-${GST_MAJORMINOR}
  gstdataprotocol-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gsttcp)
