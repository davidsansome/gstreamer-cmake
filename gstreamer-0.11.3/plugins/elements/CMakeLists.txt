cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(HAVE_SYS_SOCKET_H)
  set(GSTFDSRC gstfdsrc.c)
  set(GSTFDSINK gstfdsink.c)
endif()

set(SOURCES
  ${GSTFDSINK}
  ${GSTFDSRC}
  gstcapsfilter.c
  gstdataqueue.c
  gstelements.c
  gstfakesink.c
  gstfakesrc.c
  gstfilesink.c
  gstfilesrc.c
  gstfunnel.c
  gstidentity.c
  gstinputselector.c
  gstmultiqueue.c
  gstoutputselector.c
  gstqueue.c
  gstqueue2.c
  gsttee.c
  gsttypefindelement.c
  gstvalve.c
)

set(HEADERS
  gstcapsfilter.h
  gstdataqueue.h
  gstfakesink.h
  gstfakesrc.h
  gstfdsink.h
  gstfdsrc.h
  gstfilesink.h
  gstfilesrc.h
  gstfunnel.h
  gstidentity.h
  gstinputselector.h
  gstmultiqueue.h
  gstoutputselector.h
  gstqueue.h
  gstqueue2.h
  gsttee.h
  gsttypefindelement.h
  gstvalve.h
)

add_library(gstcoreelements MODULE ${SOURCES})

target_link_libraries(gstcoreelements
  gstreamer-${GST_MAJORMINOR}
  gstbase-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstcoreelements)
