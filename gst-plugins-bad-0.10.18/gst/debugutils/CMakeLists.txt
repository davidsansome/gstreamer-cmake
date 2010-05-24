cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  debugutilsbad.c
  fpsdisplaysink.c
  gstcapssetter.c
)

add_library(gstdebugutilsbad MODULE ${SOURCES})

target_link_libraries(gstdebugutilsbad
  gstbase-${GST_MAJORMINOR}
  gstinterfaces-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstdebugutilsbad)
