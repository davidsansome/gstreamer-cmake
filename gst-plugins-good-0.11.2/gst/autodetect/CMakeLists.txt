cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstautoaudiosink.c
  gstautoaudiosrc.c
  gstautodetect.c
  gstautovideosink.c
  gstautovideosrc.c
)

add_library(gstautodetect MODULE ${SOURCES})

target_link_libraries(gstautodetect
  gstbase-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstautodetect)
