cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstid3mux.c
  gsttagmux.c
  id3tag.c
)

add_library(gstid3tag MODULE ${SOURCES})

target_link_libraries(gstid3tag
  gstbase-${GST_MAJORMINOR}
  gsttag-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstid3tag)
