cmake_minimum_required(VERSION 2.6)

check_plugin_deps(taglib TAGLIB_FOUND)
if (${PLUGIN_taglib_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${TAGLIB_INCLUDE_DIRS})

  set(SOURCES
    gsttaglibmux.c
  )

  add_library(gsttaglib MODULE ${SOURCES})

  target_link_libraries(gsttaglib
    gstbase-${GST_MAJORMINOR}
    gsttag-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${TAGLIB_LIBRARIES}
  )

  install_plugin(gsttaglib)
endif (${PLUGIN_taglib_FOUND})
