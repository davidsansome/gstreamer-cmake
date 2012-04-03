cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${ORC_INCLUDE_DIRS})

set(SOURCES
  blend.c
  videomixer2.c
)

add_orc(SOURCES blendorc)
add_library(gstvideomixer MODULE ${SOURCES})

target_link_libraries(gstvideomixer
  gstbase-${GST_MAJORMINOR}
  gstvideo-${GST_MAJORMINOR}
  gstcontroller-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
  ${LIBOIL_LIBRARIES}
)

install_plugin(gstvideomixer)
