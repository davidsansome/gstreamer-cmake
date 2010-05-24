cmake_minimum_required(VERSION 2.6)

check_plugin_deps(good annodex LIBXML_FOUND)
if (${PLUGIN_annodex_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${LIBXML_INCLUDE_DIRS})

  set(SOURCES
    gstannodex.c
    gstcmmldec.c
    gstcmmlenc.c
    gstcmmlparser.c
    gstcmmltag.c
    gstcmmlutils.c
  )

  add_library(gstannodex MODULE ${SOURCES})

  target_link_libraries(gstannodex
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${LIBXML_LIBRARIES}
  )

  install_plugin(gstannodex)
endif (${PLUGIN_annodex_FOUND})
