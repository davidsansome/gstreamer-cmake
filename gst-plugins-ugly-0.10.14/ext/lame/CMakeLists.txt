cmake_minimum_required(VERSION 2.6)

check_plugin_deps(ugly lame LAME_INCLUDE_DIRS LAME_LIBRARIES)
if(PLUGIN_lame_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${LAME_INCLUDE_DIRS})

  set(SOURCES
    gstlame.c
    gstlamemp3enc.c
    plugin.c
  )

  add_library(gstlame MODULE ${SOURCES})

  target_link_libraries(gstlame
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${LAME_LIBRARIES}
  )

  install_plugin(gstlame)
endif()
