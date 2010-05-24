cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstrtpbuffer.c
  gstrtcpbuffer.c
  gstrtppayloads.c
  gstbasertpaudiopayload.c
  gstbasertppayload.c
  gstbasertpdepayload.c
)

set(HEADERS
  gstrtpbuffer.h
  gstrtcpbuffer.h
  gstrtppayloads.h
  gstbasertpaudiopayload.h
  gstbasertppayload.h
  gstbasertpdepayload.h
)

add_library(gstrtp-${GST_MAJORMINOR} SHARED ${SOURCES})

target_link_libraries(gstrtp-${GST_MAJORMINOR}
  gstreamer-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_lib(gstrtp)
install_headers(gst/rtp ${HEADERS})