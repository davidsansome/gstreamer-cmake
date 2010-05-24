cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  asmrules.c
  gstrdtbuffer.c
  pnmsrc.c
  rademux.c
  rdtdepay.c
  rdtjitterbuffer.c
  rdtmanager.c
  realhash.c
  realmedia.c
  rmdemux.c
  rmutils.c
  rtspreal.c
)

add_library(gstrealmedia MODULE ${SOURCES})

target_link_libraries(gstrealmedia
  gstbase-${GST_MAJORMINOR}
  gstrtsp-${GST_MAJORMINOR}
  gstsdp-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstrealmedia)
