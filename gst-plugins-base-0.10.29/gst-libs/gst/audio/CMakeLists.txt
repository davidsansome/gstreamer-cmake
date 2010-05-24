cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  audio.c
  gstringbuffer.c
  gstaudioclock.c
  mixerutils.c
  multichannel.c
  gstbaseaudiosink.c
  gstbaseaudiosrc.c
  gstaudiofilter.c
  gstaudiosink.c
  gstaudiosrc.c
)

set(HEADERS
  audio.h
  gstringbuffer.h
  gstaudioclock.h
  gstaudiofilter.h
  gstbaseaudiosink.h
  gstbaseaudiosrc.h
  gstaudiosink.h
  gstaudiosrc.h
  mixerutils.h
  multichannel.h
)

set(ENUM_HEADERS
  multichannel.h
  gstringbuffer.h
)

add_glib_enumtypes(SOURCES audio-enumtypes ${ENUM_HEADERS})

add_library(gstaudio-${GST_MAJORMINOR} SHARED ${SOURCES})

target_link_libraries(gstaudio-${GST_MAJORMINOR}
  gstreamer-${GST_MAJORMINOR}
  gstinterfaces-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_lib(gstaudio)
install_headers(gst/audio ${HEADERS})