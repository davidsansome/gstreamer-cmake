cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${LIBOIL_INCLUDE_DIRS})

set(SOURCES
  gstvideobox.c
)

add_library(gstvideobox MODULE ${SOURCES})

target_link_libraries(gstvideobox
  gstbase-${GST_MAJORMINOR}
  gstvideo-${GST_MAJORMINOR}
  gstcontroller-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
  ${LIBOIL_LIBRARIES}
)

install_plugin(gstvideobox)
