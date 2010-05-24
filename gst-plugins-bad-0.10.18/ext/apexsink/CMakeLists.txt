cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad apexsink APEXSINK_FOUND)
if (${PLUGIN_apexsink_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${APEXSINK_INCLUDE_DIRS})

  set(SOURCES
    gstapexplugin.c
    gstapexraop.c
    gstapexsink.c
  )

  add_library(gstapexsink MODULE ${SOURCES})

  target_link_libraries(gstapexsink
    gstbase-${GST_MAJORMINOR}
    gstaudio-${GST_MAJORMINOR}
    gstinterfaces-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${APEXSINK_LIBRARIES}
  )

  install_plugin(gstapexsink)
endif (${PLUGIN_apexsink_FOUND})
