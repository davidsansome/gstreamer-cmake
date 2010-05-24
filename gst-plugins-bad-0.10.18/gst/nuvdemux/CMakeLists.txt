cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstnuvdemux.c
)

add_library(gstnuvdemux MODULE ${SOURCES})

target_link_libraries(gstnuvdemux
  gstbase-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstnuvdemux)
