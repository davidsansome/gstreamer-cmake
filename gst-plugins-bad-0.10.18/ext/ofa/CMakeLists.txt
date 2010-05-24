cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad ofa OFA_FOUND)
if (${PLUGIN_ofa_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${OFA_INCLUDE_DIRS})

  set(SOURCES
    gstofa.c
  )

  add_library(gstofa MODULE ${SOURCES})

  target_link_libraries(gstofa
    gstbase-${GST_MAJORMINOR}
    gstaudio-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${OFA_LIBRARIES}
  )

  install_plugin(gstofa)
endif (${PLUGIN_ofa_FOUND})
