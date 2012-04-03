cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstaspectratiocrop.c
  gstvideocrop.c
)

add_library(gstvideocrop MODULE ${SOURCES})

target_link_libraries(gstvideocrop
  gstbase-${GST_MAJORMINOR}
  gstvideo-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstvideocrop)
