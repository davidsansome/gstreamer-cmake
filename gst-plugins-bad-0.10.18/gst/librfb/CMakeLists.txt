cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/..)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(RFB_SOURCES
  d3des.c
  rfbbuffer.c
  rfbdecoder.c
  vncauth.c
)
set(SOURCES
  gstrfbsrc.c
)

add_library(rfb SHARED ${RFB_SOURCES})
add_library(gstrfbsrc MODULE ${SOURCES})

target_link_libraries(rfb
  ${GLIB_LIBRARIES}
)
target_link_libraries(gstrfbsrc
  rfb
  gstbase-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstrfbsrc)
