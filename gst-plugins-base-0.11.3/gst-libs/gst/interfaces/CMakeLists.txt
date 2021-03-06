cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  navigation.c
  tuner.c
  tunernorm.c
  tunerchannel.c
)

set(HEADERS
  navigation.h
  tuner.h
  tunernorm.h
  tunerchannel.h
)

set(GEN_HEADERS
  interfaces-enumtypes.h
)

add_glib_marshal(SOURCES interfaces-marshal gst_interfaces_marshal gst_private.h)
add_glib_enumtypes(SOURCES interfaces-enumtypes GST_INTERFACES ${HEADERS})
add_gen_headers(SOURCES gst/interfaces ${GEN_HEADERS})

add_library(gstinterfaces-${GST_MAJORMINOR} SHARED ${SOURCES})

target_link_libraries(gstinterfaces-${GST_MAJORMINOR}
  gstreamer-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_lib(gstinterfaces)
install_headers(gst/interfaces ${HEADERS})
install_gen_headers(gst/interfaces ${GEN_HEADERS})