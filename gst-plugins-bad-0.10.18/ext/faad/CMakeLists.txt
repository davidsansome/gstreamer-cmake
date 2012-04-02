cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad faad FAAD_LIBRARIES FAAD_INCLUDE_DIRS)
if(PLUGIN_faad_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${FAAD_INCLUDE_DIRS})

  set(SOURCES
    gstfaad.c
  )

  add_library(gstfaad MODULE ${SOURCES})

  target_link_libraries(gstfaad
    gstbase-${GST_MAJORMINOR}
    gstaudio-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${FAAD_LIBRARIES}
  )

  install_plugin(gstfaad)
endif()
