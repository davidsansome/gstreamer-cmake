cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if (NOT ${GST_DISABLE_LOADSAVE})
  set(GST_LOADSAVE_SRC gstfileindex.c)
  set(GST_FILEINDEX_LIBS ${LIBXML_LIBRARIES})
endif()

set(SOURCES
  gstindexers.c
  gstmemindex.c
  ${GST_LOADSAVE_SRC}
)

set(HEADERS
  gstindexers.h
)

add_library(gstcoreindexers MODULE ${SOURCES})

target_link_libraries(gstcoreindexers
  gstreamer-${GST_MAJORMINOR}
  ${GST_FILEINDEX_LIBS}
)

install_plugin(gstcoreindexers)
