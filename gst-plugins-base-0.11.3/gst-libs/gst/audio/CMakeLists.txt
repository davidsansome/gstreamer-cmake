cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  audio.c
  gstaudiobasesink.c
  gstaudiobasesrc.c
  gstaudiocdsrc.c
  gstaudioclock.c
  gstaudiodecoder.c
  gstaudioencoder.c
  gstaudiofilter.c
  gstaudioiec61937.c
  gstaudiometa.c
  gstaudioringbuffer.c
  gstaudiosink.c
  gstaudiosrc.c
  mixer.c
  mixeroptions.c
  mixertrack.c
  mixerutils.c
  streamvolume.c
)

set(HEADERS
  audio.h
  gstaudiobasesink.h
  gstaudiobasesrc.h
  gstaudiocdsrc.h
  gstaudioclock.h
  gstaudiodecoder.h
  gstaudioencoder.h
  gstaudiofilter.h
  gstaudioiec61937.h
  gstaudiometa.h
  gstaudioringbuffer.h
  gstaudiosink.h
  gstaudiosrc.h
  mixer.h
  mixeroptions.h
  mixertrack.h
  mixerutils.h
  streamvolume.h
)

set(ENUM_HEADERS
  audio.h
  gstaudioringbuffer.h
)

set(GEN_HEADERS
  audio-enumtypes.h
)

add_glib_marshal(SOURCES audio-marshal __gst_audio_marshal gst_private.h)
add_glib_enumtypes(SOURCES audio-enumtypes GST_AUDIO ${ENUM_HEADERS})
add_gen_headers(SOURCES gst/audio ${GEN_HEADERS})

add_library(gstaudio-${GST_MAJORMINOR} SHARED ${SOURCES})

target_link_libraries(gstaudio-${GST_MAJORMINOR}
  gstreamer-${GST_MAJORMINOR}
  gstbase-${GST_MAJORMINOR}
  gstinterfaces-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_lib(gstaudio)
install_headers(gst/audio ${HEADERS})
install_gen_headers(gst/audio ${GEN_HEADERS})
