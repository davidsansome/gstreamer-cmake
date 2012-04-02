cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  riff.c
  riff-media.c
  riff-read.c
)

set(HEADERS
  riff-ids.h
  riff-media.h
  riff-read.h
)

add_library(gstriff-${GST_MAJORMINOR} SHARED ${SOURCES})

target_link_libraries(gstriff-${GST_MAJORMINOR}
  gstreamer-${GST_MAJORMINOR}
  gstaudio-${GST_MAJORMINOR}
  gsttag-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_lib(gstriff)
install_headers(gst/riff ${HEADERS})