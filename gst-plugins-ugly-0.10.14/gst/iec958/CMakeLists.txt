cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  ac3_padder.c
  ac3iec.c
)

add_library(gstiec958 MODULE ${SOURCES})

target_link_libraries(gstiec958
  gstbase-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstiec958)
