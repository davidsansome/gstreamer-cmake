cmake_minimum_required(VERSION 2.6)

check_plugin_deps(good speex SPEEX_FOUND)
if(PLUGIN_speex_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${SPEEX_INCLUDE_DIRS})

  set(SOURCES
    gstspeex.c
    gstspeexdec.c
    gstspeexenc.c
  )

  add_library(gstspeex MODULE ${SOURCES})

  target_link_libraries(gstspeex
    gstbase-${GST_MAJORMINOR}
    gsttag-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${SPEEX_LIBRARIES}
  )

  install_plugin(gstspeex)
endif()
