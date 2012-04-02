cmake_minimum_required(VERSION 2.6)

check_plugin_deps(ugly a52dec A52_INCLUDE_DIRS A52_LIBRARIES)
if(PLUGIN_a52dec_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${A52_INCLUDE_DIRS})
  include_directories(${LIBOIL_INCLUDE_DIRS})

  set(SOURCES
    gsta52dec.c
  )

  add_library(gsta52dec MODULE ${SOURCES})

  target_link_libraries(gsta52dec
    gstbase-${GST_MAJORMINOR}
    gstaudio-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${A52_LIBRARIES}
    ${LIBOIL_LIBRARIES}
  )

  install_plugin(gsta52dec)
endif()
