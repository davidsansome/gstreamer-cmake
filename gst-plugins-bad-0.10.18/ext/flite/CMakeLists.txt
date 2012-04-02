cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad flite FLITE_LIBRARIES FLITE_INCLUDE_DIRS)
if(PLUGIN_flite_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${FLITE_INCLUDE_DIRS})

  set(SOURCES
    gstflite.c
    gstflitetestsrc.c
  )

  add_library(gstflite MODULE ${SOURCES})

  target_link_libraries(gstflite
    gstbase-${GST_MAJORMINOR}
    gstaudio-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${FLITE_LIBRARIES}
  )

  install_plugin(gstflite)
endif()
