cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstapedemux.c
)

add_library(gstapetag MODULE ${SOURCES})

target_link_libraries(gstapetag
  gstbase-${GST_MAJORMINOR}
  gsttag-${GST_MAJORMINOR}
  gstpbutils-${GST_MAJORMINOR}
  gstaudio-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstapetag)
