cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad gme _FOUND)
if(PLUGIN_gme_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${_INCLUDE_DIRS})

  set(SOURCES
    gstgme.c
  )

  add_library(gstgme MODULE ${SOURCES})

  target_link_libraries(gstgme
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${_LIBRARIES}
  )

  install_plugin(gstgme)
endif()
