cmake_minimum_required(VERSION 2.6)

check_plugin_deps(ugly twolame TWOLAME_FOUND)
if (${PLUGIN_twolame_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${TWOLAME_INCLUDE_DIRS})

  set(SOURCES
    gsttwolame.c
  )

  add_library(gsttwolame MODULE ${SOURCES})

  target_link_libraries(gsttwolame
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${TWOLAME_LIBRARIES}
  )

  install_plugin(gsttwolame)
endif (${PLUGIN_twolame_FOUND})
