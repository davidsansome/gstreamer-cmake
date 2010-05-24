cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstrtspbase64.c
  gstrtsptransport.c
  gstrtspurl.c
  gstrtspmessage.c
  gstrtspconnection.c
  gstrtspdefs.c
  gstrtspextension.c
  gstrtsprange.c
)

set(HEADERS
  gstrtspbase64.h
  gstrtsptransport.h
  gstrtspurl.h
  gstrtspmessage.h
  gstrtspconnection.h
  gstrtspdefs.h
  gstrtspextension.h
  gstrtsprange.h
)

set(ENUM_HEADERS
  gstrtspdefs.h
)

set(GEN_HEADERS
  gstrtsp-enumtypes.h
)

add_glib_enumtypes(SOURCES gstrtsp-enumtypes gst_rtsp ${ENUM_HEADERS})
add_glib_marshal(SOURCES gstrtsp-marshal gst_rtsp_marshal gst_private.h)
add_gen_headers(SOURCES gst/rtsp ${GEN_HEADERS})

add_library(gstrtsp-${GST_MAJORMINOR} SHARED ${SOURCES})

target_link_libraries(gstrtsp-${GST_MAJORMINOR}
  gstreamer-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_lib(gstrtsp)
install_headers(gst/rtsp ${HEADERS})
install_gen_headers(gst/rtsp ${GEN_HEADERS})