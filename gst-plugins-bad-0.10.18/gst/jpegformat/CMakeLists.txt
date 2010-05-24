cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstjifmux.c
  gstjpegformat.c
  gstjpegparse.c
)

add_library(gstjpegformat MODULE ${SOURCES})

target_link_libraries(gstjpegformat
  gstbase-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstjpegformat)
