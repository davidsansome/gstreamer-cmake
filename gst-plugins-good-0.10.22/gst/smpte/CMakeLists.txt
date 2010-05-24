cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  barboxwipes.c
  gstmask.c
  gstsmpte.c
  gstsmptealpha.c
  paint.c
  plugin.c
)

add_library(gstsmpte MODULE ${SOURCES})

target_link_libraries(gstsmpte
  gstbase-${GST_MAJORMINOR}
  gstvideo-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstsmpte)
