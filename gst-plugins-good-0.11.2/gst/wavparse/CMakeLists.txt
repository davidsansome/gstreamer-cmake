cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstwavparse.c
)

add_library(gstwavparse MODULE ${SOURCES})

target_link_libraries(gstwavparse
  gstbase-${GST_MAJORMINOR}
  gstriff-${GST_MAJORMINOR}
  gstaudio-${GST_MAJORMINOR}
  gsttag-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstwavparse)
