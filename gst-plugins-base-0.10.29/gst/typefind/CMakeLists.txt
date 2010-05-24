cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gsttypefindfunctions.c
  gstaacutil.c
)

add_library(gsttypefind MODULE ${SOURCES})

target_link_libraries(gsttypefind
  gstbase-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gsttypefind)
