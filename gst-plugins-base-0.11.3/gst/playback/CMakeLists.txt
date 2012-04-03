cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_definitions(-DCOLORSPACE="videoconvert")

set(SOURCES
  gstdecodebin2.c
  gsturidecodebin.c
  gstplayback.c
  gstplaybin2.c
  gstplaysink.c
  gstplay-enum.c
  gstsubtitleoverlay.c
  gstplaysinkvideoconvert.c
  gstplaysinkaudioconvert.c
  gstplaysinkconvertbin.c
  gststreamsynchronizer.c
)

add_library(gstplayback MODULE ${SOURCES})

target_link_libraries(gstplayback
  gstbase-${GST_MAJORMINOR}
  gstpbutils-${GST_MAJORMINOR}
  gstvideo-${GST_MAJORMINOR}
  gstinterfaces-${GST_MAJORMINOR}
  gstaudio-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstplayback)
