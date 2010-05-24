cmake_minimum_required(VERSION 2.6)

check_plugin_deps(halelements HAL_FOUND)
if (${PLUGIN_halelements_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${HAL_INCLUDE_DIRS})

  set(SOURCES
    gsthalaudiosink.c
    gsthalaudiosrc.c
    gsthalelements.c
    hal.c
  )

  add_library(gsthalelements MODULE ${SOURCES})

  target_link_libraries(gsthalelements
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${HAL_LIBRARIES}
  )

  install_plugin(gsthalelements)
endif (${PLUGIN_halelements_FOUND})
