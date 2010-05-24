cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${LIBOIL_INCLUDE_DIRS})

set(SOURCES
  gstvolume.c
)

add_library(gstvolume MODULE ${SOURCES})

target_link_libraries(gstvolume
  gstbase-${GST_MAJORMINOR}
  gstaudio-${GST_MAJORMINOR}
  gstcontroller-${GST_MAJORMINOR}
  gstinterfaces-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
  ${LIBOIL_LIBRARIES}
)

install_plugin(gstvolume)
