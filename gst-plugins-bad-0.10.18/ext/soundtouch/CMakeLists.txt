cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad soundtouch SOUNDTOUCH_FOUND)
if (${PLUGIN_soundtouch_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${SOUNDTOUCH_INCLUDE_DIRS})

  set(SOURCES
    plugin.c
    gstbpmdetect.cc
    gstpitch.cc
  )

  add_library(gstsoundtouch MODULE ${SOURCES})

  target_link_libraries(gstsoundtouch
    gstbase-${GST_MAJORMINOR}
    gstaudio-${GST_MAJORMINOR}
    gstcontroller-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${SOUNDTOUCH_LIBRARIES}
  )

  install_plugin(gstsoundtouch)
endif (${PLUGIN_soundtouch_FOUND})
