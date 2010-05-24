cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DEXTERN_BUF -DRTP_SUPPORT)

set(SOURCES
  gstrtpdtmfmux.c
  gstrtpmux.c
  gstrtpmuxer.c
)

add_library(gstrtpmux MODULE ${SOURCES})

target_link_libraries(gstrtpmux
  gstbase-${GST_MAJORMINOR}
  gstrtp-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstrtpmux)
