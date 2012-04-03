cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  ebml-read.c
  ebml-write.c
  lzo.c
  matroska-demux.c
  matroska-ids.c
  matroska-mux.c
  matroska.c
  matroska-parse.c
  matroska-read-common.c
  webm-mux.c
)

add_library(gstmatroska MODULE ${SOURCES})

target_link_libraries(gstmatroska
  gstbase-${GST_MAJORMINOR}
  gstriff-${GST_MAJORMINOR}
  gstaudio-${GST_MAJORMINOR}
  gsttag-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstmatroska)
