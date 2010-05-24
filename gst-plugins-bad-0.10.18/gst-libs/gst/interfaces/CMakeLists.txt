cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  photography.c
)

set(GEN_HEADERS
  photography-enumtypes.h
)

set(HEADERS
  photography.h
)

add_glib_enumtypes(SOURCES photography-enumtypes GST_PHOTOGRAPHY ${HEADERS})
add_gen_headers(SOURCES gst/photography ${GEN_HEADERS})

add_library(gstphotography-${GST_MAJORMINOR} MODULE ${SOURCES})

target_link_libraries(gstphotography-${GST_MAJORMINOR}
  gstbase-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_lib(gstphotography)
install_headers(gst/photography ${HEADERS})
install_gen_headers(gst/photography ${GEN_HEADERS})
