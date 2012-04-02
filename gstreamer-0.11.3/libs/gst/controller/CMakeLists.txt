cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstargbcontrolbinding.c
  gstdirectcontrolbinding.c
  gstinterpolationcontrolsource.c
  gstlfocontrolsource.c
  gsttimedvaluecontrolsource.c
  gsttriggercontrolsource.c
)

set(HEADERS
  gstargbcontrolbinding.h
  gstdirectcontrolbinding.h
  gstinterpolationcontrolsource.h
  gstlfocontrolsource.h
  gsttimedvaluecontrolsource.h
  gsttriggercontrolsource.h
)

add_library(gstcontroller-${GST_MAJORMINOR} SHARED ${SOURCES})

target_link_libraries(gstcontroller-${GST_MAJORMINOR}
  gstreamer-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_lib(gstcontroller)
install_headers(gst/controller ${HEADERS})
