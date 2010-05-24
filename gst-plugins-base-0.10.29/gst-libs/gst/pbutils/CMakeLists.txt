cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  pbutils.c
  descriptions.c
  install-plugins.c
  missing-plugins.c
)

set(HEADERS
  pbutils.h
  descriptions.h
  install-plugins.h
  missing-plugins.h
)

add_glib_enumtypes(SOURCES pbutils-enumtypes GST_INSTALL ${HEADERS})

add_library(gstpbutils-${GST_MAJORMINOR} SHARED ${SOURCES})

target_link_libraries(gstpbutils-${GST_MAJORMINOR}
  gstreamer-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_lib(gstpbutils)
install_headers(gst/pbutils ${HEADERS})