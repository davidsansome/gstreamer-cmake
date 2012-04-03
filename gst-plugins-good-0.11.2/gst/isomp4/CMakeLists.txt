cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  qtdemux.c
  atoms.c
  qtdemux_lang.c
  properties.c
  isomp4-plugin.c
  gstrtpxqtdepay.c
  gstqtmuxmap.c
  qtdemux_types.c
  gstqtmoovrecover.c
  descriptors.c
  gstqtmux-doc.c
  gstqtmux.c
  qtdemux_dump.c
  atomsrecovery.c
)

add_library(gstisomp4 MODULE ${SOURCES})

target_link_libraries(gstisomp4
  gstriff-${GST_MAJORMINOR}
  gstaudio-${GST_MAJORMINOR}
  gstvideo-${GST_MAJORMINOR}
  gstrtp-${GST_MAJORMINOR}
  gsttag-${GST_MAJORMINOR}
  gstpbutils-${GST_MAJORMINOR}
  gstbase-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstisomp4)

