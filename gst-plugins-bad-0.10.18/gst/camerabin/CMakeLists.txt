cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  camerabingeneral.c
  camerabinimage.c
  camerabinpreview.c
  camerabinvideo.c
  gstcamerabin-enum.c
  gstcamerabin.c
  gstcamerabincolorbalance.c
  gstcamerabinphotography.c
)

add_glib_marshal(SOURCES gstcamerabin-marshal __gst_camerabin_marshal gst_private.h)

add_library(gstcamerabin MODULE ${SOURCES})

target_link_libraries(gstcamerabin
  gstbase-${GST_MAJORMINOR}
  gstphotography-${GST_MAJORMINOR}
  gstinterfaces-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstcamerabin)
