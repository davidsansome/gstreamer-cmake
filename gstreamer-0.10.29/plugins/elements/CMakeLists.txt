cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if (${HAVE_SYS_SOCKET_H})
  set(GSTFDSRC gstfdsrc.c)
  set(GSTFDSINK gstfdsink.c)
endif (${HAVE_SYS_SOCKET_H})

set(SOURCES
  gstcapsfilter.c
  gstelements.c
  gstfakesrc.c
  gstfakesink.c
  ${GSTFDSRC}
  ${GSTFDSINK}
  gstfilesink.c
  gstfilesrc.c
  gstidentity.c
  gstqueue.c
  gstqueue2.c
  gsttee.c
  gsttypefindelement.c
  gstmultiqueue.c
)

set(HEADERS
  gstcapsfilter.h
  gstfakesink.h
  gstfakesrc.h
  gstfdsrc.h
  gstfdsink.h
  gstfilesink.h
  gstfilesrc.h
  gstidentity.h
  gstqueue.h
  gstqueue2.h
  gsttee.h
  gsttypefindelement.h
  gstmultiqueue.h
)

add_library(gstcoreelements MODULE ${SOURCES})

target_link_libraries(gstcoreelements
  gstreamer-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)
