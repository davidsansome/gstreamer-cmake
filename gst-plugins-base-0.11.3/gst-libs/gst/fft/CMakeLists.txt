cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstfft.c
  gstffts16.c
  gstffts32.c
  gstfftf32.c
  gstfftf64.c
  kiss_fft_s16.c
  kiss_fft_s32.c
  kiss_fft_f32.c
  kiss_fft_f64.c
  kiss_fftr_s16.c
  kiss_fftr_s32.c
  kiss_fftr_f32.c
  kiss_fftr_f64.c
)

set(HEADERS
  gstfft.h
  gstffts16.h
  gstffts32.h
  gstfftf32.h
  gstfftf64.h
)

add_library(gstfft-${GST_MAJORMINOR} SHARED ${SOURCES})

target_link_libraries(gstfft-${GST_MAJORMINOR}
  gstreamer-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_lib(gstfft)
install_headers(gst/fft ${HEADERS})