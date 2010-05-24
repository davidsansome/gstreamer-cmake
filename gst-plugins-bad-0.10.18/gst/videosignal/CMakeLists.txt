cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstvideoanalyse.c
  gstvideodetect.c
  gstvideomark.c
  gstvideosignal.c
)

add_library(gstvideosignal MODULE ${SOURCES})

target_link_libraries(gstvideosignal
  gstbase-${GST_MAJORMINOR}
  gstvideo-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstvideosignal)
