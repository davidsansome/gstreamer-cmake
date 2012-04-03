cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstmultifile.c
  gstmultifilesink.c
  gstmultifilesrc.c
  gstsplitfilesrc.c
  patternspec.c
)

add_library(gstmultifile MODULE ${SOURCES})

target_link_libraries(gstmultifile
  gstbase-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstmultifile)
