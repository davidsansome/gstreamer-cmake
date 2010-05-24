cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstmve.c
  gstmvedemux.c
  gstmvemux.c
  mveaudiodec.c
  mveaudioenc.c
  mvevideodec16.c
  mvevideodec8.c
  mvevideoenc16.c
  mvevideoenc8.c
)

add_library(gstmve MODULE ${SOURCES})

target_link_libraries(gstmve
  gstbase-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstmve)
