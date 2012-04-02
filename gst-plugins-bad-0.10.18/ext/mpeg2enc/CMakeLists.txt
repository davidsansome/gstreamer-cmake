cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad mpeg2enc _FOUND)
if(PLUGIN_mpeg2enc_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${_INCLUDE_DIRS})

  set(SOURCES
    *.c
  )

  add_library(gstmpeg2enc MODULE ${SOURCES})

  target_link_libraries(gstmpeg2enc
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${_LIBRARIES}
  )

  install_plugin(gstmpeg2enc)
endif()
