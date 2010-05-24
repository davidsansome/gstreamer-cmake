cmake_minimum_required(VERSION 2.6)

check_plugin_deps(cacasink CACA_FOUND)
if (${PLUGIN_cacasink_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${CACA_INCLUDE_DIRS})

  set(SOURCES
    gstcacasink.c
  )

  add_library(gstcacasink MODULE ${SOURCES})

  target_link_libraries(gstcacasink
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${CACA_LIBRARIES}
  )

  install_plugin(gstcacasink)
endif (${PLUGIN_cacasink_FOUND})
