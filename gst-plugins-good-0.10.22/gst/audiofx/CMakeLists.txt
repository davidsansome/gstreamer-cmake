cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  audioamplify.c
  audiochebband.c
  audiocheblimit.c
  audiodynamic.c
  audioecho.c
  audiofirfilter.c
  audiofx.c
  audiofxbasefirfilter.c
  audiofxbaseiirfilter.c
  audioiirfilter.c
  audioinvert.c
  audiokaraoke.c
  audiopanorama.c
  audiowsincband.c
  audiowsinclimit.c
)

add_library(gstaudiofx MODULE ${SOURCES})

target_link_libraries(gstaudiofx
  gstbase-${GST_MAJORMINOR}
  gstcontroller-${GST_MAJORMINOR}
  gstaudio-${GST_MAJORMINOR}
  gstfft-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstaudiofx)
