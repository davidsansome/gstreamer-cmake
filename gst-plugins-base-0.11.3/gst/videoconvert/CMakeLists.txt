cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  videoconvert.c
  gstvideoconvert.c
)

add_orc(SOURCES gstvideoconvertorc)
add_library(gstvideoconvert MODULE ${SOURCES})

target_link_libraries(gstvideoconvert
  gstvideo-${GST_MAJORMINOR}
  gstbase-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstvideoconvert)

