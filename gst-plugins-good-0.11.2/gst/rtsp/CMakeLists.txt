cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstrtpdec.c
  gstrtsp.c
  gstrtspext.c
  gstrtspsrc.c
)

add_library(gstrtsp MODULE ${SOURCES})

target_link_libraries(gstrtsp
  gstbase-${GST_MAJORMINOR}
  gstinterfaces-${GST_MAJORMINOR}
  gstrtp-${GST_MAJORMINOR}
  gstrtsp-${GST_MAJORMINOR}
  gstsdp-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstrtsp)
