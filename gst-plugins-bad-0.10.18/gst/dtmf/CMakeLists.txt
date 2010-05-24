cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstdtmf.c
  gstdtmfdetect.c
  gstdtmfsrc.c
  gstrtpdtmfdepay.c
  gstrtpdtmfsrc.c
  tone_detect.c
)

add_library(gstdtmf MODULE ${SOURCES})

target_link_libraries(gstdtmf
  gstbase-${GST_MAJORMINOR}
  gstrtp-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstdtmf)
