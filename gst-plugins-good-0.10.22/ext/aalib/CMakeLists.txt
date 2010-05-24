cmake_minimum_required(VERSION 2.6)

check_plugin_deps(aasink AALIB_LIBRARIES AALIB_INCLUDE_DIRS)
if (${PLUGIN_aasink_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${AALIB_INCLUDE_DIRS})

  set(SOURCES
    gstaasink.c
  )

  add_library(gstaasink MODULE ${SOURCES})

  target_link_libraries(gstaasink
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${AALIB_LIBRARIES}
  )

  install_plugin(gstaasink)
endif (${PLUGIN_aasink_FOUND})
