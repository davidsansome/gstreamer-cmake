cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad swfdec SWFDEC_FOUND)
if (${PLUGIN_swfdec_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${SWFDEC_INCLUDE_DIRS})

  set(SOURCES
    gstswfdec.c
  )

  add_library(gstswfdec MODULE ${SOURCES})

  target_link_libraries(gstswfdec
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${SWFDEC_LIBRARIES}
  )

  install_plugin(gstswfdec)
endif (${PLUGIN_swfdec_FOUND})
