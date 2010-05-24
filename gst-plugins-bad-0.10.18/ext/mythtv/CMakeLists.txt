cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad mythtv GMYTH_FOUND)
if (${PLUGIN_mythtv_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${GMYTH_INCLUDE_DIRS})

  set(SOURCES
    gstmythtvsrc.c
  )

  add_library(gstmythtv MODULE ${SOURCES})

  target_link_libraries(gstmythtv
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${GMYTH_LIBRARIES}
  )

  install_plugin(gstmythtv)
endif (${PLUGIN_mythtv_FOUND})
