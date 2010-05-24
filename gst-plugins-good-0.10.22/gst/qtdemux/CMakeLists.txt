cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstrtpxqtdepay.c
  qtdemux.c
  qtdemux_dump.c
  qtdemux_lang.c
  qtdemux_types.c
  quicktime.c
)

add_library(gstqtdemux MODULE ${SOURCES})

target_link_libraries(gstqtdemux
  gstbase-${GST_MAJORMINOR}
  gstriff-${GST_MAJORMINOR}
  gstaudio-${GST_MAJORMINOR}
  gstrtp-${GST_MAJORMINOR}
  gsttag-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstqtdemux)
