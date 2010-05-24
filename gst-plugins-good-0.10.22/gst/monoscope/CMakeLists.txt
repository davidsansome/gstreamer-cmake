cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  convolve.c
  gstmonoscope.c
  monoscope.c
)

add_library(gstmonoscope MODULE ${SOURCES})

target_link_libraries(gstmonoscope
  gstbase-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstmonoscope)
