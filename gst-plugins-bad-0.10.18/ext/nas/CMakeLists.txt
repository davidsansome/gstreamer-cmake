cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad nassink NAS_LIBRARIES NAS_INCLUDE_DIRS)
if (${PLUGIN_nassink_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${NAS_INCLUDE_DIRS})

  set(SOURCES
    nassink.c
  )

  add_library(nassink MODULE ${SOURCES})

  target_link_libraries(nassink
    gstbase-${GST_MAJORMINOR}
    gstaudio-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${NAS_LIBRARIES}
  )

  install_plugin(nassink)
endif (${PLUGIN_nassink_FOUND})
