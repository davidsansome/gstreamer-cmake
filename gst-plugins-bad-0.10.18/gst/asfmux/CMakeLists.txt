cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstasf.c
  gstasfmux.c
  gstasfobjects.c
  gstasfparse.c
  gstrtpasfpay.c
)

add_library(gstasfmux MODULE ${SOURCES})

target_link_libraries(gstasfmux
  gstbase-${GST_MAJORMINOR}
  gstrtp-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstasfmux)
