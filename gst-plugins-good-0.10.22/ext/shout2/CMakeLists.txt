cmake_minimum_required(VERSION 2.6)

check_plugin_deps(good shout2 SHOUT2_FOUND)
if(PLUGIN_shout2_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${SHOUT2_INCLUDE_DIRS})

  set(SOURCES
    gstshout2.c
  )

  add_library(gstshout2 MODULE ${SOURCES})

  target_link_libraries(gstshout2
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${SHOUT2_LIBRARIES}
  )

  install_plugin(gstshout2)
endif()
