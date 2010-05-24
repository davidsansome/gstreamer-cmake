cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad modplug LIBMODPLUG_FOUND)
if (${PLUGIN_modplug_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${LIBMODPLUG_INCLUDE_DIRS})

  set(SOURCES
    gstmodplug.cc
  )

  add_library(gstmodplug MODULE ${SOURCES})

  target_link_libraries(gstmodplug
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${LIBMODPLUG_LIBRARIES}
  )

  install_plugin(gstmodplug)
endif (${PLUGIN_modplug_FOUND})
