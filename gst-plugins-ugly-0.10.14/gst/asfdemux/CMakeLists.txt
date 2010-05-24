cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  asfheaders.c
  asfpacket.c
  gstasf.c
  gstasfdemux.c
  gstrtpasfdepay.c
  gstrtspwms.c
)

add_library(gstasf MODULE ${SOURCES})

target_link_libraries(gstasf
  gstbase-${GST_MAJORMINOR}
  gstriff-${GST_MAJORMINOR}
  gstrtsp-${GST_MAJORMINOR}
  gstsdp-${GST_MAJORMINOR}
  gstrtp-${GST_MAJORMINOR}
  gstaudio-${GST_MAJORMINOR}
  gsttag-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstasf)
