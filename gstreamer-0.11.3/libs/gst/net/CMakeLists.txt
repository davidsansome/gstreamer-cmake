cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstnetaddressmeta.c
  gstnetclientclock.c
  gstnettimepacket.c
  gstnettimeprovider.c
)

set(HEADERS
  gstnet.h
  gstnetaddressmeta.h
  gstnetclientclock.h
  gstnettimepacket.h
  gstnettimeprovider.h
)

add_library(gstnet-${GST_MAJORMINOR} SHARED ${SOURCES})

target_link_libraries(gstnet-${GST_MAJORMINOR}
  gstreamer-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_lib(gstnet)
install_headers(gst/net ${HEADERS})
