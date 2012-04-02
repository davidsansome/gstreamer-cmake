cmake_minimum_required(VERSION 2.6)

check_plugin_deps(bad rsvg RSVG_FOUND)
if(PLUGIN_rsvg_FOUND)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR})
  include_directories(${CMAKE_CURRENT_BINARY_DIR})
  include_directories(${RSVG_INCLUDE_DIRS})

  set(SOURCES
    gstrsvg.c
    gstrsvgdec.c
  )

  add_library(gstrsvg MODULE ${SOURCES})

  target_link_libraries(gstrsvg
    gstbase-${GST_MAJORMINOR}
    gstvideo-${GST_MAJORMINOR}
    ${GLIB_LIBRARIES}
    ${RSVG_LIBRARIES}
  )

  install_plugin(gstrsvg)
endif()
