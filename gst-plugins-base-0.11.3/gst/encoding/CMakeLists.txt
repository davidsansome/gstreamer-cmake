cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gststreamcombiner.c
  gstsmartencoder.c
  gstencodebin.c
  gststreamsplitter.c
)

add_glib_marshal(SOURCES gstencode-marshal gst_encode_marshal gst_private.h)
add_library(gstencodebin MODULE ${SOURCES})

target_link_libraries(gstencodebin
  gstpbutils-${GST_MAJORMINOR}
  gstaudio-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstencodebin)

