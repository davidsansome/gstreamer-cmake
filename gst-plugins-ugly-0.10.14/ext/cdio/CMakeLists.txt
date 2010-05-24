cmake_minimum_required(VERSION 2.6)

check_plugin_deps(ugly cdio CDIO_FOUND)
if (${PLUGIN_cdio_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${CDIO_INCLUDE_DIRS})

  set(SOURCES
    gstcdio.c
    gstcdiocddasrc.c
  )

  add_library(gstcdio MODULE ${SOURCES})

  target_link_libraries(gstcdio
    gstbase-${GST_MAJORMINOR}
    gstcdda-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${CDIO_LIBRARIES}
  )

  install_plugin(gstcdio)
endif (${PLUGIN_cdio_FOUND})
