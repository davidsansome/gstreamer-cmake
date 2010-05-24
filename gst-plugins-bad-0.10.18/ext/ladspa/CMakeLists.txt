cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad ladspa LRDF_FOUND)
if (${PLUGIN_ladspa_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${LRDF_INCLUDE_DIRS})

  set(SOURCES
    gstladspa.c
  )

  add_library(gstladspa MODULE ${SOURCES})

  target_link_libraries(gstladspa
    gstbase-${GST_MAJORMINOR}
    gstsignalprocessor-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${LRDF_LIBRARIES}
  )

  install_plugin(gstladspa)
endif (${PLUGIN_ladspa_FOUND})
