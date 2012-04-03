cmake_minimum_required(VERSION 2.6)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(SOURCES
  gstvorbistag.c
  gstid3tag.c
  gstxmptag.c
  lang.c
  tags.c
  gsttagdemux.c
  gstexiftag.c
  gsttageditingprivate.c
  gsttagmux.c
  id3v2.c
  id3v2frames.c
  licenses.c
  xmpwriter.c
)

set(HEADERS
  tag.h
  gsttagdemux.h
  gsttagmux.h
  xmpwriter.h
)

add_library(gsttag-${GST_MAJORMINOR} SHARED ${SOURCES})

target_link_libraries(gsttag-${GST_MAJORMINOR}
  gstreamer-${GST_MAJORMINOR}
  gstbase-${GST_MAJORMINOR}
  ${GLIB_LIBRARIES}
)

install_lib(gsttag)
install_headers(gst/tag ${HEADERS})
