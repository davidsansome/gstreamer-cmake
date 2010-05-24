cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  colorbalance.c
  colorbalancechannel.c
  mixer.c
  mixeroptions.c
  mixertrack.c
  navigation.c
  propertyprobe.c
  streamvolume.c
  tuner.c
  tunernorm.c
  tunerchannel.c
  videoorientation.c
  xoverlay.c
)

set(HEADERS
  colorbalance.h
  colorbalancechannel.h
  mixer.h
  mixeroptions.h
  mixertrack.h
  navigation.h
  propertyprobe.h
  streamvolume.h
  tuner.h
  tunernorm.h
  tunerchannel.h
  videoorientation.h
  xoverlay.h
)

add_glib_marshal(SOURCES interfaces-marshal gst_interfaces_marshal gst_private.h)
add_glib_enumtypes(SOURCES interfaces-enumtypes GST_INTERFACES ${HEADERS})

add_library(gstinterfaces-${GST_MAJORMINOR} SHARED ${SOURCES})

target_link_libraries(gstinterfaces-${GST_MAJORMINOR}
  gstreamer-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_lib(gstinterfaces)
install_headers(gst/interfaces ${HEADERS})