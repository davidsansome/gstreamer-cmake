cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  common.c
  dct4.c
  decoder.c
  encoder.c
  gstsiren.c
  gstsirendec.c
  gstsirenenc.c
  huffman.c
  rmlt.c
)

add_library(gstsiren MODULE ${SOURCES})

target_link_libraries(gstsiren
  gstbase-${GST_MAJORMINOR}
  gstrtp-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstsiren)
