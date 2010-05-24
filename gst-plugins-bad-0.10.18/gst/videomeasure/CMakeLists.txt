cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstvideomeasure.c
  gstvideomeasure_collector.c
  gstvideomeasure_ssim.c
)

add_library(gstvideomeasure MODULE ${SOURCES})

target_link_libraries(gstvideomeasure
  gstbase-${GST_MAJORMINOR}
  gstvideo-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstvideomeasure)
