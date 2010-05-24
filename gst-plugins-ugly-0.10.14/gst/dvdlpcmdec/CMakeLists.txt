cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstdvdlpcmdec.c
)

add_library(gstdvdlpcmdec MODULE ${SOURCES})

target_link_libraries(gstdvdlpcmdec
  gstbase-${GST_MAJORMINOR}
  gstaudio-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstdvdlpcmdec)
