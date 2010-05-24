cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstaudioresample.c
  speex_resampler_int.c
  speex_resampler_float.c
  speex_resampler_double.c
)

add_library(gstaudioresample MODULE ${SOURCES})

target_link_libraries(gstaudioresample
  gstaudio-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstaudioresample)
