cmake_minimum_required(VERSION 2.6)

check_plugin_deps(ugly mpeg2dec MPEG2DEC_FOUND)
if (${PLUGIN_mpeg2dec_FOUND})
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${MPEG2DEC_INCLUDE_DIRS})

  set(SOURCES
    gstmpeg2dec.c
  )

  add_library(gstmpeg2dec MODULE ${SOURCES})

  target_link_libraries(gstmpeg2dec
    gstbase-${GST_MAJORMINOR}
    gstvideo-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${MPEG2DEC_LIBRARIES}
  )

  install_plugin(gstmpeg2dec)
endif (${PLUGIN_mpeg2dec_FOUND})
