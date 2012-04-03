cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstrtpbin.c
  gstrtpjitterbuffer.c
  gstrtpmanager.c
  gstrtpptdemux.c
  gstrtpsession.c
  gstrtpssrcdemux.c
  rtpjitterbuffer.c
  rtpsession.c
  rtpsource.c
  rtpstats.c
)

add_glib_marshal(SOURCES gstrtpbin-marshal gst_rtp_bin_marshal gst/gst_private.h)

add_library(gstrtpmanager MODULE ${SOURCES})

target_link_libraries(gstrtpmanager
  gstbase-${GST_MAJORMINOR}
  gstrtp-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstrtpmanager)
