cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstaging.c
  gstdice.c
  gstedge.c
  gsteffectv.c
  gstop.c
  gstquark.c
  gstradioac.c
  gstrev.c
  gstripple.c
  gstshagadelic.c
  gststreak.c
  gstvertigo.c
  gstwarp.c
)

add_library(gsteffectv MODULE ${SOURCES})

target_link_libraries(gsteffectv
  gstbase-${GST_MAJORMINOR}
  gstvideo-${GST_MAJORMINOR}
  gstcontroller-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gsteffectv)
