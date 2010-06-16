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
  dv1394.c
  #libdc1394.c
  #vfwcap.c
)

if(LINUX)
  list(APPEND SOURCES
    alsa-audio-common.c
    alsa-audio-dec.c
    alsa-audio-enc.c
    jack_audio.c
    oss_audio.c
    v4l.c
    v4l2.c
    x11grab.c
  )
endif(LINUX)

add_library(libavdevice SHARED ${SOURCES})
target_link_libraries(libavdevice
  libavcodec
  libavformat
  libswscale
  ${JACK_LIBRARIES}
)
