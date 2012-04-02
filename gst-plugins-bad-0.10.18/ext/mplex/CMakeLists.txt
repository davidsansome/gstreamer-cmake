cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad mplex _FOUND)
if(PLUGIN_mplex_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${_INCLUDE_DIRS})

  set(SOURCES
    *.c
  )

  add_library(gstmplex MODULE ${SOURCES})

  target_link_libraries(gstmplex
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${_LIBRARIES}
  )

  install_plugin(gstmplex)
endif()
