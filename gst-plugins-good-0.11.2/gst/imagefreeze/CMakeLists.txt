cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstimagefreeze.c
)

add_library(gstimagefreeze MODULE ${SOURCES})

target_link_libraries(gstimagefreeze
  ${GLIB_LIBRARIES}
)

install_plugin(gstimagefreeze)

