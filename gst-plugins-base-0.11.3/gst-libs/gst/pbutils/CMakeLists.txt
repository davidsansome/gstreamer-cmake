cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/gstpluginsbaseversion.h.in
  ${CMAKE_CURRENT_BINARY_DIR}/gstpluginsbaseversion.h
)

set(SOURCES
  codec-utils.c
  descriptions.c
  encoding-profile.c
  encoding-target.c
  gstdiscoverer-types.c
  gstdiscoverer.c
  gstpluginsbaseversion.c
  install-plugins.c
  missing-plugins.c
  pbutils.c
)

set(HEADERS
  codec-utils.h
  encoding-profile.h
  encoding-target.h
  gstdiscoverer.h
  pbutils.h
  descriptions.h
  install-plugins.h
  missing-plugins.h
)

set(GEN_HEADERS
  pbutils-enumtypes.h
  gstpluginsbaseversion.h
)

add_glib_marshal(SOURCES pbutils-marshal pbutils_marshal gst_private.h)
add_glib_enumtypes(SOURCES pbutils-enumtypes GST_INSTALL ${HEADERS})
add_gen_headers(SOURCES gst/pbutils ${GEN_HEADERS})

add_library(gstpbutils-${GST_MAJORMINOR} SHARED ${SOURCES})

target_link_libraries(gstpbutils-${GST_MAJORMINOR}
  gstreamer-${GST_MAJORMINOR}
  gstvideo-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_lib(gstpbutils)
install_headers(gst/pbutils ${HEADERS})
install_gen_headers(gst/pbutils ${GEN_HEADERS})