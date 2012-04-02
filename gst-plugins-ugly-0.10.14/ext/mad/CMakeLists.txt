cmake_minimum_required(VERSION 2.6)

check_plugin_deps(ugly mad MAD_FOUND)
if(PLUGIN_mad_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${MAD_INCLUDE_DIRS})

  set(SOURCES
    gstmad.c
  )

  add_library(gstmad MODULE ${SOURCES})

  target_link_libraries(gstmad
    gstbase-${GST_MAJORMINOR}
    gsttag-${GST_MAJORMINOR}
    gstaudio-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${MAD_LIBRARIES}
  )

  install_plugin(gstmad)
endif()
