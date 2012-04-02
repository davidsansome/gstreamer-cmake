cmake_minimum_required(VERSION 2.6)

check_plugin_deps(ugly sid SIDPLAY_INCLUDE_DIRS SIDPLAY_INCLUDE_DIRS)
if(PLUGIN_sid_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${SIDPLAY_INCLUDE_DIRS})

  set(SOURCES
    gstsiddec.cc
  )

  add_library(gstsid MODULE ${SOURCES})

  target_link_libraries(gstsid
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${SIDPLAY_LIBRARIES}
  )

  install_plugin(gstsid)
endif()
