cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstrganalysis.c
  gstrglimiter.c
  gstrgvolume.c
  replaygain.c
  rganalysis.c
)

add_library(gstreplaygain MODULE ${SOURCES})

target_link_libraries(gstreplaygain
  gstbase-${GST_MAJORMINOR}
  gstpbutils-${GST_MAJORMINOR}
  gstaudio-${GST_MAJORMINOR}
  gsttag-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstreplaygain)
