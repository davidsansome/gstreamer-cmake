cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstgdp.c
  gstgdppay.c
  gstgdpdepay.c
)

add_library(gstgdp MODULE ${SOURCES})

target_link_libraries(gstgdp
  gstbase-${GST_MAJORMINOR}
  gstdataprotocol-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstgdp)
