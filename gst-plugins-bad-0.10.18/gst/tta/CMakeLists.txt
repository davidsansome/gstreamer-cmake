cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gsttta.c
  gstttadec.c
  gstttaparse.c
)

add_library(gsttta MODULE ${SOURCES})

target_link_libraries(gsttta
  gstbase-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gsttta)
