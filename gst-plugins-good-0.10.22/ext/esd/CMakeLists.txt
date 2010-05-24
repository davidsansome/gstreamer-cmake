cmake_minimum_required(VERSION 2.6)

check_plugin_deps(esd ESOUND_FOUND)
if (${PLUGIN_esd_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${ESOUND_INCLUDE_DIRS})

  set(SOURCES
    esdsink.c
    gstesd.c
  )

  add_library(gstesd MODULE ${SOURCES})

  target_link_libraries(gstesd
    gstbase-${GST_MAJORMINOR}
    gstaudio-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${ESOUND_LIBRARIES}
  )

  install_plugin(gstesd)
endif (${PLUGIN_esd_FOUND})
