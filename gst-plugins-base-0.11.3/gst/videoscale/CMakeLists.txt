cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(${LIBOIL_INCLUDE_DIRS})

set(SOURCES
  gstvideoscale.c
  vs_image.c
  vs_scanline.c
  vs_4tap.c
  vs_fill_borders.c
  vs_lanczos.c
)

add_orc(SOURCES gstvideoscaleorc)
add_library(gstvideoscale MODULE ${SOURCES})

target_link_libraries(gstvideoscale
  gstbase-${GST_MAJORMINOR}
  gstvideo-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
  ${LIBOIL_LIBRARIES}
)

install_plugin(gstvideoscale)
