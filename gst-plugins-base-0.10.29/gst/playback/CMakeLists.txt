cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

add_glib_marshal(MARSHAL_SOURCES gstplay-marshal gst_play_marshal gst_private.h)

set(PLAYBIN_SOURCES
  gstplayback.c
  gstplaybin.c
  gstplaybin2.c
  gstplaysink.c
  gstplaybasebin.c
  gstplay-enum.c
  gstfactorylists.c
  gstinputselector.c
  gstscreenshot.c
  gststreaminfo.c
  gststreamselector.c
  gstsubtitleoverlay.c
  ${MARSHAL_SOURCES}
)

set(DECODEBIN_SOURCES
  gstdecodebin.c
  ${MARSHAL_SOURCES}
)

set(DECODEBIN2_SOURCES
  gstdecodebin2.c
  gsturidecodebin.c
  gstfactorylists.c
  gstplay-enum.c
  ${MARSHAL_SOURCES}
)

add_library(gstplaybin MODULE ${PLAYBIN_SOURCES})
add_library(gstdecodebin MODULE ${DECODEBIN_SOURCES})
add_library(gstdecodebin2 MODULE ${DECODEBIN2_SOURCES})

target_link_libraries(gstplaybin
  gstbase-${GST_MAJORMINOR}
  gstpbutils-${GST_MAJORMINOR}
  gstvideo-${GST_MAJORMINOR}
  gstinterfaces-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

target_link_libraries(gstdecodebin
  gstbase-${GST_MAJORMINOR}
  gstpbutils-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

target_link_libraries(gstdecodebin2
  gstbase-${GST_MAJORMINOR}
  gstpbutils-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstdecodebin)
install_plugin(gstplaybin)
install_plugin(gstdecodebin2)






