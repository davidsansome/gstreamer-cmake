cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

if(${LIBXML_FOUND})
  set(SAMIPARSE_SOURCES samiparse.c)
endif(${LIBXML_FOUND})

set(SOURCES
  gstssaparse.c
  gstssaparse.h
  gstsubparse.c
  gstsubparse.h
  ${SAMIPARSE_SOURCES}
  tmplayerparse.c
  tmplayerparse.h
  mpl2parse.c
  mpl2parse.h
  qttextparse.c
  qttextparse.h
)

add_library(gstsubparse MODULE ${SOURCES})

target_link_libraries(gstsubparse
  gstbase-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_plugin(gstsubparse)
