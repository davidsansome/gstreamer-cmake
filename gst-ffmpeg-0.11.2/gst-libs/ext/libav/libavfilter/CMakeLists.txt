cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/..)

set(CMAKE_C_FLAGS "-std=c99")
add_definitions(-DHAVE_AV_CONFIG_H)

set(SOURCES
  allfilters.c
  avfilter.c
  avfiltergraph.c
  defaults.c
  drawutils.c
  formats.c
  graphparser.c
  vsrc_buffer.c
)

add_library(avfilter SHARED ${SOURCES})
target_link_libraries(avfilter
  avcodec
  avutil
  swscale
)

install(TARGETS avfilter
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)
