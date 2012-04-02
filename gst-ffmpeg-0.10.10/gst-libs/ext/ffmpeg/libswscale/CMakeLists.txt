cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/..)

set(CMAKE_C_FLAGS "-std=c99")
add_definitions(-DHAVE_AV_CONFIG_H)

set(SOURCES
  options.c
  rgb2rgb.c
  swscale.c
  utils.c
  #x86/yuv2rgb_mmx.c
  yuv2rgb.c
)

if(LINUX)
  list(APPEND SOURCES
  )
endif()

if(WINDOWS)
  list(APPEND SOURCES
  )
endif()
add_library(libswscale SHARED ${SOURCES})
target_link_libraries(libswscale
  libavutil
)
