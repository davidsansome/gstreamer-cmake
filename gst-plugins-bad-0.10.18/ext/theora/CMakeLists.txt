cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad theora THEORADEC_FOUND)
if (${PLUGIN_theora_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${THEORADEC_INCLUDE_DIRS})

  set(SOURCES
    theoradec.c
  )

  add_library(gsttheora MODULE ${SOURCES})

  target_link_libraries(gsttheora
    gstbase-${GST_MAJORMINOR}
    gsttag-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${THEORADEC_LIBRARIES}
  )

  install_plugin(gsttheora)
endif (${PLUGIN_theora_FOUND})
