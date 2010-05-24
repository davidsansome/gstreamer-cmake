cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstbasevideocodec.c
  gstbasevideodecoder.c
  gstbasevideoencoder.c
  gstbasevideoparse.c
  gstbasevideoutils.c
)

set(HEADERS
  gstbasevideoutils.h
  gstbasevideocodec.h
  gstbasevideodecoder.h
  gstbasevideoencoder.h
  gstbasevideoparse.h
)

add_library(gstbasevideo-${GST_MAJORMINOR} SHARED ${SOURCES})

target_link_libraries(gstbasevideo-${GST_MAJORMINOR}
  gstbase-${GST_MAJORMINOR}
  gstvideo-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_lib(gstbasevideo)
install_headers(gst/basevideo ${HEADERS})
