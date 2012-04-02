cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad zbar _FOUND)
if(PLUGIN_zbar_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${_INCLUDE_DIRS})

  set(SOURCES
    gstzbar.c
  )

  add_library(gstzbar MODULE ${SOURCES})

  target_link_libraries(gstzbar
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${_LIBRARIES}
  )

  install_plugin(gstzbar)
endif()
