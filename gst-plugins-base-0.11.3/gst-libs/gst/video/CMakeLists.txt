cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  colorbalance.c
  colorbalancechannel.c
  convertframe.c
  gstvideometa.c
  gstvideopool.c
  videoorientation.c
  videooverlay.c
  video.c
  gstvideosink.c
  gstvideofilter.c
)

set(HEADERS
  colorbalance.h
  colorbalancechannel.h
  video.h
  gstvideosink.h
  gstvideofilter.h
  gstvideometa.h
  gstvideopool.h
  videoorientation.h
  videooverlay.h
)

set(GEN_HEADERS
  video-enumtypes.h
)

set(ENUM_HEADERS
  video.h
)

add_glib_marshal(SOURCES video-marshal gst_video_marshal gst_private.h)
add_glib_enumtypes(SOURCES video-enumtypes GST_VIDEO ${ENUM_HEADERS})
add_gen_headers(SOURCES gst/video ${GEN_HEADERS})

add_library(gstvideo-${GST_MAJORMINOR} SHARED ${SOURCES})

target_link_libraries(gstvideo-${GST_MAJORMINOR}
  gstreamer-${GST_MAJORMINOR}
  gstbase-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_lib(gstvideo)
install_headers(gst/video ${HEADERS})
install_gen_headers(gst/video ${GEN_HEADERS})
