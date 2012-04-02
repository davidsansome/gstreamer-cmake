cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad sndfile SNDFILE_FOUND)
if(PLUGIN_sndfile_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${SNDFILE_INCLUDE_DIRS})

  set(SOURCES
    gstsf.c
    gstsfsink.c
    gstsfsrc.c
  )

  add_library(gstsndfile MODULE ${SOURCES})

  target_link_libraries(gstsndfile
    gstbase-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${SNDFILE_LIBRARIES}
  )

  install_plugin(gstsndfile)
endif()
