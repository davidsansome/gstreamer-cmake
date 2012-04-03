cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/..)

set(CMAKE_C_FLAGS "-std=c99")
add_definitions(-DHAVE_AV_CONFIG_H)

set(SOURCES
  postprocess.c
)

add_library(postproc SHARED ${SOURCES})
target_link_libraries(postproc
  avutil
)

install(TARGETS postproc
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)