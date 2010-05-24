cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  mxf.c
  mxfaes-bwf.c
  mxfalaw.c
  mxfd10.c
  mxfdemux.c
  mxfdms1.c
  mxfdv-dif.c
  mxfessence.c
  mxfjpeg2000.c
  mxfmetadata.c
  mxfmpeg.c
  mxfmux.c
  mxfquark.c
  mxftypes.c
  mxful.c
  mxfup.c
  mxfvc3.c
)

add_library(gstmxf MODULE ${SOURCES})

target_link_libraries(gstmxf
  gstbase-${GST_MAJORMINOR}
  gstvideo-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstmxf)
