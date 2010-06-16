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
  formats.c
  graphparser.c
  parseutils.c
  vf_crop.c
  vf_format.c
  vf_null.c
  vf_scale.c
  vf_slicify.c
  vf_vflip.c
  vsink_nullsink.c
  vsrc_nullsrc.c
)

add_library(libavfilter SHARED ${SOURCES})
target_link_libraries(libavfilter
  libavcodec
  libavutil
  libswscale
)
