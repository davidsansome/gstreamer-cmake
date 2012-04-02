cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstgio.c
  gstgiostreamsink.c
  gstgiobasesrc.c
  gstgiosink.c
  gstgiosrc.c
  gstgiostreamsrc.c
  gstgiobasesink.c
)

add_library(gstgio MODULE ${SOURCES})

target_link_libraries(gstgio
  gstbase-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
  ${GIO_LIBRARIES}
)

install_plugin(gstgio)

