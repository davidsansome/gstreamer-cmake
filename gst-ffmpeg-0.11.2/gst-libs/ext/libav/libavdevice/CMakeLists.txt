cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})
include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/..)

set(CMAKE_C_FLAGS "-std=c99")
add_definitions(-DHAVE_AV_CONFIG_H)

set(SOURCES
  alldevices.c
  avdevice.c
  #bktr.c
  #dv1394.c
  #libdc1394.c
  #vfwcap.c
)

add_library(avdevice SHARED ${SOURCES})
target_link_libraries(avdevice
  avcodec
  avformat
  swscale
  ${JACK_LIBRARIES}
)

install(TARGETS avdevice
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
)