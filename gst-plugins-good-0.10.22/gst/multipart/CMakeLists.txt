cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  multipart.c
  multipartdemux.c
  multipartmux.c
)

add_library(gstmultipart MODULE ${SOURCES})

target_link_libraries(gstmultipart
  gstbase-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstmultipart)
