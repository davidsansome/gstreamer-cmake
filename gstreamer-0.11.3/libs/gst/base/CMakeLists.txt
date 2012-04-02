cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstadapter.c
  gstbaseparse.c
  gstbasesink.c
  gstbasesrc.c
  gstbasetransform.c
  gstbitreader.c
  gstbytereader.c
  gstbytewriter.c
  gstcollectpads2.c
  gstpushsrc.c
  gsttypefindhelper.c
)

set(HEADERS
  gstadapter.h
  gstbaseparse.h
  gstbasesink.h
  gstbasesrc.h
  gstbasetransform.h
  gstbitreader.h
  gstbytereader.h
  gstbytewriter.h
  gstcollectpads2.h
  gstindex.h
  gstpushsrc.h
  gsttypefindhelper.h
)

add_library(gstbase-${GST_MAJORMINOR} SHARED ${SOURCES})

target_link_libraries(gstbase-${GST_MAJORMINOR}
  gstreamer-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_lib(gstbase)
install_headers(gst/base ${HEADERS})
