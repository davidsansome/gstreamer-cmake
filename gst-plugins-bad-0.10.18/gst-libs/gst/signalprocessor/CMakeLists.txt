cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstsignalprocessor.c
)

add_library(gstsignalprocessor-${GST_MAJORMINOR} MODULE ${SOURCES})

target_link_libraries(gstsignalprocessor-${GST_MAJORMINOR}
  gstbase-${GST_MAJORMINOR}
  gstaudio-${GST_MAJORMINOR}
  gstcontroller-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_lib(gstsignalprocessor)
install_headers(gst/signalprocessor ${HEADERS})
