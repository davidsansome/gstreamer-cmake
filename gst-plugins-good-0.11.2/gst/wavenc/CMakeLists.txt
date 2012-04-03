cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstwavenc.c
)

add_library(gstwavenc MODULE ${SOURCES})

target_link_libraries(gstwavenc
  gstbase-${GST_MAJORMINOR}
  gstriff-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstwavenc)
