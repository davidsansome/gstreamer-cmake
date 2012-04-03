cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstaacparse.c
  gstac3parse.c
  gstamrparse.c
  gstbaseparse.c
  gstflacparse.c
  plugin.c
)

add_library(gstaudioparsersbad MODULE ${SOURCES})

target_link_libraries(gstaudioparsersbad
  gstbase-${GST_MAJORMINOR}
  gstaudio-${GST_MAJORMINOR}
  gsttag-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
  ${GIO_LIBRARIES}
)

install_plugin(gstaudioparsersbad)
