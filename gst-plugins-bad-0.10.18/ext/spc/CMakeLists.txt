cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad spc SPC_LIBRARIES SPC_INCLUDE_DIRS)
if(PLUGIN_spc_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${SPC_INCLUDE_DIRS})

  set(SOURCES
    gstspc.c
    tag.c
  )

  add_library(gstspc MODULE ${SOURCES})

  target_link_libraries(gstspc
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${SPC_LIBRARIES}
  )

  install_plugin(gstspc)
endif()
