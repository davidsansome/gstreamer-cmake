cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstdvdspu-render.c
  gstdvdspu.c
  gstspu-pgs.c
  gstspu-vobsub-render.c
  gstspu-vobsub.c
)

add_library(gstdvdspu MODULE ${SOURCES})

target_link_libraries(gstdvdspu
  gstbase-${GST_MAJORMINOR}
  gstvideo-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstdvdspu)
