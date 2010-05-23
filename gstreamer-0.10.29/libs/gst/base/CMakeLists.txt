cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstadapter.c
  gstbasesink.c
  gstbasesrc.c
  gstbasetransform.c
  gstbitreader.c
  gstbytereader.c
  gstbytewriter.c
  gstcollectpads.c
  gstpushsrc.c
  gsttypefindhelper.c
  gstdataqueue.c
)

set(HEADERS
  gstadapter.h
  gstbasesink.h
  gstbasesrc.h
  gstbasetransform.h
  gstbitreader.h
  gstbytereader.h
  gstbytewriter.h
  gstcollectpads.h
  gstpushsrc.h
  gsttypefindhelper.h
  gstdataqueue.h
)

add_library(gstbase-${GST_MAJORMINOR} SHARED ${SOURCES})

target_link_libraries(gstbase-${GST_MAJORMINOR}
  gstreamer-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_lib(gstbase)
install_headers(gst/base ${HEADERS})
