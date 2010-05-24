cmake_minimum_required(VERSION 2.6)

check_plugin_deps(souphttpsrc SOUP_FOUND)
if (${PLUGIN_souphttpsrc_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${SOUP_INCLUDE_DIRS})

  set(SOURCES
    gstsouphttpsrc.c
  )

  add_library(gstsouphttpsrc MODULE ${SOURCES})

  target_link_libraries(gstsouphttpsrc
    gstbase-${GST_MAJORMINOR}
    gsttag-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${SOUP_LIBRARIES}
  )

  install_plugin(gstsouphttpsrc)
endif (${PLUGIN_souphttpsrc_FOUND})
